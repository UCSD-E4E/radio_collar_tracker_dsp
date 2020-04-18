#!/usr/bin/env python3
###############################################################################
#     Radio Collar Tracker Ground Control Software
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
# 04/07/20    NH    Implemented initial class structure
#
###############################################################################

import subprocess
import os

class sdrRecord:
    def __init__(self, paramFile=None):
        self.__testConfig = False
        self.__samplingFreq = -1
        self.__centerFreq = -1
        self.__runNum = -1
        self.__runDir = None
        self.__testData = None

        self.__errorCallbacks = []

        if paramFile is None:
            #self.__paramFile = '/usr/local/etc/rct_config'
            self.__paramFile = 'rct_config'
        else:
            self.__paramFile = paramFile
        pass

    def start(self):
        '''
        '''
        if self.__testConfig:
            sdr_record_cmd = ('sdr_record -g %.1f ' % (self.__gain) +
                              '-s %d ' % (self.__samplingFreq) +
                              '-c %d ' % (self.__centerFreq) +
                              '-r %d ' % (self.__runNum) +
                              '-o %s ' % (self.__runDir) +
                              '--test_config ' +
                              '--test_data %s' % (self.__testData) +
                              '| tee -a rtt.log') #tee -a /var/log/rtt.log'

        else:
            sdr_record_cmd = ('sdr_record -g 22.0 ' +
                              '-s %d ' % (self.__samplingFreq) +
                              '-c %d ' % (self.__centerFreq) +
                              '-r %d ' % (self.__runNum) +
                              '-o %s ' % (self.__runDir) +
                              '| tee -a rtt.log')  # tee -a /var/log/rtt.log'

            #sdr_record_cmd = ('sdr_record -g 22.0 -s %d -c %d -r %d -o %s | tee -a rtt.log' % (self.__samplingFreq, self.__centerFreq, self.__runNum, self.__runDir))
        #self.__process = subprocess.Popen(sdr_record_cmd, shell=True)
        self.__process = subprocess.Popen('echo hello | tee -a rtt.log', shell=True)

        # start output monitoring thread

        pass

# def configure(self, gain=0.0, samplingFreq=0, centerFreq=0, runNum=0, runDir="", testConfig=False, testData=""):
# sdrRecord.configure(frequencies=[172350000, 173000000])
# sdrRecord.configure(gain=20.0)
    def configure(self, **kwargs):
        if "gps_target" in kwargs:
            # type checking the argument
            arg = kwargs['gps_target']
            if type(arg) is str:
                self.insert_config('gps_target', arg)
            else:
                print("Wrong argument type for gps_target\n")
        if "gps_baud" in kwargs:
            arg = kwargs['gps_baud']
            if type(arg) is int:
                self.insert_config('gps_baud', arg)
            else:
                print("Wrong argument type for gps_baud\n")
        if "gps_mode" in kwargs:
            arg = kwargs['gps_mode']
            if type(arg) is bool:
                self.insert_config('gps_mode', arg)
            else:
                print("Wrong argument type for gps_mode\n")
        if "ping_width_ms" in kwargs:
            arg = kwargs['ping_width_ms']
            if type(arg) is int:
                self.insert_config('ping_width_ms', arg)
            else:
                print("Wrong argument type for ping_width_ms\n")
        if "ping_max_len_mult" in kwargs:
            arg = kwargs['ping_max_len_mult']
            if type(arg) is float:
                self.insert_config('ping_max_len_mult', arg)
            else:
                print("Wrong argument type for ping_max_len_mult\n")
        if "ping_min_len_mult" in kwargs:
            arg = kwargs['ping_min_len_mult']
            if type(arg) is float:
                self.insert_config('ping_min_len_mult', arg)
            else:
                print("Wrong argument type for ping_min_len_mult\n")
        if "ping_min_snr" in kwargs:
            arg = kwargs['ping_min_snr']
            if type(arg) is float:
                self.insert_config('ping_min_snr', arg)
            else:
                print("Wrong argument type for ping_min_snr\n")
        if "sampling_freq" in kwargs:
            arg = kwargs['sampling_freq']
            if type(arg) is int:
                self.insert_config('sampling_freq', arg)
            else:
                print("Wrong argument type for sampling_freq\n")
        if "center_freq" in kwargs:
            arg = kwargs['center_freq']
            if type(arg) is int:
                self.insert_config('center_freq', arg)
            else:
                print("Wrong argument type for center_freq\n")
        if "output_dir" in kwargs:
            arg = kwargs['output_dir']
            if type(arg) is str:
                self.insert_config('output_dir', arg)
            else:
                print("Wrong argument type for output_dir\n")
        if "autostart" in kwargs:
            arg = kwargs['autostart']
            if type(arg) is bool:
                self.insert_config('autostart', arg)
            else:
                print("Wrong argument type for autostart\n")
        if "frequencies" in kwargs:
            arg = kwargs['frequencies']
            if type(arg) is int:
                self.insert_config('frequencies', arg)
            else:
                print("Wrong argument type for frequencies\n")

    def insert_config(self, param, value):
        lines = []
        found = False
        toAdd = str(param) + '=' + str(value) + '\n'
        with open(self.__paramFile, 'r') as configFile:
            lines = configFile.readlines()
            for i in range(len(lines)):
                line = lines[i]
                if not line.startswith('#'):
                    if param in line:
                        found = True
                        lines[i] = toAdd
            if not found:
                lines.append(toAdd)
        with open(self.__paramFile, 'w') as configFile:
            configFile.writelines(lines)

    def stop(self, timeout):
        if self.__process is not None:
            self.__process.send_signal(2)
            self.__process.wait(timeout=timeout)
            self.__process = None
        pass

    def registerErrorCallback(self, func):
        '''

        '''
        self.__errorCallbacks.append(func)
        pass

    def outputMonitoringThread(self):
        while True:
            # do output filtering for key words that might suggest payload has errored out
            with open('rtt.log', 'r') as log:
                last_line = log.readlines()[-1]
                if 'hello' in last_line:
                    print('found error!')
            # on error
            for func in self.__errorCallbacks:
                func()
