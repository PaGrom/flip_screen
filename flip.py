#!/usr/bin/env python
# -*- coding: utf-8 -*-

from thserial import ThSerial
import os
import datetime

port=ThSerial("/dev/ttyS0", 9600, 5)
print datetime.datetime.now(), "Connected"

word = 'habrahabr'

port.write(word)
signal=port.read(9)
if len(signal)==len(word):
    vert=True
    os.popen('xrandr -o left')
else:
    vert=False
    os.popen('xrandr -o normal')
print datetime.datetime.now(), "first Vert ==",vert
while True:
    #print vert
    port.write(word)
    signal=port.read(len(word))
    print signal
    if len(signal)==len(word):
        if vert==False:
            print datetime.datetime.now(), 'перевернуть вертикально'
            
            os.popen('xrandr -o left')
            vert=True
    else:
        if vert==True:
            print datetime.datetime.now(), 'перевернуть горизонтально'
            
            os.popen('xrandr -o normal')
            vert=False
    print datetime.datetime.now(), "Vert ==",vert
