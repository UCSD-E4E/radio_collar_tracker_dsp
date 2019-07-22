#!/usr/bin/env python

import socket
import argparse
import os
import json
import datetime
import time

def main():
	parser = argparse.ArgumentParser("UDP Client")
	parser.add_argument('data_dir', help='run directory')
	parser.add_argument('run_num', help='run_num', type=int)

	args = parser.parse_args()

	data_dir = args.data_dir
	run_num = args.run_num

	localization_file = open(os.path.join(data_dir, 'LOCALIZE_%06d' % (run_num)))

	UDP_PORT = 9000
	UDP_IP = '255.255.255.255'

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	prevTime = datetime.datetime.now()

	try:
		while True:
			now = datetime.datetime.now()
			if (now - prevTime).total_seconds() > 1:
				msg = "{heartbeat:{time: %d}}\n" % (time.mktime(now.timetuple()))
				print msg
				sock.sendto(msg, (UDP_IP, UDP_PORT))
				prevTime = now
			line = localization_file.readline()
			if line == '':
				continue
			if 'stop' in json.loads(line):
				break
			print line
			sock.sendto(line.encode('utf-8'), (UDP_IP, UDP_PORT))
	except:
		print("Early Fail!")
		pass
	localization_file.close()
	sock.close()


if __name__ == '__main__':
	main()
	