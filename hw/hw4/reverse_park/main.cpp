#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

//bbcar related
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
BBCar car(pin5, pin6, servo_ticker);
DigitalIn encoder(D11);
BufferedSerial pc(USBTX, USBRX);

//xbee
BufferedSerial xbee(D1, D0);

//LED for test
DigitalOut myled(LED1);

//variable
int d1;
int d2;
char direct[20];
volatile int steps;
volatile int last;
#define HOLES 32
#define DIS90	17.27875
#define DIS180	8.63937
#define DIS270	5.75958
#define SpeedPerSec 17
#define NinetyDegreeMilliSec 1250
float walk=0; 

//s = r * theta
//r = 2 wheel distance


// function declaration
void ReversePark(Arguments *in, Reply *out);
RPCFunction rpcReversePark(&ReversePark, "ReversePark");
void west();
void encoder_control();

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
   printf("haha\n");

	//encoder
	pc.set_baud(9600);
	encoder_ticker.attach(&encoder_control, 10ms);

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

//function definition
void encoder_control() {
    int value = encoder;
    if (!last && value) steps++;
    last = value;
}

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
   			;
   			break;
   		case 'e'://turn CCW 180 degree;
			car.turn(100,0.3);
   		/*steps=0;last=0;
			while(steps*6.5*3.14 <  DIS180) {
				ThisThread::sleep_for(100ms);
			}*/
         walk = float(NinetyDegreeMilliSec) * (180/90);
         ThisThread::sleep_for(walk);
			car.stop();
   			west();
   			break;
   		case 's'://turn CCW 270 degree;
		   car.turn(100,0.3);
   		/*steps=0;last=0;
			while(steps*6.5*3.14 < DIS270) {
				ThisThread::sleep_for(100ms);
			}*/
         walk = float(NinetyDegreeMilliSec) * (270/90);
         ThisThread::sleep_for(walk);
			car.stop();
   			west();
   			;
   			break;
   		case 'n'://turn CCW 90 degree;
		   car.turn(100,0.3);
   		/*steps=0;last=0;
			while(steps*6.5*3.14 < DIS90) {
				ThisThread::sleep_for(100ms);
			}*/
         walk = float(NinetyDegreeMilliSec) * (90/90);
         ThisThread::sleep_for(walk);
			car.stop();
   			west();
   			break;
   		default:
   			break;

   }


}

void west(){
   //led & printf for test
   myled = 1;
   printf("enter west\n");

	//go back d2+1 cm
   car.goStraight(-100);
	steps=0;last=0;
	/*while(steps*6.5*3.14/HOLES < float( (d2+1) ) ){
      printf("steps is %d\n",steps);//....................test
		ThisThread::sleep_for(100ms);
	}*/
   walk = (float(d2)+1)/SpeedPerSec*1000;
   //printf("dist is %f\n",dist);//......................test
   ThisThread::sleep_for(walk);
	car.stop();
	
	//turn CCW 90 degree
	car.turn(100,0.3);
	/*steps=0;last=0;
	while(steps*6.5*3.14 < DIS90 ) {
		ThisThread::sleep_for(100ms);
	}*/
   walk = float(NinetyDegreeMilliSec) * (90/90);
   ThisThread::sleep_for(walk);
	car.stop();

	//turn back d1+3 cm
   car.goStraight(-100);
	/*steps=0;last=0;
	while(steps*6.5*3.14/HOLES < float( (d1+3) ) ){
		ThisThread::sleep_for(100ms);
	}*/
   walk = (float(d1)+3)/SpeedPerSec*1000;
   ThisThread::sleep_for(walk);
	car.stop();
}


還要修正