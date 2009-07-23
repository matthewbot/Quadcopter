#!/usr/bin/python

import serial
import string

ACK = 0x79
NACK = 0x1F
CMD_GET = 0x00
CMD_GET_VERSION = 0x01
CMD_GET_ID = 0x02
CMD_READ_MEMORY = 0x11
CMD_GO = 0x21
CMD_WRITE_MEMORY = 0x31
CMD_ERASE_MEMORY = 0x43
CMD_READOUT_UNPROTECT = 0x92

class BootloaderConnection:
	def __init__(self, port, baud=115200):
		self.serial = serial.Serial(port, baud, parity=serial.PARITY_EVEN, timeout=.1)
		self.Sync()
		
	def Close(self):
		self.serial.close()
		
	# The Sync function attempts to sync.
	# It attempts multiple times, useful if board has already been started, 
	# because it will show up as an invalid command and we get a NACK
		
	def Sync(self, maxcount=6):
		for count in range(maxcount):
			try:
				self.serial.write("\x7F")
				byte = ord(self.ReadByte())
				if byte == ACK or byte == NACK:
					return True
			except:
				pass
			
		raise RuntimeError("Failed to sync with board")
		
	def Get(self):
		self.SendCommand(CMD_GET, "get command")
		length = ord(self.ReadByte())
		versionnum = ord(self.ReadByte())
		versionstr = self.VersionNumToString(versionnum)
		commands = map(ord, self.ReadBytes(length))
		self.ExpectAck("get command termination")
		
		return (versionstr, versionnum, commands)

	def GetVersion(self):
		self.SendCommand(CMD_GET_VERSION, "get version command")
		versionnum = ord(self.ReadByte())
		versionstr = self.VersionNumToString(versionnum)
		nulls = self.ReadBytes(2)
		self.ExpectAck("get version command termination")
		
		return (versionstr, versionnum)

	def GetID(self):
		self.SendCommand(CMD_GET_ID, "get id command")
		length = ord(self.ReadByte())
		pidbytes = self.ReadBytes(length+1)
		pid = ord(pidbytes[0]) << 8 | ord(pidbytes[1])
		pidstr = "0x%04X" % pid
		self.ExpectAck("get id command termination")
		
		return (pidstr, pid)

	def LongReadMemory(self, addr, count, callback=None):
		def readcallback(p, l):
			if callback != None:
				callback("read", p, count)
			return self.ReadMemory(addr + p, l)
	
		data = ''
		pos = 0
		while pos < count-256:
			data = data + readcallback(pos, 256)
			pos = pos + 256
			
		data = data + readcallback(pos, count-pos)
		
		return data

	def ReadMemory(self, addr, count=256):
		self.SendCommand(CMD_READ_MEMORY, "read memory command")
		self.SendAddress(addr)
		self.ExpectAck("read memory address")
		self.SendByteChecksummed(chr(count-1))
		self.ExpectAck("read memory length")
		
		return self.ReadBytes(count)
		
	def LongWriteMemory(self, addr, data, callback=None):
		def writecallback(p, d):
			if callback != None:
				callback("write", p, len(data))
			self.WriteMemory(addr + p, d)
	
		pos = 0
		while pos < len(data)-256:
			writecallback(pos, data[pos:pos+256])
			pos = pos + 256
			
		writecallback(pos, data[pos:])
		
	def WriteMemory(self, addr, data):
		count = len(data)
		assert(count <= 256)
		self.SendCommand(CMD_WRITE_MEMORY, "write memory command 0x%08X count %d" % (addr, count))
		self.SendAddress(addr)
		self.ExpectAck("write memory address 0x%08X count %d" % (addr, count))
		self.SendBytesChecksummed(chr(count-1) + data)
		self.ExpectAck("write memory data 0x%08X count %d" % (addr, count))
		
	def EraseMemory(self):
		self.SendCommand(CMD_ERASE_MEMORY, "erase memory command")
		self.SendByteChecksummed(chr(0xFF))
		self.ExpectAck("erase memory global erase")

	def ReadoutUnprotect(self):
		self.SendCommand(CMD_READOUT_UNPROTECT, "readout unprotect command")
		self.ExpectAck("readout unprotect command success")
		self.Sync()
		
	def Go(self, addr):
		self.SendCommand(CMD_GO, "go command")
		self.SendAddress(addr)
		self.ExpectAck("go command address")

	def SendAddress(self, addr):
		byte3 = (addr >> 0) & 0xFF
		byte2 = (addr >> 8) & 0xFF
		byte1 = (addr >> 16) & 0xFF
		byte0 = (addr >> 24) & 0xFF
		crc = byte0 ^ byte1 ^ byte2 ^ byte3
		bytes = chr(byte0) + chr(byte1) + chr(byte2) + chr(byte3) + chr(crc)
		self.serial.write(bytes)

	def SendCommand(self, cmd, msg=None):
		self.SendByteChecksummed(chr(cmd))
		self.ExpectAck(msg or "command 0x%02X" % cmd)
		
	def SendByteChecksummed(self, byte):
		self.serial.write(byte + chr(ord(byte) ^ 0xFF))
		
	def SendBytesChecksummed(self, bytes):
		crc = 0
		for byte in bytes:
			crc = crc ^ ord(byte)
		self.serial.write(bytes + chr(crc))
		
	def ExpectAck(self, msg):
		byte = ord(self.ReadByte())
		
		if byte == ACK:
			return True
		elif byte == NACK:
			raise RuntimeError("Got nack in reply to " + msg)
		else:
			raise RuntimeError("Got unknown response (0x" + hex(byte) + ") in reply to " + msg)

	def ReadByte(self):
		return self.ReadBytes(1)

	def ReadBytes(self, num):
		try:
			bytes = ''
		
			while len(bytes) < num:
				got = self.serial.read(num - len(bytes))
				if got == '':
					raise RuntimeError("Timeout while reading serial port!")
				bytes += got
				
			return bytes
		except:
			raise RuntimeError("Error while reading serial port!")
			
	def VersionNumToString(self, versionnum):
		return "%x.%x" % (versionnum >> 4, versionnum & 0x0F)
		
def write(filename, port, verify=True, go=False, callback=None):
	data = file(filename).read()
	conn = BootloaderConnection(port)
	
	versionstr, version = conn.GetVersion()
	if version > 0x22:
		raise RuntimeError("Unsupported bootloader version " + versionstr)
	
	conn.EraseMemory()
	conn.LongWriteMemory(0x08000000, data, callback)
	
	if verify:
		dataverify = conn.LongReadMemory(0x08000000, len(data), callback)
		retval = (dataverify == data)
	else:
		retval = True
		
	if go:
		conn.Go(0x08000000)
		
	conn.Close()
	callback("complete", 0, 1)
	
	return retval
	
if __name__ == '__main__':
	import sys
			
	def maincallback(mode, pos, tot):
		if pos != 0:
			return
		
		if mode == "write":
			print "Writing flash"
		elif mode == "read":
			print "Verifying flash"

	if write(sys.argv[1], "/dev/ttyS0", callback=maincallback, go=True):
		print("Done!")
	else:
		print("VERIFY FAILED")
			
	
