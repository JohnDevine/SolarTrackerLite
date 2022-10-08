#ifndef jd_Sun_h
    #define jd_Sun_h
    #include "jd_global.h"
    #include "sundata.h"
    #include "jd_timeFunctions.h"
    struct jd_azel
    {
        float az; // Azmith in degrees
        float el; // Elevation in degrees
    };

    int getHoursFromDouble(double hours);
    int getMinutesFromDouble(double hours);

    // get the azmith and elevation for the sun given date,time and lat, long and time zone offset (Bangkok offset = 7)
    void jd_GetTrackerAzEl( jd_azel *az_el_out, jd_DateTime dateNtime, float lat, float lng );


#endif // jd_Sun_h