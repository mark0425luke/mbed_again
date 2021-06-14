
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


//thread & event queue
Thread t;
EventQueue queue(32 * EVENTS_EVENT_SIZE);

//variable
float transX;
float transY;
float transZ;
float rotatX;
float rotatY;
float rotatZ;
float angle = 625;//1250ms for 90 degree
#define transX_LeftMax -0.3
#define transX_RightMin 0.3
#define rotatY_LeftMin 1
#define rotatY_LeftMax 90
#define rotatY_RightMin 270
#define rotatY_RightMax 359

#define GO 0
#define ROTATE 1
int last =0;

// function declaration
void PositionCalib(Arguments *in, Reply *out);
RPCFunction rpcPositionCalib(&PositionCalib, "PositionCalib");
void RealPositionCalib(float a,float b);

//main
int main() {
	//xbee
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");

   //thread & event queue
   t.start(callback(&queue, &EventQueue::dispatch_forever));

   //wait for rpc
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
   
   //get data
   transX = in->getArg<float>();
   //transY = in->getArg<float>();
   //transZ = in->getArg<float>();
   //rotatX = in->getArg<float>();
   rotatY = in->getArg<float>();
   //rotatZ = in->getArg<float>();
   //printf("%d,%d,%d,%d\r\n",x_1,y_1,x_2,y_2);//.......................test

   //put to queue
   queue.call( RealPositionCalib,transX,rotatY );
	
}


void RealPositionCalib(float transX, float rotatY){

   //float transX = in->getArg<float>();
   //float rotatY = in->getArg<float>();

   if( rotatY>rotatY_LeftMin && rotatY<rotatY_LeftMax ){
      if( transX > transX_RightMin ){
         car.turn(75,0.3);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else if( transX < transX_LeftMax ){
         while( !(transX > transX_RightMin) ){
            car.turn(75,-0.3);
            ThisThread::sleep_for(50ms);
            car.stop();
         }
      }
      else{
         car.goStraight(75);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
   }
   else if( rotatY<rotatY_RightMax && rotatY>rotatY_RightMin ){
      if( transX < transX_LeftMax ){
         car.turn(75,-0.3);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else if( transX > transX_RightMin  ){
         while( !(transX < transX_LeftMax) ){
            car.turn(75,0.3);
            ThisThread::sleep_for(50ms);
            car.stop();
         }
      }
      else{
         car.goStraight(75);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
   }
   else{

   }

}