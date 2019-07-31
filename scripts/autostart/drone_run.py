#!/usr/bin/env python3

import os
import subprocess
import shlex
import signal

class DroneRunner(object):
	"""docstring for DroneRunner"""
	def __init__(self):
		super(DroneRunner, self).__init__()
		self.data_directory = self.get_var('output_dir')
		if not os.path.ismount(self.data_directory):
			print('Storage not ready!')

		if not os.path.isfile(os.path.join(self.data_directory, 'LAST_RUN.TXT')):
			print("Storage not ready - no run file!")
			raise Exception('Storage not ready - no run file!')

		with open(os.path.join(self.data_directory, 'LAST_RUN.TXT')) as lastrun:
			last_run = int(lastrun.readline().strip())
		with open(os.path.join(self.data_directory, 'LAST_RUN.TXT'), 'w') as lastrun:
			lastrun.write(str(last_run + 1))
		
		run_num = last_run + 1

		run_dir = os.path.join(self.data_directory, "RUN_%06d" % (run_num))
		os.makedirs(run_dir)

		localize_file = os.path.join(run_dir, "LOCALIZE_%06d" % (run_num))
		with open(localize_file, 'w') as file:
			file.write("")

		sampling_freq = int(self.get_var('sampling_freq'))
		center_freq = int(self.get_var('center_freq'))

		sdr_record_cmd = ('sdr_record -g 22.0 -s %d -c %d'
			' -r %d -o %s' % (sampling_freq, center_freq, run_num, run_dir))
		udp_server_cmd = 'udp_client.py %s %d' % (run_dir, run_num)

		signal.signal(signal.SIGINT, self.sigint_handler)

		self.udp_server = subprocess.Popen(shlex.split(udp_server_cmd))
		self.sdr_record = subprocess.Popen(shlex.split(sdr_record_cmd))

	def sigint_handler(self, signal, frame):
		# print("Received sig")
		# global init_thread_op
		# global master_thread_op
		# init_thread_op = False
		# master_thread_op = False
		self.sdr_record.send_signal(2)
		self.sdr_record.wait()

	def get_var(self, var):
		var_file = open('&INSTALL_PREFIX/etc/rct_config')
		for line in var_file:
			if line.split('=')[0].strip() == var:
				value = line.split('=')[1]
				return value.strip().strip('"').strip("'")
		return None

	def wait(self):
		self.sdr_record.wait()
		self.udp_server.wait()

def main():
	droneRun = DroneRunner()

	signal.pause()

	droneRun.wait()

if __name__ == '__main__':
	main()
