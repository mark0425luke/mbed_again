#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

//bbcar related
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);
BufferedSerial pc(USBTX, USBRX);

//xbee
BufferedSerial xbee(D1, D0);

//LED for test
DigitalOut myled(LED1);

//variable
int d1;
int d2;
char direct[20];
#define HOLES 32
#define DIS90	17.27875
#define DIS180	8.63937
#define DIS270	5.75958
#define SpeedPerSec 17
#define NinetyDegreeMilliSec 1250
#define CarLength 10.5
#define CarWidth 11
float walk; 

//s = r * theta
//r = 2 wheel distance


// function declaration
void ReversePark(Arguments *in, Reply *out);
RPCFunction rpcReversePark(&ReversePark, "ReversePark");
void west();



//main
int main() {

   //test speed
   //car.goStraight(-100);
   //ThisThread::sleep_for(1000ms);
   //car.stop();
   //car.turn(100,0.3);
   //ThisThread::sleep_for(1250);
   //car.stop();

   //led & printf for test
   myled = 0;
   //printf("haha\n");

	
	//
	pc.set_baud(9600);
	

	//xbee
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
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

//function definition


void ReversePark(Arguments *in, Reply *out){
   

   //
   d1 = in->getArg<int>();
   d2 = in->getArg<int>();
   *direct = in->getArg<char>();
   printf("%d,%d,%s\r\n",d1,d2,direct);//.......................test

   //direction, only need to perform west
   switch(direct[0]){
   		case 'w':
   			west();
   			break;
   		case 'e'://turn CCW 180 degree;
		   
			//becuase d2 - CarLength
			car.goStraight(-100);
			walk = float(CarLength)/SpeedPerSec*1000;
			ThisThread::sleep_for(walk);
			car.stop();
			
			//because d1 - CarWidth
			car.turn(100,0.3);
			walk = float(NinetyDegreeMilliSec) * (90/90);
			ThisThread::sleep_for(walk);
			car.stop();
			car.goStraight(-100);
			walk = float(CarWidth)/SpeedPerSec*1000;
			ThisThread::sleep_for(walk);
			car.stop();
			
			//
			car.turn(100,0.3);
			walk = float(NinetyDegreeMilliSec) * (90/90);
			ThisThread::sleep_for(walk);
			car.stop();
			
			//
			west();
			break;
   		case 's'://turn CCW 270 degree;
		   
		   	//becuase d1 - (CarWidth - CarLength)
			car.goStraight(100);
			walk = float(CarWidth - CarLength)/SpeedPerSec*1000;
			ThisThread::sleep_for(walk);
			car.stop();

			//becuase d2 - CarLength
			car.turn(100,0.3);
			walk = float(NinetyDegreeMilliSec) * (90/90);
			ThisThread::sleep_for(walk);
			car.stop();
			car.goStraight(-100);
			walk = float(CarLength)/SpeedPerSec*1000;
			ThisThread::sleep_for(walk);
			car.stop();

		   	//
			car.turn(100,0.3);
			walk = float(NinetyDegreeMilliSec) * (180/90);
			ThisThread::sleep_for(walk);
			car.stop();
   			
			//
			west();
   			break;
   		case 'n'://turn CCW 90 degree;
		   
		   	//because d1 - CarWidth
			car.goStraight(-100);
			walk = float(CarWidth)/SpeedPerSec*1000;
			ThisThread::sleep_for(walk);
			car.stop();

			//because d2 + (CarWidth - CarLength)
			car.turn(100,0.3);
         	walk = float(NinetyDegreeMilliSec) * (90/90);
         	ThisThread::sleep_for(walk);
			car.stop();
			car.goStraight(-100);
			walk = float(CarWidth - CarLength)/SpeedPerSec*1000;
			ThisThread::sleep_for(walk);
			car.stop();

			
			//
   			west();
   			break;
   		default:
   			break;

   }


}

void west(){
   //led & printf for test
   myled = 1;
  // printf("enter west\n");

	//go back d2 + CarLength + 5 cm
	car.goStraight(-100);
   	walk = (float(d2 + CarLength)+10)/SpeedPerSec*1000;
   	//printf("dist is %f\n",dist);//......................test
   	ThisThread::sleep_for(walk);
	car.stop();
	
	//turn CCW 90 degree
	car.turn(100,0.3);
   	walk = float(NinetyDegreeMilliSec) * (90/90);
   	ThisThread::sleep_for(walk);
	car.stop();

	//turn back d1 + CarLength + 5 cm
  	car.goStraight(-100);
   	walk = (float(d1 + CarLength)+10)/SpeedPerSec*1000;
   	ThisThread::sleep_for(walk);
	car.stop();
}


