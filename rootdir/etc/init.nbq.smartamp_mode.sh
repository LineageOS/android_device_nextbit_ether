#!/system/bin/sh

echo ---NXP_smartamp_change_mode--- > /dev/kmsg &

mode=`getprop audio.smartamp.mode`
parameter=`getprop audio.smartamp.parameter`
boot_completed=`getprop sys.boot_completed`

function change_mode () {
	tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 1
	tinyplay /system/etc/silence.wav -d 1 >/dev/null &
	TINYPLAY_PID=$!
	fihnxptest change $1 > /dev/null 2>&1
	# To prevent selinux logs for ps command, KennyChu
	#fihnxptest kill tinyplay
	kill $TINYPLAY_PID
	setprop audio.smartamp.parameter $1
}

	case "$mode" in
		"power_on")
			fihnxptest on > /dev/null 2>&1
			;;
		"power_off")
			fihnxptest off > /dev/null 2>&1
			;;
        "power1_off")
			fihnxptest one > /dev/null 2>&1
			;;
		"clk_on")
			tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 1
			tinyplay /system/etc/silence.wav -d 1 >/dev/null &
			;;
		"clk_off")
			tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 0
			;;
		"kill_tinyplay")
			fihnxptest kill tinyplay;
			;;
		"bypass")
			if [ "$parameter" == "bypass"  ]; then
				fihnxptest on > /dev/null 2>&1
			else
				change_mode bypass;
			fi
			;;
		"playback")
			if [ "$parameter" == "playback"  ]; then
				fihnxptest on > /dev/null 2>&1
			else
				change_mode playback;
				sleep 1;
				tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 0
			fi
			;;
        "keytone")
			if [ "$parameter" == "keytone"  ]; then
				fihnxptest on > /dev/null 2>&1
			else
				change_mode keytone;
				sleep 1;
				tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 0
			fi
			;;
		"alarm")
			if [ "$parameter" == "alarm"  ]; then
				fihnxptest on > /dev/null 2>&1
			else
				change_mode alarm;
				sleep 1;
				tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 0
			fi
			;;
		*)
			;; #Do nothing
	esac