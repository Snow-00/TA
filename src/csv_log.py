import serial
import numpy as np
from time import sleep
from datetime import datetime

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 115200 #arduino uno runs at 9600 baud
fileName="dataset_new_sp.csv" #name of the CSV file generated
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
        
    # start mixer before all actuator (pH and tds)
    ser.write("mixer,0\n".encode())
    sleep(4)
    command = "all,0\n"
    ser.write(command.encode())

    file = open(fileName, "a")
    prev_time = datetime.now()
    file.write("new iteration\n")
    
    # while any actuator on
    while command != "all,1\n":
        command = []
        # input data sensor 
        try:
            data_text = str(ser.readline(), "utf-8").strip("\r\n")
            data = data_text.split(',')
            data = np.array(data, dtype='float')
        except Exception as e:
            print(e)
            break

        counter = round((datetime.now() - prev_time).total_seconds(), 2)
        counter_arr = np.array([counter, counter])
        
        # check whether the setpoint has been reached / not
        if data[0] <= sp[0]:
            command.append("ph down,1")
            counter_arr[0] = 0
        else:
            command.append("ph down,0")

        if data[1] >= sp[1]:
            command.append("tds up,1")
            counter_arr[1] = 0
        else:
            command.append("tds up,0")

        if data[0] <= sp[0] and data[1] >= sp[1]:
            print("all set")
            break

        # if counter_arr[0] >= sp[0]:
        #     command = "ph down,1\n"
        #     counter_arr = np.array([0, counter])
        # if counter_arr[1] >= sp[1]:
        #     command = "tds up,1\n"
        #     counter_arr = np.array([counter, 0])
        # if counter_arr[0]>=sp[0] and counter_arr[1]>=sp[1]:
        #     command = "all,1\n"
        #     counter_arr = np.array([0, 0])
        
        command = ";".join(command)
        command = command + "\n"
        ser.write(command.encode())
        sleep(2)
        data = f"{data_text},{counter_arr[0]},{counter_arr[1]}"
        file.write(data + "\n") #write data with a newline
        print(data)
    
    command = "all,1\n"
    ser.write(command.encode())
    sleep(2)
    print("finish get dataset")

    # reading sensors when stable
    command = "monitor\n"
    ser.write(command.encode())
    prev_time = datetime.now()

    while command != "all,1\n":
        # input data sensor 
        data_text = str(ser.readline(), "utf-8").strip("\r\n")
        data = data_text.split(',')
        data = np.array(data, dtype='float')

        counter = round((datetime.now() - prev_time).total_seconds(), 2)
        
        if counter >= 15:
            break

        data = f"{data_text},{counter_arr[0]},{counter_arr[1]}"
        file.write(data + "\n") #write data with a newline
        print(data)
    
    command = "all,1\n"
    ser.write(command.encode())
    sleep(2)
    file.close()