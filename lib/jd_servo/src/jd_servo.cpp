// Servo related functions

#include "jd_servo.h"

// Do a test for the servo
// Params
//      servo = servo easing oibject that has been setup and initialised beforehand
//      wait_for_physical = time in ms to wait for function to complete
//      servo_speed = sweep speed in degrees per second
//      start_angle = 0->360 (180) to start stepping at
//      end_angle = 0->360 (180) to end stepping at
//      steps = number of steps. 0 = do nothing,  1 = do start, 2 is start + end, 3 is start + middle + end etc.
void jd_test_servo(ServoEasing *servo, int wait_for_physical, int servo_speed, int start_angle, int end_angle, int steps)
{
    //******** Test servo
    int step_size = 0;
    int steps_to_do = 0;

    // If no steps just exit
    if (steps <= 0)
    {
        return;
    }

    // if Start > End angle just bail out
    if (start_angle > end_angle)
    {
        return;
    }
    // if steps == 1 or start == end angle just go to start and bail
    if (steps == 1 || start_angle == end_angle)
    {
        servo->easeTo(start_angle, servo_speed);
        jd_delay(wait_for_physical);
        return;
    }

    // If steps == 2 do start and then end and bail
    if (steps == 2)
    {
        servo->easeTo(start_angle, servo_speed);
        jd_delay(wait_for_physical);
        servo->easeTo(end_angle, servo_speed);
        jd_delay(wait_for_physical);
        return;
    }

    // calculate step size in degrees
    step_size = (end_angle - start_angle) / (steps - 1);
    steps_to_do = steps - 2; // drop beginning and end steps

    // If step_size too small or no steps go to end and exit
    if (step_size <= 0 || steps_to_do <= 0)
    {
        servo->easeTo(end_angle, servo_speed);
        jd_delay(wait_for_physical);
        return;
    }
    // loop by stepping through but not the last step as we will go to precise end_angle for that
    for (int i = 1; i <= steps_to_do; i++)
    {
        servo->easeTo(start_angle + i * step_size, servo_speed);
        jd_delay(wait_for_physical);
    }
    // finalise by going to end angle
    servo->easeTo(end_angle, servo_speed);
    jd_delay(wait_for_physical);
    return;

    //******** End of Test servo
}