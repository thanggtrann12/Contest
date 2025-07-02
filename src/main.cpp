#include "motors.h"
#include "PS2_controller.h"
#include <SPI.h>
void setup()
{
  Serial.begin(115200);
  initMotors();
  setupMovementController();
  Serial.println("Done setup!");
}

void loop()
{
  ps2x.read_gamepad(0, 0);
  MovementControl();
  LiftControl();
  BasketControl();
  // ballControll();
  // ballOut();
  delay(50);
}