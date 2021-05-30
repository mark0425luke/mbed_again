
#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);

BBCar car(pin5, pin6, servo_ticker);

int main() {
   // please contruct you own calibration table with each servo

   NOT YET !!!!!

   double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table0[] = {-14.669, -15.147, -14.430, -12.118, -6.298, 0.000, 5.740, 11.879, 14.031, 14.828, 15.386};
   double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table1[] = {-14.669, -15.147, -14.430, -12.118, -6.298, 0.000, 5.740, 11.879, 14.031, 14.828, 15.386};



   // first and fourth argument : length of table
   car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

   while (1) {
      car.goStraightCalib(5);
      ThisThread::sleep_for(5s);
      car.stop();
      ThisThread::sleep_for(5s);
   }
}




/*
//////////////////// generate speed table //////////////////// 
#include "mbed.h"
#include "bbcar.h"

BufferedSerial pc(USBTX, USBRX);

Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalIn encoder(D11);

Timer t;


volatile int steps;
volatile int last;

BBCar car(pin5, pin6, servo_ticker);


void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

int main() {

   pc.set_baud(9600);

   encoder_ticker.attach(&encoder_control, 10ms);

   

   int i = -150;
   while (i <= 150) {

      car.goStraight(i);

      steps = 0;
      t.reset();
      t.start();

      ThisThread::sleep_for(8000ms);

      float time = t.read();

      printf("%1.3f\r\n", (float)steps * 6.5 * 3.14 / 32 / time);

      i += 30;
   }
   car.goStraight(0);

   while(1);
}
*/