import serial
import fcntl

SIOCDEVPRIVATE = 0x89F0
CP210x_IOCTL_GPIOSET = SIOCDEVPRIVATE + 1

CP210x_CFG_GPIO_3 = 0x0800
CP210x_CFG_GPIO_2 = 0x0400
CP210x_CFG_GPIO_1 = 0x0200
CP210x_CFG_GPIO_0 = 0x0100

class CP2103(serial.Serial):
	def set_gpios(self, gpio0=False, gpio1=False, gpio2=False, gpio3=False):
		fd = self.fileno()
		
		bitset = 0
		if gpio0:
			bitset |= (1 << 0)
		if gpio1:
			bitset |= (1 << 1)
		if gpio2:
			bitset |= (1 << 2)
		if gpio3:
			bitset |= (1 << 3)
			
		fcntl.ioctl(fd, CP210x_IOCTL_GPIOSET, bitset)
		
