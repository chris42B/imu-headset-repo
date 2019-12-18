/*
Zero-position correction: The acc values of a wearable IMU are taken 
to determine the standard orientation of this device (= zero position) 
to change the IMU-ICs inertial system to how the device is worn.

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
#define OFFS_X 3 * lu8_i + 0
#define OFFS_Y 3 * lu8_i + 1
#define OFFS_Z 3 * lu8_i + 2
#define FILTER_DEPTH_FACT 42.0f /* increase strength by this factor */
/**
 * @brief Considers the zero position of the ECU in space to correct the roll, pitch and yaw
 *
 * @param apf_in Pointer to float array which contains ax, ay, az, gx, gy, gz, mx, my, mz
 * @param apf_a0 Values of acceleration sensor x, y, z in zero-position
 * @return void
 */
void ZeroPosCorrection(float * apf_in, float * apf_a0)
{
    float lpf_tmp[9];
    uint8_t lu8_i;
    for (lu8_i = 0; lu8_i < 9; lu8_i++) /* for all */
    {
        lpf_tmp[lu8_i] = apf_in[lu8_i]; /* copy in tmp buffer */
    }
    for (lu8_i = 0; lu8_i < 3; lu8_i++) /* for all 3 DOFs / sensors */
    {
        apf_in[OFFS_X] = (apf_a0[0] * apf_a0[0] + apf_a0[2] * apf_a0[2]) * lpf_tmp[OFFS_Y] - apf_a0[1] * apf_a0[1] * lpf_tmp[OFFS_Z]; /* (aï¿½+cï¿½)y-bï¿½z */
        apf_in[OFFS_Y] = (apf_a0[1] * apf_a0[1] + apf_a0[2] * apf_a0[2]) * lpf_tmp[OFFS_X] - apf_a0[0] * apf_a0[0] * lpf_tmp[OFFS_Z]; /* (bï¿½+cï¿½)x-aï¿½z */
        apf_in[OFFS_Z] = apf_a0[0] * apf_a0[0] * lpf_tmp[OFFS_X] + apf_a0[1] * apf_a0[1] * lpf_tmp[OFFS_Y] + apf_a0[2] * apf_a0[2] * lpf_tmp[OFFS_Z]; /* aï¿½x+bï¿½y+cï¿½z */
    }

}

/**
 * @brief Low pass filters the acc values to calc the zero pos
 *
 * @param apf_in Values of acceleration sensor x, y, z in zero-position
 * @param au8_filterStrength How strong the filter should be
 * @return void
 */
void FilterAccSensorValues(float * apf_in, uint8_t au8_filterStrength)
{
    static float lf_old[3] = {0.0f};
    uint8_t lu8_i;
    if (lf_old[0] == 0.0f && lf_old[2] == 0.0f && lf_old[3] == 0.0f) /* first run */
    {
        for (lu8_i = 0; lu8_i < 3; lu8_i++) /* for all dimensions x, y, z */
        {
            lf_old[lu8_i] = apf_in[lu8_i]; /* for good start value */
        }
    }
    for (lu8_i = 0; lu8_i < 3; lu8_i++) /* for all dimensions x, y, z */
    {
        apf_in[lu8_i] = (apf_in[lu8_i] - lf_old[lu8_i]) / (FILTER_DEPTH_FACT * (float)au8_filterStrength + 1.0f) + lf_old[lu8_i];
    }
    for (lu8_i = 0; lu8_i < 3; lu8_i++) /* for all dimensions x, y, z */
    {
        lf_old[lu8_i] = apf_in[lu8_i]; /* for next run */
    }
//Serial.print(10.0f*apf_in[0]); Serial.print(","); Serial.print(10.0f*apf_in[1]); Serial.print(","); Serial.println(10.0f*apf_in[2]);
}
