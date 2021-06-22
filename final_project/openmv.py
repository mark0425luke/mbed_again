import pyb, sensor, image, time, math


#setup
enable_lens_corr = False # turn on for straighter lines...
sensor.reset()
#sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

f_x = (2.8 / 3.984) * 160 # find_apriltags defaults to this if not set
f_y = (2.8 / 2.952) * 120 # find_apriltags defaults to this if not set
c_x = 160 * 0.5 # find_apriltags defaults to this if not set (the image.w * 0.5)
c_y = 120 * 0.5 # find_apriltags defaults to this if not set (the image.h * 0.5)

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

targetRGB = (3, 100, -86, 127, -15, 127)

#function
def degrees(radians):
   return (180 * radians) / math.pi

def find_area(blob):
    return blob.h()*blob.w()

def find_max_blob(blobs):
    maxArea = 0
    #print (blobs[0])#..................test
    maxBlob = blobs[0]
    for blob in blobs:
        tmpArea = find_area(blob)
        if tmpArea > maxArea :
          if blob.y()+blob.h() > 60:
            maxArea = tmpArea
            maxBlob = blob
    return maxBlob


while(True):
   clock.tick()
   img = sensor.snapshot()
   #apriltag
   for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # defaults to TAG36H11
      img.draw_rectangle(tag.rect(), color = (255, 0, 0))
      img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
      # The conversion is nearly 6.2cm to 1 -> translation
      print_args = (tag.x_translation(), tag.y_translation(), tag.z_translation(), \
            degrees(tag.x_rotation()), degrees(tag.y_rotation()), degrees(tag.z_rotation()))
      # Translation units are unknown. Rotation units are in degrees.

      func = '/PositionCalib/run '
      trans = str(tag.x_translation()) + ' ' + str(tag.z_translation()) + ' '
      rotat = str(degrees(tag.y_rotation())) + '\n'
      data = func + trans + rotat
      print(data)#.......................test
      uart.write( (data).encode())
      #sensor.skip_frames(time = 1000)

   #line follow
   blobs = img.find_blobs([targetRGB]) # all the blobs found
   #print('blobs are',blobs)#................test
   targetBlob = find_max_blob(blobs) # find the max size blob, then let it be our target
   #print (targetBlob)#.....................test
   img.draw_rectangle( targetBlob.x() , targetBlob.y() , targetBlob.w() , targetBlob.h() )#x,y,w,h #..........test
   func = '/LineFollow/run '
   data1 = str(targetBlob.x())                + ' ' + str(targetBlob.y())                + ' '
   data2 = str(targetBlob.x()+targetBlob.w()) + ' ' + str(targetBlob.y()+targetBlob.h()) + '\n'
   data = func + data1 + data2
   print( data ) #.......test
   uart.write( (data).encode())
   


