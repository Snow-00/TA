import serial
import numpy as np
from time import sleep
from datetime import datetime

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 115200 #arduino uno runs at 9600 baud
fileName="dataset_all_2.csv" #name of the CSV file generated
actuator = ["ph down", "tds up", "water"] # command actuator

ser = serial.Serial(port=arduino_port, baudrate=baud)
print("Connected to Arduino port:" + arduino_port)

while True:    
    # input all setpoints
    input_sp = input("enter command: ")
    if input_sp == "stop":
        break

    try:
        sp = input_sp.split(",")
        sp = np.array(sp, dtype='float')
        print(f"target pH: {sp[0]}; target tds: {sp[1]}; target level: {sp[2]}")
    except:
        print("wrong input, try again!")
        continue
        
    # start all actuator (pH and tds)
    command = "all,0\n"
    ser.write(command.encode())

    file = open(fileName, "a")
    prev_time = datetime.now()
    counter_arr = np.array([1])
    
    # while any actuator on
    while command != "all,1\n":
        # input data sensor 
        data_text = str(ser.readline(), "utf-8").strip("\r\n")
        data = data_text.split(',')
        data = np.array(data, dtype='float')

        counter = round((datetime.now() - prev_time).total_seconds(), 2)
        counter_arr = np.array([counter, counter])
        
        # check whether the setpoint has been reached / not
        if data[0] <= sp[0]:
            ser.write("ph down,1\n".encode())
            counter_arr = np.array([0, counter])
        else:
            ser.write("ph down,0\n".encode())
            counter_arr = np.array([counter, counter])
            
        if data[1] >= sp[1]:
            ser.write("tds up,1\n".encode())
            counter_arr = np.array([counter, 0])
        else:
            ser.write("tds up,0\n".encode())
            counter_arr = np.array([counter, counter])

        if data[2] >= sp[2]:
            ser.write("all,1\n".encode())
            counter_arr = np.array([0, 0])

        # if counter_arr[0] >= sp[0]:
        #     command = "ph down,1\n"
        #     counter_arr = np.array([0, counter])
        # if counter_arr[1] >= sp[1]:
        #     command = "tds up,1\n"
        #     counter_arr = np.array([counter, 0])
        # if counter_arr[0]>=sp[0] and counter_arr[1]>=sp[1]:
        #     command = "all,1\n"
        #     counter_arr = np.array([0, 0])
        
        sleep(2)
        data = f"{data_text},{counter_arr[0]},{counter_arr[1]}"
        file.write(data + "\n") #write data with a newline
        print(data)
    
    file.close()