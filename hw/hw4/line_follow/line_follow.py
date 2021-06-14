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

targetRGB = (3, 100, -86, 127, -15, 127)

def find_area(blob):
    return blob.h()*blob.w()

def find_max_blob(blobs):
    maxArea = 0
    #print (blobs[0])#..................test
    maxBlob = blobs[0]
    for blob in blobs:
        tmpArea = find_area(blob)
        if tmpArea > maxArea :
            maxArea = tmpArea
            maxBlob = blob
    return maxBlob



while(True):
    clock.tick()
    img = sensor.snapshot()
    blobs = img.find_blobs([targetRGB]) # all the blobs found
    #print('blobs are',blobs)#................test
    targetBlob = find_max_blob(blobs) # find the max size blob, then let it be our target
    #print (targetBlob)#.....................test
    img.draw_rectangle( targetBlob.x() , targetBlob.y() , targetBlob.w() , targetBlob.h() )#x,y,w,h #..........test
    func = '/LineFollow/run '
    data1 = str(targetBlob.x())                + ' ' + str(targetBlob.y())                + ' '
    data2 = str(targetBlob.x()+targetBlob.w()) + ' ' + str(targetBlob.y()+targetBlob.h()) + '\n'
    data = func + data1 + data2
    #print( data ) #.......test
    uart.write( (data).encode())




'''
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





'''
while(True):
    clock.tick()
    img = sensor.snapshot()

   #for blobs in img.find_blobs(  , false ):
    for blobs in img.find_blobs( [target], area_threshold=5000, merge=False ):
        img.draw_rectangle(blobs.x(),blobs.y(), blobs.x()+ int(blobs.w()/2),blobs.y()+blobs.h())
   #for blobs in img.find_blobs( [green_threshold] ):

      #print(blob.h(),blob.w())
'''


'''image.find_blobs(
thresholds[,
invert=False[,
roi[,
x_stride=2
[, y_stride=1
[, area_threshold=10
[, pixels_threshold=10
[, merge=False
[, margin=0
[, threshold_cb=None
[, merge_cb=None
]]]]]]]]]])'''
