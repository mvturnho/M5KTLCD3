// Do not remove the include below
#include "M5KTLCD3.h"
#include <M5Stack.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"

//The setup function is called once at startup of the sketch
void setup()
{
	  M5.begin();

	  // text print
	  M5.Lcd.fillScreen(BLACK);
	  M5.Lcd.setCursor(10, 10);
	  M5.Lcd.setTextColor(WHITE);
	  M5.Lcd.setTextSize(1);
	  M5.Lcd.printf("Display Test!");

	  uart2_init();
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
}
