#!/usr/bin/python3

import os
import glob
import subprocess
import shlex
import signal

def sigint_handler(signal, frame):
	# print("Received sig")
	# global init_thread_op
	# global master_thread_op
	# init_thread_op = False
	# master_thread_op = False
	pass


if __name__ == '__main__':
	data_directory = '/mnt/RAW_DATA'
	# data_directory = '/media/ntlhui/RCT_DATA'
	if not os.path.ismount(data_directory):
		print("Storage not ready!")
		exit()

	if not os.path.isfile(os.path.join(data_directory, 'LAST_RUN.TXT')):
		print("Storage not ready - no run file!")
		exit()

	with open(os.path.join(data_directory, 'LAST_RUN.TXT')) as lastrun:
		last_run = int(lastrun.readline().strip())
	with open(os.path.join(data_directory, 'LAST_RUN.TXT'), 'w') as lastrun:
		lastrun.write(str(last_run + 1))

	run_num = last_run + 1

	run_dir = os.path.join(data_directory, "RUN_%06d" % (run_num))
	os.makedirs(run_dir)

	localize_file = os.path.join(run_dir, "LOCALIZE_%06d" % (run_num))
	with open(localize_file, 'w') as file:
		file.write("")


	sdr_record_cmd = ('src/sdr_record/sdr_record -g 22.0 -s 250000 -c 17300000'
		' -r %d -o %s --gps_target /dev/ttyACM0' % (run_num, run_dir))
	tcp_server_cmd = 'src/python/tcp_server.py %s %d' % (run_dir, run_num)

	tcp_server = subprocess.Popen(shlex.split(tcp_server_cmd))
	sdr_record = subprocess.Popen(shlex.split(sdr_record_cmd))

	signal.signal(signal.SIGINT, sigint_handler)

	signal.pause()

	sdr_record.send_signal(9)
	sdr_record.wait()

