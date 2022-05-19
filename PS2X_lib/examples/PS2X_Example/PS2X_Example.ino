// PS2X Library for STM32Duino found at: https://chowdera.com/2022/04/202204080627299092.html archived at: https://web.archive.org/web/20220519035526/https://chowdera.com/2022/04/202204080627299092.html
// https://chowdera.com/2022/04/202204080604524719.html archived at: https://web.archive.org/web/20220519041333/https://chowdera.com/2022/04/202204080604524719.html
// https://blog.csdn.net/weixin_43336281/article/details/88628049 archived at: https://web.archive.org/web/20220519041544/https://blog.csdn.net/weixin_43336281/article/details/88628049
// https://blog.csdn.net/qq_38288618/article/details/107208606 archived at: https://web.archive.org/web/20220519041846/https://blog.csdn.net/qq_38288618/article/details/107208606

#include "PS2X_lib.h"
#define onboardLed PC13

/******************************************************************
   set pins connected to PS2 controller:
     - 1e column: original
     - 2e colmun: Stef?
   replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        PA0  //14    
#define PS2_CMD        PA1  //15
#define PS2_SEL        PA2  //16
#define PS2_CLK        PA3  //17

/******************************************************************
   select modes of PS2 controller:
     - pressures = analog reading of push-butttons
     - rumble    = motor rumbling
   uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   true
//#define rumble      true
#define rumble      true

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you connect the controller,
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;

void setup() {
  pinMode(onboardLed, OUTPUT);
  digitalWrite(onboardLed, LOW); // Debug LED

  Serial.begin(500000);

  //delay(5000);  //added delay to give wireless ps2 module some time to startup, before configuring it

  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************

  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  Serial.print("A error = ");
  Serial.print(error);
  Serial.print(" , type = ");
  Serial.println(type);
  //error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  do {
    Serial.println("\nAttempting to configure controller...\n");
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    Serial.println("\nDid it work???\n");
    if (error == 0) {
      Serial.println("\nConfigured successfully!\n");
      break;
    } else {
      Serial.println("\nTrying again...\n");
      delay(1000);
    }
  } while (1);

  Serial.print("B error = ");
  Serial.print(error);
  Serial.print(" , type = ");
  Serial.println(type);

  if (error == 0) {
    Serial.print("\nFound Controller, configured successfully!\n");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true");
    else
      Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder.");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }
  else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  Serial.print("C error = ");
  Serial.print(error);
  Serial.print(" , type = ");
  Serial.println(type);
  //  Serial.print(ps2x.Analog(1), HEX);

  type = ps2x.readType();
  Serial.print("D error = ");
  Serial.print(error);
  Serial.print(" , type = ");
  Serial.println(type);
  switch (type) {
    case 0:
      Serial.println("Unknown Controller type found ");
      break;
    case 1:
      Serial.println("DualShock Controller found ");
      break;
    case 2:
      Serial.println("GuitarHero Controller found ");
      break;
    case 3:
      Serial.println("Wireless Sony DualShock Controller found ");
      break;
  }
  Serial.print("E error = ");
  Serial.print(error);
  Serial.print(" , type = ");
  Serial.println(type);
  //delay(5000);
  digitalWrite(onboardLed, HIGH);
}

void loop() {
  Serial.println("LOOP START");
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
  */
  if (error == 1) //skip loop if no controller found
    return;

  if (type == 2) { //Guitar Hero Controller
    ps2x.read_gamepad();          //read controller

    if (ps2x.ButtonPressed(GREEN_FRET))
      Serial.println("Green Fret Pressed");
    if (ps2x.ButtonPressed(RED_FRET))
      Serial.println("Red Fret Pressed");
    if (ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println("Yellow Fret Pressed");
    if (ps2x.ButtonPressed(BLUE_FRET))
      Serial.println("Blue Fret Pressed");
    if (ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println("Orange Fret Pressed");

    if (ps2x.ButtonPressed(STAR_POWER))
      Serial.println("Star Power Command");

    if (ps2x.Button(UP_STRUM))         //will be TRUE as long as button is pressed
      Serial.println("Up Strum");
    if (ps2x.Button(DOWN_STRUM))
      Serial.println("DOWN Strum");

    if (ps2x.Button(PSB_START))        //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");

    if (ps2x.Button(ORANGE_FRET)) {    // print stick value IF TRUE
      Serial.print("Wammy Bar Position:");
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC);
    }
  }
  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

    if (ps2x.Button(PSB_START))        //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");

    if (ps2x.Button(PSB_PAD_UP)) {     //will be TRUE as long as button is pressed
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if (ps2x.Button(PSB_PAD_RIGHT)) {
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if (ps2x.Button(PSB_PAD_LEFT)) {
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if (ps2x.Button(PSB_PAD_DOWN)) {
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }

    vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if (ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if (ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if (ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if (ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if (ps2x.Button(PSB_TRIANGLE))
        Serial.println("Triangle pressed");
    }

    if (ps2x.ButtonPressed(PSB_CIRCLE))              //will be TRUE if button was JUST pressed
      Serial.println("Circle just pressed");
    if (ps2x.NewButtonState(PSB_CROSS))              //will be TRUE if button was JUST pressed OR released
      Serial.println("X just changed");
    if (ps2x.ButtonReleased(PSB_SQUARE))             //will be TRUE if button was JUST released
      Serial.println("Square just released");

    if (ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
      Serial.print("Stick Values:");
      Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_LX), DEC);
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_RY), DEC);
      Serial.print(",");
      Serial.println(ps2x.Analog(PSS_RX), DEC);
    }
  }
  //delay(50);
  Serial.println("LOOP END");
}
