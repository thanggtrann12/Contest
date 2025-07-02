#include <PS2X_lib.h>

PS2X ps2x; // create PS2 Controller Class object
// #define BEBUG_CTRL

// calibration for different kinds of PS2 controller, this value only suitable for the PS2 controller comes with VRC2023 K12 Maker kit
#define X_JOY_CALIB 127
#define Y_JOY_CALIB 128

#define PS2_DAT 12 // MOSI  19
#define PS2_CMD 13 // MISO  23
#define PS2_SEL 15 // SS     5
#define PS2_CLK 14 // SLK   18

#define TOP_SPEED 4095
#define NORM_SPEED 2048
#define TURNING_FACTOR 1

#define SERVO_CHANNEL_2 2 // Only use servo on channel 2
#define SERVO_CHANNEL_3 3 // Only use servo on channel 3

#define SERVO_STOP 300    // Neutral pulse (stop)
#define SERVO_FORWARD 420 // Forward speed

#define SINGLE_HAND_DRIVING 0
#define TWO_HAND_DRIVING 1
bool driving_mode = SINGLE_HAND_DRIVING;
void setupMovementController()
{
    int err = -1;
    while (err != 0)
    {
        err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, true);
    }
}
bool MovementControl()
{
    int speed = NORM_SPEED;
    // if (ps2x.Button(PSAB_PAD_UP))
    //     speed = TOP_SPEED;
    if (ps2x.ButtonPressed(PSB_SELECT))
        driving_mode = !driving_mode;

    int nJoyX = (ps2x.Analog(PSS_RY) - X_JOY_CALIB); // forward/backward
    int nJoyY = (ps2x.Analog(PSS_RX) - Y_JOY_CALIB); // turning

    int nMotMixL; // Motor (left) mixed output
    int nMotMixR; // Motor (right) mixed output

    if (nJoyX == -1 && nJoyY == 0) // in case of lost connection with the wireless controller, only used in VRC2023 PS2 wireless controller
    {
        setDirectionMotors(0, 0, 0, 0);
        return 0;
    }

    bool temp = (nJoyY * nJoyX > 0);
    if (nJoyX) // Turning
    {
        nMotMixL = -nJoyX + (nJoyY * temp);
        nMotMixR = nJoyX + (nJoyY * !temp);
    }
    else // Forward or Reverse
    {
        nMotMixL = nJoyY;
        nMotMixR = nJoyY;
    }
#ifdef BEBUG_CTRL
    Serial.print(F("Calculated value from joystick: "));
    Serial.print(nJoyY);
    Serial.print("\t");
    Serial.println(nJoyX);
#endif
    int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

    if (nMotMixR > 0)
    {
        c3 = nMotMixR;
        c3 = map(c3, 0, 128, 0, speed);
    }

    else if (nMotMixR < 0)
    {
        c4 = abs(nMotMixR) + 1;
        c4 = map(c4, 0, 128, 0, speed);
    }

    if (nMotMixL > 0)
    {
        c1 = nMotMixL;
        c1 = map(c1, 0, 128, 0, speed);
    }
    else if (nMotMixL < 0)
    {
        c2 = abs(nMotMixL);
        c2 = map(c2, 0, 128, 0, speed);
    }
    setDirectionMotors(c1, c2, c3, c4);
    return 1;
}

bool LiftControl()
{
    int speed = NORM_SPEED;
    int c1 = 0, c2 = 0;

    // Get joystick vertical position (Y-axis)
    int nJoyY = ps2x.Analog(PSS_LY) - X_JOY_CALIB;

    // Check if controller disconnected
    if (nJoyY == -1)
    {
        setLiftMotors(0, 0);
        return false;
    }

    // Deadzone to avoid drift
    const int DEADZONE = 10;

    if (nJoyY < -DEADZONE) // Stick pushed up
    {
        c1 = speed;
        c2 = 0;
    }
    else if (nJoyY > DEADZONE) // Stick pushed down
    {
        c1 = 0;
        c2 = speed;
    }
    else // Stick in neutral position
    {
        c1 = 0; // Stop lift
        c2 = 0;
    }
    setLiftMotors(c1, c2);
    return true;
}

bool BasketControl()
{
    int speed = NORM_SPEED / 2;
    int c1 = 0, c2 = 0;

    if (ps2x.Button(PSB_PAD_UP))
    {
        c1 = speed; // Basket up
        c2 = 0;
    }
    else if (ps2x.Button(PSB_PAD_DOWN))
    {
        c1 = 0;
        c2 = speed; // Basket down
    }
    else
    {
        c1 = 0; // Stop basket
        c2 = 0;
    }
    setBasketMotors(c1, c2);
    return 1;
}

int angle = 180;
// bool ballControll()
// {
//     if (ps2x.Button(PSB_GREEN))
//     {
//         angle -= 10;
//         Serial.println(angle);
//         setServoAngle(SERVO_CHANNEL_2, angle);
//     }
//     else
//     {
//         angle = 180;
//         setServoAngle(SERVO_CHANNEL_2, 0);
//     }
//     return 1;
// }

// bool ballOut()
// {
//     if (ps2x.Button(PSB_PAD_UP))
//     {
//         setServoRotate(SERVO_CHANNEL_3, SERVO_FORWARD);
//     }
//     else
//     {
//         setServoRotate(SERVO_CHANNEL_3, SERVO_STOP);
//     }
//     return 1;
// }