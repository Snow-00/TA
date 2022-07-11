import urllib.request
import serial

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 9600 #arduino uno runs at 9600 baud
ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)

while True:
    data = str(ser.readline(), "utf-8")
    msg = data.strip("\r\n")
    
    try:
        # msg = msg.replace(' ', "%20")
        # msg = msg.replace('\n', "%0A")
        msg = float(msg)
        urllib.request.urlopen(f'https://api.thingspeak.com/update?api_key=GPFMAB99YI1AZDY9&field3={msg}')
        print("Sending data:", msg)

    except:
        print("Something went wrong!")
        break