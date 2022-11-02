// Timing related functions
#ifndef jd_servo
    #define jd_servo

    #include "jd_global.h"
    #include "jd_timing.h"
    #include "ServoEasing.h"


    void jd_test_servo(ServoEasing *servo, int wait_for_physical, int servo_speed, int start_angle, int end_angle, int steps);

#endif // jd_servo end 