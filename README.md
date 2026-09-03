# Latent Gloves

![Demo](images/demo.gif)

Latent Gloves is a wearable Digital Musical Instrument (DMI) designed for the embodied exploration of neural audio latent spaces through hand gestures. The instrument combines a DIY glove-based controller, embedded sensor processing, and a neural audio synthesis engine based on the nn~ Max package to transform a vocal audio recording using a RAVE timbre transfer model.

This instrument was used in performances at:
* Interactive Digital Multimedia Techniques (IDMT) module concert from the Sound and Music Computing MSc at Queen Mary University of London, Dec 2025.
* AI Playground at Sónar+D, Barcelona, Jun 2026.
* NIME LATAM Gathering, at NIME 2026, London, Jun 2026.


## Build your own glove

[Read the building guide](how_to_build_your_own_glove.md)

TODO: Update with new prototype and lessons learned.


## Software Requirements

![alt text](images/Max%20Patch.png)

* Max 8 or later
* Arduino IDE
* [nn~ Max Package](https://forum.ircam.fr/projects/detail/nn/)
* Download pretrained RAVE model [models by Shuoyang Zheng](https://huggingface.co/shuoyang-zheng/jaspers-rave-models/blob/main/gtsinger_b2048_r44100_z16_noncausal.ts)
* Place downloaded model in nn~ models folder


## Instructions for playing the instrument

### Setup
1. Put the glove on and connect it via to the PC via USB

2. Connect MIDI Interface to PC (if additional control is wanted)

3. Open `Arduino/Latent_gloves/Latent_gloves.ino` on Arduino IDE.
   
4. Install the `Adafruit MPU6050` and `OCS` Libraries, and the drivers for `Ardino Nano ESP32` boards in the Boards manager.

5. Click Upload. _You only need to do this one time to load the code on the board, the following times it will already work once you plug it in._

7. (Calibration) While holding the hand in a neutral, horizontal position with fingers extended and the palm facing the floor, press the reset button. The LED will start bliking. Wait until the blinking stops.

8. Open the Serial Monitor to check if the calibration ran without errors, you should be seeing the values of flexion in each finger and the pitch and roll angles updating.

9. Close Serial Monitor.

### Play

For playing, assuming everything is working correctly, you will only need to plug the glove, as the program is already saved in the board. Remember that everytime the board is turned on the calibration is triggered, so act accordingly, or manually trigger the calibration by pressing the reset button on the board.

1. Connect Glove to PC

2. Wait for Calibration to finish (wait until the blinking stops)

3. Ensuring the Serial Port is closed, open `Max/Latent Gloves.maxpat` Max Patch in Max and follow instructions on screen.


## Acknowledgments
* Max Patch is `Max/Latent Gloves.maxpat` based on examples of acid-ircam's [nn~ Max Package](https://github.com/acids-ircam/nn_tilde)
* Pre-trained RAVE model downloaded from [Shuoyang Zheng's pre-trained RAVE models](https://huggingface.co/shuoyang-zheng/jaspers-rave-models)
* Max Patch test `Max/Tests/Latent Gloves - regression test.maxpat` is based on [FluCoMa's tutorial, Controlling a Synth using a Neural Network](https://learn.flucoma.org/learn/regression-neural-network/)
* Arduino MPU6050 code in `Arduino/Latent_gloves/Latent_gloves.ino` based on examples of [Adafruit's MPU6050 library](https://docs.arduino.cc/libraries/adafruit-mpu6050/)
* Arduino Kalman filter code in `Arduino/Latent_gloves/Latent_gloves.ino` based on [Kristian Lauszus (TKJ Electronics) implementation](https://github.com/TKJElectronics/KalmanFilter/blob/master/Kalman.cpp)
