#include "mbed.h"

#define CENTER_BASE 1500

BufferedSerial pc(USBTX, USBRX);
DigitalIn encoder(D10);
PwmOut servo(D11);

Timer t;
Ticker encoder_ticker;

volatile int steps;
volatile int last;

void servo_control(int speed) {
   if (speed > 200)       speed = 200;
   else if (speed < -200) speed = -200;

   servo=(CENTER_BASE + speed)/20000.0f;
}

void encoder_control() {
   int value = encoder;
   if(!last && value) steps++;
   last = value;
}


int main() {

   pc.set_baud(9600);

   encoder_ticker.attach(&encoder_control, .01);

   servo.period_ms(20);

   /*
   //collect data for CW table, speed<0
   int i = 0;
   while (i >= -150) {

      servo_control(i);

      steps = 0;
      t.reset();
      t.start();

      ThisThread::sleep_for(8000ms);

      float time = t.read();

      printf("%1.3f\r\n", (float)steps * 6.5 * 3.14 / 32 / time);

      i -= 30;
   }
   servo_control(0);
   */

   
   //start running
   while(1) {

      // 5cm/sec CW for 5sec
      servo_control(-31.63313773161316);
      steps = 0;
      t.reset();
      t.start();
      ThisThread::sleep_for(5000ms);
      float time = t.read();
      printf("CW %1.3f\r\n", (float) steps * 6.5 * 3.14 / 32 / time);

      // 8cm/sec CCW for 5sec
      servo_control(45);
      steps = 0;
      t.reset();
      t.start();
      ThisThread::sleep_for(5000ms);
      time = t.read();
      printf("CCW %1.3f\r\n", (float) steps * 6.5 * 3.14 / 32 / time);

   }
   

}