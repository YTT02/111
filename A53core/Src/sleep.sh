echo enabled > /sys/bus/platform/devices/90000000.rpmsg/power/wakeup
modprobe imx_rpmsg_tty
/unit_tests/Remote_Processor_Messaging/mxc_mcc_tty_test.out /dev/ttyRPMSG30 115200 R 100 1000 &
echo deadbeaf > /dev/ttyRPMSG30
sleep 1
echo mem > /sys/power/state