import serial
from datetime import datetime

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 9600 #arduino uno runs at 9600 baud
fileName="dataset_pH_test.csv" #name of the CSV file generated

ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)

while True:    
    input_command = input("enter command: ")
    if input_command == "stop":
        break
    try:
        command, duration = input_command.split(",")
    except:
        print("wrong input, try again!")
        continue
    print(f"command: {command}; time: {duration}")
    command = command + '\n'
    ser.write(command.encode())
    file = open(fileName, "a")

    prev_time = datetime.now()
    counter = 0
    while counter < int(duration):
        data = str(ser.readline(), "utf-8")
        data = data.strip("\r\n")
        counter = round((datetime.now() - prev_time).total_seconds(), 2)
        data = f"{counter},{data}"
        
        file.write(data + "\n") #write data with a newline
        print(data)

    command = "t\n"
    ser.write(command.encode())
    
    file.close()