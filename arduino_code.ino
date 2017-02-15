// #include "VisualStimulator.h"

// States
#define Idle       1
#define Wait_4_IR1 2
#define Wait_4_IR2 3
#define Do_CMD  4
#define Stim_Vis   5
#define Wait_4_IR3 6
#define Wait_STP   7
#define Stimul     8

// Events
#define NO_EVENT    0
#define event_IR1   2
#define event_IR2   3
#define event_IR3R  4
#define event_IR3L  5
#define event_Start 1
#define event_Stop  6
#define event_USER_CMD  7
#define event_VIS_COMP 8
#define event_AUD_COMP 9
#define event_LAG_COMP 10
#define event_STM_COMP 11


// Other constants
#define LEDPIN 10
#define BUZPIN 12
#define LEFT 0
#define RIGHT 1


// IR connected Pins:
const int pin_IR1 =   2;    // the pin that the IR1 is attached
const int pin_IR2 =   3;
const int pin_IR3R =  4;
const int pin_IR3L =  5;
const int led_GREEN = 10;
const int led_RED =   11;
const int led_VIS  =  10;
const int led_LEFT =  10;
const int led_RIGHT = 11;
const int buz_LEFT =  12;
const int buz_RIGHT = 13;


/////////
class AudioStimulator
{
  private:
     int start_time;
     int elaps_time;
     int lag_time;
     int interval_time;
     int buzzer;
     char state;
  ///    
  public: 
    boolean isStarted;  
    boolean isExpired;  
    //
    AudioStimulator(int a){
      
    }
    
    void Setup(int Lag, int Interval, int BuzzPort){
      lag_time = Lag;
      interval_time = Interval;
      buzzer = BuzzPort;
    }
    //
    void restart() {
      start_time = millis();
      isStarted = true;
      isExpired = false;
      digitalWrite(buzzer, 0);
    }    
    void update(){      

      if (isStarted){ 
         elaps_time =  millis() - start_time;
         if (elaps_time > lag_time)
            state = 1;
         if (elaps_time > (interval_time+lag_time)){
            state = 0;
            isExpired = true;
            isStarted = false;
         }
         digitalWrite(buzzer, state);
      }
    }
};

/////////

class VisualStimulator
{
  private:
     int start_time;
     int elaps_time;
     int lag_time;
     int interval_time;
     int led;
     char state;
  ///    
  public: 
    boolean isStarted;  
    boolean isExpired;  
    //
    VisualStimulator(int a){
      
    }
    
    void Setup(int Lag, int Interval, int LEDPort){
      lag_time = Lag;
      interval_time = Interval;
      led = LEDPort;
    }
    //
    void restart() {
      start_time = millis();
      isStarted = true;
      isExpired = false;
      digitalWrite(led, 0);
    }    
    void update(){      

      if (isStarted){ 
         elaps_time =  millis() - start_time;
         if (elaps_time > lag_time)
            state = 1;
         if (elaps_time > (interval_time+lag_time)){
            state = 0;
            isExpired = true;
            isStarted = false;
         }
         digitalWrite(led, state);
      }
    }
};

struct MessagePacket
{
  boolean Direction;      // 1: Right    0:Left
  boolean Audio;          // 1: Active   0: Deactive
  boolean Visual;         // 1: Active   0: Deactive
  int AudioFreq;          // Herz
  int AudioDurn;          // mSec
  int VisualDurn;         // mSec
  int ALag;              // mSec
  int VLag;              // mSec  
} ;



char inChar = 0;
char State ;
char Event ;
int  sign_position;
String inputString = "";         // a string to hold incoming data
boolean StateTransition = false;
boolean stringComplete = false;  // whether the string is complete
unsigned long StartTime;          // to measure time with  millis();
unsigned long ElapsTime;          // to measure time with  millis();
boolean EventTable[20] = {0};
MessagePacket Cmd_Msg;
VisualStimulator vstim(10);
AudioStimulator astim(10);

