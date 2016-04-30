#!/system/bin/sh

project_name=`getprop ro.boot.device`

if [ "$project_name" == "nbq" ]; then
	setprop persist.nfc.support true
else
	setprop persist.nfc.support false
fi
