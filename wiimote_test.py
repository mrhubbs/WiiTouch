# pymouse

# import the module
from pymouse import PyMouse

# instantiate an mouse object
m = PyMouse()

# move the mouse to int x and int y (these are absolute positions)
#m.move(200, 200)

# click works about the same, except for int button possible values are 1: left, 2: middle, 3: right
#m.click(500, 300, 1)

# get the screen size
#print(m.screen_size())
# (1024, 768)

# get the mouse position
#print(m.position())
# (500, 300)

# cwiid

import cwiid
import time

#wm = cwiid.Wiimote("00:25:A0:CE:3B:6D")
wm = cwiid.Wiimote()

wm.rpt_mode = cwiid.RPT_BTN# | cwiid.RPT_IR

while True:
	pos = m.position()
	x = pos[0]
	y = pos[1]

	if (wm.state['buttons'] & cwiid.BTN_UP):
		m.move(x, y - 10)
	elif (wm.state['buttons'] & cwiid.BTN_DOWN):
		m.move(x, y + 10)
	elif (wm.state['buttons'] & cwiid.BTN_LEFT):
		m.move(x - 10, y)
	elif (wm.state['buttons'] & cwiid.BTN_RIGHT):
		m.move(x + 10, y)
	elif (wm.state['buttons'] & cwiid.BTN_A):
		m.click(x, y, 1)
		time.sleep(.2)
	elif (wm.state['buttons'] & cwiid.BTN_B):
		m.click(x, y, 2)
		time.sleep(.2)

	time.sleep(.02)
