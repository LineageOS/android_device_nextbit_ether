#!/system/bin/sh
sleep 7
echo ------NXP_speaker_INIT------ >/dev/kmsg &
tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 1
tinyplay /system/etc/silence.wav -d 1 >/dev/null &
fihnxptest init  &
sleep 2
tinymix 'PRI_MI2S_RX Audio Mixer MultiMedia2' 0
