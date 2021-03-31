#include "mbed.h"
#include "mbed_events.h"
using namespace std::chrono;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
InterruptIn btn(USER_BUTTON);

EventQueue LowQueue;
EventQueue NormalQueue;

/*
void blink_led2() {
// this runs in the normal priority thread
led2 = !led2;
}

void print_toggle_led() {
// this runs in the lower priority thread
printf("Toggle LED!\r\n");
}

void btn_fall_irq() {
led1 = !led1;
// defer the printf call to the low priority thread
printfQueue.call(&print_toggle_led);
}
*/
void Low(){
    while(1){
        printf("Low\r\n");
        //ThisThread::sleep_for(1s);    
    }
    
} 
void Normal(){
    while(1){
        printf("Normal\r\n");
        ThisThread::sleep_for(1s);    
    }
}
int main() {


Thread LowThread(osPriorityLow);
Thread NormalThread(osPriorityNormal);
LowThread.start(Low);
NormalThread.start(Normal);



/*
Ticker ledTicker;
ledTicker.attach(eventQueue.event(&blink_led2), 1s);

// button fall still runs in the ISR
btn.fall(&btn_fall_irq);
*/


while (1) {ThisThread::sleep_for(1s);}

}