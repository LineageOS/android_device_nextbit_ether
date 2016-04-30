#!/system/bin/sh

tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 1
tinyplay /system/media/silence.wav -d 1 >/dev/null &
sleep 2
tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 0