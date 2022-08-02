import serial
import numpy as np
from time import sleep
from datetime import datetime

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 115200 #arduino uno runs at 9600 baud
fileName="../dataset/testing.csv" #name of the CSV file generated
actuator = ["ph down", "tds up", "water"] # command actuator

ser = serial.Serial(port=arduino_port, baudrate=baud)
print("Connected to Arduino port:" + arduino_port)
file = open(fileName, "a")
file.write("new iteration\n")

while True:    
    ser.write("all,1\n".encode())
    # input all setpoints
    input_sp = input("enter command: ")
    if input_sp == "stop":
        break

    try:
        sp = input_sp.split(",")
        sp[1] = float(sp[1])
        print(f"command: {sp[0]}; time: {sp[1]}")
    except:
        print("wrong input, try again!")
        continue
        
    command = sp[0]
    ser.write(f"{command},0\n".encode())
    sleep(1)
    
    start_time = datetime.now()
    
    # while time hasn't run out
    while True:
        counter = round((datetime.now() - start_time).total_seconds(), 2)
        if counter > sp[1]:
            ser.write("all,1\n".encode())
            break

    # reset arduino
    ser.close()
    sleep(1)
    ser.open()
    sleep(1)
    # ser.setDTR(False)
    # sleep(1)
    # ser.flushInput()
    # ser.setDTR(True)
    
    # input data sensor
    try:
        ser.write("monitor\n".encode())
        data_text = str(ser.readline(), "utf-8").strip("\r\n")
    except Exception as e:
        print(e)
        break

    data = f"{data_text},{command},{counter}"
    file.write(data + "\n")
    print(data)

file.close()