Arduino temperature and humidity sensor using python API for predictive elements.

Parts List:

Arduino Mega 2560

Arduino data cable

DHT11 Sensor

3 Leds (of different colors preferably)

1 fan blade with 3-6V motor

1 NPN transistor

4 resistors

at LEAST 14 wires

Breadboard (optional)



*ALL components can be found in The Most Complete Starter Kit Mega 2560 Project*



There are 2 seperate modes to the program:



Live Mode:

Step 1 - Install IT254_TempControllerV6_Final (only one needed)

Step 2 - Install DHT_Sensor_Library by Adafruit

Step 3 - Assemble components, according to ports documented in IT254_TempControllerV6_Final

Step 4 - Connect Arduino data cable to USB port on your device

Step 5 - Run IT254_TempControllerV6_Final in the IDE of your choosing

Step 6 - Open the Serial monitor & answer the prompts via the Serial monitor

Step 7 - Adjust climate control systems as suggested by the program/components



Predictive Mode:

Step 1 - Install IT254_TempControllerV6_Final & APIWeatherFetcher

Step 2 - Install the following:

  DHT_Sensor_Library by Adafruit
  
  python -m pip install pyserial (Via CMD)
  
  python -m pip install requests (Via CMD)
  
Step 3 - Assemble components, according to ports documented in IT254_TempControllerV6_Final

Step 4 - Connect Arduino data cable to USB port on your device

Step 5 - Upload IT254_TempControllerV6_Final to Arduino Mega 2560 via IDE

Step 6 - Change "COM4" in line 7 of APIWeatherFetcher according to the COM port listed on IDE

Step 7 - Run APIWeatherFetcher in compiler compatible with Serial monitor (built into VS Code)

Step 8 - Navigate to Serial monitor & answer the prompts via the Serial monitor

Step 9 - Adjust climate control systems as suggested by the program/components



*NOTE: Both systems will run continously until disconnected from power or manually killed.*

Contributors:

Payton Slabaugh & Olivia Reich - Constructed the circuitry.

Eli Barlow - Constructed APIWeatherFetcher.py, worked somewhat on IT254_TempControllerV6_Final.

Jack McLemore - Constructed majority of IT254_TempControllerV6_Final & logic within IT254_TempControllerV6_Final.
