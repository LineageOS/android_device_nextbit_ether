#!/vendor/bin/sh

################################################################################
# helper functions to allow Android init like script

function write() {
    echo -n $2 > $1
}

function get-set-forall() {
    for f in $1 ; do
        cat $f
        write $f $2
    done
}

################################################################################

# take the A57s offline when thermal hotplug is disabled
write /sys/devices/system/cpu/cpu4/online 0
write /sys/devices/system/cpu/cpu5/online 0

# disable thermal bcl hotplug to switch governor
write /sys/module/msm_thermal/core_control/enabled 0
get-set-forall /sys/devices/soc.0/qcom,bcl.*/mode disable
bcl_hotplug_mask=`get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_mask 0`
bcl_hotplug_soc_mask=`get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_soc_mask 0`

# Limit A57 max freq from msm_perf module in case CPU 4 is offline
write /sys/module/msm_performance/parameters/cpu_max_freq "4:960000 5:960000"

# enable LPM
write /sys/module/lpm_levels/parameters/sleep_disabled 0

# configure governor settings for little cluster
write /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor interactive
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/use_sched_load 1
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/use_migration_notif 1
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/above_hispeed_delay 19000
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/go_hispeed_load 90
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_rate 20000
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq 960000
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/io_is_busy 1
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/target_loads "65 460800:75 960000:80"
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/min_sample_time 40000
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/max_freq_hysteresis 80000
write /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq 384000

# online CPU4
write /sys/devices/system/cpu/cpu4/online 1

# configure governor settings for big cluster
write /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor interactive
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/use_sched_load 1
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/use_migration_notif 1
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/above_hispeed_delay 19000
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/go_hispeed_load 90
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_rate 20000
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/hispeed_freq 1248000
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/io_is_busy 1
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/target_loads "70 960000:80 1248000:85"
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/min_sample_time 40000
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/max_freq_hysteresis 80000
write /sys/devices/system/cpu/cpu4/cpufreq/scaling_min_freq 384000

# plugin remaining A57s
write /sys/devices/system/cpu/cpu5/online 1

# Restore CPU 4 max freq from msm_performance
write /sys/module/msm_performance/parameters/cpu_max_freq "4:4294967295 5:4294967295"

# input boost configuration
write /sys/module/cpu_boost/parameters/input_boost_freq "0:960000"
write /sys/module/cpu_boost/parameters/input_boost_ms 40

# Configure core_ctl
write /sys/devices/system/cpu/cpu4/core_ctl/max_cpus 2
write /sys/devices/system/cpu/cpu4/core_ctl/min_cpus 0
write /sys/devices/system/cpu/cpu4/core_ctl/busy_up_thres 60
write /sys/devices/system/cpu/cpu4/core_ctl/busy_down_thres 30
write /sys/devices/system/cpu/cpu4/core_ctl/offline_delay_ms 100
write /sys/devices/system/cpu/cpu4/core_ctl/is_big_cluster 1
write /sys/devices/system/cpu/cpu4/core_ctl/task_thres 2
chown system:system /sys/devices/system/cpu/cpu4/core_ctl/min_cpus
chown system:system /sys/devices/system/cpu/cpu4/core_ctl/max_cpus

# Setting B.L scheduler parameters
write /proc/sys/kernel/power_aware_timer_migration 1
write /proc/sys/kernel/sched_migration_fixup 1
write /proc/sys/kernel/sched_small_task 30
write /proc/sys/kernel/sched_upmigrate 95
write /proc/sys/kernel/sched_downmigrate 85
write /proc/sys/kernel/sched_window_stats_policy 2
write /proc/sys/kernel/sched_ravg_hist_size 5
get-set-forall /sys/devices/system/cpu/*/sched_mostly_idle_load 20
get-set-forall /sys/devices/system/cpu/*/sched_mostly_idle_nr_run 3
write /proc/sys/kernel/sched_freq_inc_notify 400000
write /proc/sys/kernel/sched_freq_dec_notify 400000

# android background processes are set to nice 10. Never schedule these on the a57s.
write /proc/sys/kernel/sched_upmigrate_min_nice 9

# enable rps static configuration
write /sys/class/net/rmnet_ipa0/queues/rx-0/rps_cpus 8

# devfreq
get-set-forall /sys/class/devfreq/qcom,cpubw*/governor bw_hwmon
get-set-forall /sys/class/devfreq/qcom,cpubw*/bw_hwmon/io_percent 20
get-set-forall /sys/class/devfreq/qcom,cpubw*/bw_hwmon/guard_band_mbps 30
get-set-forall /sys/class/devfreq/qcom,mincpubw*/governor cpufreq

# Disable sched_boost
write /proc/sys/kernel/sched_boost 0

# re-enable thermal and BCL hotplug
write /sys/module/msm_thermal/core_control/enabled 1
get-set-forall /sys/devices/soc.0/qcom,bcl.*/low_threshold_ua 50000
get-set-forall /sys/devices/soc.0/qcom,bcl.*/high_threshold_ua 4200000
get-set-forall /sys/devices/soc.0/qcom,bcl.*/vph_low_thresh_uv 3300000
get-set-forall /sys/devices/soc.0/qcom,bcl.*/vph_high_thresh_uv 4300000
get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_mask $bcl_hotplug_mask
get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_soc_mask $bcl_hotplug_soc_mask
get-set-forall /sys/devices/soc.0/qcom,bcl.*/mode enable

# set GPU default power level to 5 (180MHz) instead of 4 (305MHz)
write /sys/class/kgsl/kgsl-3d0/default_pwrlevel 5

setprop vendor.post_boot.parsed 1
