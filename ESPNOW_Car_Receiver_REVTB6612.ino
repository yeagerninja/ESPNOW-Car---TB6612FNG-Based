// Code for Reciever (Car) for ESPNOW 2WD Car
// Updated to utilize TB6612FNG Motor Driver
// Must download the following Library for ESP32:  https://github.com/yeagerninja/TB6612FNG_ESP32_Library
// Modified by Scott Yeager from various sources

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <TB6612_ESP32.h>

//Define Motor Pins
#define AIN1 26 // ESP32 Pin D26 to TB6612 Pin AIN1
#define BIN1 14 // ESP32 Pin D14 to TB6612 Pin BIN1
#define AIN2 25 // ESP32 Pin D25 to TB6612 Pin AIN2
#define BIN2 12 // ESP32 Pin D12 to TB6612 Pin BIN2
#define PWMA 33 // ESP32 Pin D33 to TB6612 Pin PWMA
#define PWMB 13 // ESP32 Pin D13 to TB6612 Pin PWMB
#define STBY 27 // ESP32 Pin D27 to TB6612 Pin STBY


// These constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;


// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 1);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

#define MAX_MOTOR_SPEED 254 //255

const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal
unsigned long lastRecvTime = 0;

typedef struct PacketData
{
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
}PacketData;
PacketData receiverData;

// Can use this setting to add throttle capability (true) vs simple movement (false)
bool throttleAndSteeringMode = true;


// Callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  if (len == 0)
  {
    return;
  }
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  String inputData ;
  inputData = inputData + "values " + receiverData.xAxisValue + "  " + receiverData.yAxisValue + "  " + receiverData.switchPressed;
  Serial.println(inputData);
  if (receiverData.switchPressed == true)
  {
    if (throttleAndSteeringMode == false)
    {
      throttleAndSteeringMode = true;
    }
    else
    {
      throttleAndSteeringMode = false;
    }
  }

  if (throttleAndSteeringMode)
  {
    throttleAndSteeringMovements();
  }
  else
  {
    simpleMovements();
  }
  
  lastRecvTime = millis();   
}


void simpleMovements()
{
  if (receiverData.yAxisValue <= 75)       //Move car Forward
  {
    rotateMotor(MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
  }
  else if (receiverData.yAxisValue >= 178)   //Move car Backward
  {
    rotateMotor(-MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
  }
  else if (receiverData.xAxisValue >= 172)  //Move car Right
  {
    rotateMotor(MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);/////////////////////
  }
  else if (receiverData.xAxisValue <= 75)   //Move car Left
  {
    rotateMotor(-MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);//////////////////////
  }
  else                                      //Stop the car
  {
    rotateMotor(0, 0);
  }   
}


void throttleAndSteeringMovements()
{
  int throttle = map( receiverData.yAxisValue, 254, 0, -255, 255);
  int steering = map( receiverData.xAxisValue, 0, 254, -255, 255);  
  int motorDirection = 1;
  
  if (throttle < 0)       //Move car backward
  {
    motorDirection = -1;    
  }

  int rightMotorSpeed, leftMotorSpeed;
  rightMotorSpeed =  abs(throttle) - steering;
  leftMotorSpeed =  abs(throttle) + steering;
  rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);
  leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);

  rotateMotor(rightMotorSpeed * motorDirection, leftMotorSpeed * motorDirection);
}


void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  motor1.drive(rightMotorSpeed);
  motor2.drive(leftMotorSpeed);
}


void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //esp_now_register_recv_cb(OnDataRecv);  only works version 2.x and below
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}


void loop() 
{
  //Check Signal lost.
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    rotateMotor(0, 0);
  }
}
