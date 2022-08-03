import serial
import numpy as np
import random
from tensorflow import keras
from time import sleep
from datetime import datetime

# load model
model = keras.models.load_model('NutrientModel')

SP_PH = 5.75
SP_TDS = 725
sensor_pH = 7.2
sensor_TDS = 133
err_pH = SP_PH - sensor_pH
err_TDS = SP_TDS - sensor_TDS

value_ph = 7.20
random_value = [1.35, 1.36, 1.37, 1.38, 1.39, 1.50,
                1.41, 1.42, 1.43, 1.44, 1.45, 1.46, 1.47, 1.48, 1.49, 1.50,
                1.51, 1.52, 1.53, 1.54, 1.55, 1.56, 1.57, 1.58, 1.59, 1.60]

value_tds = 133
random_value_tds = [122, 125, 130, 128, 120, 118,
                135, 124, 121.5, 123.3, 126.1, 130.1, 128.7, 124.9, 124.7, 131,
                116.5, 119.2, 120.4, 119.8, 119.9, 120.56, 127.57, 129.58, 126.59, 124.60]

def countdown(t):
    # print ("Nilai pH: " + str(value_ph))
    sleep(1)
    while t:
        mins, secs = divmod(t, 60)
        timer = '{:02d}:{:02d}'.format(mins, secs)
        # print(timer, end="\r")
        new_value = round((value_ph - random.choice(random_value)), 2)
        new_value_tds = (value_tds + random.choice(random_value_tds))
        print("new value PH: " + str(new_value))
        print("new value TDS: " + str(new_value_tds))
        sleep(1)
        t -= 1
      
    print('Exit Code')

input_data = np.array([err_pH, err_TDS], dtype=float)
input_data = np.expand_dims(input_data, axis=0)

print("Nutrient Mix System ON")
print(f"pH: {sensor_pH}; TDS: {sensor_TDS}")
print(f"error pH: {err_pH}; error TDS: {err_TDS}")

output = model.predict(input_data)
out_pH = round(output[0][0], 2)
out_TDS = round(output[0][1], 2)

print(f"output time pH: {out_pH}; output time TDS: {out_TDS}\n")

# function call
countdown(int(14))