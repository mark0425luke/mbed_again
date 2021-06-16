
#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

//bbcar related
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);

//openmv
BufferedSerial openmv(D1, D0);

//xbee
//BufferedSerial xbee( D4,D9 );

//ping
DigitalInOut pin10(D10);

//thread & event queue
Thread t;

//variable
float transX;
float transY;
float transZ;
float rotatX;
float rotatY;
float rotatZ;
float angle = 625;//1250ms for 90 degree
#define SafeLeftBound -0.01
#define SafeRightBound 0.01
#define rotatY_LeftMin 3
#define rotatY_LeftMax 90
#define rotatY_RightMin 270
#define rotatY_RightMax 357
#define TooClose -8
#define TurnHowMuch 75
#define GoHowMuch 75
#define TurnTime 75
#define GoTime 75
int turnHowMuch   =  TurnHowMuch   ;
int goHowMuch     =  GoHowMuch     ;
int turnTime      =  TurnTime      ;
int goTime        =  GoTime        ;

// function declaration
void PositionCalib(Arguments *in, Reply *out);
RPCFunction rpcPositionCalib(&PositionCalib, "PositionCalib");
void RealPositionCalib(float a,float b);
void openmvRPC();




//main
int main() {

   //xbee
   //xbee.set_baud(9600);
   //car.goStraight(100);

   //ping
   parallax_ping ping1(pin10);
   
	//openmv & RPC
   t.start(openmvRPC);

   //ping
   while(1){
      char buffer[20];
      sprintf(buffer, "%5f\n", float(ping1));
      printf(buffer);//.....................test
      //xbee.write(buffer,7);
      ThisThread::sleep_for(1000ms);
   }
}

//function definition
void openmvRPC(){
   
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&openmv, "r");
   FILE *devout = fdopen(&openmv, "w");
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            //printf("\r\n");
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
   transZ = in->getArg<float>();
   //rotatX = in->getArg<float>();
   rotatY = in->getArg<float>();
   //rotatZ = in->getArg<float>();
   //printf("%d,%d,%d,%d\r\n",x_1,y_1,x_2,y_2);//.......................test

   

   if( !( rotatY<rotatY_LeftMin || rotatY>rotatY_RightMax ) ){
      if( transZ>TooClose ){
         if( rotatY>rotatY_LeftMin && rotatY<rotatY_LeftMax ){
            car.turn(-turnHowMuch,-0.3);
            ThisThread::sleep_for(turnTime);
            car.stop();
            car.goStraight(-goHowMuch);
            ThisThread::sleep_for(goTime);
            car.stop();
            //printf("1\n");//..........test
         }
         else if( rotatY<rotatY_RightMax && rotatY>rotatY_RightMin ){
            car.turn(-turnHowMuch,0.3);
            ThisThread::sleep_for(turnTime);
            car.stop();
            car.goStraight(-goHowMuch);
            ThisThread::sleep_for(goTime);
            car.stop();
            //printf("2\n");//..........test
         }
      }
      else if(transX > SafeRightBound){
         car.turn(turnHowMuch,0.3);
         ThisThread::sleep_for(turnTime);
         car.stop();
         //printf("3\n");//..........test
      }
      else if(transX < SafeLeftBound ){
         car.turn(turnHowMuch,-0.3);
         ThisThread::sleep_for(turnTime);
         car.stop();
         //printf("4\n");//..........test
      }
      else{}
   }   
}


/*
#include "mbed.h"
#include "bbcar.h"

DigitalOut led1(LED1);
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalInOut pin10(D10);

BBCar car(pin5, pin6, servo_ticker);
Thread t;
void haha();

int main() {
   parallax_ping  ping1(pin10);
   t.start(haha);
   while(1) {
      printf("%f\n",float(ping1));
      ThisThread::sleep_for(1000ms);
   }
}
 void haha(){
    int a;
    while(1){
       a ++;
       ThisThread::sleep_for(1000ms);
    }
 }
 */