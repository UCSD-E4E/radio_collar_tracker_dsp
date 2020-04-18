from app import *

sdr_object = sdrRecord()
#sdr_object.start()
#sdr_object.outputMonitoringThread()
sdr_object.configure(autostart=True, ping_width_ms=14, gps_mode=False)
#sdr_object.insert_config('autostart', 'True')
#sdr_object.insert_config('ping_width_ms', 14)
#sdr_object.insert_config('ping_width_ms', 15)
#sdr_object.insert_config('gps_mode', 'False')
