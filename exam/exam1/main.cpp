//header file
#include "mbed.h"
#include "uLCD_4DGL.h"

//forward declaration
////////////////////////////////////////////////////////////////////////////////////
#define SAMPLE_TIMES 10
#define SAMPLE_FREQ 1000 //Hz
#define SAMPLE_POINTS 500

class Menu{
    private:
        
    public:
        Menu(int a,int b);
        int circle;
        int state;
        void update(DigitalIn up, DigitalIn sel, DigitalIn down);
        void show();
        int get_circle();
        int get_state();
};


uLCD_4DGL uLCD(D1, D0, D2);
Menu uLCDmenu(0,0);


InterruptIn up(D5);
InterruptIn sel(D9);
InterruptIn down(D3);

AnalogOut aout(D7);

AnalogIn ain(A0);


Thread t;
EventQueue lcdQueue;

Thread genWave_t;
EventQueue genWaveQueue;

Thread sample_t;
EventQueue sampleQueue;


//DAC waveform
void genWav();

//ADC sample
void sample();

//DAC_to_ADC
void DAC_to_ADC(int state);

//interrupt func
/*void lcdQueueCallUp();
void lcdQueueCallDown();
void lcdQueueCallSel();*/

//button func
void up_func();
void down_func();
void sel_func();
////////////////////////////////////////////////////////////////////////////////////



//main
////////////////////////////////////////////////////////////////////////////////////
int main()
{
    //thread
    t.start         (callback(&lcdQueue, &EventQueue::dispatch_forever));
    genWave_t.start (callback(&genWaveQueue, &EventQueue::dispatch_forever));
    sample_t.start  (callback(&sampleQueue, &EventQueue::dispatch_forever));
    
    //interrupt
    up.rise(up_func);
    down.rise(down_func);
    sel.rise(sel_func);

    while (1)
    {
        //uLCD                 
        uLCDmenu.show();

        //printf("\n\n");//.........................test
        ThisThread::sleep_for(20ms);
    }
}
////////////////////////////////////////////////////////////////////////////////////




//function definition
////////////////////////////////////////////////////////////////////////////////////

//interrupt call function
void up_func(){
    if(uLCDmenu.circle<3){
        uLCDmenu.circle++;
    }
}
void down_func(){
    if(uLCDmenu.circle>0){
        uLCDmenu.circle--;
    }
}
void sel_func(){
    uLCDmenu.state = uLCDmenu.circle;
    genWaveQueue.call(genWav);
    sampleQueue.call(sample);
}

//generate waveformm
void genWav(){
    //printf("enter genWav\n");//....................test

    //period
    int period =  240;//240ms

    //duration
    float dur = (uLCDmenu.state==0) ?   10.0  :// 1/8
                (uLCDmenu.state==1) ?   20.0  :// 1/4
                (uLCDmenu.state==2) ?   40.0  :// 1/2
                                        80.0  ;
                                        
    //start aout and ain, keep generate wave
    for(int times =0; times< SAMPLE_TIMES;times++){
        float data=0.0;
        for(int i=0;i<period;i++){
            //front part
            if(i<dur){ // ex. 51~59
                data = data + (1.0/dur);
                
            }
            //middle part
            else if(i>=dur && i<period-dur){
                ; //3V peak
            }
            //back part
            else if(data>0.0){
                data = data - (1.0/dur);
            }
            aout = data;
            //printf("data = %f volt\n", data * 3.3f);//....................test
            //printf("aout = %f volt\n", aout.read() * 3.3f);//....................test
            ThisThread::sleep_for(1ms);
        }
        
    }
    
}

//sample
void sample(){
    float data[SAMPLE_POINTS];
    for(int i=0;i<SAMPLE_POINTS;i++){
        data[i] = ain;
        ThisThread::sleep_for(1ms);
    }
    for(int i=0;i<SAMPLE_POINTS;i++){
        printf("%f\r\n", data[i]);
    }
}

//class Menu
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
            uLCD.locate(4,4);uLCD.printf("1/8");
            uLCD.color(GREEN);
            uLCD.locate(4,6);uLCD.printf("1/4");
            uLCD.locate(4,8);uLCD.printf("1/2");
            uLCD.locate(4,10);uLCD.printf("1");
            break;
        case 1:
            uLCD.color(GREEN);
            uLCD.locate(4,4);uLCD.printf("1/8");
            uLCD.color(RED);
            uLCD.locate(4,6);uLCD.printf("1/4");
            uLCD.color(GREEN);
            uLCD.locate(4,8);uLCD.printf("1/2");
            uLCD.locate(4,10);uLCD.printf("1");
            break;
        case 2:
            uLCD.color(GREEN);
            uLCD.locate(4,4);uLCD.printf("1/8");
            uLCD.locate(4,6);uLCD.printf("1/4");
            uLCD.color(RED);
            uLCD.locate(4,8);uLCD.printf("1/2");
            uLCD.color(GREEN);
            uLCD.locate(4,10);uLCD.printf("1");
            break;
        case 3:
            uLCD.color(GREEN);
            uLCD.locate(4,4);uLCD.printf("1/8");
            uLCD.locate(4,6);uLCD.printf("1/4");
            uLCD.locate(4,8);uLCD.printf("1/2");
            uLCD.color(RED);
            uLCD.locate(4,10);uLCD.printf("1");
            break;
        default:
            uLCD.color(RED);
            uLCD.locate(4,4);uLCD.printf("1/8");
            uLCD.color(GREEN);
            uLCD.locate(4,6);uLCD.printf("1/4");
            uLCD.locate(4,8);uLCD.printf("1/2");
            uLCD.locate(4,10);uLCD.printf("1");
            break;
    } 


    //circle
    uLCD.circle(20 , 35 , 2, BLACK);
    uLCD.circle(20 , 50 , 2, BLACK);
    uLCD.circle(20 , 65 , 2, BLACK);
    uLCD.circle(20 , 80 , 2, BLACK);
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
        case 3:
            uLCD.circle(20 , 80 , 2, BLUE);//uLCD.circle(int x , int y , int radius, int color)
            break;
        default:
            uLCD.circle(20 , 35 , 2, BLUE);//uLCD.circle(int x , int y , int radius, int color)
            break;
    }
    

    
}
////////////////////////////////////////////////////////////////////////////////////