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


class sdrRecord():
    def __init__(self, paramFile=None):
        self.__testConfig = False
        self.__samplingFreq = None
        self.__centerFreq = None
        self.__runNum = None
        self.__runDir = None
        self.__testData = None

        self.__errorCallbacks = []

        if paramFile is None:
            self.__paramFile = '/usr/local/etc/rct_config'
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
                              '| tee -a /var/log/rtt.log')
        else:
            sdr_record_cmd = ('sdr_record -g 22.0 -s %d -c %d -r %d -o %s | tee -a /var/log/rtt.log' %
                              (self.__samplingFreq, self.__centerFreq, self.__runNum, self.__runDir))
        self.__process = subprocess.Popen(sdr_record_cmd, shell=True)

        # start output monitoring thread

        pass

#    def configure(self, gain=0.0, samplingFreq=0, centerFreq=0, runNum=0, runDir="", testConfig=False, testData=""):
# sdrRecord.configure(frequencies=[172350000, 173000000])
# sdrRecord.configure(gain=20.0)
    def configure(self, **kwargs):
        pass

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

    def __outputMonitoringThread(self):
        while True:
            # on error
            for func in self.__errorCallbacks:
                func()
