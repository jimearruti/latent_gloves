# How to build your own glove

## Required materials
- One glove (Suggested: lightweight material such as cotton, to not produce too much heat). [e.g. Migliore Wear Cotton Gloves for Eczema - 2 pairs £8.99](https://www.amazon.co.uk/Migliore-Wear-Touchscreen-Moisturising-Sleeping/dp/B0BKSBNF5M?th=1) 
- 4 flex sensors. [e.g. 5 pack £24.56](https://www.amazon.co.uk/MQODLFP-Manipulator-Electronic-Accessories-Flex2-2Inch)
- 1 MPU 6050 Accelerometer/Gyroscope module. [e.g. 4 pack £10.99](https://www.amazon.co.uk/JZK-MPU-6050-gyroscope-accelerometer-Raspberry/dp/B07Y9KL5Q1/ref=sr_1_7)
- Arduino Nano ESP32 board. [Board without headers - £14.96](https://www.amazon.co.uk/Arduino-ESP32-Without-Header-ABX00092/dp/B0C947C9QS/ref=sr_1_3_sspa)  _The controller can be built with another ESP32 board, but the code and schematic will probably need some adjustments_
- 4 10kΩ resistors
- Either:
  - Multicore cable of different colours **(tested)**
  - Conductive thread (Madeira high-conductive HC 12 recommended, it must be solderable), several pieces of fabric and fabric scissors **(not functional yet)**
- Wire stripper
- Stripboards
- Soldering iron
- Fabric glue
- Heatsink
- Crocodile cables
- Thread and needles
- Multimeter

## Building guide 

Schematic including all connections:

<img src="images/schematic.png" alt="Schematic" width="400">


### Cabled version (working prototype)

You can build this version of the controller: 

<img src="images/cabled_version.jpg" alt="Cabled version" width="400">

_Note: checking connections with the multimeter is suggested at each stage_

1. Glue the flex sensors to the glove with fabric glue (Tip: use only a couple of drops to keep it in place, you might need to adjust it later)
2. Following the schematic, solder the appropriate circuit to a stripboard.

* **VCC (red cables)**:
    - All flex sensors need to be connected to VCC. Use a connected row of the stripboard for this purpose.
    - You'll need to solder 5 cables as outputs from this row: 4 to each one of the flex sensors, and one going to the MPU6050 module
    - You'll need to solder 1 cable going as an input in this row (power by the Arduino VCC pin)

* **Flex Sensor (gray cables)**:
    - The other terminal of the flex sensors is connected to a pull down resistor
    - For each flex sensor, solder 2 gray cables to the stripboard at the same potential. Note that different sensors should not be interconnected.
    - For each flex sensor, solder one terminal of the 10kΩ resistors to the same potential point as the gray cable
    - The other terminal of the resistor is common to all resistors, and will be later connected to GND.

* **GND (black cables)**:
    - To the same GND potential point above, connect two black cables: one cable will go to the MPU6050 module, and another one will go to the GND connection of the Arduino.

3. Before connecting the loose ends of cables to the sensors or boards, sew the stripboard to the glove.

4. Once the stripboard is sewn in place, solder one red cable to each of the flex sensors.

5. For each flex sensor, connect one gray cable to the free terminal.

6. Solder one red cable to the VCC pin of the MPU6050 module.
7. Solder one black cable to the GND pin of the MPU6050 module.
8. Solder two new cables (of a different colour) to the SCL and SDA pins of the MPU6050 module, keeping in mind that they should be long enough to reach pins A5 and A4 of the Arduino.

9. Sew MPU6050 in place.
10. Choose how you want to fix the Arduino in place:
    - I choose to solder it to a stripboard and route all connections through there to make the controller more robust
    - You could also sew the Arduino in place and directly solder every connection to the board. Note that this could cause some tension in the connections when moving!

11. Looking at the schematic carefully, solder each cable to the corresponding Arduino pin.
    - Flex sensors (gray cables) are connected through pins A7, A6, A1 and A0
    - MPU6050 is connected through A5 and A4 (SCL and SDA pins respectively)
    - GND (black cable) and VCC (red cable) are connected to the respective pins in the Arduino


### Conductive thread version (not succesfully working yet!)


1. Glue the flex sensors to the glove with fabric glue (Tip: use only a couple of drops to keep it in place, you might need to adjust it later)

2. We will create **layers with different connections**. First, the **VCC layer**:

    - Sew 4 vertical lines of conductive thread, each one will go to one sensor
    - On the top end of each line, solder to the corresponding sensor (tip: use heatsink to prevent short circuits)
    - On the lower end end of each line, solder to the stripboard. All the connections should go to the same potential in the stripboard.

It should look like this:

<img src="images/conductive_thread_vcc.jpg" alt="vcc example" width="400">


- Test connections with the multimeter before continuing!
- If everything is OK, solder a red cable to the stripboard, connected to the previously soldered threads. It should be long enough to reach the Arduino on the wrist.

3. Now, the **GND layer**. 
    - Place a cloth panel between the previous layer and this one to prevent short circuits
    - In a new panel, sew 4 vertical lines of conductive thread, each one will go to one sensor
    - The top end of each line needs to be soldered to the free terminal of the sensor (tip: use heatsink, or this will surely touch the previous connections).
    - Grab a new stripboard.
    - The lower end of each line should be soldered to the stripboard. Each should be in a separate potential.
    - For each connection above, solder one terminal of a resistor.
    - The other terminal of all resistors should be soldered at the same potential (GND)


The connections should look like this, though different stripboards might be displayed differently:

<img src="images/conductive_thread_gnd_crop.jpg" alt="gnd example" width="200">


- Test with multimeter
- If everything is OK, solder a black cable to the stripboard in the common potential. It should be long enough to reach the Arduino

4. Last, the **sensor layer**.
    - Place a cloth panel between the previous layer and this one to prevent short circuits
    - Sew 4 vertical lines of conductive thread, they will connect each sensor to an Arduino input
    - The top end should be soldered to the same terminal of the sensor that was connected to the pull-down resistor (remember the heatsink!)
    - The bottom end will be soldered to the corresponding Arduino input after checking everything is working correctly.

It should look like this:

<img src="images//conductive_thread_sensors_connection.jpg" alt="sensor example" width="400">

_Note that the material separating the layers in the photo (velostat) is not appropriate, it should be cloth or other non-conductive material._

5. Before soldering the sensors to the Arduino, run individual tests for each sensor using Crocodile clips. This will help you find possible bugs before everything is set in place.

6. Sew the Arduino into a piece of cloth, you will later fix this cloth to the glove (sewing or using fabric glue).

7. Solder the VCC and GND cables to the board.

8. Knot and solder each thread to its corresponding input, following the schematic (pins A7, A6, A1 and A0 are to be used)

_To Do:_ 

_9. Sew the MPU6050 module into a piece of cloth (this will be the top layer)._

