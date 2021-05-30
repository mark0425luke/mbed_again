#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);

DigitalOut myled(LED3);

int main() {
   while(1){
      car.goStraight(200);
      //car.goStraight(0);//.........................clibrate
      ThisThread::sleep_for(5s);
      car.stop();
      ThisThread::sleep_for(5s);

      //myled=0;
      //ThisThread::sleep_for(500ms);
      //myled=1;
      //ThisThread::sleep_for(500ms);
   }
}