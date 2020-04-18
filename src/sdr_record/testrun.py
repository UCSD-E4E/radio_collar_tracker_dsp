from app import *

sdr_object = sdrRecord()
#sdr_object.outputMonitoringThread()
sdr_object.configure(gain=22.0, sampling_freq=1500000, center_freq=173000000, run_dir='./', run_num=1, rtt_loc='./')
sdr_object.start()
