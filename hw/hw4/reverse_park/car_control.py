import time
import serial
import sys,tty,termios
class _Getch:
    def __call__(self):
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

def get():
    '''
    inkey = _Getch()
    while(1):
        k=inkey()
        if k!='':break
    if k=='\x1b':
        k2 = inkey()
        k3 = inkey()
        if k3=='A':
            print ("up")
            s.write("/goStraight/run 100 \n".encode())
        if k3=='B':
            print ("down")
            s.write("/goStraight/run -100 \n".encode())
        if k3=='C':
            print ("right")
            s.write("/turn/run 100 -0.3 \n".encode())
        if k3=='D':
            print ("left")
            s.write("/turn/run 100 0.3 \n".encode())
        time.sleep(1)
        s.write("/stop/run \n".encode())
    elif k=='q':
        print ("quit")
        return 0
    else:
        print ("not an arrow key!")
    return 1
    '''
    
    
    '''
    keyboard =  input('')
    d1 = int(keyboard)
    keyboard =  input('')
    d2 = int(keyboard)
    keyboard =  input('')
    direction = keyboard

    #print(d1,',',d2,',',direction)#.....................test
    if direction[0]=='w':
        print('west')
    elif direction[0]=='e':
        print('east')
    elif direction[0]=='n':
        print('north')
    elif direction[0]=='s':
        print('south')
    '''
    
    keyboard = input('')
    haha = keyboard.split(',')
    data = '/ReversePark/run'+' '+haha[0]+' '+haha[1]+' '+haha[2]+' \n'
    #print(data)#........test
    s.write(data.encode())

    return True


if len(sys.argv) < 1:
    print ("No port input")
s = serial.Serial(sys.argv[1])
while get():
    i = 0