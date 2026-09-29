/* Linux status updater for stock dwm. See LICENSE for license details. */
#include <errno.h>
#include <ifaddrs.h>
#include <linux/wireless.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <pulse/pulseaudio.h>

static volatile sig_atomic_t running = 1;
static pa_context *context;
static char vol[32] = "🔊 N/A";
static int pending;
static int refresh;
static Display *display;
static Atom utf8;
static char status[256], memory[48], network[48], date[32], usage[32];
static int status_ready;
static void audio(pa_mainloop *loop);

static void
publish(void)
{
	if (!status_ready)
		return;
	snprintf(status, sizeof status, " %s  %s  %s  %s  %s ",
	         usage, memory, network, vol, date);
	if (display) {
		XChangeProperty(display, DefaultRootWindow(display), XA_WM_NAME,
		                utf8, 8, PropModeReplace, (unsigned char *)status, strlen(status));
		XFlush(display);
	}
}

static void
stop(int sig)
{
	(void)sig;
	running = 0;
}

static double
now(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

static int
cpu(unsigned long long *total, unsigned long long *idle)
{
	unsigned long long v[8];
	FILE *f = fopen("/proc/stat", "r");
	int n, i;
	if (!f)
		return 0;
	n = fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
	           &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7]);
	fclose(f);
	if (n != 8)
		return 0;
	/* Guest time is already counted in user/nice. */
	for (*total = 0, i = 0; i < 8; i++)
		*total += v[i];
	*idle = v[3] + v[4];
	return 1;
}

static void
ram(char *out, size_t size)
{
	char line[256];
	unsigned long total = 0, available = 0;
	FILE *f = fopen("/proc/meminfo", "r");
	if (f) {
		while (fgets(line, sizeof line, f)) {
			sscanf(line, "MemTotal: %lu", &total);
			sscanf(line, "MemAvailable: %lu", &available);
		}
		fclose(f);
	}
	if (total)
		snprintf(out, size, "💾 %.1f/%.1fG",
		         (total - available) / 1048576.0, total / 1048576.0);
	else
		snprintf(out, size, "💾 N/A");
}

static void
wifi(int sock, char *out, size_t size)
{
	struct ifaddrs *interfaces, *p;
	struct iwreq req;
	char ssid[IW_ESSID_MAX_SIZE + 1];
	int wireless = 0, supported = 0;
	snprintf(out, size, "📶 N/A");
	if (sock < 0 || getifaddrs(&interfaces) < 0)
		return;
	for (p = interfaces; p; p = p->ifa_next) {
		/* Each interface has one AF_PACKET entry, avoiding duplicate queries. */
		if (!p->ifa_addr || p->ifa_addr->sa_family != AF_PACKET)
			continue;
		memset(&req, 0, sizeof req);
		snprintf(req.ifr_name, sizeof req.ifr_name, "%s", p->ifa_name);
		if (ioctl(sock, SIOCGIWNAME, &req) < 0)
			continue;
		wireless = 1;
		memset(ssid, 0, sizeof ssid);
		req.u.essid.pointer = ssid;
		req.u.essid.length = IW_ESSID_MAX_SIZE;
		req.u.essid.flags = 0;
		if (ioctl(sock, SIOCGIWESSID, &req) < 0)
			continue;
		supported = 1;
		if (!ssid[0])
			continue;
		/* Check association rather than a configured but disconnected SSID. */
		if (ioctl(sock, SIOCGIWAP, &req) < 0)
			continue;
		if (!memcmp(req.u.ap_addr.sa_data, "\0\0\0\0\0\0", 6))
			continue;
		for (size_t i = 0; ssid[i]; i++)
			if ((unsigned char)ssid[i] < 32 || ssid[i] == 127)
				ssid[i] = '?';
		snprintf(out, size, "📶 %s", ssid);
		freeifaddrs(interfaces);
		return;
	}
	freeifaddrs(interfaces);
	if (!wireless || supported)
		snprintf(out, size, "📶 off");
}

static void
sink_info(pa_context *c, const pa_sink_info *info, int end, void *data)
{
	(void)c;
	(void)data;
	if (end) {
		pending = 0;
		if (end < 0)
			snprintf(vol, sizeof vol, "🔊 N/A");
		publish();
		return;
	}
	if (info->mute)
		snprintf(vol, sizeof vol, "🔇 muted");
	else
		snprintf(vol, sizeof vol, "🔊 %.0f%%",
		         100.0 * pa_cvolume_avg(&info->volume) / PA_VOLUME_NORM);
	publish();
}

