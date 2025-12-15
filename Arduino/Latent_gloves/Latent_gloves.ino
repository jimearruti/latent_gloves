#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

int FSR[] = {A7, A6, A1};
bool debug = true;
const int listSize = sizeof(FSR) / sizeof(FSR[0]);

// Flex sensors config
const int numReadings = 70;
int readings[listSize][numReadings];
int readIndex[listSize]; 
int total[listSize];       
int force[listSize];
float forceAvg;

// MPU6050 config
Adafruit_MPU6050 mpu;
const int mpu_numReadings = 10;
int mpu_readIndex = 0;
float accel_readings[3][mpu_numReadings];
float gyro_readings[3][mpu_numReadings];
float accel_filtered[3];
float gyro_filtered[3];

// Calibration offsets
float gyro_offset[3] = {0, 0, 0};
int fsr_baseline[3] = {0, 0, 0};
float pitch_zero = 0, roll_zero = 0;  // Calibrated zero position

// Kalman filter variables for pitch and roll
float pitch = 0, roll = 0;
float pitch_bias = 0, roll_bias = 0;
float P[2][2] = {{1, 0}, {0, 1}}; // Error covariance matrix for pitch
float P_roll[2][2] = {{1, 0}, {0, 1}}; // Error covariance matrix for roll
float Q_angle = 0.001;  // Process noise variance for angle
float Q_bias = 0.003;   // Process noise variance for bias
float R_measure = 0.03; // Measurement noise variance
unsigned long lastTime = 0;

struct Orientation {
  float pitch;
  float roll;
};


void setup() {
  Serial.begin(9600);

  // Set all flex sensor values to 0
  for (int j = 0; j < listSize; j++){
    for (int i = 0; i < numReadings; i++) {
      readings[j][i] = 0;
    }
    readIndex[j] = 0;
    total[j] = 0;
    force[j] = 0;
  }

  // Set all accelerometer+gyroscope sensor values to 0
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < mpu_numReadings; j++) {
      accel_readings[i][j] = 0;
      gyro_readings[i][j] = 0;
    }
    accel_filtered[i] = 0;
    gyro_filtered[i] = 0;
  }

  // Initialize accelerometer+gyroscope
  initMPU6050();
  
  // Calibrate the sensors
  calibrateSensors();
  
  lastTime = millis();
  delay(100);
}


void loop(){
  readFSR();
  Orientation orientation = readMPU();
  String output = String((int)round(forceAvg))+"_"+String((int)round(force[0]))+"_"+String((int)round(force[1]))+"_"+String((int)round(force[2]))+"_"+String((int)round(orientation.pitch))+"_"+String((int)round(orientation.roll));
  Serial.println(output);
}

void readFSR() {
  // Readings for flex sensors
  for (int j = 0; j < listSize; j++){
    total[j] = total[j] - readings[j][readIndex[j]]; 
    readings[j][readIndex[j]] = analogRead(FSR[j]) - fsr_baseline[j]; 
    total[j] = total[j] + readings[j][readIndex[j]]; 
    readIndex[j] = readIndex[j] + 1;

    if (readIndex[j] >= numReadings) { 
      readIndex[j]= 0; 
    }

    force[j] = total[j] / numReadings; 
  }

  // Get average flexion
  forceAvg = 0;
  for (int j = 0; j < listSize; j++){
    forceAvg += force[j];
  }
  forceAvg = forceAvg/listSize;
}

Orientation readMPU() {
  // Get new sensor events with the readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Store readings and apply moving average
  float accel_raw[3] = {a.acceleration.x, a.acceleration.y, a.acceleration.z};
  float gyro_raw[3] = {g.gyro.x - gyro_offset[0], g.gyro.y - gyro_offset[1], g.gyro.z - gyro_offset[2]};

  for (int i = 0; i < 3; i++) {
    accel_readings[i][mpu_readIndex] = accel_raw[i];
    gyro_readings[i][mpu_readIndex] = gyro_raw[i];
    
    float accel_sum = 0, gyro_sum = 0;
    for (int j = 0; j < mpu_numReadings; j++) {
      accel_sum += accel_readings[i][j];
      gyro_sum += gyro_readings[i][j];
    }
    accel_filtered[i] = accel_sum / mpu_numReadings;
    gyro_filtered[i] = gyro_sum / mpu_numReadings;
  }
  mpu_readIndex = (mpu_readIndex + 1) % mpu_numReadings;

  // Calculate time delta
  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0; // Convert to seconds
  lastTime = currentTime;

  // Calculate angles from accelerometer
  float accel_pitch = atan2(accel_filtered[1], sqrt(accel_filtered[0] * accel_filtered[0] + accel_filtered[2] * accel_filtered[2]));
  float accel_roll = atan2(-accel_filtered[0], accel_filtered[2]);

  // Apply Kalman filter for pitch
  pitch = kalmanFilter(pitch, pitch_bias, gyro_filtered[0], accel_pitch, dt, P);
  
  // Apply Kalman filter for roll
  roll = kalmanFilter(roll, roll_bias, gyro_filtered[1], accel_roll, dt, P_roll);

  // Convert to degrees relative to calibrated zero position
  float pitch_deg = (pitch - pitch_zero) * 180.0 / PI;
  float roll_deg = (roll - roll_zero) * 180.0 / PI;

  Orientation result;
  result.pitch = pitch_deg;
  result.roll = roll_deg;
  return result;
}

