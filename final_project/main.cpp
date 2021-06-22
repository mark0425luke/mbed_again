
#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

//bbcar 
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);

//xbee
BufferedSerial xbee(D10, D9);

//openmv
BufferedSerial openmv(D1,D0);

//ultrasound ping
DigitalInOut pin13(D13);
parallax_ping ping1(pin13);


//thread
Thread t;

//variable
int x_1;
int x_2;
int y_1;
int y_2;

float mean;
float walk;
#define SpeedPerSec 17
#define NinetyDegreeMilliSec 1250

int state;
#define STATE_START   0
#define STATE_TAG_1  1
#define STATE_GET_PING  2
#define STATE_CIRCLE 3
#define STATE_TAG_2  4
#define STATE_LAST_MILE 5
#define STATE_END 6

#define LINE_IMAGE_MIDDLE 80 
#define LINE_LEFT_BOUND -35
#define LINE_RIGHT_BOUND 35

int tagCount;
int calibCount;
#define TAG_COUNT_THRESHOLD 5
#define CALIB_COUNT_THRESHOLD 20

float transX;
float transY;
float transZ;
float rotatX;
float rotatY;
float rotatZ;
#define TAG_ROTATY_LEFT_MIN 5
#define TAG_ROTATY_LEFT_MAX 90
#define TAG_ROTATY_RIGHT_MIN 270
#define TAG_ROTATY_RIGHT_MAX 355
#define TAG_TRANSX_LEFT_BOUND -0.01
#define TAG_TRANSX_RIGHT_BOUND 0.01
#define TAG_TOO_CLOSE -3

#define TurnHowMuch 80
#define GoHowMuch 100
#define TurnTime 80
#define GoTime 100
int turnHowMuch   =  TurnHowMuch   ;
int goHowMuch     =  GoHowMuch     ;
int turnTime      =  TurnTime      ;
int goTime        =  GoTime        ;



// function declaration
void openmvRPC();

void getPing();

void circleAround();

void testState();

void LineFollow(Arguments *in, Reply *out);
RPCFunction rpcLineFollow(&LineFollow, "LineFollow");

void PositionCalib(Arguments *in, Reply *out);
RPCFunction rpcPositionCalib(&PositionCalib, "PositionCalib");

   


//main
int main() {

	

	//..............................test circleAround
	//state = STATE_GET_PING;
	//getPing();
	//circleAround();	

	



   //setup
   openmv.set_baud(9600);
   xbee.set_baud(9600);

   //start
   //while(true){
   state = STATE_START;
   //char buffer[30];
   //sprintf(buffer, "START\n" );
   //openmv.write(buffer,6);
   //ThisThread::sleep_for(500ms);
   //}
   


	//......................................test
   t.start(testState);

   //openmv & RPC
   openmvRPC();
   

}

//function definition
void testState(){
	while(true){
		switch(state){
			case STATE_START:
				printf("STATE_START\n");
				break;
			case STATE_TAG_1 : 
				printf("STATE_TAG_1\n");
				break;
			case STATE_GET_PING:  
				printf("STATE_GET_PING\n");
				break;
			case STATE_CIRCLE :
				printf("STATE_CIRCLE\n");
				break;
			case STATE_TAG_2  :
				printf("STATE_TAG_2\n");
				break;
			case STATE_LAST_MILE: 
				printf("STATE_LAST_MILE\n");
				break;
			case STATE_END :
				printf("STATE_END\n");
				break;
			default:
				printf("error\n");
				break;
		}
		ThisThread::sleep_for(800ms);
	}
	
}

void circleAround(){

	printf("enter circleAround\n");//....................test
	
	if(state == STATE_CIRCLE){
		//
		car.turn(100,0.3);
		walk = float(NinetyDegreeMilliSec) * (150/90);
		ThisThread::sleep_for(walk*1.5);
		car.stop();
		car.goStraight(100);
		walk = (mean)/SpeedPerSec*1000;
		//printf("walk time  is %f\n",walk);//......................test
		ThisThread::sleep_for(walk);
		car.stop();
		//printf("first side done\n");//................test

      //
      for(int i=0;i<4;i++){
         car.turn(100,-0.3);
         walk = float(NinetyDegreeMilliSec) * (140/90);
         ThisThread::sleep_for(walk*1.5);
         car.stop();
         car.goStraight(100);
         walk = (mean*1.1)/SpeedPerSec*1000;
         //printf("dist is %f\n",dist);//......................test
         ThisThread::sleep_for(walk);
         car.stop();
      }
	  //printf("sides done\n");//................test


      //
      car.turn(100,-0.3);
      walk = float(NinetyDegreeMilliSec) * (150/90);
      ThisThread::sleep_for(walk);
      car.stop();
      car.goStraight(100);
      walk = (mean)/SpeedPerSec*1000;
      //printf("dist is %f\n",dist);//......................test
      ThisThread::sleep_for(walk);
      car.stop();
   }
   

}

