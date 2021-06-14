import sensor, image, time
import pyb
import math

enable_lens_corr = False # turn on for straighter lines...
sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()


#uart
uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)


# All lines also have `x1()`, `y1()`, `x2()`, and `y2()` methods to get their end-points
# and a `line()` method to get all the above as one 4 value tuple for `draw_line()`.

while(True):
   clock.tick()
   img = sensor.snapshot()
   if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...

   # `merge_distance` controls the merging of nearby lines. At 0 (the default), no
   # merging is done. At 1, any line 1 pixel away from another is merged... and so
   # on as you increase this value. You may wish to merge lines as line segment
   # detection produces a lot of line segment results.

   # `max_theta_diff` controls the maximum amount of rotation difference between
   # any two lines about to be merged. The default setting allows for 15 degrees.

   for l in img.find_line_segments(merge_distance = 10, max_theta_diff = 15):
      if( l.length()>50 ):
        img.draw_line(l.line(), color = (255, 0, 0))
        func = '/LineFollow/run '
        data1 = str(l.x1()) + ' ' + str(l.y1()) + ' '
        data2 = str(l.x2()) + ' ' + str(l.y2()) + '\n'
        data = func + data1 + data2
        #print( data ) #.......test
        uart.write( (data).encode())

   #print("FPS %f" % clock.fps())






'''

while(True):
   clock.tick()
   img = sensor.snapshot()
   for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # defaults to TAG36H11
      img.draw_rectangle(tag.rect(), color = (255, 0, 0))
      img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
      # The conversion is nearly 6.2cm to 1 -> translation
      print_args = (tag.x_translation(), tag.y_translation(), tag.z_translation(), \
            degrees(tag.x_rotation()), degrees(tag.y_rotation()), degrees(tag.z_rotation()))
      # Translation units are unknown. Rotation units are in degrees.
      uart.write(("Tx: %f, Ty %f, Tz %f, Rx %f, Ry %f, Rz %f" % print_args).encode())
   uart.write(("FPS %f\r\n" % clock.fps()).encode())
'''
