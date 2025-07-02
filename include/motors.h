#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define MIN_PWM 0
#define MAX_PWM 4095

#define SERVO_MIN_PULSE 150 // Pulse for 0 degrees
#define SERVO_MAX_PULSE 400 // Pulse for 180 degrees

// Motor direction PWM channels
#define PWM_CHANNEL1 8
#define PWM_CHANNEL2 9
#define PWM_CHANNEL3 10
#define PWM_CHANNEL4 11

// Lift motor PWM channels
#define PWM_CHANNEL5 12
#define PWM_CHANNEL6 13

// Basket motor PWM channels
#define PWM_CHANNEL7 14
#define PWM_CHANNEL8 15

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setDirectionMotors(int c1, int c2, int c3, int c4)
{
    char dbg_str[64];
    snprintf(dbg_str, sizeof(dbg_str), "setDirectionMotors - C1: %d\tC2: %d\tC3: %d\tC4: %d", c1, c2, c3, c4);
    Serial.println(dbg_str);

    pwm.setPin(PWM_CHANNEL1, c1);
    pwm.setPin(PWM_CHANNEL2, c2);
    pwm.setPin(PWM_CHANNEL3, c3);
    pwm.setPin(PWM_CHANNEL4, c4);
}

void setLiftMotors(int c1, int c2)
{
    char dbg_str[64];
    snprintf(dbg_str, sizeof(dbg_str), "setLiftMotors - C1: %d\tC2: %d", c1, c2);
    Serial.println(dbg_str);

    pwm.setPin(PWM_CHANNEL5, c1);
    pwm.setPin(PWM_CHANNEL6, c2);
}

void setBasketMotors(int c1, int c2)
{
    char dbg_str[64];
    snprintf(dbg_str, sizeof(dbg_str), "setBasketMotors - C1: %d\tC2: %d", c1, c2);
    Serial.println(dbg_str);

    pwm.setPin(PWM_CHANNEL7, c1);
    pwm.setPin(PWM_CHANNEL8, c2);
}

void initMotors()
{
    Wire.begin();
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(50);
    Wire.setClock(400000);

    setDirectionMotors(0, 0, 0, 0);
    setLiftMotors(0, 0);
    setBasketMotors(0, 0);
}

void setServoAngle(uint8_t channel, float angle)
{
    // Clamp angle
    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;

    // Map angle to pulse length (0° = 150, 180° = 400)
    uint16_t pulse = map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
    pwm.setPWM(channel, 0, pulse);
}

void setServoRotate(uint8_t channel, uint16_t pulse)
{
    pwm.setPWM(channel, 0, pulse);
}