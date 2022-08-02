import serial
import urllib.request
import numpy as np
from datetime import datetime
from tensorflow import keras

ARDUINO_PORT = "/dev/ttyACM0" # serial port of Arduino
OTHER_PORT = "/dev/"
BAUD = 9600 # arduino uno runs at 9600 baud
# OTHER_BAUD = 
LED_ON = 64800
LED_OFF = 86400 - LED_ON

NUTRIENT_MODEL_FILE = "xxx.h5" # name of the saved nutrient model file
ENV_MODEL_FILE = "env_collect.h5" # name of the saved env model file
nutrient_model = keras.models.load_model(NUTRIENT_MODEL_FILE)
env_model = keras.models.load_model(ENV_MODEL_FILE)

command_nutrient = ["ph down", "tds up"]
command_env = ["pump", "peltier"]

nn_nutrient = 0
nn_env = 0

ser = serial.Serial(ARDUINO_PORT, BAUD)
ser2 = serial.Serial(OTHER_PORT, BAUD)
print("Connected to Arduino port:" + ARDUINO_PORT)

# turn on led first time
led_state = 1
ser.write("led,0\n".encode())
prev_led = datetime.now()

def create_arr(part_data_arr):
    arr = np.array(part_data_arr, dtype='float')
    arr = np.expand_dims(arr, axis=0)

    return arr

def send_data(arr):
    for n in range(len(arr)):
        urllib.request.urlopen(f'https://api.thingspeak.com/update?api_key=GPFMAB99YI1AZDY9&field{n+1}={arr[n]}')

def en_actuator(command, output):
    act = np.array([0, 0])
    cmd = []
    for n in range(len(command)):
        if output[0][n] >= 1:
            cmd.append(f"{command[n]},0")
            act[n] = 1
        else:
            cmd.append(f"{command[n]},1")
    
    cmd = ";".join(cmd)
    cmd = cmd + "\n"
    ser.write(cmd.encode())
    return act

def dis_actuator(command, output, time):
    act = np.array([0, 0])
    cmd = []
    for n in range(len(command)):
        if time > output[0][n]:
            cmd.append(f"{command[n]},1")
            act[n] = 0
        else:
            cmd.append(f"{command[n]},0")

    cmd = ";".join(cmd)
    cmd = cmd + "\n"
    ser.write(cmd.encode())
    return act

while True: 
    try:
        # get new data sensor from arduino
        data_str = str(ser.readline(), "utf-8").strip("\r\n")
        data_arr = data_str.split(",")
        arr_env = create_arr(data_arr[:2])
        arr_nutrient = create_arr(data_arr[2:5])
    
    except:
        urllib.request.urlopen('https://api.thingspeak.com/update?api_key=GPFMAB99YI1AZDY9&field1=1')
        continue

    # send data to thingspeak
    send_data(data_arr)

    # timer for the led
    if led_state:
        led_timer = round((datetime.now() - prev_led).total_seconds(), 2)
        if led_timer > LED_ON:
            ser.write("led,1\n".encode())
            led_state = 0
            prev_led = datetime.now()
    
    else:
        led_timer = round((datetime.now() - prev_led).total_seconds(), 2)
        if led_timer > LED_OFF:
            ser.write("led,0\n".encode())
            led_state = 1
            prev_led = datetime.now()

    # check whether there's active actuator before running nutrient
    if not nn_nutrient.any() and not nn_env.any():
        # predict the output of nutrient
        out_nutrient = nutrient_model.predict(arr_nutrient)
        
        # activate nutrient's actuator
        nn_nutrient = en_actuator(command_nutrient, out_nutrient)
        if nn_nutrient.any():
            prev_time = datetime.now()
    
    if nn_nutrient.any():
        timer = round((datetime.now() - prev_time).total_seconds(), 2)
        
        # turn off actuator when duration is over
        nn_nutrient = dis_actuator(command_nutrient, out_nutrient, timer)
    
    # check whether there's active actuator before running env
    if not nn_nutrient.any() and not nn_env.any():
        # predict the output of env
        out_env = env_model.predict(arr_env)
        
        # activate env's actuator
        nn_env = en_actuator(command_env, out_env)
        if nn_env.any():
            prev_time = datetime.now()
    
    if nn_env.any():
        timer = round((datetime.now() - prev_time).total_seconds(), 2)
        
        # turn off actuator when duration is over
        nn_env = dis_actuator(command_nutrient, out_nutrient, timer)