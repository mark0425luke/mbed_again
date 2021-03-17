//Show your student ID number and a 30 second count down on QC1602A.
//////////////////////////////////////////////////////////////////////

#include "mbed.h"
#include "TextLCD.h"

DigitalOut led(LED1);      //LED1 = PA_5
TextLCD lcd(D2, D3, D4, D5, D6, D7);

int main()
{
      int x=0;
      lcd.printf("106081021\n");
      for(int i=30;i>=0;i--){
          lcd.locate(5,1);
          lcd.printf("%5d",i);
          ThisThread::sleep_for(1s);
      }
}

//////////////////////////////////////////////////////////////////////





//Show your student ID number in blue, a 30 second count down in green, and a white background on uLCD-144G2-AR.
//////////////////////////////////////////////////////////////////////
/*
#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;

int main()
{
    //background
     uLCD.background_color(0xFFFFFF);
     //student ID
     uLCD.color(BLUE);
     uLCD.printf("\n106081021\n"); 
     //count down
     uLCD.text_width(2); //4X size text
     uLCD.text_height(2);
     uLCD.color(GREEN);
     for (int i=30; i>=0; --i) {
         uLCD.locate(5,5);
         uLCD.printf("%2d",i);
         ThisThread::sleep_for(1s);
     }
}    
*/
//////////////////////////////////////////////////////////////////////




