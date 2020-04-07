#!/usr/bin/env python3

import socket
import os
import json
import datetime
import time
import threading
import select
import subprocess
import sys
import glob
import traceback


class RCTOpts(object):
    def __init__(self):
        self._configFile = '&INSTALL_PREFIX/etc/rct_config'
        self.options = ['ping_width_ms',
                        'ping_min_snr',
                        'ping_max_len_mult',
                        'ping_min_len_mult',
                        'gps_mode',
                        'gps_target',
                        'gps_baud',
                        'frequencies',
                        'autostart',
                        'output_dir',
                        'sampling_freq',
                        'center_freq']
        self._params = {key: self.get_var(key) for key in self.options}

    def get_var(self, var):
        retval = []
        with open(self._configFile) as var_file:
            for line in var_file:
                if line.split('=')[0].strip() == var:
                    retval.append(line.split(
                        '=')[1].strip().strip('"').strip("'"))
        return retval

    def loadParams(self):
        self._params = {key: self._get_var(key) for key in self.options}

    def getOption(self, option):
        return self._params[option]

    def setOption(self, option, param):
        if option == 'ping_width_ms':
            assert(isinstance(param, str))
            test = float(param)
            assert(test > 0)
        elif option == 'ping_min_snr':
            assert(isinstance(param, str))
            test = float(param)
            assert(test > 0)
        elif option == 'ping_max_len_mult':
            assert(isinstance(param, str))
            test = float(param)
            assert(test > 1)
        elif option == 'ping_min_len_mult':
            assert(isinstance(param, str))
            test = float(param)
            assert(test < 1)
            assert(test > 0)
        elif option == 'gps_mode':
            assert(isinstance(param, str))
            assert(param == 'true' or param == 'false')
        elif option == 'gps_target':
            assert(isinstance(param, str))
        elif option == 'gps_baud':
            assert(isinstance(param, str))
            test = int(param)
            assert(param > 0)
        elif option == 'frequencies':
            assert(isinstance(param, list))
            assert(all(isinstance(freq, int) and freq > 0 for freq in param))
        elif option == 'autostart':
            assert(isinstance(param, str))
            assert(param == 'true' or param == 'false')
        elif option == 'output_dir':
            assert(isinstance(param, str))
        elif option == 'sampling_freq':
            assert(isinstance(param, str))
            test = int(param)
            assert(test > 0)
        elif option == 'center_freq':
            assert(isinstance(param, str))
            test = int(param)
            assert(test > 0)
        self._params[option] = param

    def setOptions(self, options):
        # Error check first before committing
        for key, value in options.items():
            print("Option: ")
            print(key)
            print(value)
            if key == 'ping_width_ms':
                assert(isinstance(value, str))
                test = float(value)
                assert(test > 0)
            elif key == 'ping_min_snr':
                assert(isinstance(value, str))
                test = float(value)
                assert(test > 0)
            elif key == 'ping_max_len_mult':
                assert(isinstance(value, str))
                test = float(value)
                assert(test > 1)
            elif key == 'ping_min_len_mult':
                assert(isinstance(value, str))
                test = float(value)
                assert(test < 1)
                assert(test > 0)
            elif key == 'gps_mode':
                assert(isinstance(value, str))
                assert(value == 'true' or value == 'false')
            elif key == 'gps_target':
                assert(isinstance(value, str))
            elif key == 'gps_baud':
                assert(isinstance(value, str))
                test = int(value)
                assert(value > 0)
            elif key == 'frequencies':
                assert(isinstance(value, list))
                assert(all(isinstance(freq, int) and freq > 0 for freq in param))
            elif key == 'autostart':
                assert(isinstance(value, str))
                assert(value == 'true' or value == 'false')
            elif key == 'output_dir':
                assert(isinstance(value, str))
            elif key == 'sampling_freq':
                assert(isinstance(value, str))
                test = int(value)
                assert(test > 0)
            elif key == 'center_freq':
                assert(isinstance(value, str))
                test = int(value)
                assert(test > 0)

        for key, value in options.items():
            if isinstance(value, list):
                self._params[key] = value
            else:
                self._params[key] = [value]

    def writeOptions(self):
        backups = glob.glob("&INSTALL_PREFIX/etc/*.bak")
        if len(backups) > 0:
            backup_numbers = [os.path.basename(path).split(
                '.')[0].lstrip('rct_config') for path in backups]
            backup_numbers = [int(number)
                              for number in backup_numbers if number != '']
            nextNumber = max(backup_numbers) + 1
        else:
            nextNumber = 1

        os.rename('&INSTALL_PREFIX/etc/rct_config',
                  '&INSTALL_PREFIX/etc/rct_config%d.bak' % nextNumber)

        with open(self._configFile, 'w') as var_file:
            for key, value in list(self._params.items()):
                for val in value:
                    opt = '%s=%s\n' % (key, val)
                    print(opt.strip())
                    var_file.write(opt)

    def getAllOptions(self):
        return self._params


