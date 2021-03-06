import sys
import math
import pygame
import numpy
import time
import subprocess

class Point(object):
	def __init__(self, x, y):
		self.x = x
		self.y = y

	def move(self, xDif, yDif):
		self.x += xDif
		self.y += yDif

	def __repr__(self):
		return 'Point: (' + str(self.x) + "," + str(self.y) + ')'

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

def main():
#	X = [Point(50.0, 50.0), Point(300.0, 100.0), Point(325.0, 300.0), Point(75.0, 350.0)]
#	X = [Point(34.0, 62.0), Point(253.0, 41.0), Point(233.0, 237.0), Point(69.0, 311.0)]
	X = [Point(34, 62), Point(253, 41), Point(233, 237), Point(69, 311)]
	XNames = ['X', 'B', 'C', 'D']

#	x = [Point(25.0, 25.0), Point(350.0, 25.0), Point(350.0, 375.0), Point(25.0, 375.0)]
#	x = [Point(51.0, 55.0), Point(51.0 + 223.0, 55.0), Point(51.0 + 223.0, 55.0 + 240.0), Point(51.0, 55.0 + 240.0)]
	x = [Point(51, 55), Point(51 + 223, 55), Point(51 + 223, 55 + 240), Point(51, 55 + 240)]
	xNames = ["X'", "B'", "C'", "D'"]

	pygame.init()

	#create the screen
	window = pygame.display.set_mode((800, 600))

	surface = pygame.Surface((800, 600))
	font = pygame.font.Font(pygame.font.get_default_font(), 16)

	# draw the input and map-to shapes and corner letters
	for p, pn in [(X, XNames), (x, xNames)]:
		for i in range(4):
			c = i + 1

			if (c > 3):
				c = 0

			pygame.draw.line(surface, (0, 200, 0), (p[i].x, p[i].y), (p[c].x, p[c].y), 3)
			drawText(font, surface, p[i].x + 3, p[i].y + 3, pn[i], True, (255, 255, 255))

	pygame.draw.line(surface, (255, 0, 0), (X[0].x, X[0].y), (X[2].x, X[2].y), 1)
	pygame.draw.line(surface, (255, 0, 0), (X[1].x, X[1].y), (X[3].x, X[3].y), 1)

	pygame.draw.line(surface, (0, 0, 255), (x[0].x, x[0].y), (x[2].x, x[2].y), 1)
	pygame.draw.line(surface, (0, 0, 255), (x[1].x, x[1].y), (x[3].x, x[3].y), 1)

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
	weights = numpy.matrix([[lam[0], lam[1], lam[2]], [lam[3], lam[4], lam[5]], [lam[6], lam[7], 1]])

	while True:
		pos = pygame.mouse.get_pos()
		ipt = numpy.matrix([[pos[0]], [pos[1]], [1]])

		trans = weights.I * ipt
		window.blit(surface, (0, 0))

		o = Point(trans[0] / trans[2], trans[1] / trans[2])
		pygame.draw.circle(window, (255, 255, 255), (int(o.x), int(o.y)), 4)

		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				sys.exit(0)
			else:
				pass
#				print event

		pygame.display.flip()

if (__name__ == '__main__'):
	main()
