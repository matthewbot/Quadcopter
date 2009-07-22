#!/usr/bin/python

import wx

class MainFrame(wx.Frame):
	def __init__(self):
		wx.Frame.__init__(self, None, title='QuadCom', size=wx.Size(600, 500))
		
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
		
		self.gauge.SetValue(25)
		
		self.Write("Hello World!")
		
	def Write(self, txt):
		self.output.SetInsertionPointEnd()
		start = self.output.GetInsertionPoint()
		self.output.write(txt)
		end = self.output.GetInsertionPoint()
		self.output.SetStyle(start, end, wx.TextAttr(font = wx.Font(12, wx.FONTFAMILY_TELETYPE, 0, wx.FONTWEIGHT_NORMAL)))
		
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

