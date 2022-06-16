import serial
import threading
from datetime import datetime

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 9600 #arduino uno runs at 9600 baud
fileName="dataset_pH.csv" #name of the CSV file generated

ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)
samples = 10 #how many samples to collect
print_labels = False

while True:    
    input_command = input("enter command: ")
    command, duration = input_command.split(",")
    print(f"command: {command}; time: {duration}")
    command = command + '\n'
    ser.write(command.encode())
    file = open(fileName, "a")

    prev_time = datetime.now()
    counter = 0
    while counter < int(duration):
        data = str(ser.readline(), "utf-8")
        data = data.strip("\r\n")
        counter = (datetime.now() - prev_time).seconds
        data = f"{counter},{data}"
        
        file.write(data + "\n") #write data with a newline
        print(data)

    command = "t\n"
    ser.write(command.encode())
    
    file.close()