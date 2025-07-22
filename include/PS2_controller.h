#include <PS2X_lib.h>
PS2X ps2x;

// Calibration
#define X_JOY_CALIB 127
#define Y_JOY_CALIB 128

// PS2 pins
#define PS2_DAT 12
#define PS2_CMD 13
#define PS2_SEL 15
#define PS2_CLK 14

// Speed settings
#define TOP_SPEED 4095
#define NORM_SPEED 2048

int current_speed = 0;
const int ACCEL_INCREMENT = 100;
const int ACCEL_INTERVAL = 50;
unsigned long lastAccelTime = 0;

int smoothed_c1 = 0, smoothed_c2 = 0, smoothed_c3 = 0, smoothed_c4 = 0;

const int SMOOTH_STEP_UP = 80;
const int SMOOTH_STEP_DOWN = 300;

int smoothApproach(int current, int target);

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
    int target_speed = TOP_SPEED;

    int nJoyX = ps2x.Analog(PSS_RY) - X_JOY_CALIB;
    int nJoyY = ps2x.Analog(PSS_RX) - Y_JOY_CALIB;

    int nMotMixL = 0, nMotMixR = 0;

    if (nJoyX == -1 && nJoyY == 0)
    {
        setDirectionMotors(0, 0, 0, 0);
        return 0;
    }

    bool turningRight = (nJoyY * nJoyX > 0);
    if (nJoyX)
    {
        nMotMixL = -nJoyX + (nJoyY * turningRight);
        nMotMixR = nJoyX + (nJoyY * !turningRight);
    }
    else
    {
        nMotMixL = nJoyY;
        nMotMixR = nJoyY;
    }

    unsigned long now = millis();
    if (now - lastAccelTime >= ACCEL_INTERVAL)
    {
        if (current_speed < target_speed)
        {
            current_speed += ACCEL_INCREMENT;
            if (current_speed > target_speed)
                current_speed = target_speed;
        }
        else if (current_speed > target_speed)
        {
            current_speed -= ACCEL_INCREMENT;
            if (current_speed < target_speed)
                current_speed = target_speed;
        }
        lastAccelTime = now;
    }

    int target_c1 = 0, target_c2 = 0, target_c3 = 0, target_c4 = 0;

    if (nMotMixR > 0)
    {
        target_c3 = map(nMotMixR, 0, 128, 0, current_speed);
    }
    else if (nMotMixR < 0)
    {
        target_c4 = map(abs(nMotMixR), 0, 128, 0, current_speed);
    }

    if (nMotMixL > 0)
    {
        target_c1 = map(nMotMixL, 0, 128, 0, current_speed);
    }
    else if (nMotMixL < 0)
    {
        target_c2 = map(abs(nMotMixL), 0, 128, 0, current_speed);
    }

    smoothed_c1 = smoothApproach(smoothed_c1, target_c1);
    smoothed_c2 = smoothApproach(smoothed_c2, target_c2);
    smoothed_c3 = smoothApproach(smoothed_c3, target_c3);
    smoothed_c4 = smoothApproach(smoothed_c4, target_c4);

    setDirectionMotors(smoothed_c1, smoothed_c2, smoothed_c3, smoothed_c4);
    return 1;
}

int smoothApproach(int current, int target)
{
    int step = (target == 0) ? SMOOTH_STEP_DOWN : SMOOTH_STEP_UP;

    if (current < target)
    {
        current += step;
        if (current > target)
            current = target;
    }
    else if (current > target)
    {
        current -= step;
        if (current < target)
            current = target;
    }
    return current;
}

bool nang_ha_tay_nang()
{
    int speed = TOP_SPEED;
    int c1 = 0, c2 = 0;

    int nJoyY = ps2x.Analog(PSS_LY) - X_JOY_CALIB;
    if (nJoyY == -1)
    {
        setLiftMotors(0, 0);
        return false;
    }

    const int DEADZONE = 10;
    if (nJoyY < -DEADZONE)
    {
        c1 = speed;
        c2 = 0;
    }
    else if (nJoyY > DEADZONE)
    {
        c1 = 0;
        c2 = speed;
    }
    else
    {
        c1 = 0;
        c2 = 0;
    }

    setLiftMotors(c1, c2);
    return true;
}

bool nang_ha_ro_bong()
{
    int speed = TOP_SPEED;
    int c1 = 0, c2 = 0;

    if (ps2x.Button(PSB_R1))
    {
        c1 = speed;
        c2 = 0;
    }
    else
    {
        c1 = 0;
        c2 = 0;
    }
    setBasketMotors(c1, c2);
    return 1;
}

bool tang_giam_chieu_dai_ro_dung_bong()
{
    if (ps2x.Button(PSB_GREEN))
    {
        servoForward(); // giữ nút xanh lá thì quay xuôi
    }
    else if (ps2x.Button(PSB_BLUE))
    {
        servoBackward(); // giữ nút xanh dương thì quay ngược
    }
    else
    {
        servoStop(); // không giữ gì thì dừng
    }
    return true;
}

bool mo_cang_chan()
{
    if (ps2x.Button(PSB_PAD_UP))
    {
        setServoAngle(0); // giữ nút xanh lá thì quay xuôi
    }
    else if (ps2x.Button(PSB_PAD_DOWN))
    {
        setServoAngle(90); // giữ nút xanh dương thì quay ngược
    }
    return true;
}
