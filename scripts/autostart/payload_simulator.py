#!/usr/bin/env python3
###############################################################################
#     Radio Collar Tracker Payload Simulator
#     Copyright (C) 2020  Nathan Hui
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################
#
# DATE        Name  Description
# -----------------------------------------------------------------------------
# 02/15/20    NH    Initial commit
#
###############################################################################

import argparse
import threading
import socket
import datetime as dt
import time
import json
from enum import Enum
import select
import traceback
import logging
import sys


class DroneSimulator:
    class SDR_STATES(Enum):
        find_devices = 0
        wait_recycle = 1
        usrp_probe = 2
        rdy = 3
        fail = 4

    class EXT_SENSOR_STATES(Enum):
        get_tty = 0
        get_msg = 1
        wait_recycle = 2
        rdy = 3
        fail = 4

    class STORAGE_STATES(Enum):
        get_output_dir = 0
        check_output_dir = 1
        check_space = 2
        wait_recycle = 3
        rdy = 4
        fail = 5

    class SYS_STATES(Enum):
        init = 0
        wait_init = 1
        wait_start = 2
        start = 3
        wait_end = 4
        finish = 5
        fail = 6

    class SW_STATES(Enum):
        stop = 0
        start = 1

    __SDR_STATES_POS = 0
    __STG_STATES_POS = 1
    __EXT_STATES_POS = 2
    __SYS_STATES_POS = 3
    __SWT_STATES_POS = 4

    def __init__(self, port=9000, target='255.255.255.255', log=False):
        self._port = port
        self._target = target
        self.__log = logging.getLogger('simulator.DroneSimulator')
        self.system_states = [None] * 5
        self.system_states[self.__SDR_STATES_POS] = self.SDR_STATES.find_devices
        self.system_states[self.__STG_STATES_POS] = self.STORAGE_STATES.get_output_dir
        self.system_states[self.__EXT_STATES_POS] = self.EXT_SENSOR_STATES.get_tty
        self.system_states[self.__SYS_STATES_POS] = self.SYS_STATES.init
        self.system_states[self.__SWT_STATES_POS] = self.SW_STATES.stop
        self.frequencies = []

        self.numErrors = 0
        self.numMsgSent = 0
        self.numMsgReceived = 0

        self.socketLock = threading.Lock()

        self.MENU = {
            'cmd': self._processCommand}

        self.CMD_MENU = {
            'getF': self._transmitFreqs,
        }

        self._socket = None

    def start(self):
        print("Simulator started")
        self.numErrors = 0
        self.numMsgSent = 0
        self.numMsgReceived = 0
        self._run = True
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self._socket.setblocking(0)
        self._socket.bind(("", self._port))
        self._senderThread = threading.Thread(target=self.__sender)
        self._receiveThread = threading.Thread(target=self.__receiver)
        self._senderThread.start()
        self._receiveThread.start()

    def stop(self):
        self._run = False
        self._senderThread.join()
        self._receiveThread.join()
        self._socket.close()
        self.__log.info("Stopped at %d" %
                        (dt.datetime.now().timestamp()))
        self.__log.info("Received %d messages" % self.numMsgReceived)
        self.__log.info("Sent %d messages" % self.numMsgSent)
        self.__log.info("%d Errors" % self.numErrors)
        print("Simulator stopped")

    def __sender(self):
        prevTime = dt.datetime.now()
        sendTarget = (self._target, self._port)

        while self._run is True:
            now = dt.datetime.now()

#             Heartbeat
            if (now - prevTime).total_seconds() > 1:
                heartbeatPacket = {}
                heartbeatPacket['heartbeat'] = {}
                heartbeatPacket['heartbeat']['time'] = now.timestamp()
                heartbeatPacket['heartbeat']['id'] = 'sim'
                status_string = "%d%d%d%d%d" % (self.system_states[0].value,
                                                self.system_states[1].value,
                                                self.system_states[2].value,
                                                self.system_states[3].value,
                                                self.system_states[4].value)
                heartbeatPacket['heartbeat']['status'] = status_string
                self.sendPacket(heartbeatPacket, sendTarget)
                prevTime = now
            time.sleep(0.5)

    def __receiver(self):
        ownIPs = getIPs()
        while self._run is True:
            ready = select.select([self._socket], [], [], 1)
            if ready[0]:
                with self.socketLock:
                    data, addr = self._socket.recvfrom(1024)
    #                 Ignore own IP
                if addr[0] in ownIPs:
                    continue
