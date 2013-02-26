# -*- coding: utf-8 -*-

import sys, os
import serial
import threading
import time


class ThSerial:
    """
    Serial port daemon class.
    """
    
    def __init__(self, port, baudrate, timeout):
        try:
            self.serial = serial.Serial(port, baudrate)
        except serial.SerialException, e:
            sys.stderr.write(u"Не удается открыть порт %s.\n" % port.encode('cp866'))
            sys.exit(1)
            
        self.in_data=[]
        self.out_data=[]
        self.timeout = timeout
        self.open()
    
    def open(self):
        self.alive = True
        self.receiver_thread = threading.Thread(target=self.reader)
        self.receiver_thread.setDaemon(1)
        self.receiver_thread.start()
        self.transmitter_thread = threading.Thread(target=self.writer)
        self.transmitter_thread.setDaemon(1)
        self.transmitter_thread.start()

    def close(self):
        self.alive = False
        self.transmitter_thread.join(1)
        self.receiver_thread.join(1)
        self.serial.close()

    def reader(self):
        while self.alive:
            data = self.serial.read(1)
            for character in data:
                self.in_data.append(character)
                
    def writer(self):
        while self.alive:
            if len(self.out_data) > 0:
                character = self.out_data.pop(0)
                self.serial.write(character)
            
    def read(self, num):
        line = ""
        while (num > 0):
            new_char = None
            t = time.clock()
            while (time.clock() - t) < self.timeout:
                try:
                    new_char = self.in_data.pop(0)
                    break
                except:
                    pass
            if new_char is None:
                return line
            
            line += new_char
            num = num - 1
        
        return line
        
    def readline(self):
        line = ""
        while True:
            character = self.read(1)
            if (character is None) or (len(character) == 0):
                return line
            elif character == '\n':
                line += character
                break
            else:
                line += character
                
        return line
        
    def write(self, line):
        for character in line:
            self.out_data.append(character)

