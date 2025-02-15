# ESPNOW-Car---TB6612FNG-Based
ESP32 and TB6612FNG (Motor Driver) Based ESPNOW Car

The accompanying code is for an ESPNOW controlled car with 2 versions supported, a receiver with TB6612FNG motor driver, and a receiver with L298N motor driver.  The code for transmitter for both versions is identical.

The circuit for the receiver based on the TB6612FNG motor driver:

![ESPNOW_Car_Receiver_TB6612](https://github.com/user-attachments/assets/dc69481d-a275-4d22-b26e-a33c1c4c2ba1)

For the TB6612, there is a library necessary which can also be found on Gibhub: https://github.com/yeagerninja/TB6612FNG_ESP32_Library

The circuit for the transmitter:

![ESPNOW_Car_Transmitter](https://github.com/user-attachments/assets/6b973f36-b26f-43f1-a4e4-ef19863af5e5)

The circuit for the receiver based on the L298N motor driver (alternate build from TB6612):

![ESPNOW_Car_Receiver](https://github.com/user-attachments/assets/7de2cfc0-c257-490f-9daa-d5553e6f563c)

NOTE:  Because of some deprecated functions, its necessary to downgrade your board to version 2.0.17 if you are using the L298N version of the code:  https://forum.arduino.cc/t/downgrade-from-v3-0-x-to-v2-0-x/1272211
