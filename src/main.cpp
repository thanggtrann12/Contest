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
  nang_ha_tay_nang();
  nang_ha_ro_bong();
  tang_giam_chieu_dai_ro_dung_bong();
  mo_cang_chan();
  delay(50);
}