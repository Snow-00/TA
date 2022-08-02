from time import sleep
from datetime import datetime
import serial
import numpy as np
import urllib.request

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 115200 #arduino uno runs at 9600 baud

file_name="dataset_new_sp.csv" #name of the CSV file generated
log_file = "err_log.txt"    # name of err log file
# file = open(file_name, "a")
err_file = open(log_file, "a")

ser = serial.Serial(port=arduino_port, baudrate=baud)
print("Connected to Arduino port:" + arduino_port)

while True:
    try:
        data_text = str(ser.readline(), "utf-8").strip("\r\n")
        data = data_text.split(',')
    except Exception as e:
        _timestamp = str(datetime.now().time())
        err_data = f"{_timestamp} --> {e}\n"
        err_file.write(err_data)
        continue
    
    if datetime.now().second % 20:
        continue
    
    for n in range(len(data)):
        print(data[n])
        urllib.request.urlopen(f'https://api.thingspeak.com/update?api_key=KVP5P3M75QYVA9E7&field{n+1}={data[n]}')
    print("next loop")