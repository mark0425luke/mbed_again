#include "mbed.h"
#include "mbed_events.h"
using namespace std::chrono;

DigitalOut led1(LED3);
//DigitalOut led2(LED3);
//InterruptIn btn(USER_BUTTON);

EventQueue E;

Thread t;

void toggle(){
    led1 = !led1;
}

int main() {


t.start(callback(&E, &EventQueue::dispatch_forever));

Ticker ledTicker;
ledTicker.attach(E.event(&toggle), 500ms);


while (1) {ThisThread::sleep_for(1s);}
}