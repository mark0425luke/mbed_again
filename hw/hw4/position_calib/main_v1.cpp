
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
float transX;
float transY;
float transZ;
float rotatX;
float rotatY;
float rotatZ;
float angle = 625;//1250ms for 90 degree
#define transX_LeftMax -0.5
#define transX_RightMin 0.5
#define rotatY_LeftMin 1
#define rotatY_LeftMax 90
#define rotatY_RightMin 270
#define rotatY_RightMax 359

// function declaration
void PositionCalib(Arguments *in, Reply *out);
RPCFunction rpcPositionCalib(&PositionCalib, "PositionCalib");


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

void PositionCalib(Arguments *in, Reply *out){
   //
   transX = in->getArg<float>();
   //transY = in->getArg<float>();
   //transZ = in->getArg<float>();
   //rotatX = in->getArg<float>();
   rotatY = in->getArg<float>();
   //rotatZ = in->getArg<float>();
   //printf("%d,%d,%d,%d\r\n",x_1,y_1,x_2,y_2);//.......................test

   if( transX < transX_LeftMax ){
      if( rotatY>rotatY_LeftMin && rotatY<rotatY_LeftMax ){
         car.goStraight(75);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else if( rotatY<rotatY_RightMax && rotatY>rotatY_RightMin ){
         car.goStraight(-75);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
   }
   else if( transX > transX_RightMin){
      if( rotatY>rotatY_LeftMin && rotatY<rotatY_LeftMax ){
         car.goStraight(-75);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else if( rotatY<rotatY_RightMax && rotatY>rotatY_RightMin ){
         car.goStraight(75);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
   }
   else{
      if( rotatY>rotatY_LeftMin && rotatY<rotatY_LeftMax ){
         car.turn(-75,-0.3);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else if( rotatY<rotatY_RightMax && rotatY>rotatY_RightMin ){
         car.turn(-75,0.3);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else{

      }
   }
	
}