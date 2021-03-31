//header file
#include "mbed.h"
#include "uLCD_4DGL.h"

//forward declaration
////////////////////////////////////////////////////////////////////////////////////
#define LOW 20
#define MED 50
#define HIGH 100
#define STEP 9
#define SAMPLE_FREQ 1000 //Hz
#define SAMPLE_POINTS 500 
#define PEAK 9000 //9ms/1us = 9000

class Menu{
    private:
        int circle;
        int state;
    public:
        Menu(int a,int b);
        void update(DigitalIn up, DigitalIn sel, DigitalIn down);
        void show();
        int get_circle();
        int get_state();
};

void uLCDThread();

uLCD_4DGL uLCD(D1, D0, D2);
Menu uLCDmenu(0,0);


DigitalIn up(D5);
DigitalIn sel(D9);
DigitalIn down(D3);

AnalogOut aout(D7);

AnalogIn ain(A0);


Thread t;

//DAC waveform
void traingle();

//ADC sample
void sample();

//DAC_to_ADC
void DAC_to_ADC(int state);
////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////
//main
int main()
{
    
    //t.start( traingle );

    while (1)
    {
        //uLCD                 ???ms
        uLCDmenu.update(up,sel,down);
        //printf("up : %d   sel : %d   down : %d\n",up.read(),sel.read(),down.read());//.........................test
        //printf("circle : %d   state : %d\n",uLCDmenu.get_circle(),uLCDmenu.get_state());//.........................test
        uLCDmenu.show();

        /*
        //DAC waveform          
        traingle(uLCDmenu.get_state());

        //ADC sample
        sample();
        */

        //not available to choose
        uLCD.locate(2,2);
        uLCD.color(GREEN);
        uLCD.printf("don't choose");

        //DAC_to_ADC
        //改成 sample
        DAC_to_ADC(uLCDmenu.get_state());


        //sleep
        //printf("\n\n");//.........................test
        ThisThread::sleep_for(20ms);
    }
}
////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////
//function definition
/*void uLCDThread(){
    while(1){
        printf("up:%d   sel:%d   down:%d \n",up.read(),sel.read(),down.read());
        uLCDmenu.show();
    }
}*/
void DAC_to_ADC(int state){
    //period
    int period =    (state==0)  ?( 1000/LOW )   ://100ms
                    (state==1)  ?( 1000/MED )   ://50ms
                    /*(state==2)  ?*/( 1000/HIGH )  ;//40ms
    //store data for later printf
    float data[SAMPLE_POINTS];
    //start aout and ain
    aout = 0.0;
    for(int i=0;i<SAMPLE_POINTS;i++){
        //front part
        if( (i%period)<STEP){ // ex. 51~59
            aout = aout + (1.0/9.0);
            data[i] = ain ;
            //printf("i = %d\n",i);//....................test
            //printf("aout = %f volt\n", aout.read() * 3.3f);//....................test
            //printf("data = %f volt\n", data * 3.3f);//....................test
            //printf("%f\r\n", data);//....................test
            ThisThread::sleep_for(1ms);
        }
        //back part
        else{
            aout = aout - 1.0/(period - STEP);
            data[i] = ain ;
            //printf("i = %d\n",i);//....................test
            //printf("aout = %f volt\n", aout.read() * 3.3f);//....................test
            //printf("data = %f volt\n", data * 3.3f);//....................test
            //printf("%f\r\n", data);//....................test
            ThisThread::sleep_for(1ms);
        }
    }
    //printf for python
    for(int i=0;i<SAMPLE_POINTS;i++){
        printf("%f\r\n", data[i]);
        ThisThread::sleep_for(1ms);
    }
}

//unused
void sample(){
    float data;
    for(int i=0;i<SAMPLE_POINTS;i++){
        data = ain;
        printf("data : %f\n",data);
        ThisThread::sleep_for(1000ms/SAMPLE_FREQ);
    }
}

