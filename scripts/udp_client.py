#!/usr/bin/env python

import socket
import argparse
import os
import json

def main():
	parser = argparse.ArgumentParser("UDP Client")
	parser.add_argument('data_dir', help='run directory')
	parser.add_argument('run_num', help='run_num', type=int)

	args = parser.parse_args()

	data_dir = args.data_dir
	run_num = args.run_num

	localization_file = open(os.path.join(data_dir, 'LOCALIZE_%06d' % (run_num)))

	UDP_PORT = 9000

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

	try:
		while True:
			line = localization_file.readline()
			if line == '':
				continue
			if 'stop' in json.loads(line):
				break
			sock.sendto(line.encode('utf-8'), ('255.255.255.255', UDP_PORT))
	except:
		pass
	localization_file.close()
	sock.close()


if __name__ == '__main__':
	main()
	