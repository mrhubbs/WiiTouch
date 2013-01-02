## pymouse

## import the module
#from pymouse import PyMouse

## instantiate an mouse object
#m = PyMouse()

## move the mouse to int x and int y (these are absolute positions)
#m.move(200, 200)

## click works about the same, except for int button possible values are 1: left, 2: middle, 3: right
#m.click(500, 300, 1)

## get the screen size
#print(m.screen_size())
## (1024, 768)

## get the mouse position
#print(m.position())
## (500, 300)

## cwiid

#import cwiid

#wm = cwiid.Wiimote("00:25:A0:CE:3B:6D")
#wm.rpt_mode = cwiid.RPT_BTN | cwiid.RPT_IR
#wm.state
#wm.state['buttons'] & cwiid.BTN_A

import sys
import math
import pygame

import time
import subprocess

CASE0 = 0	# A -> A'
CASE1 = 1	# A -> B'
CASE2 = 2	# A -> C'
CASE3 = 3	# A -> D'

mapCase = CASE0

class Vector(object):
	def __init__(self, x, y):
		self.x = float(x)
		self.y = float(y)

	def getSlope(self):
		return self.y / self.x

	def __repr__(self):
		return 'Vector: (' + str(self.x) + "," + str(self.y) + ')'

#class PosVector(object):
#	def __init__(self, x, y, xDir, yDir):
#		self.x = x
#		self.y = y
#		self.xDir = xDir
#		self.yDir = yDir

#	def __repr__(self):
#		return 'PosVector: (' + str(self.x) + "," + str(self.y) + ') -> (' + str(self.xDir) + "," + str(self.yDir) + ')'

class Point(object):
	def __init__(self, x, y):
		self.x = x
		self.y = y

	def move(self, xDif, yDif):
		self.x += xDif
		self.y += yDif

	def __repr__(self):
		return 'Point: (' + str(self.x) + "," + str(self.y) + ')'

def distance(p1, p2):
	return math.sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y))

def calibrateTest():
	p = subprocess.Popen(['./calibrate'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

	time.sleep(1)
	p.stdin.write('1\n')

	time.sleep(1)
	p.stdin.write('2\n')

	time.sleep(1)
	p.stdin.write('3\n')

	time.sleep(1)
	p.stdin.write('4\n')

	mousePoints = []
	res = p.stdout.read()
	res = res.split('\n')

	for p in res:
		if (p.count(',') > 0):
			vals = p.split(',')
			mousePoints.append(Point(float(vals[0]), float(vals[1])))

	print(mousePoints)

def drawText(font, surface, x, y, text, antialias, color):
	s = font.render(text, antialias, color)
	surface.blit(s, (x, y))

# get the vector from the wii point to the given screen point
def getPrimeVector(wiiPoint, screenPoint):
	return Vector(screenPoint.x - wiiPoint.x, screenPoint.y - wiiPoint.y)

# returns the index of the smallest value in the list
def findSmallest(l):
	smallest = 0

	for i in range(len(l)):
		if (l[i] < l[smallest]):
			smallest = i

	return smallest

# set every wii point as corresponding to the screen point closest to it
# returns a tuple of two lists
#	first list: matched wii-screen points, with the A wii point first in the list (text)
#	second list: matched wii-screen points, with the A wii point first in the list (indices)
def findCorrespondingPoints(wiiPoints, screenPoints, wiiPointNames, screenPointNames):
	# for every wii point, get the distance from it to every other screen point (16 total distance values)
	distList = []

	for i in range(4):
		dist = []

		for c in range(4):
			dist.append(distance(wiiPoints[i], screenPoints[c]))

		distList.append(dist)

	res1 = []
	res2 = []

	for i in range(4):
		res1.append((wiiPointNames[i], screenPointNames[findSmallest(distList[i])]))
		res2.append(findSmallest(distList[i]))

	print('corresponding names: ' + str(res1))
	print('corresponding indicies: ' + str(res2))

	return (res1, res2)

def setMapCase(m):
	if (m[0][1] == "A'"):
		mapCase = CASE0
	elif (m[0][1] == "B'"):
		mapCase = CASE1
	elif (m[0][1] == "C'"):
		mapCase = CASE2
	elif (m[0][1] == "D'"):
		mapCase = CASE3

def vectorsFromPoints(points):
	res = []

	for i in range(4):
		c = i + 1

		if (c > 3):
			c = 0

		res.append(getPrimeVector(points[i], points[c]))

	return res

def pushA(l):
	l.insert(0, l.pop())
	return l

# return the combined sum of c1 and c2, based on the distances d1 and d2
def avgComponents(c1, c2, d1, d2):
	d1d2 = d1 / d2
	d2d1 = d2 / d1
	total = d1d2 + d2d1
	d1d2 /= total
	d2d1 /= total

	return c1 * d2d1 + c2 * d1d2

def main():
	wiiPoints = [Point(50.0, 50.0), Point(300.0, 100.0), Point(325.0, 300.0), Point(75.0, 350.0)]
	wiiPointNames = ['A', 'B', 'C', 'D']
	wiiPointVects = vectorsFromPoints(wiiPoints)

	print('point vectors: %s' % str(wiiPointVects))
	print('point vec slopes:')

	for i in range(4):
		print('\t%f' % (wiiPointVects[i].getSlope()))

	screenPoints = [Point(25.0, 25.0), Point(350.0, 25.0), Point(350.0, 375.0), Point(25.0, 375.0)]
	screenPointNames = ["A'", "B'", "C'", "D'"]

	corNames, corIndices = findCorrespondingPoints(wiiPoints, screenPoints, wiiPointNames, screenPointNames)
	setMapCase(corNames)

	# get the prime vectors
	primes = []

	for i in range(4):
		primes.append(getPrimeVector(wiiPoints[i], screenPoints[corIndices[i]]))

	print('primes: %s' % (str(primes)))

	pygame.init()

	#create the screen
	window = pygame.display.set_mode((800, 600))

	font = pygame.font.Font(pygame.font.get_default_font(), 16)

	# draw the corner vectors
	for i in range(4):
		# prime vector
		pygame.draw.line(window, (255, 0, 0), (wiiPoints[i].x, wiiPoints[i].y), (wiiPoints[i].x + primes[i].x, wiiPoints[i].y + primes[i].y), 3)
		# components
		pygame.draw.aaline(window, (255, 0, 0), (wiiPoints[i].x, wiiPoints[i].y), (wiiPoints[i].x + primes[i].x, wiiPoints[i].y))
		pygame.draw.aaline(window, (255, 0, 0), (wiiPoints[i].x, wiiPoints[i].y), (wiiPoints[i].x, wiiPoints[i].y + primes[i].y))

	# draw the input and map-to shapes and corner letters
	for p, pn in [(wiiPoints, wiiPointNames), (screenPoints, screenPointNames)]:
		for i in range(4):
			c = i + 1

			if (c > 3):
				c = 0

			pygame.draw.line(window, (0, 200, 0), (p[i].x, p[i].y), (p[c].x, p[c].y), 3)
			drawText(font, window, p[i].x + 3, p[i].y + 3, pn[i], True, (255, 255, 255))

	pygame.display.flip()

	while True:
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				sys.exit(0)
			else:
				pass
#				print event

if (__name__ == '__main__'):
	main()
