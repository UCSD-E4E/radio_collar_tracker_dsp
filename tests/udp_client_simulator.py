#!/usr/bin/python3
# coding: utf-8

# In[29]:


import socket
import argparse
import os
import json
import datetime
import time
import numpy as np
import utm

# In[5]:


pings = np.array([[0, 0, 30, 1],
                  [1, 0, 30, 0.5],
                  [-1, 0, 30, 0.5],
                  [0, 1, 30, 0.5],
                  [0, -1, 30, 0.5]])

x0 = np.array([0, 0, 6, 2])



# In[36]:


tx = (477954, 3638577, 11, 'S')
txp = 100
n = 4
k = 1
d = 1000

tx1 = (477854, 3638477, 11, 'S')


# In[20]:


UDP_PORT = 9000
UDP_IP = '255.255.255.255'
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
prevTime = datetime.datetime.now()


# In[ ]:


tx_loc = np.array([tx[0], tx[1], 0])
tx1_loc = np.array([tx1[0], tx1[1], 0])
dx_loc = np.array([tx[0] - 100, tx[1], 30])
vx = np.array([[5, 1, 0],
                [-1, 5, 0],
                [-5, -1, 0],
                [1, -5, 0]])
n1 = 30
c_loc = dx_loc
leg = 0
count = 0
while True:
    for i in range(n1):
        now = datetime.datetime.now()
        if (now - prevTime).total_seconds() > 1:
            packet = {}
            packet['heartbeat'] = {}
            packet['heartbeat']['time'] = time.mktime(now.timetuple())
            packet['heartbeat']['id'] = 'tmav'
            status_string = '%d%d%d%d%d' % (1, 2, 3, 4, 5)
            packet['heartbeat']['status'] = status_string
            msg = json.dumps(packet)
            print(msg.strip())
            sent = sock.sendto(msg.encode("UTF-8"), (UDP_IP, UDP_PORT))
            assert(sent == len(msg))
            prevTime = now
        c_loc += vx[leg,:]
        dist = np.linalg.norm(tx_loc - c_loc)
        dist1 = np.linalg.norm(tx1_loc - c_loc)
        R = txp - 10 * n * np.log10(dist) + k
        R1 = txp - 10 * n * np.log10(dist1) + k
        ll = utm.to_latlon(c_loc[0], c_loc[1], tx[2], tx[3])
        ll1 = utm.to_latlon(c_loc[0], c_loc[1], tx1[2], tx1[3])
        msg = '{"ping": {"time": %d, "lat": %d, "lon": %d, "alt": %.5f, "amp": %.5f, "txf": %d}}\n' % (time.mktime(now.timetuple()), ll[0] * 1e7, ll[1] * 1e7, dx_loc[2], R, 173965000)
        print(msg.strip())
        sent = sock.sendto(msg.encode("UTF-8"), (UDP_IP, UDP_PORT))
        assert(sent == len(msg))
        print(count)
        msg = '{"ping": {"time": %d, "lat": %d, "lon": %d, "alt": %.5f, "amp": %.5f, "txf": %d}}\n' % (time.mktime(now.timetuple()), ll1[0] * 1e7, ll1[1] * 1e7, dx_loc[2], R1, 173969000)
        print(msg.strip())
        sent = sock.sendto(msg.encode("UTF-8"), (UDP_IP, UDP_PORT))
        assert(sent == len(msg))
        print(count)
        
        count += 1
        time.sleep(1)
    leg = leg + 1
    if leg > 3:
	    c_loc = dx_loc
	    print("Position Reset!")
    leg = (leg) % 3


# In[ ]:




