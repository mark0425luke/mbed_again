
#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

//bbcar related
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);
DigitalIn encoder(D11);

//xbee
BufferedSerial xbee(D1, D0);

//variable
int x_1;
int x_2;
int y_1;
int y_2;
#define IMAGE_MIDDLE 80 

// function declaration
void LineFollow(Arguments *in, Reply *out);
RPCFunction rpcLineFollow(&LineFollow, "LineFollow");

//variable


//main
int main() {
	//xbee
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
      }
   RPC::call(buf, outbuf);
   }
}

void LineFollow(Arguments *in, Reply *out){
   //
   x_1 = in->getArg<int>();
   y_1 = in->getArg<int>();
   x_2 = in->getArg<int>();
   y_2 = in->getArg<int>();
   //printf("%d,%d,%d,%d\r\n",x_1,y_1,x_2,y_2);//.......................test

   float middle = (x_1+x_2)/2;
   if(  -50<(x_2-x_1) && (x_2-x_1)<50 ){
		car.goStraight(50);
		ThisThread::sleep_for(50ms);
		car.stop();
   }
   else{
	   if( middle<IMAGE_MIDDLE ){
		   car.turn(50,0.3);
		   ThisThread::sleep_for(50ms);
		   car.stop();
	   }
	   else{
		   car.turn(50,-0.3);
		   ThisThread::sleep_for(50ms);
		   car.stop();
	   }
   }
		
}