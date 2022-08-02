import serial
import numpy as np
from datetime import datetime
from time import sleep

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 115200 #arduino uno runs at 9600 baud
fileName="dataset_environmentLast2.csv" #name of the CSV file generated
actuator = ["ph down", "tds up"] # command actuator

ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)

while True:    
    # input all setpoints
    input_sp = input("enter setpoint: ")
    if input_sp == "stop":
        break

    try:
        sp = input_sp.split(",")
        sp = np.array(sp, dtype='float')
    except:
        print("wrong input, try again!")
        continue
    print(f"target humidity: {sp[0]}; target temperatur: {sp[1]}")
    
    # start all actuator (pH and tds)
    command = "all,0\n"
    ser.write(command.encode())

    file = open(fileName, "a")
    prev_time = datetime.now()
    counter_arr = np.array([1])
    
    # while any actuator on
    while counter_arr.any():
        # input data sensor 
        data = str(ser.readline(), "utf-8")
        data = data.strip("\r\n").split(',')
        data = np.array(data, dtype='float')

        counter = round((datetime.now() - prev_time).total_seconds(), 2)
        counter_arr = np.array([counter, counter])
        
        # check whether the setpoint has been reached / not
        if counter >= sp[0]:
            # ser.write("pump,1\n".encode())
            command = f"{actuator[0]},1\n"
            counter_arr = np.array([0, counter])
        if counter >= sp[1]:
            # ser.write("peltier,1\n".encode())
            command= f"{actuator[1]},1\n"
            counter_arr = np.array([counter, 0])
        if (counter>=sp[0]) and (counter>=sp[1]):
            # ser.write("all,1\n".encode())
            command = "all,1\n"
            counter_arr = np.array([0, 0])
        
        ser.write(command.encode())
        sleep(3)
        data = f"{data[0]},{data[1]},{counter_arr[0]},{counter_arr[1]}"
        file.write(data + "\n") #write data with a newline
        print(data)
    
    ser.write("monitor".encode())
    sleep(2)
    data = str(ser.readline(), "utf-8").strip("\r\n")
    print(data)
    file.close()