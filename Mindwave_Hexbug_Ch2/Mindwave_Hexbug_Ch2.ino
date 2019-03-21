#include "CH2-ilab-hexbug.h"
#include <Brain.h>

#include <Mindwave.h>
#define ATTENTION_THRESHOLD 30
#define MEDITATION_THRESHOLD 30

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


}

void loop() {

                   // get new brainwave data
 mindwave.update();

 if (mindwave.hasNewData()) //loop only runs when new packet arrives

 {
   MeditationValue = mindwave.getMeditation();
   AttentionValue = mindwave.getAttention();
   Quality = mindwave.getQuality();

                     
                         //Serial monitor will print out Quality, attention, and meditation.
   Serial.print("Quality: ");
   Serial.println(Quality, DEC);
   Serial.print(" Attention: ");
   Serial.println(AttentionValue, DEC);
   Serial.print(" Meditation: ");
   Serial.println(MeditationValue, DEC);
   Serial.println("\n");

   // forwardPin will be on so long as the person's AttentionValue is above this value (check the serial monitor)
   if (AttentionValue >= ATTENTION_THRESHOLD)
     {
       hexbug_spider_forward(); //hexbug_spider_right();
       Serial.print ("FORWARD");
       Serial.print ("\n");
     }
  
   
    if (MeditationValue >= MEDITATION_THRESHOLD)
     {
       hexbug_spider_right(); //hexbug_spider_backward();
       Serial.print ("RIGHT");
       Serial.print ("\n");
     }
   
     
 }
}
