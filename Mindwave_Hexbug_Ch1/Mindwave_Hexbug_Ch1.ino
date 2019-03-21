#include "CH1-ilab-hexbug.h"
#include <Brain.h>

#include <Mindwave.h>
#define QUALITY_THRESHOLD 174
#define ATTENTION_THRESHOLD 15
#define MEDITATION_THRESHOLD 15

Mindwave mindwave;
int MeditationValue = 0;
int AttentionValue = 0;
int Quality = 0;

//---------------------------------------------------------------------------------

void setup() {
                   // start mindwave and tell it to debug
                   // this will automatically open the Serial ports
 mindwave.setup();
 mindwave.setDebug(true);
 // mindwave.setTimeout(10000);

 // Move the HexBug just to demonstrate that Arduino->HexBug is working
 hexbug_spider_left();
 delay (1000);
 hexbug_spider_right();

// pinMode(LED_BUILTIN, OUTPUT);

 Serial.println("Quality, Attention, Meditation, Forward/Reverse");
}

void loop() {

 // get new brainwave data
 mindwave.update();

 if (mindwave.hasNewData()) //loop only runs when new packet arrives

 {
   MeditationValue = mindwave.getMeditation();
   AttentionValue = mindwave.getAttention();
   Quality = mindwave.getQuality();
                  
   Serial.print(Quality, DEC);
   Serial.print(",");
   Serial.print(AttentionValue, DEC);
   Serial.print(",");
   Serial.print(MeditationValue, DEC);
   Serial.print(",");

   // Rick: Only move the HexBug if Quality is good enough. Either move forward or right,
   // but not both
   
   // forwardPin will be on so long as the person's AttentionValue is above this value (check the serial monitor)

   if (Quality >= QUALITY_THRESHOLD)
   {
     if (AttentionValue - MeditationValue >= ATTENTION_THRESHOLD)
       {
         hexbug_spider_forward();
         // Serial.println ("FORWARD");
         // Serial.println("");
         Serial.print("F");
       }
  
     else if (MeditationValue - AttentionValue >= MEDITATION_THRESHOLD)
     {
       hexbug_spider_right();
       // Serial.println ("RIGHT");
       // Serial.println("");
        Serial.print("R");
     }
   }
   
   Serial.println("");

   }

// digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
// delay(200);                       // wait for a second
// digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
// delay(200);                       // wait for a second
}
