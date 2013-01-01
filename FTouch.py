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

#c = Wiimote()

import time
import subprocess

class Point(object):
	def __init__(self, x, y):
		self.x = x
		self.y = y

	def __repr__(self):
		return '(' + str(self.x) + "," + str(self.y) + ')'

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
		mousePoints.append(Point(int(vals[0]), int(vals[1])))

print(mousePoints)
