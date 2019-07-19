#!/usr/bin/env python3

import socket
import argparse
import os
import json

def main():
	parser = argparse.ArgumentParser("Radio Telemetry Tracker Payload Receiver")

	args = parser.parse_args()

	UDP_PORT = 9000
	BUFFER_LEN = 1024

	sock = socket.socket(socket.AF_INET, sock.SOCK_DGRAM)
	sock.bind('', UDP_PORT)

	try:
		while True:
			data, addr = sock.recvfrom(BUFFER_LEN)
			print(data)

if __name__ == '__main__':
	main()