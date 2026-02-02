# Latent Gloves

## Introduction


## Build your own glove

[Read the building guide](how_to_build_your_own_glove.md)

<img src="images/cabled_version.jpg" alt="Cabled version" width="400">

## Software Requirements

![alt text](images/Max%20Patch.png)

* Max 8 or later
* Arduino IDE
* Install [nn~ Max Package](https://forum.ircam.fr/projects/detail/nn/)
* Download pretrained RAVE model [models by Shuoyang Zheng](https://huggingface.co/shuoyang-zheng/jaspers-rave-models/blob/main/gtsinger_b2048_r44100_z16_noncausal.ts)
* Place downloaded model in nn~ models folder



## Instructions for playing the instrument

1. Put the glove on and connect via USB to PC

2. Connect MIDI Interface to PC (if additional control is wanted)

3. Open code on Arduino IDE

4. Run code while the hand is in a neutral position

5. Open the Serial Monitor to check if the calibration was done correctly

6. Close Serial Monitor

7. Open Max Patch and follow instructions

Steps 3-6 can be bypassed once the code is already stored in the Arduino. 
In that case, pressing the reset button on the board while holding the hand in a neutral position is advised.
