#include "mbed.h"

DigitalOut myLED1(LED1);
DigitalOut myLED3(LED3);

int main()
{
   
   myLED3 = 0;
   myLED1 = 0;
   while (true)
   {
      //3 times
      for(int i=0;i<5;i++){ 
        myLED3 = !myLED3;
        ThisThread::sleep_for(1000ms);
      }
      //reset
      myLED3 = 0;
      myLED1 = 0;
      //2 times
      for(int i=0;i<3;i++){ //2 times
        myLED1 = !myLED1;
        ThisThread::sleep_for(1000ms);
      }
      //reset
      myLED3 = 0;
      myLED1 = 0;
   }
}