#                 Decode
                msg = data.decode()
                self.__log.info("Received %s at %.1f from %s" % (
                    msg, dt.datetime.now().timestamp(), addr))
                packet = dict(json.loads(msg))
                self.numMsgReceived += 1
                for key in packet.keys():
                    try:
                        self.MENU[key](packet[key], addr)
                    except Exception as e:
                        self.numErrors += 1
                        self.__log.exception(str(e))
                        errorPacket = {"exception": str(e),
                                       "traceback": traceback.format_exc()}
                        self.sendPacket(errorPacket, addr)

    def _processCommand(self, packet, addr):
        self.__log.info("Received Command Packet")
        try:
            self.CMD_MENU[packet['action']](packet, addr)
        except Exception as e:
            self.numErrors += 1
            self.__log.exception(str(e))
            errorPacket = {"exception": str(e),
                           "traceback": traceback.format_exc()}
            self.sendPacket(errorPacket, addr)

    def setSDRState(self, state):
        assert(isinstance(state, self.SDR_STATES))
        self.system_states[self.__SDR_STATES_POS] = state

    def setEXTState(self, state):
        assert(isinstance(state, self.EXT_SENSOR_STATES))
        self.system_states[self.__EXT_STATES_POS] = state

    def setSTGState(self, state):
        assert(isinstance(state, self.STORAGE_STATES))
        self.system_states[self.__STG_STATES_POS] = state

    def setSYSState(self, state):
        assert(isinstance(state, self.SYS_STATES))
        self.system_states[self.__SYS_STATES_POS] = state

    def setSWTState(self, state):
        assert(isinstance(state, self.SW_STATES))
        self.system_states[self.__SYS_STATES_POS] = state

    def _transmitFreqs(self, packet, addr):
        self.__log.info("Request to transmit frequencies")
        freqPacket = {"frequencies": self.frequencies}
        self.sendPacket(freqPacket, addr)

    def sendPacket(self, packet: dict, addr):
        msg = json.dumps(packet)
        with self.socketLock:
            self._socket.sendto(msg.encode(), addr)
        self.numMsgSent += 1
        self.__log.info("Sent %s at %.1f" % (
            msg, dt.datetime.now().timestamp()))

    def setFreqs(self, freqs):
        self.frequencies = freqs
        self.__log.info("Frequencies updated to %s" % freqs)


def getIPs():
    ip = set()

    ip.add(socket.gethostbyname_ex(socket.gethostname())[2][0])
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('8.8.8.8', 53))
    ip.add(s.getsockname()[0])
    s.close()
    return ip


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Radio Collar Tracker Payload Control Simulator')
    parser.add_argument('--port', type=int, default=9000)
    parser.add_argument('--target', type=str, default='255.255.255.255',
                        help='Target IP Address.  Use 255.255.255.255 for broadcast')
    args = parser.parse_args()
    logName = dt.datetime.now().strftime('%Y.%m.%d.%H.%M.%S.log')
    logger = logging.getLogger('simulator.DroneSimulator')
    ch = logging.StreamHandler(sys.stdout)
    ch.setLevel(logging.WARNING)
    formatter = logging.Formatter(
        '%(asctime)s.%(msecs)03d: %(levelname)s:%(name)s: %(message)s', datefmt='%Y-%M-%d %H:%m:%S')
    ch.setFormatter(formatter)
    logger.addHandler(ch)
    ch = logging.FileHandler(logName)
    ch.setLevel(logging.DEBUG)
    ch.setFormatter(formatter)
    simulator = DroneSimulator(args.port, args.target, True)

    simulator.start()