//
void traingle(){
    /*
   while(1){
       //period
        int period =    (uLCDmenu.get_state()==0)  ?( 1000/LOW )   ://100ms
                        (uLCDmenu.get_state()==1)  ?( 1000/MED )   ://50ms
                                                    ( 1000/HIGH )  ;//40ms
        //start aout and ain
        float data = 0.0f;
        //
        for(int i=0;i<period * 1000;i++){
            if(i<PEAK){
                data = data +0.000111f;
                aout = data;
                i++;
                //printf("aout : %f\n",aout.read());//...............test
                //ThisThread::sleep_for(1us);
                wait_us(1);
            }
            else{
                data = data - 1.0/(period*1000.0 - 9000.0);
                aout = data;
                i--;
                //printf("aout : %f\n",aout.read());//...............test
                //ThisThread::sleep_for(1us);
                wait_us(1);
            }
        }
   }
   */

    //period
    while(1){
        int period =    (uLCDmenu.get_state()==0)  ?( 1000/LOW )   ://100ms
                        (uLCDmenu.get_state()==1)  ?( 1000/MED )   ://50ms
                        ( 1000/HIGH )  ;//40ms
        //start aout and ain
        aout = 0.0;
        for(int i=0;i<period;i++){
            //front part
            if( i<STEP){ // ex. 51~59
                aout = aout + (1.0/9.0);
                ThisThread::sleep_for(1ms);
            }
            //back part
            else{
                aout = aout - 1.0/(period - STEP);
                ThisThread::sleep_for(1ms);
            }
        }
    }
    
    
}

int Menu::get_circle(){
    return circle;
}

int Menu::get_state(){
    return state;
}

Menu::Menu(int a,int b){
    circle=a;
    state=b;
}

void Menu::update(DigitalIn up, DigitalIn sel, DigitalIn down){
    //circle
    if(up==1 && down ==0){
        if(circle<2){
            circle++;
        }
    }
    else if(up==0 && down==1){
        if(circle>0){
            circle--;
        }
    }
    else{}

    //state
    if(sel==1 && up==0 && down==0){
        state = circle;
    }
    //printf("state : %d\n\n",state);//......................test
    
    
}

void Menu::show(){
    /*
    choose frequency
    o   50
        80
        100
    

    circle blue
    normal green
    chosen red
    */

    //
    uLCD.locate(2,2);
    uLCD.color(GREEN);
    uLCD.printf("      choose");//save space for "don't"

    //state
    switch(state) { 
        case 0:
            uLCD.color(RED);
            uLCD.locate(4,4);uLCD.printf("%dHz",LOW);
            uLCD.color(GREEN);
            uLCD.locate(4,6);uLCD.printf("%dHz",MED);
            uLCD.locate(4,8);uLCD.printf("%dHz",HIGH);
            break;
        case 1:
            uLCD.color(GREEN);
            uLCD.locate(4,4);uLCD.printf("%dHz",LOW);
            uLCD.color(RED);
            uLCD.locate(4,6);uLCD.printf("%dHz",MED);
            uLCD.color(GREEN);
            uLCD.locate(4,8);uLCD.printf("%dHz",HIGH);
            break;
        case 2:
            uLCD.color(GREEN);
            uLCD.locate(4,4);uLCD.printf("%dHz",LOW);
            uLCD.locate(4,6);uLCD.printf("%dHz",MED);
            uLCD.color(RED);
            uLCD.locate(4,8);uLCD.printf("%dHz",HIGH);
            break;
        default:
            uLCD.color(RED);
            uLCD.locate(4,4);uLCD.printf("%dHz",LOW);
            uLCD.color(GREEN);
            uLCD.locate(4,6);uLCD.printf("%dHz",MED);
            uLCD.locate(4,8);uLCD.printf("%dHz",HIGH);
            break;
    } 


    //circle
    uLCD.circle(20 , 35 , 2, BLACK);
    uLCD.circle(20 , 50 , 2, BLACK);
    uLCD.circle(20 , 65 , 2, BLACK);
    switch(circle) { 
        case 0:
            uLCD.circle(20 , 35 , 2, BLUE);//uLCD.circle(int x , int y , int radius, int color)
            break;
        case 1:
            uLCD.circle(20 , 50 , 2, BLUE);//uLCD.circle(int x , int y , int radius, int color)
            break;
        case 2:
            uLCD.circle(20 , 65 , 2, BLUE);//uLCD.circle(int x , int y , int radius, int color)
            break;
        default:
            uLCD.circle(20 , 35 , 2, BLUE);//uLCD.circle(int x , int y , int radius, int color)
            break;
    }

    
}
////////////////////////////////////////////////////////////////////////////////////