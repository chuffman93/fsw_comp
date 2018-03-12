#!/usr/bin/env python

import struct
import os
import time
import numpy as np

class DITLSch:
	def __init__(self,m,e,d):
		self.mode = struct.pack(">B",m)
		self.epoch = struct.pack(">I",e)
		self.duration = struct.pack(">I",d)



def createSchedule(nextSchedule):
	F = open("SCH",mode = "wb")
	for x in nextSchedule:
		F.write(x.mode)
		F.write(x.epoch)
		F.write(x.duration)
	F.close()

def createNDSchedule(nextSchedule):
	F = open("NDSCH",mode = "wb")
	for x in nextSchedule:
		F.write(x.mode)
		F.write(x.epoch)
		F.write(x.duration)
	F.close()

def createIEF(nextIEF):
	F = open("IEF.txt","w")
	F.write(nextIEF)
	F.close()

def sendSOT():
	F = open("SOT","wb")
	buff = []
	F.write(struct.pack('B',0))
	F.close()
	os.rename("SOT","./opt/uplink/SOT")


def sendIEF():
	os.rename("IEF.txt","./opt/uplink/IEF.txt")

def sendSchedule():
	os.rename("SCH","./opt/uplink/SCH")

def sendGPSConfig():
	os.rename("ConfigUpGPS","./opt/uplink/ConfigUpGPS")






print("----------Running Integrated Software Ground Simulation----------")

nextSchedule = []
nextSchedule.append(DITLSch(2,20,20))
nextSchedule.append(DITLSch(2,40,20))
nextSchedule.append(DITLSch(2,100,20))

'''
if not os.path.exists('./home/.uplink/DSCH'):
	newDefault = []
	newDefault.append(DITLSch(2,60,60))
	newDefault.append(DITLSch(3,60,60))
	newDefault.append(DITLSch(2,60,60))
	createNDSchedule(newDefault)
	os.rename("NDSCH","./home/.uplink/NDSCH")

'''




for i in nextSchedule:
	print("Schedule is Mode: %u TSE: %u Duration: %u" %(struct.unpack(">B",i.mode)[0],struct.unpack(">I",i.epoch)[0],struct.unpack(">I",i.duration)[0]))
createSchedule(nextSchedule)
createIEF("")
of = open("ConfigUpGPS",'wb')
timeout = np.uint16(600)
timein = np.uint16(7200)
of.write(timeout)
of.write(timein)
of.close()

print("----------Sending Next Schedule for Payload Mode----------")
sendSOT()
time.sleep(5)
sendSchedule()
sendGPSConfig()
time.sleep(15)
sendIEF()
