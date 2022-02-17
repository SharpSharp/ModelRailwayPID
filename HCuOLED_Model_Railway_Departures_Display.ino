/* FILE:    HCuOLED_Model_Railway_Deaprtures_Display
   DATE:    20/01/22
   VERSION: 0.1
   AUTHOR: Chris Sharp
   
REVISIONS:

20/01/22 Created version 0.1


This is an example of how create a railway station departures board for a OO scale
model railway with an uOLED displays using the HCuOLED 
library with a SH1106 based OLED display module such as:

Hobby Components 0.96" uOLED displays (HCMODU0050 & HCMODU0052)


This sketch will require one the above displays. To use this example
connect your uOLED displays to the following pins of your 
Arduino:


MODULE1..........ARDUINO
GND.............GND
VCC.............3.3V
D0 (CLK)........D13 
D1 (DATA).......D11
RST (RESET).....D8
DC..............D9
CS (SS).........D10


You may copy, alter and reuse this code in any way you like, but please leave
reference to HobbyComponents.com in your comments if you redistribute this code.
This software may not be used directly for the purpose of promoting products that
directly compete with Hobby Components Ltd's own range of products.

THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES, 
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR
LACK OF NEGLIGENCE. HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE
FOR ANY DAMAGES INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR 
CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER. */

#include "HCuOLED.h"
#include "SPI.h"

 // These connect to the CS, DC, and RST pins on the display.

#define CS_DI 10
#define DC_DI 9
#define RST_DI 8


const byte NUMBER_OF_DEPARTURES = 6;

/* Create an instance of the library for the display */
HCuOLED HCuOLED(SSD1306, CS_DI, DC_DI, RST_DI);


/* struct to contain the depature data for each of the train serives */
struct TrainDeparture
{
  byte hours;
  byte minutes;
  byte platform;
  char *destination;
};

/* create the train depatures for each train: Hours, Minutes, Platform, Destination */
TrainDeparture departures[NUMBER_OF_DEPARTURES] = {
  {11, 11, 1, "Sheffield"},
  {11, 14, 3, "Nottingham"},
  {11, 28, 1, "Amblethorpe"},
  {11, 40, 2, "Newcastle"},
  {11, 49, 4, "Blyth"},
  {11, 58, 1, "Amblethorpe"}
};

/* building a depatures row of text in the buffer */
void displayDeparture(byte row)
{
  const byte ROW_HEIGHT = 8;                  // set the height of a row
  byte train = row;                           // set the train which will appear on this row
  row = row * ROW_HEIGHT;                     // set the vetical position of the row on the screen
  HCuOLED.SetFont(sharpsharp_6pt);            // set the font 

  HCuOLED.Cursor(0, row);                     // set text poistion for x and y axis 0, 0 is top left
  HCuOLED.Print(departures[train].hours);     // print the hours value for this train
  
  HCuOLED.Cursor(9, row);
  HCuOLED.Print(":");                         // print a colon to seperate hours and minutes
  
  HCuOLED.Cursor(12, row);
  HCuOLED.Print(departures[train].minutes);
  
  HCuOLED.Cursor(25, row);
  HCuOLED.Print(departures[train].destination);
  
  HCuOLED.Cursor(91, row); 
  HCuOLED.Print(departures[train].platform);
  
  HCuOLED.Cursor(100, row);
  HCuOLED.Print("On Time");                   // print On Time at the end of the row
}

void setup() 
{
  /* Reset the display */
  HCuOLED.Reset();
}
void loop() 
{ 
  /* loop through all the dipartures to generate the display */
  for (int i = 0; i < NUMBER_OF_DEPARTURES; i++)
  {
    displayDeparture(i);
  }
  
  /* load the contents of the display buffer to the screen */
  HCuOLED.Refresh();

}