////////////////////////
void setup() {
  // put your setup code here, to run once:
  //Initialize serial and wait for port to open:
  State  = Idle;
  Event = NO_EVENT;
  StateTransition = true;
  memset(EventTable,0,20);
  // Building message
  Cmd_Msg.Direction = 0;      // LEFT
  Cmd_Msg.Audio =  1;          // Yes audio
  Cmd_Msg.Visual = 1;          // Yes Visual
  Cmd_Msg.AudioFreq = 100;
  Cmd_Msg.VLag  = 0;           // Visual stimulator's time lag
  Cmd_Msg.ALag  = 0;           // Visual stimulator's time lag
  Cmd_Msg.AudioDurn = 1000;          // mSec
  Cmd_Msg.VisualDurn =1000  ;         // mSec

  // serial port setup
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // input pins setup
  pinMode(pin_IR1, INPUT_PULLUP);
  pinMode(pin_IR2, INPUT_PULLUP);
  pinMode(pin_IR3R, INPUT_PULLUP);
  pinMode(pin_IR3L, INPUT_PULLUP);

  pinMode(led_GREEN, OUTPUT);
  pinMode(led_RED, OUTPUT);
  pinMode(buz_LEFT, OUTPUT);
  pinMode(buz_RIGHT, OUTPUT);
  
  // Initial bootup
  vstim.Setup(Cmd_Msg.VLag, 5000, Cmd_Msg.Direction + LEDPIN);
  astim.Setup(Cmd_Msg.ALag, 7000, Cmd_Msg.Direction + BUZPIN);

  Serial.println("Maze : Ver 1.00");

  //digitalWrite(buz_LEFT, 1);
}

// put your main code here, to run repeatedly:
// Serial.write(133);
// delay(1000);

////////////////////////////////
void loop() {
  switch (State) {
    //----------------
    case Idle: {
        if (StateTransition) {            // doing something at state entrance
          Serial.println("Idle State: Waiting for serial Command...");
          clearLEDs();  // turn off the lights
          StateTransition = false;
        }

        //if (Event == event_Start) {
        if (EventTable[event_Start]){  
          Serial.println("Start Command Received...");
          //StimCounter = 0;                 // No Simulators applied
          Event = NO_EVENT;               // clear the event
          EventTable[event_Start] = 0; // clear the event
          memset(EventTable, 0, 6);   // clear all IR events          
          State  = Wait_4_IR1;            // change the state
          StateTransition = true;
        }
        //if (Event == event_USER_CMD){
        if (EventTable[event_USER_CMD]){   
            // Update the stimulators
             vstim.Setup(Cmd_Msg.VLag, Cmd_Msg.VisualDurn , Cmd_Msg.Direction + LEDPIN);
             astim.Setup(Cmd_Msg.ALag, Cmd_Msg.AudioDurn ,  Cmd_Msg.Direction + BUZPIN);
             EventTable[event_USER_CMD] = 0; // clear the event
        }
        
        break;
      }
    //----------------
    case Wait_4_IR1: {
        if (StateTransition) {
          Serial.println("New State: Waiting for IR1");
          //digitalWrite(led_RED, 1);  // RED LED ON
          StateTransition = false;
        }
         // if (Event == event_IR1) {
         if (EventTable[event_IR1]){
          Serial.println("IR1 triggered: Statring the timer");
          StartTime = millis();      // start the timer
          Event = NO_EVENT;          // clear the event
          memset(EventTable, 0, 6);   // clear all IR events
          State  = Wait_4_IR2;       // change the state
          StateTransition = true;
        }
        break;
      }
    //----------------
    case Wait_4_IR2: {
        if (StateTransition){
          Serial.println("New State: Waiting for IR2");
           StateTransition = false;
        }
        //if (Event == event_IR2) {
        if (EventTable[event_IR2]){
          Serial.println("IR2 triggered: Timer stopped.");
          ElapsTime = millis() - StartTime;          // measure the time difference
          Serial.print("TA = ");
          Serial.print(ElapsTime);
          Serial.print("  miliSec \n");
          Event = NO_EVENT;          // clear the event
          memset(EventTable, 0, 6);   // clear all IR events
          State  = Stimul;   
          StateTransition = true;
        }
        break;
      }
    //----------------
    case Wait_4_IR3: {
        if (StateTransition){
          StateTransition = false;
          Serial.println("New State: Stimulators Completed, Waiting for IR3 ...");      
          }      
        //if (Event == event_IR3R){
        if (EventTable[event_IR3R]){  
          Serial.println("Right IR crossed"); 
          Serial.print("TB = "); 
          ElapsTime = millis() - StartTime;          // measure the time difference
          Serial.print(ElapsTime);
          Serial.print("  miliSec \n");  
          Event = NO_EVENT;          // clear the event
          memset(EventTable, 0, 6);   // clear all IR events
          State  = Idle;   
          StateTransition = true;                      
        }
        //if (Event == event_IR3L){
        if (EventTable[event_IR3L]){
          Serial.println("Left IR crossed"); 
          Serial.print("TB = ");           
          ElapsTime = millis() - StartTime;          // measure the time difference
          Serial.print(ElapsTime);
          Serial.print("  miliSec \n");     
          Event = NO_EVENT;          // clear the event
          memset(EventTable, 0, 6);   // clear all IR events
          State  = Idle;   
          StateTransition = true;           
        }             
        break;
      }
    //----------------
    case Stimul: {
        if (StateTransition){
            StateTransition = false;
            Serial.println("New State: Stimulator Timers Activated... ");      
            vstim.restart();
            astim.restart();
          }
        //if (Event == event_STM_COMP){
        if (EventTable[event_STM_COMP]){
          Serial.println("All Timers expired... ");
          vstim.isExpired = false;
          astim.isExpired = false; 
          // Event = NO_EVENT;            
          EventTable[event_STM_COMP] = 0; // clear the event
          State  = Wait_4_IR3;       // change the state          
          StateTransition = true; 
        }
        
        break;      
    }
      //----------------

  }

  Event = NO_EVENT;
  Event = checkEvents();
}


