# imu-headset-repo
ergonomic and efficient cursor control with AHRS / IMU
# DESCRIPTION
The mouse as a HMI to the PC is annoying me: You always have to change the position of your hand from the keyboard to the mouse and maybe move it to find where the cursor currently is - this slows down my working speed and is "only" annoying. More important, after x hours of intensive work with the mouse you feel that your arm does not like it and it can lead to CTD, RSI syndrom etc..
This headset uses your head's orientation as HMI input so your hands can stay on the keyboard and you can control the cursor at the same time!
# DETAILS
There are alternatives to the normal mouse, e.g. the gyro mouse (cheap, but blocks one hand), the roller mouse / bar mouse (very expensive, but comes very close to what I want) or the trackball (very close to what I want, but way too hard to control the cursor because of the limited movement range / sensibility).

The objective of this project is to develop a headset, which may include microphone and earphone and a sensor to read the orientation (e.g. gyroscope, or gyro + accelerometer + magnetometer). The coursor should be controlled by moving the head up/down and left/right. There are different approaches to realize the click buttons etc..

The advantages of this HMI are exact positioning (because of huge movement angles compared to e.g. the size of a mousepad) AND fast movement (because of no keyboard-to-mouse-switching-time). In addition, this device can be used by people that are not able to use their hands to control the cursor.
# Prototypes
## IMU board prototype
### 1st
Teensy + Prop Shield --> 40 $, but easy to come to a first working prototype quickly
### 2nd
to adapt the results for cheaper parts, e.g. Arduino Micro + MPU 9250 breakout board
### 3rd
custom PCB with smaller form factor (for easy wearing) based on previous results
## lick-by-keyboard prototype
### 1st
Autohotkey script for using keyboard shortcuts to make left-/rightclicks
### 2nd
should fix the bug when e.g. selecting multiple files via rectangle

## audio+click board prototype

## finger-ring IMU board prototype

# Roadmap
currently todo: 
* develop filter algorithm to minimize drift and noise
