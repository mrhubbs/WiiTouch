import sys
import math
import pygame
import numpy
import time
import subprocess

from pymouse import PyMouse
import cwiid

NEUTRAL		= 0
FIRST_INPUT	= 1
PRESSED		= 2

FIRST_INPUT_TO_PRESSED	= 0.3	# seconds

class Point(object):
	def __init__(self, x, y):
		self.x = x
		self.y = y

	def __repr__(self):
		return 'Point: (' + str(self.x) + "," + str(self.y) + ')'

def getWiiPoint(wm):
	# wait until there is no IR data
	# wait until there is IR data
	# capture that point

	while True:
		while wm.state['ir_src'][0] == None:
			pass

		prePos = wm.state['ir_src'][0]

		if (prePos == None):
			continue
		else:
			pos = prePos['pos']
			return Point(pos[0], 768 - pos[1])

def getWiiPointNonBlock(wm):
	prePos = wm.state['ir_src'][0]

	if (prePos != None):
		pos = prePos['pos']
		return Point(pos[0], 768 - pos[1])

	return None

def calibrate(wm):
	wiiPoints = []

	p = subprocess.Popen(['./calibrate'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

	for i in range(1, 5):
		wiiPoints.append(getWiiPoint(wm))
		p.stdin.write(str(i) + '\n')
		time.sleep(.5)

	mousePoints = []
	res = p.stdout.read()
	res = res.split('\n')

	for p in res:
		if (p.count(',') > 0):
			vals = p.split(',')
			mousePoints.append(Point(float(vals[0]), float(vals[1])))

	if (len(mousePoints) < 4):
		raise Exception("Only %d screen coordinates" % (len(mousePoints)))

	if (len(wiiPoints) < 4):
		raise Exception("Only %d WiiMote coordinates" % (len(WiiPoints)))

	return (mousePoints, wiiPoints)

def getTransMatrix(X, x):
	res = []

	zero = 0
	one = 1

	for i in range(4):
		res.append([x[i].x, x[i].y, one, zero, zero, zero, -X[i].x * x[i].x, -X[i].x * x[i].y])
		res.append([zero, zero, zero, x[i].x, x[i].y, one, -X[i].y * x[i].x, -X[i].y * x[i].y])

	A = numpy.matrix(res)

	res = []

	for i in range(4):
		res.append([X[i].x])
		res.append([X[i].y])

	B = numpy.matrix(res)
	ATrans = A.transpose()

	lam = (ATrans * A).I * ATrans * B
	return numpy.matrix([[lam[0], lam[1], lam[2]], [lam[3], lam[4], lam[5]], [lam[6], lam[7], 1]])

def main():
	mouse = PyMouse()

	wm = cwiid.Wiimote("00:25:A0:CE:3B:6D")
	wm.rpt_mode = cwiid.RPT_BTN | cwiid.RPT_IR

	X,x = calibrate(wm)
	trans = getTransMatrix(x, X)

	screen = mouse.screen_size()
	lastTime = time.time()
	o = None

	state = NEUTRAL

	print('battery: %f%%' % (float(wm.state['battery']) / float(cwiid.BATTERY_MAX) * 100.0))

	window = pygame.display.set_mode((200, 150))

	while True:
		for event in pygame.event.get():
			if (event.type == pygame.QUIT):
				sys.exit(0)

		pos = getWiiPointNonBlock(wm)

		if (pos != None):
			ipt = numpy.matrix([[pos.x], [pos.y], [1]])
			optMat = trans.I * ipt
			o = Point(optMat[0] / optMat[2], optMat[1] / optMat[2])

			if (o.x < 0):
				o.x = 0
			elif (o.x >= screen[0]):
				o.x = screen[0] - 1

			if (o.y < 0):
				o.y = 0
			elif (o.y >= screen[1]):
				o.y = screen[1] - 1

			if (state == NEUTRAL):
				state = FIRST_INPUT
				lastTime = time.time()
			elif (state == FIRST_INPUT):
				if (time.time() - FIRST_INPUT_TO_PRESSED > lastTime):
					mouse.press(o.x, o.y)
					state = PRESSED
			elif (state == PRESSED):
				mouse.move(o.x, o.y)

		if (state == FIRST_INPUT and
			pos == None and
			time.time() - FIRST_INPUT_TO_PRESSED < lastTime):
			mouse.click(o.x, o.y)
			time.sleep(0.2)
			state = NEUTRAL

		if (state == PRESSED and
			pos == None and
			time.time() - 1 > lastTime):
			mouse.release(o.x, o.y)
			state = NEUTRAL

if (__name__ == '__main__'):
	main()