////////////////////////
int checkEvents() {
  int    cmd_val;
  String cmd_str = ""; 
  
/////// IR Trigger:
  if (digitalRead(pin_IR1) == 0)
    EventTable[event_IR1]  = 1;
  //
  if (digitalRead(pin_IR2) == 0)
     EventTable[event_IR2]  = 1;
  //
  if (digitalRead(pin_IR3L) == 0)
    EventTable[event_IR3L]  = 1;
  //
  if (digitalRead(pin_IR3R) == 0)
      EventTable[event_IR3R]  = 1;

/////// Serial Data received :
  if (stringComplete) {
        Serial.print("Serial Data Recieved: ") ;
        Serial.println(inputString);
        //Serial.println(String(sign_position));
        
        // Check the strings
        if (inputString == "START") {
            inputString = "";
            stringComplete = false;
            EventTable[event_Start] = true;
            //return event_Start;
        }
        if (inputString == "STOP") {
            inputString = "";
            stringComplete = false;
            EventTable[event_Stop] = true;
            //return event_Stop;
        }
      
        // extracting command and value from comm string
        cmd_str = inputString.substring(0,sign_position-1);    
        cmd_val = inputString.substring(sign_position,inputString.length()).toInt();


        if (cmd_str == "VLAG")
           Cmd_Msg.VLag = cmd_val;
        if (cmd_str == "ALAG")
           Cmd_Msg.ALag = cmd_val;           
         if (cmd_str == "VDUR")
           Cmd_Msg.VisualDurn = cmd_val;
         if (cmd_str == "ADUR")
           Cmd_Msg.AudioDurn = cmd_val;           
         if (cmd_str == "AFRQ")
           Cmd_Msg.AudioFreq = cmd_val;    
         if (cmd_str == "ADUR")
           Cmd_Msg.AudioDurn = cmd_val;    
         if (cmd_str == "ENA")
           Cmd_Msg.Audio = (boolean)cmd_val;    
         if (cmd_str == "ENV")
           Cmd_Msg.Visual = (boolean)cmd_val; 
         if (cmd_str == "DIR")
           Cmd_Msg.Direction = (boolean)cmd_val; 
              

        //Serial.println(Cmd_Msg.Direction);           
        // clear the string
        inputString = "";
        stringComplete = false;
        EventTable[event_USER_CMD] = true;
        //return event_USER_CMD;
                
        /*Serial.print("Command is ");
        Serial.println(cmd_str);    
        Serial.print("Value is ");
        Serial.println(cmd_val);*/
  }
  

//////// Checking the timers:
  vstim.update();
  astim.update();
  if (vstim.isExpired && astim.isExpired) {
    //Serial.println("uh oh!");
     EventTable[event_STM_COMP] = true;
  }


/////// if nothing happend:
  return NO_EVENT;
  
}


/////////////////////////
void serialEvent() {  
  while (Serial.available()) {
    // get the new byte:
    inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar != '\n'){
      inputString += inChar;
      if (inChar == '=')
          sign_position = inputString.length();
          //Serial.println(sign_position) ;
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

///////////////////////
void clearLEDs(){
      digitalWrite(led_RED, 0);  // turn off the lights
      digitalWrite(led_GREEN, 0);  
}

/*void clearSounds(){
  
}*/



