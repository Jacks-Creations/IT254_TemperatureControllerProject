#IT 254
#weatherFetcher Normal IL
import requests
import serial
import time

ser = serial.Serial("COM4", 9600, timeout = 2)
time.sleep(8)
ser.write("p".encode())
lowTemp = input("Enter lowest acceptable temperature in Fehrenheit: ")
highTemp = input("Enter highest acceptable temperature in Fehrenheit: ")
lowHumid = input("Enter lowest acceptable humidity as whole number: ")
highHumid = input("Enter highest acceptable humidity as whole number: ")

ser.write(f"{lowTemp}\n".encode())
ser.write(f"{highTemp}\n".encode())
ser.write(f"{lowHumid}\n".encode())
ser.write(f"{highHumid}\n".encode())
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
        ser.write(b"START\n")
        ser.write(f"{tC}\n".encode())
        ser.write(f"{tA}\n".encode())
        ser.write(f"{hC}\n".encode())
        ser.write(f"{hA}\n".encode())
        ser.write(b"END\n")
        ser.flush()
while True:
    weather = getWeather()
    if weather:
        sendPacket(weather["tC"], weather["tA"], weather["hC"], weather["hA"])
    time.sleep(15)