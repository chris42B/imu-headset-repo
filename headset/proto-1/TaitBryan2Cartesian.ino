/*
Tait-Bryan to cartesian converter: The yaw, pitch and roll angles provided
by an IMU used by a person are converted to X, Y coordinates to control
a pointer or pointing device for a display.

    Copyright (C) 2018  ChrisB (github.com/chris42B) (yoga@chris-brand.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	*/
#define MOUSE_FACTOR_X 100.0f//42.0f // factor to increase effect of movement
#define MOUSE_FACTOR_Y 100.0f//42.0f // factor to increase effect of movement
#define MOUSE_MOV_OFFSET 0.075f

/**
 * @brief Converts Tait-Bryan (roll-pitch-yaw angles into cartesian (x, y) coordinates
 *
 * @param apf_in Pointer to float array which contains ax, ay, az, gx, gy, gz, mx, my, mz
 * @param apu8_out X, y coordinates for the mouse cursor
 * @return void
 */
void TaitBryan2Cartesian(float * apf_in, float * apf_a0, uint8_t * apu8_out)
{

//    Mouse.move(mouseX, mouseY);


}

/**
 * @brief Calculates difference of two angles - taking care of steps at the angle limits
 *
 * @param af_new New / current angle
 * @param af_old Last angle value
 * @return float The difference of old and new angle
 */
static float calcDiff(float af_new, float af_old)
{
  float lf_d;
  if (af_old < (-180.0f + MAX_RPY_DELTA) && af_new > (180.0f - MAX_RPY_DELTA)) // -180 / 180
  {
    lf_d = af_new - af_old - 360.0f; // tested
  }
  else if (af_old > (180.0f - MAX_RPY_DELTA) && af_new < (-180.0f + MAX_RPY_DELTA)) // 180 / -180
  {
    lf_d = 360.0f - af_old + af_new; // tested
  }
  else if (af_old < (0.0f + MAX_RPY_DELTA) && af_new > (360.0f - MAX_RPY_DELTA)) // 0 / 360
  {
    lf_d = - 360.0f - af_old + af_new; // tested
  }
  else if (af_old > (360.0f - MAX_RPY_DELTA) && af_new < (0.0f + MAX_RPY_DELTA)) // 360 / 0
  {
    lf_d = - af_old + af_new + 360.0f; // tested
  }
  else if (af_old < (-360.0f + MAX_RPY_DELTA) && af_new > (0.0f - MAX_RPY_DELTA)) // -360 / -0
  {
    lf_d = - 360.0f - af_old + af_new; // tested
  }
  else if (af_old > (0.0f - MAX_RPY_DELTA) && af_new < (-360.0f + MAX_RPY_DELTA)) // -0 / -360
  {
    lf_d = - af_old + af_new + 360.0f; // tested
  }
  else // normal case
  {
    lf_d = af_new - af_old;
  }
  return lf_d;
}

/**
 * @brief Counts how trustworthy a signal is to filter out drifting effects
 *
 * @param apf_rpyD Input matrix with roll, pitch, yaw delta values
 * @param apf_rpyDC Output matrix containing how trustworthy the signal is
 * @return void
 */
static void calcDeltaCounter(float * apf_rpyD, float * apf_rpyDC)
{
  /*
  static float rpyDOld = 0.0f;
  if (abs(apf_rpyD[2]) > abs(rpyDOld))
  {
    apf_rpyDC[1] += (abs(apf_rpyD[2]) - abs(rpyDOld)) * 1.75f; // groeße der diff beachten
  }
  else
  {

    apf_rpyDC[1] *= 0.75f; 
  }

  
  rpyDOld = apf_rpyD[2];
*/
  float lf_simpleMap = apf_rpyD[2];
  if (lf_simpleMap > MOUSE_MOV_OFFSET)
  {
    lf_simpleMap -= MOUSE_MOV_OFFSET;
  }
  else if (lf_simpleMap < -MOUSE_MOV_OFFSET)
  {
    lf_simpleMap += MOUSE_MOV_OFFSET;
  }
  else
  {
    lf_simpleMap = 0.0f;
  }
  lf_simpleMap *= MOUSE_FACTOR_X;
// Besser: den mean() über die letzte Zeit x von apf_rpyD machen (oder TP-filtern?) und die Entwicklung prognostizieren. 
// Wenn sich der Wert mit einer gewissen Langsamkeit Richtung 0 bewegt, driftet der Sensor vmtl gerade --> Signal ignorieren
/*
  bis 0,005 stillstand
  0,02 soll merkbar sein

  0,01 --> 1, d.h. Faktor 100
  0,1 --> Faktor 42...100
*/
  
Serial.print(TMP_PLOT_SCALER*apf_rpyD[2]); Serial.print(","); Serial.println(lf_simpleMap);
}

static void calcDiffTest(void) // testfunction for calcDiff
{
  const float list[4] = {-4.9, -0.5, -359.3, -358.0};
  float fres;
  static int i = 0;
  if (i < 3) { 
  fres = calcDiff(list[i+1], list[i]);
  Serial.print(list[i]); Serial.print(" --> "); Serial.print(list[i+1]); Serial.print(": D = "); Serial.println(fres);
  delay(5);
  i++;
  }
}