void getPing(){
   if(state == STATE_GET_PING){
      mean = 0;
      for(int i=0;i<15;i++){
		 printf("%dth ping is %f\n",i,float(ping1));
         mean = mean + float(ping1);
         ThisThread::sleep_for(800ms);
      }
      mean = mean/15;
	  //mean = -mean;//because the original mean is negative
	  printf("distance is %f\n",mean);
      state = STATE_CIRCLE;
   }
}

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

void LineFollow(Arguments *in, Reply *out){
   if( state==STATE_START || state==STATE_LAST_MILE ){
      //
      x_1 = in->getArg<int>();
      y_1 = in->getArg<int>();
      x_2 = in->getArg<int>();
      y_2 = in->getArg<int>();
      printf(" (%d,%d) , (%d,%d)  \r\n",x_1,y_1,x_2,y_2);//.......................test

      float middle = (x_1+x_2)/2;
      if( x_2<LINE_LEFT_BOUND && x_1>LINE_RIGHT_BOUND){
         car.goStraight(50);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else if( middle < LINE_IMAGE_MIDDLE ){
         car.turn(50,-0.3);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else if(middle > LINE_IMAGE_MIDDLE ){
         car.turn(50,0.3);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
      else{ // 先不倒退好了
         car.goStraight(-50);
         ThisThread::sleep_for(50ms);
         car.stop();
      }
   }
}


void PositionCalib(Arguments *in, Reply *out){
   
   //preprocess
   if( state==STATE_START || state==STATE_CIRCLE // || state==STATE_LAST_MILE
                                                                              ){
      tagCount++;
      if( tagCount>TAG_COUNT_THRESHOLD ){
         if(state==STATE_START){
            tagCount = 0;//reset
            calibCount = 0;//reset
            state = STATE_TAG_1;

            //go back a little
            //car.goStraight(-goHowMuch);
            //ThisThread::sleep_for(goTime);
            //car.stop();
         }
         else if(state==STATE_CIRCLE){
            tagCount = 0;//reset
            calibCount = 0;//reset
            state = STATE_TAG_2;
         }
         //else if(state==STATE_LAST_MILE){
         //   tagCount = 0;//reset
         //   calibCount = 0;//reset
         //   state = STATE_END;
         //}
      }
         
	}
   
   
   //
   if( state==STATE_TAG_1 || state==STATE_TAG_2 || state == STATE_END){

      //end
      if(state == STATE_END){
         car.stop();
         char buffer[30];
         sprintf(buffer, "!!!!! END !!!!!\n" );
         xbee.write(buffer,sizeof(buffer));
		 return;
      }

      //get data
      transX = in->getArg<float>();
      //transY = in->getArg<float>();
      transZ = in->getArg<float>();
      //rotatX = in->getArg<float>();
      rotatY = in->getArg<float>();
      //rotatZ = in->getArg<float>();
      printf("transX %f,transZ %f,rotatY %f\r\n",transX,transZ,rotatY);//.......................test

      if( rotatY<TAG_ROTATY_LEFT_MIN || rotatY>TAG_ROTATY_RIGHT_MAX ){
         
		 calibCount ++;
		 printf("calib %d\n",calibCount);//............test
         
		 if( calibCount>CALIB_COUNT_THRESHOLD ){
            if(state==STATE_TAG_1){
               state = STATE_GET_PING;
               calibCount = 0;
               getPing();
			   circleAround();
            }
            else if(state==STATE_TAG_2){
               //state = STATE_LAST_MILE;
               state = STATE_END;
               calibCount = 0;
               
            }
         }
      }

      if( !( rotatY<TAG_ROTATY_LEFT_MIN || rotatY>TAG_ROTATY_RIGHT_MAX ) ){
         if( transZ>TAG_TOO_CLOSE ){
            if( rotatY>TAG_ROTATY_LEFT_MIN && rotatY<TAG_ROTATY_LEFT_MAX ){
               car.turn(-turnHowMuch,-0.3);
               ThisThread::sleep_for(turnTime);
               car.stop();
               car.goStraight(-goHowMuch);
               ThisThread::sleep_for(goTime);
               car.stop();
               //printf("1\n");//..........test
            }
            else if( rotatY<TAG_ROTATY_RIGHT_MAX && rotatY>TAG_ROTATY_RIGHT_MIN ){
               car.turn(-turnHowMuch,0.3);
               ThisThread::sleep_for(turnTime*2);
               car.stop();
               car.goStraight(-goHowMuch);
               ThisThread::sleep_for(goTime);
               car.stop();
               //printf("2\n");//..........test
            }
            else{
               /*car.goStraight(-goHowMuch);
               ThisThread::sleep_for(goTime);
               car.stop();*/
            }
         }
         else if(transX > TAG_TRANSX_RIGHT_BOUND){
            car.turn(turnHowMuch,0.3);
            ThisThread::sleep_for(turnTime);
            car.stop();
            //printf("3\n");//..........test
         }
         else if(transX < TAG_TRANSX_LEFT_BOUND ){
            car.turn(turnHowMuch,-0.3);
            ThisThread::sleep_for(turnTime);
            car.stop();
            //printf("4\n");//..........test
         }
         else{}
      }   
   }

}