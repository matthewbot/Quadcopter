#!/usr/bin/python

import wx
import time

class Grapher(wx.Panel):
	def __init__(self, parent, id, yscale_auto_callback=None, sampletime_callback=None, sample_count=150):
		wx.Panel.__init__(self, parent, id)
		
		self.sample_count = sample_count
		self.outputbuf = ''
		self.graphsamples = []
		self.yscale_max = 1
		self.yscale_min = -1
		self.yscale_auto = True
		self.yscale_auto_callback = yscale_auto_callback
		
		self.sampletime = 0
		self.sampletime_callback = sampletime_callback
		self.lastsampletime = time.time()
		
		self.Bind(wx.EVT_PAINT, self.OnPaint)
		
	def SetYScale(self, yscale_auto=None, yscale_auto_callback=None, yscale_min=None, yscale_max=None):
		if yscale_auto != None:
			self.yscale_auto = yscale_auto
		if yscale_auto_callback != None:
			self.yscale_auto_callback = yscale_auto_callback
		if yscale_min != None:
			self.yscale_min = yscale_min
		if yscale_max != None:
			self.yscale_max = yscale_max
			
	def GetYScaleMin(self):
		return self.yscale_min
		
	def GetYScaleMax(self):
		return self.yscale_max
		
	def GetYScaleAuto(self):
		return self.yscale_auto
		
	def GotOutput(self, txt):
		self.outputbuf += txt
		if self.outputbuf[-1] == '\n':
			self.ParseOutput()
			
	def ParseOutput(self):
		curtime = time.time()
		self.sampletime = curtime -  self.lastsampletime
		self.lastsampletime = curtime
		if self.sampletime_callback:
			self.sampletime_callback(self.sampletime)
		
		numstrs = self.outputbuf.split() # Split up the output buffer
		self.outputbuf = ''
		
		nums = [] # convert the numbers
		for numstr in numstrs:
			try:
				nums.append(float(numstr))
			except ValueError:
				pass
				
		self.graphsamples.append(nums)
		while len(self.graphsamples) >= self.sample_count:
			del self.graphsamples[0]
			
		if self.yscale_auto:
			allsamples = merge(self.graphsamples)
			self.yscale_min = min(allsamples)
			self.yscale_max = max(allsamples)
			if self.yscale_auto_callback:
				self.yscale_auto_callback(self.yscale_min, self.yscale_max)
			
		self.Refresh(True)
			
	def OnPaint(self, event):
		dc = wx.PaintDC(self)
		dc.Clear()
		dc.BeginDrawing()
	
		if len(self.graphsamples) > 0:
			(width, height) = self.GetSizeTuple()
		
			xscale = float(width) / self.sample_count
			yscale = float(height) / (self.yscale_max - self.yscale_min)

			for samplenum in range(1, len(self.graphsamples)):
				sample = self.graphsamples[samplenum]
				prevsample = self.graphsamples[samplenum-1]
				
				colornum = 0
				for prevval, val in zip(prevsample, sample):
					dc.SetPen(wx.Pen(self.colors[colornum], 2))
					colornum = colornum + 1
					
					x1 = (samplenum-1)*xscale
					x2 = samplenum*xscale
					y1 = (prevval - self.yscale_min)*yscale
					y2 = (val - self.yscale_min)*yscale
				
					dc.DrawLine(x1, height - y1, x2, height - y2)
				
		dc.EndDrawing()
		
	colors = ["RED", "GREEN", "BLUE", "BLACK", "BLACK", "BLACK"]

class GraphFrame(wx.Frame):
	def __init__(self, mainframe):
		wx.Frame.__init__(self, None, title='QuadComm Grapher', size=wx.Size(800, 300))
		
		self.mainframe = mainframe
		
		panel = wx.Panel(self, -1)
		bottompanel = wx.Panel(panel, -1)
		
		self.grapher = Grapher(panel, -1, self.OnGrapherAutoScale, self.OnGrapherSampleTime)
		self.autoscale = wx.CheckBox(bottompanel, -1, 'Auto')
		self.autoscale.SetValue(True)
		self.minscale = wx.TextCtrl(bottompanel, -1, "-1")
		self.maxscale = wx.TextCtrl(bottompanel, -1, "1")
		self.outputtime = wx.StaticText(bottompanel, -1, '')

		panel_box = wx.BoxSizer(wx.VERTICAL)
		panel.SetSizer(panel_box)
		panel_box.Add(self.grapher, 1, wx.EXPAND)
		panel_box.Add(bottompanel, 0, wx.EXPAND)
		
		bottompanel_box = wx.BoxSizer(wx.HORIZONTAL)
		bottompanel.SetSizer(bottompanel_box)
		bottompanel_box.Add(self.autoscale, 0, wx.ALIGN_CENTER)
		bottompanel_box.Add(wx.StaticText(bottompanel, -1, 'Min'), 0, wx.ALIGN_CENTER)
		bottompanel_box.Add(self.minscale, 0, wx.ALIGN_CENTER)
		bottompanel_box.Add(wx.StaticText(bottompanel, -1, 'Max'), 0, wx.ALIGN_CENTER)
		bottompanel_box.Add(self.maxscale, 0, wx.ALIGN_CENTER)
		bottompanel_box.Add(self.outputtime, 0, wx.ALIGN_CENTER)
		
		self.autoscale.Bind(wx.EVT_CHECKBOX, self.OnToggleAutoScale)
		self.minscale.Bind(wx.EVT_TEXT, self.OnScaleChanged)
		self.maxscale.Bind(wx.EVT_TEXT, self.OnScaleChanged)
		self.Bind(wx.EVT_CLOSE, self.OnClose)
		
	def OnGrapherAutoScale(self, minscale, maxscale):
		def callback():
			self.minscale.SetValue(str(minscale))
			self.maxscale.SetValue(str(maxscale))
		wx.CallAfter(callback)
		
	def OnGrapherSampleTime(self, sampletime):
		def callback():
			self.outputtime.SetLabel("%2.2f ms" % (sampletime*1000))
		wx.CallAfter(callback)
		
	def OnScaleChanged(self, event):
		if self.grapher.GetYScaleAuto():
			return
		
		try:
			self.grapher.SetYScale(yscale_min=float(self.minscale.GetValue()), yscale_max=float(self.maxscale.GetValue()))
		except ValueError:
			pass
		
	def OnToggleAutoScale(self, event):
		self.grapher.SetYScale(yscale_auto=self.autoscale.IsChecked())
		
	def OnClose(self, event):
		self.mainframe.OnGrapherClose()
		self.Destroy()
		
	def GotOutput(self, txt):
		self.grapher.GotOutput(txt)
		
def merge(seq):
    merged = []
    for s in seq:
        for x in s:
            merged.append(x)
    return merged 
		
