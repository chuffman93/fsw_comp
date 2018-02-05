#!/usr/bin/env python

import struct
import os
import time

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
	os.rename("SOT","./home/.uplink/SOT")


def sendIEF():
	os.rename("IEF.txt","./home/.uplink/IEF.txt")

def sendSchedule():
	os.rename("SCH","./home/.uplink/SCH")






print("----------Running Integrated Software Ground Simulation----------")

nextSchedule = []

nextSchedule.append(DITLSch(2,60,120))
nextSchedule.append(DITLSch(3,120,120))
nextSchedule.append(DITLSch(2,180,120))

if not os.path.exists('./home/.uplink/DSCH'):
	newDefault = []
	newDefault.append(DITLSch(2,60,60))
	newDefault.append(DITLSch(3,60,60))
	newDefault.append(DITLSch(2,60,60))
	createNDSchedule(newDefault)
	os.rename("NDSCH","./home/.uplink/NDSCH")





for i in nextSchedule:
	print("Schedule is Mode: %u TSE: %u Duration: %u" %(struct.unpack(">B",i.mode)[0],struct.unpack(">I",i.epoch)[0],struct.unpack(">I",i.duration)[0]))
createSchedule(nextSchedule)
createIEF("")

print("----------Sending Next Schedule for Payload Mode----------")
sendSOT()
sendSchedule()
sendIEF()
