#!/usr/bin/env python3

import socket
import argparse
import os
import json

if __name__ == '__main__':
	parser = argparse.ArgumentParser('TCP Server')
	parser.add_argument('data_dir', help='run directory')
	parser.add_argument('run_num', help='run_num', type=int)


	args = parser.parse_args()

	data_dir = args.data_dir
	run_num = args.run_num

	localization_file = open(os.path.join(data_dir, 'LOCALIZE_%06d' % (run_num)))

	TCP_IP = '127.0.0.1'

	TCP_PORT = 6001
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind((TCP_IP, TCP_PORT))
	sock.listen(1)

	print("Waiting for connection on port %d" % (TCP_PORT))

	conn, addr = sock.accept()


	try:
		while True:
			line = localization_file.readline()
			print(line)
			if line == '':
				continue
			if 'stop' in json.loads(line):
				break
			conn.sendall(line.encode('utf-8'))
	finally:
		localization_file.close()
		sock.close()
