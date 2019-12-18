// Inertial Monitoring Unit (IMU) using Mahony filter.
//
// To view this data, use the Arduino Serial Monitor to watch the
// scrolling angles, or run the OrientationVisualiser example in Processing.

#include <NXPMotionSense.h>
#include "MahonyAHRS.h"
#include <Wire.h>
#include <EEPROM.h>
#include "Mouse.h"
#include<stdlib.h>

#define IMU_SW_UPDATE_WAIT_TIME 3000 // to be able to change the SW after mouse mode activated
/* Anteile der Achsen an der g-Achse: */
#define MY_GX 0.85373f
#define MY_GY 0.06239f
#define MY_GZ 0.51697f
#define MAHONY_READ_ARGS(m)  m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8] // ax, ay, az, gx, gy, gz, mx, my, mz
#define MAHONY_UPDATE_ARGS(m)  m[3], m[4], m[5], m[0], m[1], m[2], m[6], m[7], m[8] // gx, gy, gz, ax, ay, az, mx, my, mz
#define MAHONY_UPDATE_APPROX_ARGS(m)  m[4], m[5], m[3], m[1], m[2], m[0], m[7], m[8], m[6] // gy, gz, gx, ay, az, ax, my, mz, mx
#define TMP_PLOT_SCALER 20.0f // 1 = normal, more helps to see the plotter float variables easier
#define MAX_RPY_DELTA 10.0f // max delta between two roll/pitch/yaw samples in normal usage

NXPMotionSense imu;
Mahony filter;

static float calcDiff(float af_new, float af_old);
static void calcDeltaCounter(float * apf_rpyD, float * apf_rpyDC);
static void calcDiffTest(void);

void setup() {
  Serial.begin(115200); //9600
  imu.begin();
  filter.begin(100); // 100 measurements per second
}

void loop() {
  float matrix[9]; /* axyz, gxyz, mxyz */
  float rpy[3]; /* roll, pitch, yaw */
  static float rpyO[3] = {0.0f, 0.0f, -180.0f}; /* roll, pitch, yaw Old */
  float rpyD[3]; /* roll, pitch, yaw Delta */
  float rpyDC[3] = {0.0f, 0.0f, 1.0f}; /* roll, pitch, yaw Delta Counter */
  float filtA[3]; /* PT1 filtered acc values */
  float zeroPosA[3] = {MY_GX, MY_GY, MY_GZ};

  if (imu.available() && (millis() > IMU_SW_UPDATE_WAIT_TIME)) {
    imu.readMotionSensor(MAHONY_READ_ARGS(matrix));
    filtA[0] = matrix[0]; filtA[1] = matrix[1], filtA[2] = matrix[2]; /* create copy of axyz for filtering */
    FilterAccSensorValues(filtA, 130); /* filtA now contains low pass filtered a */
#define CORRECT_POS FALSE
#if CORRECT_POS
//    ZeroPosCorrection(matrix, filtA); /* matrix now contains zero-pos-aligned values */
    ZeroPosCorrection(matrix, zeroPosA); /* fixed zero-pos-aligned values */
        filter.update(MAHONY_UPDATE_ARGS(matrix)); // Teensys original
#else /* no position correction - approximation instead */
        filter.update(MAHONY_UPDATE_APPROX_ARGS(matrix)); // nearly my headset, less drift! Don't ZeroPosCorrection when using this!
#endif

    rpy[0] = - filter.getRoll();
    rpy[1] = - filter.getPitch();
    rpy[2] = - filter.getYaw();
    /**** calc delta: ****/
    rpyD[0] = calcDiff(rpy[0], rpyO[0]);
    rpyD[1] = calcDiff(rpy[1], rpyO[1]);
    rpyD[2] = calcDiff(rpy[2], rpyO[2]);
    //Serial.print(rpy[0]); Serial.print(","); Serial.print(rpy[1]); Serial.print(","); Serial.println(rpy[2]);
    //Serial.print(TMP_PLOT_SCALER*rpyD[0]); Serial.print(","); Serial.print(TMP_PLOT_SCALER*rpyD[1]); Serial.print(","); Serial.println(TMP_PLOT_SCALER*rpyD[2]);

    calcDeltaCounter(rpyD, rpyDC);
    // Weiter gehts so:
// counter ausgeben, der hochzaehlt, solange aufeinanderfolgende deltas kontinuierlich steigen oder fallen und der
// zurueckgesetzt wird, wenn die Kontinuitaet unterbrochen wird --> mit dem signal evtl den delta output verstaerken


    //Serial.print(rpyDC[0]); Serial.print(","); Serial.print(rpyDC[1]); Serial.print(","); Serial.println(rpyDC[2]);

    /**** set old for next run: ****/
    rpyO[0] = rpy[0];
    rpyO[1] = rpy[1];
    rpyO[2] = rpy[2];


// mal die warnings anschaun und testen, ob das Auswirkungen hat...

    //!!!!!!!!!!!!!! VOR DEM UPLOAD DEN PLOTTER SCHLIESSEN!!!!!!!!!!!!!!!!!!!
    // Seriellen Port (Teensy) OHNE Keyboard, Mouse etc waehlen!!!
  }
}

