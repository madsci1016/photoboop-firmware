  /* 
 Arduino-powered photobooth firmware
 Simple Switch Labs
 April 10, 2011
 */
#include "pins.h"
#include "sevenseg.h"
#include "music.h"
#include "lights.h"

#ifdef RGBLEDMOD
#define playTone ToneOut
long last_photo;

#endif

int current_state = 0; //0==first time through, 1==photo booth mode, 2==time lapse mode, 3==sound activated mode
int interval_time = 0; 

void setup()
{
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);
  pinMode(PIN_E, OUTPUT);
  pinMode(PIN_F, OUTPUT);
  pinMode(PIN_G, OUTPUT);

  pinMode(PIN_1, OUTPUT);
  pinMode(PIN_2, OUTPUT);
  pinMode(PIN_3, OUTPUT);
  pinMode(PIN_4, OUTPUT);
  
  pinMode(PIEZO_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(USB_REMOTE, OUTPUT);
#ifdef RGBLEDMOD
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);
  
  digitalWrite(REDLED, HIGH);
  digitalWrite(GREENLED, HIGH);
  digitalWrite(BLUELED, LOW);
  
  randomSeed(analogRead(6));
#else
  pinMode(WIRED_FOCUS, OUTPUT);
  pinMode(WIRED_SHUTTER, OUTPUT);
  
  digitalWrite(WIRED_FOCUS,0);
  digitalWrite(WIRED_SHUTTER,0);
#endif

  //a quick test of the displays
  for(int i = 0; i<10; i++)
  {
    show(i);
    delay(120);
  }

  segments_off();

  for(int j = 1; j<5; j++)
  {
    photo_light(j);
    delay(120);
    photo_dark(j);
  }


}


void loop()
{
//  char stufftosay[ ]="ACDEeFGgHhIJLOoPSY"; //letter test mode
  switch(current_state)
  {
    case 0:
      find_mode();  
//      for(int i=0;i<sizeof(stufftosay);i++)
//      {
//        showLetter(stufftosay[i]);
//        delay(1000);
//      }
      break;
      
    case 1:
      photo_booth();
      break;
      
    case 2:
      time_lapse();
      break;
      
    case 3:
      sound_trigger();
      break;
  } 

}

void find_mode()
{
  for(int i=1; i<5; i++)
  {
    photo_light(i);
  }
    
  int mode = 0;      
  showLetter('P');
  
  for(int j=4; j>0; j--)
  {
    for(int i=0; i<75; i++)
    {
      delay(25);
      int button_state = digitalRead(BUTTON_PIN);
      if(button_state ==1)
      {
        mode++;
        if(mode>2)
        {
          mode = 0;
        }
        
        j=4;
        for(int k=1; k<5; k++)
        {photo_light(k);}
        
        switch(mode)
        {
          case 0:
            showLetter('P');
            break;
          case 1:
            showLetter('L');
            break;
          case 2:
            showLetter('S');
        }
        delay(175); //delay for button debouncing
      }
    }

    photo_dark(j);
  }
  
  current_state = mode + 1;  //translates mode into current_state

}

void photo_booth()
{
  delay(50);
  int button_state = digitalRead(BUTTON_PIN);
  
  if(button_state == 1)
  {
    camera_wakeup();
#ifdef RGBLEDMOD
    digitalWrite(REDLED, LOW);
    digitalWrite(GREENLED, HIGH);
    digitalWrite(BLUELED, HIGH);
#endif
    for(int i = 1; i<5; i++)
    {
      photo_light(i);
      PlayNote(i,LEN_E);
      delay(100);
    }
    
    delay(500);
    
    for(int photo_number = 4; photo_number>0; photo_number--)
    {
#ifdef RGBLEDMOD 
      photo_show(photo_number);
      countdown(3, photo_number); 
      digitalWrite(USB_REMOTE, HIGH);
      show(0);
      digitalWrite(REDLED, HIGH);
      PlayNote(0,800);
      digitalWrite(USB_REMOTE, LOW);     
      //take_a_picture_booth();
      delay(100);
      digitalWrite(REDLED, LOW);
      digitalWrite(GREENLED, LOW);
      digitalWrite(BLUELED, LOW);
      delay(100);
      digitalWrite(GREENLED, HIGH);
      digitalWrite(REDLED, HIGH);
      digitalWrite(BLUELED, HIGH);
      delay(300);
      digitalWrite(REDLED, LOW);
      //delay(500);
#else
       photo_show(photo_number);
      countdown(3, photo_number);    
      take_a_picture_booth();
#endif
      
    }
#ifdef RGBLEDMOD    
    last_photo = millis();
    digitalWrite(REDLED, HIGH);
    digitalWrite(GREENLED, LOW);
    digitalWrite(BLUELED, HIGH);
#endif
  }
  
  else
  {
    show(0);
#ifdef RGBLEDMOD
  if(millis()-last_photo > 10000)
    additude();
#endif
  }
}