float kalmanFilter(float &angle, float &bias, float gyro_rate, float accel_angle, float dt, float P_matrix[2][2]) {
  // Step 1: Predict
  float rate = gyro_rate - bias;
  angle += dt * rate;

  // Update error covariance
  P_matrix[0][0] += dt * (dt * P_matrix[1][1] - P_matrix[0][1] - P_matrix[1][0] + Q_angle);
  P_matrix[0][1] -= dt * P_matrix[1][1];
  P_matrix[1][0] -= dt * P_matrix[1][1];
  P_matrix[1][1] += Q_bias * dt;

  // Step 2: Update
  float y = accel_angle - angle; // Innovation
  float S = P_matrix[0][0] + R_measure; // Innovation covariance
  float K[2]; // Kalman gain
  K[0] = P_matrix[0][0] / S;
  K[1] = P_matrix[1][0] / S;

  // Update estimate
  angle += K[0] * y;
  bias += K[1] * y;

  // Update error covariance
  float P00_temp = P_matrix[0][0];
  float P01_temp = P_matrix[0][1];
  P_matrix[0][0] -= K[0] * P00_temp;
  P_matrix[0][1] -= K[0] * P01_temp;
  P_matrix[1][0] -= K[1] * P00_temp;
  P_matrix[1][1] -= K[1] * P01_temp;

  return angle;
}

void initMPU6050(){
   Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
}

void calibrateSensors() {
  Serial.println("\n=== CALIBRATION START ===");
  Serial.println("Mantain hand in calibration position...");
  delay(2000);
  
  // Calibrate gyroscope
  Serial.println("Calibrating gyroscope...");
  float gyro_sum[3] = {0, 0, 0};
  const int calibration_samples = 100;
  
  for (int i = 0; i < calibration_samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    gyro_sum[0] += g.gyro.x;
    gyro_sum[1] += g.gyro.y;
    gyro_sum[2] += g.gyro.z;
    delay(10);
  }
  
  gyro_offset[0] = gyro_sum[0] / calibration_samples;
  gyro_offset[1] = gyro_sum[1] / calibration_samples;
  gyro_offset[2] = gyro_sum[2] / calibration_samples;
  
  Serial.print("Gyro offsets - X: ");
  Serial.print(gyro_offset[0], 4);
  Serial.print(", Y: ");
  Serial.print(gyro_offset[1], 4);
  Serial.print(", Z: ");
  Serial.println(gyro_offset[2], 4);
  
  // Calibrate initial pitch and roll from accelerometer
  Serial.println("\nCalibrating initial orientation...");
  float accel_sum[3] = {0, 0, 0};
  
  for (int i = 0; i < calibration_samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    accel_sum[0] += a.acceleration.x;
    accel_sum[1] += a.acceleration.y;
    accel_sum[2] += a.acceleration.z;
    delay(10);
  }
  
  float accel_x_avg = accel_sum[0] / calibration_samples;
  float accel_y_avg = accel_sum[1] / calibration_samples;
  float accel_z_avg = accel_sum[2] / calibration_samples;
  
  // Set initial pitch and roll based on gravity
  pitch = atan2(accel_y_avg, sqrt(accel_x_avg * accel_x_avg + accel_z_avg * accel_z_avg));
  roll = atan2(-accel_x_avg, accel_z_avg);
  
  // Save as zero reference
  pitch_zero = pitch;
  roll_zero = roll;
  
  Serial.print("Initial Pitch: ");
  Serial.print(pitch * 180.0 / PI);
  Serial.print("°, Initial Roll: ");
  Serial.print(roll * 180.0 / PI);
  Serial.println("°");
  
  // Calibrate FSR sensors
  Serial.println("\nCalibrating FSR sensors (no pressure)...");
  delay(1000);
  
  for (int j = 0; j < listSize; j++) {
    int fsr_sum = 0;
    for (int i = 0; i < 50; i++) {
      fsr_sum += analogRead(FSR[j]);
      delay(10);
    }
    fsr_baseline[j] = fsr_sum / 50;
    Serial.print("FSR A");
    Serial.print(FSR[j] - A0);
    Serial.print(" baseline: ");
    Serial.println(fsr_baseline[j]);
  }
  
  Serial.println("\n=== CALIBRATION COMPLETE ===");
  Serial.println("Starting measurements...\n");
  delay(1000);
}