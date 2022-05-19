// PS2X Library for STM32Duino found at: https://chowdera.com/2022/04/202204080627299092.html archived at: https://web.archive.org/web/20220519035526/https://chowdera.com/2022/04/202204080627299092.html
// https://chowdera.com/2022/04/202204080604524719.html archived at: https://web.archive.org/web/20220519041333/https://chowdera.com/2022/04/202204080604524719.html
// https://blog.csdn.net/weixin_43336281/article/details/88628049 archived at: https://web.archive.org/web/20220519041544/https://blog.csdn.net/weixin_43336281/article/details/88628049
// https://blog.csdn.net/qq_38288618/article/details/107208606 archived at: https://web.archive.org/web/20220519041846/https://blog.csdn.net/qq_38288618/article/details/107208606

#include <USBComposite.h>
#include "PS2X_lib.h"
#define onboardLed PC13
USBXBox360 XBox360;

/******************************************************************
   set pins connected to PS2 controller:
     - 1e column: original
     - 2e colmun: Stef?
   replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        PA0
#define PS2_CMD        PA1
#define PS2_SEL        PA2
#define PS2_CLK        PA3

/******************************************************************
   select modes of PS2 controller:
     - pressures = analog reading of push-butttons
     - rumble    = motor rumbling
   uncomment 1 of the lines for each mode selection
 ******************************************************************/
#define pressures   true
#define rumble      true

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you connect the controller,
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;

uint8_t vibrationLevelLeftMotor = 0;
uint8_t vibrationLevelRightMotor = 0;

void setup() {
  pinMode(onboardLed, OUTPUT);
  digitalWrite(onboardLed, LOW); // Debug LED
  //delay(1000);  //added delay to give wireless ps2 module some time to startup, before configuring it
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
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
  type = ps2x.readType();
  XBox360.begin();
  while (!USBComposite);
  XBox360.setRumbleCallback(rumbleMotors);
  XBox360.setManualReportMode(true);
  //digitalWrite(onboardLed, HIGH);
  //error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  //type = ps2x.readType();
  digitalWrite(onboardLed, HIGH);
}

void loop() {
  ps2x.read_gamepad(vibrationLevelRightMotor, vibrationLevelLeftMotor); // enable small motor (Right motor, on or off) and large motor (Left motor, 0-255)
  sendXbox360Data();
}

void rumbleMotors(uint8_t leftMotor, uint8_t rightMotor) {
  vibrationLevelLeftMotor = leftMotor;
  vibrationLevelRightMotor = rightMotor;
}