void time_lapse()
{
  if(interval_time == 0)
  {
    interval_time = time_lapse_setup();
  }
  
  for(int i = 0; i<interval_time; i++)
  {
    for(int j=6; j>0; j--)
    {
      circle(j);
      delay(166);
    }
  }
  
  take_a_picture();
}

int time_lapse_setup()
{ 
  //settings = 1s,3s,5s,10s,30s,1m,5m,10m
  // 1 seconds = no lights
  // 10s seconds = 1 light
  // 1s minutes = 2 lights
  // 10s minutes = 3 lights
  
  interval_time = 1;
  int interval_setting = 0; 
  show(1);
  for(int i=1; i<5 ;i++)
  {photo_dark(i);}
  
  for(int i=0; i<400; i++)
  {
    delay(25);
    int button_state = digitalRead(BUTTON_PIN);
    if(button_state == 1)
    {
      i = 0;
      
      interval_setting++;
      if(interval_setting > 7)
      { interval_setting = 0; }
      
      for(int j = 0; j<5; j++)
      {photo_dark(j);}      
      
      switch(interval_setting)
      {
        case(0): //1s
          show(1);
          interval_time = 1;
          break;
        case(1): //2s
          show(3);
          interval_time = 3;
          break;
        case(2): //5s
          show(5);    
          interval_time = 5;
          break;
        case(3): //10s
          show(1);    
          interval_time = 10;
          photo_light(1);
          break;
        case(4): //30s
          show(3);    
          interval_time = 30;
          photo_light(1);
          break;
        case(5): //1m
          show(1);    
          interval_time = 60;
          photo_light(1);
          photo_light(2);
          break;          
        case(6): //5m
          show(5);    
          interval_time = 300;
          photo_light(1);
          photo_light(2);          
          break;          
        case(7): //10m
          show(1);    
          interval_time = 300;
          photo_light(1);
          photo_light(2);          
          photo_light(3);              
          break;
      }  
     delay(175);  //delay for button debounce
    } 
  }

  for(int j = 0; j<5; j++)
  {photo_dark(j);} 
  
  return interval_time;  
}

void camera_wakeup()
{
  //triggers the remote so the camera wakes up
#ifndef RGBLEDMOD
  digitalWrite(WIRED_FOCUS,HIGH);
#endif
  digitalWrite(USB_REMOTE,HIGH);
  //digitalWrite(WIRED_SHUTTER,HIGH);
  delay(CAMERA_TRIGGER_DELAY);
  //digitalWrite(WIRED_SHUTTER,LOW);
  //digitalWrite(WIRED_FOCUS,LOW);
//  digitalWrite(USB_REMOTE,LOW);
//  delay(CAMERA_TRIGGER_DELAY);
  //digitalWrite(WIRED_FOCUS,HIGH);
  //digitalWrite(USB_REMOTE,HIGH);
//  digitalWrite(WIRED_SHUTTER,HIGH);
//  delay(CAMERA_TRIGGER_DELAY);
#ifndef RGBLEDMOD
  digitalWrite(WIRED_SHUTTER,LOW);
  digitalWrite(WIRED_FOCUS,LOW);
#endif
  digitalWrite(USB_REMOTE,LOW);
}

void sound_trigger()
{
  
  
  pinMode(PIEZO_PIN, INPUT);
  
  int signal = analogRead(PIEZO_PIN);
  
  if(signal>SOUNDLEVEL)
  {
    take_a_picture();
    showLetter('S');
    delay(500); //prevents constant triggering
  }
  else
  {
    showLetter('S');
  }
 
}
  

void take_a_picture()
{
  //this 600ms sequence should handle most cameras
  //if needed, make CAMERA_TRIGGER_DELAY longer
  
  circle(0);
#ifndef RGBLEDMOD
  digitalWrite(WIRED_FOCUS,HIGH);
#endif
  digitalWrite(USB_REMOTE,HIGH);
  delay(CAMERA_TRIGGER_DELAY);

  circle(1);
  delay(CAMERA_TRIGGER_DELAY);
  
  circle(2);
  digitalWrite(USB_REMOTE,LOW);
  delay(CAMERA_TRIGGER_DELAY);
  
  circle(3);
#ifndef RGBLEDMOD
  digitalWrite(WIRED_SHUTTER,HIGH);
#endif
  delay(CAMERA_TRIGGER_DELAY);
  
  circle(4);
  digitalWrite(USB_REMOTE,HIGH);
  delay(CAMERA_TRIGGER_DELAY);
  
  circle(5);
  delay(CAMERA_TRIGGER_DELAY);
#ifndef RGBLEDMOD
  digitalWrite(WIRED_FOCUS,LOW);
  digitalWrite(WIRED_SHUTTER,LOW);
#endif
  digitalWrite(USB_REMOTE,LOW);
}

