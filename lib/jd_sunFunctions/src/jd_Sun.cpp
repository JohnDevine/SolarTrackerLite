#include "jd_Sun.h"

/** Calculate the azmith
 * Given:
 * Latitude
 * Longitude
 * Date (assume set correctly)
 * Time (assume set correctly)
 * NOTE .. the servo can only swing 180 degrees and at times the angle between
 * sunrise and sunset is > 180. So to allow for this allow for even non-movement for sunrise
 * and sunset. EG if sunrise is 84 degrees and sunset is 277 travel is 277-84=193 degrees. So split non-movement
 * between sunrise and sunset ie (193-180)/2=6.5 so traverse to 0 for sunrise and do not move till degree > 6.5
 * For sunset when degree > 179 just go to 179.
 */
int getHoursFromDouble(double hours)
{
  int m = int(round(hours * 60));
  int hr = (m / 60) % 24;
  int mn = m % 60;
  DUMP(hr);
  return hr;
}
int getMinutesFromDouble(double hours)
{
  int m = int(round(hours * 60));
  int hr = (m / 60) % 24;
  int mn = m % 60;
  DUMP(mn);
  return mn;
}
// get the azmith and elevation for sunrise and sunset for today
// set is_sunrise to true for sunrise else false to get sunset

/*
void Sun_GetServoAzelSunriseSunset(bool is_sunrise, jd_azel *az_el_out, double lat, double lng)
{

  double transit, sunrise, sunset; // Event times, in hours (UTC)
  double az, el;                   // Horizontal coordinates, in degrees

  // Get current date

  uint16_t year = myTZ.year();
  uint8_t month = myTZ.month();
  uint8_t day = myTZ.day();

  uint8_t hour;
  uint8_t minute;
  uint8_t second = 0;

  time_t utc;

  // Calculate the times of sunrise, transit, and sunset, in hours (UTC)
  calcSunriseSunset(year, month, day, lat, lng, transit, sunrise, sunset);

  // Get Azmith at sunrise
  if (is_sunrise)
  {
    hour = getHoursFromDouble(sunrise);
    minute = getMinutesFromDouble(sunrise);
    second = 0;
  }
  else
  {
    hour = getHoursFromDouble(sunset);
    minute = getMinutesFromDouble(sunset);
    second = 0;
  }

  DUMP(year);
  DUMP(month);
  DUMP(day);
  DUMP(hour);
  DUMP(minute);
  DUMP(second);
  DUMP(lat);
  DUMP(lng);

  utc = makeTime(hour, minute, second, day, month, year);
  calcHorizontalCoordinates(utc, lat, lng, az, el);

  DUMP(az);
  DUMP(el);
  az_el_out->az = az;
  az_el_out->el = el;
}
*/
/* 
Note that this takes into account a start from sunrise being at 0 degrees for the unit so outputs actual 
angle with sunrise at 0 degrees.
*/
void jd_GetTrackerAzEl( jd_azel *az_el_out, jd_DateTime dateNtime, float lat, float lng )
{

  float jd_sunrise_time;
  int jd_sunrise_hr;
  int jd_sunrise_min;
  int jd_az_sunrise;
  int jd_el_sunrise;
  float jd_working_offset;
  //****** NOTE sundata uses the offset the reverse sign of ezTime so it is * -1 in the below call. Also it comes in
  //** as minutes and needs to be hours
  jd_working_offset = (dateNtime.offset_gmt * -1) / 60;  // reverse sign and convert to hours
  //sundata jd_sundata = sundata(lat, lng, dateNtime.offset_gmt * -1);  // Create object with this lat, long and time offset 
  sundata jd_sundata = sundata(lat, lng, jd_working_offset);  // Create object with this lat, long and time offset 
 
  // Get angles for current time
  jd_sundata.time(dateNtime.yr,dateNtime.mnth, dateNtime.day, dateNtime.hr, dateNtime.min, dateNtime.sec); // insert year, month, day, hour, minutes and seconds
  jd_sundata.calculations();  // update calculations for last inserted time (inc sunrise)
  az_el_out->az = jd_sundata.azimuth_deg();
  az_el_out->el = jd_sundata.elevation_deg();
  
   // work out sunrise angle            
   jd_sunrise_time = jd_sundata.sunrise_time();
   jd_sunrise_hr = getHoursFromDouble(jd_sunrise_time);
   jd_sunrise_min = getMinutesFromDouble(jd_sunrise_time);
   jd_sundata.time(dateNtime.yr,dateNtime.mnth, dateNtime.day, jd_sunrise_hr, jd_sunrise_min, 0); // insert year, month, day, hour, minutes and seconds
  jd_sundata.calculations();              // update calculations for last inserted time

  jd_az_sunrise = jd_sundata.azimuth_deg();
  jd_el_sunrise = jd_sundata.elevation_deg();

  // Now calculate and return  sun degrees for tracker if sunrise id 0 degrees for tracker

  az_el_out->az = az_el_out->az - jd_az_sunrise;
  az_el_out->el = az_el_out->el - jd_el_sunrise;



  // jd_sundata.time(2022,10, 7, 13, 43, 0); // insert year, month, day, hour, minutes and seconds
  // jd_sundata.time(dateNtime.yr,dateNtime.mnth, dateNtime.day, dateNtime.hr, dateNtime.min, dateNtime.sec); // insert year, month, day, hour, minutes and seconds
  // jd_sundata.calculations();              // update calculations for last inserted time

  DUMP(dateNtime.yr);
  DUMP(dateNtime.mnth);
  DUMP(dateNtime.day);
  DUMP(dateNtime.hr);
  DUMP(dateNtime.min);
  DUMP(dateNtime.sec);
  DUMP(jd_working_offset);
  DUMP(jd_az_sunrise);
  DUMP(jd_el_sunrise);
  DUMP(jd_sunrise_time);
  DUMP(jd_sunrise_hr);
  DUMP(jd_sunrise_min);
  DUMP(az_el_out->az);
  DUMP(az_el_out->el);

  return;
}
