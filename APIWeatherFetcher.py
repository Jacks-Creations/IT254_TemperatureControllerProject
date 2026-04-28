#Eli Barlow
#IT 254
#weatherFetcher Normal IL
import requests
import serial
import time

ser = serial.Serial("COM3", 9600, timeout = 2)
time.sleep(2)

def getWeather():
    try:
        url = "https://api.weather.gov/gridpoints/ILX/41,71/forecast/hourly"
        data = requests.get(url, headers={"User-Agent": "weatherFetcher"}).json()

        hours = data["properties"]["periods"]
        hourCurrent = hours[0]
        hourAhead = hours[1]

        hourCurrentTemp = hourCurrent["temperature"]
        hourCurrentHumidity = hourCurrent["relativeHumidity"]["value"]

        hourAheadTemp = hourAhead["temperature"]
        hourAheadHumidity = hourAhead["relativeHumidity"]["value"] 
    
        return {"tC": hourCurrentTemp, "hC": hourCurrentHumidity, "tA": hourAheadTemp, "hA": hourAheadHumidity}
    except Exception as e:
        print("API ERROR", e)
        return None

def sendPacket(tC, tA, hC, hA):
        print("Sending packet:")
        print("Current Temp: ", tC)
        print("Ahead Temp: ", tA)
        print("Current Humidity:", hC)
        print("Ahead Humidity:", hA)

        ser.write(b"START\n")
        ser.write(f"{tC}\n".encode())
        ser.write(f"{tA}\n".encode())
        ser.write(f"{hC}\n".encode())
        ser.write(f"{hA}\n".encode())
        ser.write(b"END\n")
        ser.flush()
#TESTING STUFF
        time.sleep(0.5)
while ser.in_waiting:
    line = ser.readline().decode().strip()
    print("Arduino:", line)
#TESTING STUFF
while True:
      weather = getWeather()
      if weather:
        sendPacket(weather["tC"], weather["tA"], weather["hC"], weather["hA"])
      time.sleep(30)