void take_a_picture_booth()
{
  //this sequence is for photo booth mode - provides better visual indication of when the photo is being taken
  //this 600ms sequence should handle most cameras
  //if needed, make CAMERA_TRIGGER_DELAY longer
  
  show(0);
#ifndef RGBLEDMOD
  digitalWrite(WIRED_FOCUS,HIGH);
#endif
  digitalWrite(USB_REMOTE,HIGH);
  delay(CAMERA_TRIGGER_DELAY);

  delay(CAMERA_TRIGGER_DELAY);
  
  photo_light_all();
  segments_on();
  digitalWrite(USB_REMOTE,LOW);
  delay(CAMERA_TRIGGER_DELAY/2);
  photo_dark_all();
  segments_off();  
  delay(CAMERA_TRIGGER_DELAY/2);
  
  photo_light_all();
  segments_on();
#ifndef RGBLEDMOD
  digitalWrite(WIRED_SHUTTER,HIGH);
#endif
  delay(CAMERA_TRIGGER_DELAY/2);
  photo_dark_all();
  segments_off();  
  delay(CAMERA_TRIGGER_DELAY/2);
  
  photo_light_all();
  segments_on();
  digitalWrite(USB_REMOTE,HIGH);
  delay(CAMERA_TRIGGER_DELAY);
  photo_dark_all();
  segments_off();
  
  delay(CAMERA_TRIGGER_DELAY);
#ifndef RGBLEDMOD
  digitalWrite(WIRED_FOCUS,LOW);
  digitalWrite(WIRED_SHUTTER,LOW);
#endif
  digitalWrite(USB_REMOTE,LOW);

  //an extra flash for good measure
  photo_light_all();
  segments_on();
  delay(CAMERA_TRIGGER_DELAY);
  photo_dark_all();
  segments_off();
  delay(CAMERA_TRIGGER_DELAY);

}

void countdown(int number, int note)
{
  for(int i = number; i>0; i--)
  {
    show(i);
    PlayNote(note,LEN_E);
    delay(1000-LEN_E); //makes each pause 1 second long
  }
}

#ifdef RGBLEDMOD
//called every 50ms (or whatever the delay is set too) and the button is not pressed. 
void additude(){
  
  int roll = random(0,120);
  
  if(roll == 42) {
    
    roll = random(0,5);
    
     switch(roll){
      case 0:
        digitalWrite(REDLED,HIGH);
        digitalWrite(BLUELED,LOW);
      break;
      case 1:
        digitalWrite(GREENLED,HIGH);
        digitalWrite(REDLED,LOW);
      break;
      case 2:
        digitalWrite(BLUELED,HIGH);
        digitalWrite(GREENLED,LOW);
      break;
      case 3:
        digitalWrite(REDLED,LOW);
      break;
      case 4:
        digitalWrite(GREENLED,LOW);
      break;
      case 5:
        digitalWrite(BLUELED,LOW);
      break;
    }

    roll = random(0,4);
    
    switch(roll){
      case 0:
        playTone(260, 70);
        playTone(280, 70);
        playTone(300, 70);
      break;
      case 1:
        playTone(80, 70);
        playTone(100, 70);
        playTone(120, 70);
      break;
      case 2:
        playTone(200, 70);
        playTone(300, 140);
        playTone(100, 70);
      break;
      case 3:
        playTone(150, 100);
        playTone(100, 70);
        playTone(200, 80);
        playTone(300, 70);
      break;
      case 4:
        playTone(80, 100);
        playTone(60, 70);
        playTone(30, 80);
      break;
    }
    
    /*
    digitalWrite(BLUELED, LOW);
    for(int i=300; i<=350; i++)
      ToneOut(i, 8);
    digitalWrite(BLUELED, HIGH);
    digitalWrite(GREENLED, LOW);
    for(int i=350; i>=300; i--)
      ToneOut(i, 8);
    digitalWrite(GREENLED, HIGH);
    */
  }
  

}



#endif


