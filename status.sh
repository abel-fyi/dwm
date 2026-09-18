#!/bin/sh

# cumulative CPU counters from /proc/stat: "idle total"
cpu_snapshot() {
    read -r _ user nice system idle iowait irq softirq steal _ < /proc/stat
    echo $((idle + iowait)) $((user + nice + system + idle + iowait + irq + softirq + steal))
}

while :; do
    set -- $(cpu_snapshot)
    idle1=$1 total1=$2

    sleep 1   # this doubles as the CPU sampling window

    set -- $(cpu_snapshot)
    idle2=$1 total2=$2

    d_idle=$((idle2 - idle1))
    d_total=$((total2 - total1))
    if [ "$d_total" -gt 0 ]; then
        cpu=$(( (d_total - d_idle) * 100 / d_total ))
    else
        cpu=0
    fi

    ram=$(free | awk '/^Mem:/ {printf "%.0f%%", $3 / $2 * 100}')

    bat=""
    for b in /sys/class/power_supply/BAT*; do
        [ -r "$b/capacity" ] || continue   # no battery (desktop) -> skip
        capacity=$(cat "$b/capacity")
        if [ "$(cat "$b/status")" = "Discharging" ]; then
            icon="🔋"
        else                               # Charging / Full
            icon="🔌"

        fi
        bat="$bat $icon$capacity%"
    done

    xsetroot -name " 🧠 ${cpu}%  💾 $ram$bat  📅 $(date '+%Y-%m-%d  %H:%M:%S') "
done