static void
server_info(pa_context *c, const pa_server_info *info, void *data)
{
	pa_operation *op;
	(void)data;
	if (info && info->default_sink_name &&
	    (op = pa_context_get_sink_info_by_name(c, info->default_sink_name, sink_info, NULL))) {
		pa_operation_unref(op);
	} else {
		pending = 0;
		snprintf(vol, sizeof vol, "🔊 N/A");
		publish();
	}
}

static void
audio_event(pa_context *c, pa_subscription_event_type_t event,
            uint32_t index, void *data)
{
	pa_subscription_event_type_t facility = event & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
	(void)c;
	(void)index;
	(void)data;
	if (facility == PA_SUBSCRIPTION_EVENT_SINK || facility == PA_SUBSCRIPTION_EVENT_SERVER)
		refresh = 1;
}

static void
context_state(pa_context *c, void *data)
{
	pa_operation *op;
	if (pa_context_get_state(c) == PA_CONTEXT_READY) {
		pa_context_set_subscribe_callback(c, audio_event, NULL);
		op = pa_context_subscribe(c, PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SERVER,
		                          NULL, NULL);
		if (op) pa_operation_unref(op);
		audio(data);
	} else if (!PA_CONTEXT_IS_GOOD(pa_context_get_state(c))) {
		pending = 0;
		snprintf(vol, sizeof vol, "🔊 N/A");
		publish();
	}
}

static void
audio(pa_mainloop *loop)
{
	pa_operation *op;
	pa_context_state_t state;
	if (context) {
		state = pa_context_get_state(context);
		if (!PA_CONTEXT_IS_GOOD(state)) {
			pa_context_disconnect(context);
			pa_context_unref(context);
			context = NULL;
			pending = 0;
			snprintf(vol, sizeof vol, "🔊 N/A");
		}
	}
	if (!context) {
		context = pa_context_new(pa_mainloop_get_api(loop), "dwm-status");
		if (!context)
			return;
		pa_context_set_state_callback(context, context_state, loop);
		pa_context_connect(context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);
	}
	if (pa_context_get_state(context) == PA_CONTEXT_READY && !pending) {
		op = pa_context_get_server_info(context, server_info, NULL);
		if (op) {
			pending = 1;
			refresh = 0;
			pa_operation_unref(op);
		}
	}
}

int
main(int argc, char **argv)
{
	pa_mainloop *loop;
	unsigned long long previous = 0, prev_idle = 0, total, idle;
	double deadline;
	time_t wall;
	int once = argc == 2 && !strcmp(argv[1], "--once");
	int sock;
	if (argc > 1 && !once) {
		fprintf(stderr, "usage: %s [--once]\n", argv[0]);
		return 1;
	}
	if (!once) {
		if (!(display = XOpenDisplay(NULL))) {
			fprintf(stderr, "dwm-status: cannot open X display\n");
			return 1;
		}
		utf8 = XInternAtom(display, "UTF8_STRING", False);
	}
	if (!(loop = pa_mainloop_new())) {
		if (display) XCloseDisplay(display);
		return 1;
	}
	signal(SIGINT, stop);
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	cpu(&previous, &prev_idle);
	audio(loop);
	deadline = now() + 1;
	while (running) {
		/* PulseAudio's poll handles events while sleeping between updates. */
		if (pa_mainloop_prepare(loop, 100) < 0 || pa_mainloop_poll(loop) < 0 ||
		    pa_mainloop_dispatch(loop) < 0) {
			if (errno == EINTR) continue;
			break;
		}
		if (refresh && !pending)
			audio(loop);
		if (now() < deadline)
			continue;
		snprintf(usage, sizeof usage, "🧠 N/A");
		if (cpu(&total, &idle)) {
			if (total > previous && idle >= prev_idle && idle - prev_idle <= total - previous)
				snprintf(usage, sizeof usage, "🧠 %.0f%%",
				         100.0 * (1.0 - (double)(idle - prev_idle) / (total - previous)));
			previous = total;
			prev_idle = idle;
		}
		ram(memory, sizeof memory);
		wifi(sock, network, sizeof network);
		wall = time(NULL);
		strftime(date, sizeof date, "📅 %Y-%m-%d 🕒 %H:%M:%S", localtime(&wall));
		status_ready = 1;
		publish();
		if (once) {
			puts(status);
			break;
		}
		audio(loop);
		deadline += 1;
		if (deadline < now()) deadline = now() + 1;
	}
	if (context) {
		pa_context_disconnect(context);
		pa_context_unref(context);
	}
	pa_mainloop_free(loop);
	if (sock >= 0) close(sock);
	if (display) XCloseDisplay(display);
	return 0;
}