class CommandListener(object):
    """docstring for CommandListener"""

    def __init__(self, memoryMap, switchOffset):
        super(CommandListener, self).__init__()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.sock.setblocking(0)
        self.port = 9000
        self.target_ip = '255.255.255.255'
        self.ping_file = None
        self.num = None
        self.newRun = False
        self._run = True
        self.sender = threading.Thread(target=self._sender)
        self.receiver = threading.Thread(target=self._listener)
        self.startFlag = False
        self.sharedStates = memoryMap
        self.startOffset = switchOffset
        self.sharedStates[self.startOffset] = False

        self._options = RCTOpts()

        self.sender.start()
        self.receiver.start()

    def __del__(self):
        self._run = False
        self.sender.join()
        self.sock.close()
        if self.ping_file is not None:
            self.ping_file.close()
            print('Closing file')

    def stop(self):
        self._run = False
        self.sender.join()
        self.sharedStates[self.startOffset] = False

    def setRun(self, runDir, runNum):
        self.newRun = True
        if self.ping_file is not None:
            self.ping_file.close()
            print('Closing file')
        path = os.path.join(runDir, 'LOCALIZE_%06d' % (runNum))
        if os.path.isfile(path):
            self.ping_file = open(path)
            print("Set and open file to %s" %
                  (os.path.join(runDir, 'LOCALIZE_%06d' % (runNum))))
        else:
            raise Exception("File non existent!")

    def getStartFlag(self):
        return self.startFlag

    def _sender(self):
        prevTime = datetime.datetime.now()
        sendTarget = (self.target_ip, self.port)

        while self._run:
            try:
                now = datetime.datetime.now()
                if (now - prevTime).total_seconds() > 1:
                    heartbeatPacket = {}
                    heartbeatPacket['heartbeat'] = {}
                    heartbeatPacket['heartbeat']['time'] = time.mktime(
                        now.timetuple())
                    heartbeatPacket['heartbeat']['id'] = 'mav'
                    status_string = "%d%d%d%d%d" % (self.sharedStates[0],
                                                    self.sharedStates[1], self.sharedStates[2],
                                                    self.sharedStates[3], self.sharedStates[4])
                    heartbeatPacket['heartbeat']['status'] = status_string
                    msg = json.dumps(heartbeatPacket)
                    self.sock.sendto(msg.encode('utf-8'), sendTarget)
                    prevTime = now

                if self.ping_file is not None:
                    line = self.ping_file.readline()
                    if line == '':
                        continue
                    if 'stop' in json.loads(line):
                        print('Got stop')
                    # 	break
                    self.sock.sendto(line.encode('utf-8'), sendTarget)
            except Exception as e:
                print("Early Fail!")
                print(e)
                continue

    def _gotStartCmd(self, commandPacket, addr):
        self.startFlag = True
        self.sharedStates[self.startOffset] = True
        print("Set start flag")

    def _gotStopCmd(self, commandPacket, addr):
        self.startFlag = False
        self.sharedStates[self.startOffset] = False
        try:
            self.ping_file.close()
        except Exception as e:
            print(e)
        self.ping_file = None

    def _gotSetFCmd(self, commandPacket, addr):
        if 'frequencies' not in commandPacket:
            return
        freqs = commandPacket['frequencies']
        self._options.setOption('frequencies', freqs)
        self._options.writeOptions()
        packet = {}
        packet['frequencies'] = freqs
        msg = json.dumps(packet)
        self.sock.sendto(msg.encode('utf-8'), addr)

    def _gotGetFCmd(self, commandPacket, addr):
        freqs = self._options.getOption('frequencies')
        packet = {}
        packet['frequencies'] = freqs
        msg = json.dumps(packet)
        self.sock.sendto(msg.encode('utf-8'), addr)

    def _gotGetOptsCmd(self, commandPacket, addr):
        opts = self._options.getAllOptions()
        packet = {}
        packet['options'] = opts
        msg = json.dumps(packet)
        self.sock.sendto(msg.encode('utf-8'), addr)

    def _gotWriteOptsCmd(self, commandPacket, addr):
        if 'confirm' not in commandPacket:
            return
        if commandPacket['confirm'] == 'true':
            self._options.writeOptions()
            print("Writing params")
        else:
            # load from backup
            self._options.loadParams()
            print('Reloading params')

    def _gotSetOptsCmd(self, commandPacket, addr):
        if 'options' not in commandPacket:
            return
        opts = commandPacket['options']
        self._options.setOptions(opts)
        packet = {}
        packet['options_readback'] = self._options.getAllOptions()
        msg = json.dumps(packet)
        print(msg)
        self.sock.sendto(msg.encode('utf-8'), addr)

    def _upgradeCmd(self, commandPacket, addr):
        packet = {}
        packet['upgrade_ready'] = "true"
        msg = json.dumps(packet)
        self.sock.sendto(msg.encode('utf-8'), addr)

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        host = socket.gethostname()
        sock.settimeout(10)
        port = 9500
        sock.bind(('', port))
        byteCounter = 0
        sock.listen(1)
        try:
            conn, tcp_addr = sock.accept()

            with open('/tmp/upgrade.zip', 'wb') as archiveFile:
                frame = conn.recv(1024)
                byteCounter += len(frame)
                while frame:
                    archiveFile.write(frame)
                    frame = conn.recv(1024)
                    byteCounter += len(frame)
        except:
            return
        conn.close()
        print("Received %d bytes" % byteCounter)
        statusPacket = {}
        statusPacket['upgrade_status'] = "Received %d bytes" % (byteCounter)
        msg = json.dumps(statusPacket)
        self.sock.sendto(msg.encode('utf-8'), addr)

        try:
            retval = subprocess.call(
                'unzip -u -o /tmp/upgrade.zip -d /tmp', shell=True)
            assert(retval == 0)
            statusPacket['upgrade_status'] = "Unzipped"
            msg = json.dumps(statusPacket)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)
            self.sock.sendto(msg.encode('utf-8'), addr)

            retval = subprocess.call(
                './autogen.sh', shell=True, cwd='/tmp/radio_collar_tracker_drone-online_proc')
            assert(retval == 0)
            statusPacket['upgrade_status'] = "autogen complete"
            msg = json.dumps(statusPacket)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)

            retval = subprocess.call(
                './configure', shell=True, cwd='/tmp/radio_collar_tracker_drone-online_proc')
            assert(retval == 0)
            statusPacket['upgrade_status'] = "configure complete"
            msg = json.dumps(statusPacket)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)

            retval = subprocess.call(
                'make', shell=True, cwd='/tmp/radio_collar_tracker_drone-online_proc')
            assert(retval == 0)
            statusPacket['upgrade_status'] = "Make complete"
            msg = json.dumps(statusPacket)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)

            retval = subprocess.call(
                'make install', shell=True, cwd='/tmp/radio_collar_tracker_drone-online_proc')
            assert(retval == 0)
            statusPacket['upgrade_status'] = "Make installed"
            msg = json.dumps(statusPacket)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)

            packet = {}
            packet['upgrade_complete'] = 'true'
            msg = json.dumps(packet)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)

            print("I was run with: ")
            print(sys.argv)
            print(sys.argv[1:])
            os.execv(sys.argv[0], sys.argv)

        except Exception as e:
            packet = {}
            packet['upgrade_complete'] = 'false'
            packet['reason'] = str(e)

            msg = json.dumps(packet)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)

    def _processCommand(self, commandPacket, addr):
        commands = {
            'test': lambda: None,
            'start': self._gotStartCmd,
            'stop': self._gotStopCmd,
            'setF': self._gotSetFCmd,
            'getF': self._gotGetFCmd,
            'getOpts': self._gotGetOptsCmd,
            'setOpts': self._gotSetOptsCmd,
            'writeOpts': self._gotWriteOptsCmd,
            'upgrade': self._upgradeCmd
        }

        print('Got action: %s' % (commandPacket['action']))

        try:
            commands[commandPacket['action']](commandPacket, addr)
        except Exception as e:
            print(e)
            packet = {}
            packet['exception'] = str(e)
            packet['traceback'] = traceback.format_exc()
            msg = json.dumps(packet)
            print(msg)
            self.sock.sendto(msg.encode('utf-8'), addr)

    def _listener(self):

        self.sock.bind(("", self.port))

        while self._run:
            ready = select.select([self.sock], [], [], 1)
            if ready[0]:
                data, addr = self.sock.recvfrom(1024)
                msg = data.decode('utf-8')
                packet = json.loads(msg)
                if 'cmd' in packet:
                    print(packet['cmd']['action'])
                    self._processCommand(packet['cmd'], addr)
