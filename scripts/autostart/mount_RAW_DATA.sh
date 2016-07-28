# -* bash *-
### BEGIN INIT INFO
# Provides: RAW_DATA
# Required-Start: $local_fs
# Required-Stop:
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Mount RAW_DATA partition to /media/RAW_DATA
### END INIT INFO

PATH=/sbin:/bin:/usr/sbin:/usr/bin
DESC="RCT RAW_DATA Daemon"

case "$1" in
	start)
		mount /dev/sda1 /media/RAW_DATA -o dmask=000,fmask=111
		;;
	stop)
		umount /media/RAW_DATA
		;;
	restart)
		umount /media/RAW_DATA
		mount /dev/sda1 /media/RAW_DATA -o dmask=000,fmask=111
		;;
	start)
		;;
esac
exit 0
