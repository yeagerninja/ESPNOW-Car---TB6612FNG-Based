// Code for Transmitter (Controller) for ESPNOW 2WD Car
// Updated to utilize TB6612FNG Motor Driver (Transmitter code should be motor driver agnostic)
// Modified by Scott Yeager from various sources

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>

#define X_AXIS_PIN 32
#define Y_AXIS_PIN 33
#define SWITCH_PIN 25

// REPLACE WITH YOUR RECEIVER MAC Address
//uint8_t receiverMacAddress[] = {0xEC,0x62,0x60,0xB3,0x04,0x48};  // EC:62:60:B3:04:48
uint8_t receiverMacAddress[] = {0xC0,0x49,0xEF,0x9F,0x8F,0x10};  // C0:49:EF:9F:8F:10

typedef struct PacketData
{
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
}PacketData;
PacketData data;

esp_now_peer_info_t peerInfo;

//This function is used to map 0-4095 joystick value to 0-254. hence 127 is the center value which we send.
//It also adjust the deadband in joystick.
//Jotstick values range from 0-4095. But its center value is not always 2047. It is little different.
//So we need to add some deadband to center value. in our case 1800-2200. Any value in this deadband range is mapped to center 127.
int mapAndAdjustJoystickDeadBandValues(int value, bool reverse)
{
  if (value >= 2200)
  {
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1800)
  {
    value = map(value, 1800, 0, 127, 0);  
  }
  else
  {
    value = 127;
  }

  if (reverse)
  {
    value = 254 - value;
  }
  return value;
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
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
  else
  {
    Serial.println("Success: Initialized ESP-NOW");
  }

  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  else
  {
    Serial.println("Success: Added peer");
  } 

  pinMode(SWITCH_PIN, INPUT_PULLUP);   
}


void loop() 
{
  data.xAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(X_AXIS_PIN), false);
  data.yAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(Y_AXIS_PIN), false);  
  data.switchPressed = false; 

  if (digitalRead(SWITCH_PIN) == LOW)
  {
    data.switchPressed = true;
  }
  
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &data, sizeof(data));
  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }    
  
  if (data.switchPressed == true)
  {
    delay(500);
  }
  else
  {
    delay(50);
  }
}