void sendXbox360Data() {

  // BYTE 0
  XBox360.button(XBOX_BACK, ps2x.Button(PSB_SELECT)); // Back maps to Select
  XBox360.button(XBOX_L3, ps2x.Button(PSB_L3)); // L3 maps to L3 (Left stick click)
  XBox360.button(XBOX_R3, ps2x.Button(PSB_R3)); // R3 maps to R3 (Right stick click)
  XBox360.button(XBOX_START, ps2x.Button(PSB_START)); // Start maps to Start
  XBox360.button(XBOX_DUP, ps2x.Button(PSB_PAD_UP)); // Dpad Up (Dup) maps to Dpad Up
  XBox360.button(XBOX_DRIGHT, ps2x.Button(PSB_PAD_RIGHT)); // Dpad Right (Dright) maps to Dpad Right
  XBox360.button(XBOX_DDOWN, ps2x.Button(PSB_PAD_DOWN)); // Dpad Down (Ddown) maps to Dpad Ddown
  XBox360.button(XBOX_DLEFT, ps2x.Button(PSB_PAD_LEFT)); // Dpad Left (Dleft) maps to Dpad Left

  // BYTE 1
  // Convert analog triggers to digital
  if (ps2x.Button(PSB_L2) == true) {
    XBox360.sliderLeft(255); // Left Trigger (LT, Slider Left) maps to L2
  }
  if (ps2x.Button(PSB_L2) == false) {
    XBox360.sliderLeft(0); // Left Trigger (LT, Slider Left) maps to L2
  }
  if (ps2x.Button(PSB_R2) == true) {
    XBox360.sliderRight(255); // Right Trigger (RT, Slider Right) maps to R2
  }
  if (ps2x.Button(PSB_R2) == false) {
    XBox360.sliderRight(0); // Right Trigger (RT, Slider Right) maps to R2
  }
  //XBox360.sliderLeft(ps2x.Analog(PSAB_L2)); // Left Trigger (LT, Slider Left) maps to L2 (As analog, experimental, I probably won't like it)
  //XBox360.sliderRight(ps2x.Analog(PSAB_R2)); // Right Trigger (RT, Slider Right) maps to R2 (As analog, experimental, I probably won't like it)
  XBox360.button(XBOX_LSHOULDER, ps2x.Button(PSB_L1)); // Left Shoulder (LB) maps to L1
  XBox360.button(XBOX_RSHOULDER, ps2x.Button(PSB_R1)); // Right Shoulder (RB) maps to R1
  XBox360.button(XBOX_Y, ps2x.Button(PSB_TRIANGLE)); // Y maps to Triangle (△)
  XBox360.button(XBOX_B, ps2x.Button(PSB_CIRCLE)); // B maps to Circle (O)
  XBox360.button(XBOX_A, ps2x.Button(PSB_CROSS)); // A maps to Cross (X)
  XBox360.button(XBOX_X, ps2x.Button(PSB_SQUARE)); // X maps to Square (□)

  // BYTES 2, 3, 4, 5
  int16_t leftStickXAxisMapped = map(ps2x.Analog(PSS_LX), 0, 255, -32768, 32767);
  int16_t leftStickYAxisMapped = map(ps2x.Analog(PSS_LY), 0, 255, 32767, -32768); // Y Axis is inverted
  int16_t rightStickXAxisMapped = map(ps2x.Analog(PSS_RX), 0, 255, -32768, 32767);
  int16_t rightStickYAxisMapped = map(ps2x.Analog(PSS_RY), 0, 255, 32767, -32768); // Y Axis is inverted
  // Apply deadzones because my controller is all over the place, change these as needed or disable if necessary
  if ((ps2x.Analog(PSS_LX) >= 60) && (ps2x.Analog(PSS_LX) <= 200)) {
    //leftStickXAxisMapped = map(127, 0, 255, -32768, 32767);
    leftStickXAxisMapped = 0;
  }
  if ((ps2x.Analog(PSS_LY) >= 60) && (ps2x.Analog(PSS_LY) <= 200)) {
    //leftStickYAxisMapped = map(127, 0, 255, 32767, -32768); // Y Axis is inverted
    leftStickYAxisMapped = 0; // Y Axis is inverted
  }
  if ((ps2x.Analog(PSS_RX) >= 60) && (ps2x.Analog(PSS_RX) <= 200)) {
    //rightStickXAxisMapped = map(127, 0, 255, -32768, 32767);
    rightStickXAxisMapped = 0;
  }
  if ((ps2x.Analog(PSS_RY) >= 60) && (ps2x.Analog(PSS_RY) <= 200)) {
    //rightStickYAxisMapped = map(127, 0, 255, 32767, -32768); // Y Axis is inverted
    rightStickYAxisMapped = 0; // Y Axis is inverted
  }
  //int16_t leftStickXAxisMapped = map(ps2x.Analog(PSS_LX), 0, 255, -32768, 32767);
  XBox360.X(leftStickXAxisMapped); // Left Stick X Axis maps to Left Stick X Axis
  //int16_t leftStickYAxisMapped = map(ps2x.Analog(PSS_LY), 0, 255, 32767, -32768); // Y Axis is inverted
  XBox360.Y(leftStickYAxisMapped); // Left Stick Y Axis maps to Left Stick Y Axis
  //int16_t rightStickXAxisMapped = map(ps2x.Analog(PSS_RX), 0, 255, -32768, 32767);
  XBox360.XRight(rightStickXAxisMapped); // Right Stick X Axis maps to Right Stick X Axis
  //int16_t rightStickYAxisMapped = map(ps2x.Analog(PSS_RY), 0, 255, 32767, -32768); // Y Axis is inverted
  XBox360.YRight(rightStickYAxisMapped); // Right Stick Y Axis maps to Right Stick Y Axis
  // And return the data!
  XBox360.send();
}
