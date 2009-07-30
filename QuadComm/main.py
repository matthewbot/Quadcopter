#!/usr/bin/python

import wx
import serialthread
import bootloader
import string
import threading

class MainFrame(wx.Frame):
	def __init__(self):
		wx.Frame.__init__(self, None, title='QuadComm', size=wx.Size(600, 500))
		
		panel = wx.Panel(self, -1)
		bottom_buttons = wx.Panel(panel, -1)
		
		self.sb = self.CreateStatusBar()
		self.output = wx.TextCtrl(panel, -1, style=wx.TE_MULTILINE)
		self.flash = wx.Button(bottom_buttons, -1, 'Flash')
		self.gauge = wx.Gauge(bottom_buttons, -1, 50)	
		
		panel_box = wx.BoxSizer(wx.VERTICAL)
		panel.SetSizer(panel_box)
		bottom_buttons_box = wx.BoxSizer(wx.HORIZONTAL)
		bottom_buttons.SetSizer(bottom_buttons_box)
		
		panel_box.Add(self.output, 1, wx.EXPAND)
		panel_box.Add(bottom_buttons, 0, wx.EXPAND)

		bottom_buttons_box.Add(self.gauge, 1, wx.EXPAND)
		bottom_buttons_box.Add(self.flash, 0)
		
		self.flash.Bind(wx.EVT_BUTTON, self.OnFlash)
		self.output.Bind(wx.EVT_CHAR, self.OnKeyChar)
		self.output.SetFocus()
		
		self.StartIOThread()
		
	def StartIOThread(self):
		self.iothread = serialthread.SerialIOThread("/dev/ttyS0", 9600, self.WriteCallback)
		
	def WriteCallback(self, txt, kind="output"):
		wx.CallAfter(self.Write, txt, kind)
		
	def Write(self, txt, kind="output"):
		self.output.SetInsertionPointEnd()
		start = self.output.GetInsertionPoint()
		self.output.write(unicode(txt, 'ascii', errors="replace"))
		end = self.output.GetInsertionPoint()
		
		if kind == "input":
			color = 'BLUE'
		elif kind == "error" or kind == "system":
			color = 'RED'
		else:
			color = 'BLACK'
		
		self.output.SetStyle(start, end, wx.TextAttr(color, font = wx.Font(12, wx.FONTFAMILY_TELETYPE, 0, wx.FONTWEIGHT_NORMAL)))
		
	def OnKeyChar(self, event):
		if self.iothread != None:
			char = chr(event.GetKeyCode())
			self.Write(char, "input")
			self.iothread.Write(char)
		event.StopPropagation()
		
	def OnFlash(self, event):
		self.Write("\nFlashing...\n", "system")
		
		self.flashthread = threading.Thread(target=self.DoFlash, name="Flash Thread")
		self.flashthread.daemon = True
		self.flashthread.start()
		
	def DoFlash(self):
		self.iothread.Close()
		self.iothread = None
		
		def callback(mode, pos, tot):
			self.gauge.SetRange(tot)
			self.gauge.SetValue(pos)
		
		def callbackwrap(mode, pos, tot):
			wx.CallAfter(callback, mode, pos, tot)
		
		result = False
		try:
			result = bootloader.write("../FC/out/FC.bin", "/dev/ttyS0", callback=callbackwrap, go=True)
		except Exception as err:
			result = err
		
		if result == True:
			self.WriteCallback("Flash complete!\n", "system")
		elif result == False:
			self.WriteCallback("Flash verification error\n", "error")
		else:
			self.WriteCallback("Flash exception: " + str(result) + "\n", "error")
			
		self.StartIOThread()
		
class QuadCommApp(wx.App):
	def OnInit(self):
		frame = MainFrame()
		frame.Show()
		return True
		
def main():
	app = QuadCommApp()
	app.MainLoop()
	
if __name__ == '__main__':
	main()

