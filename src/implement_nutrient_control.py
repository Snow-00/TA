import serial
import numpy as np
from tensorflow import keras
from time import sleep
from datetime import datetime

# load model
model = keras.models.load_model('latestModel')

SP_PH = 5.75
SP_TDS = 725
sensor_pH = 7.14
sensor_TDS = 133
err_pH = SP_PH - sensor_pH
err_TDS = SP_TDS - sensor_TDS

input_data = np.array([err_pH, err_TDS], dtype=float)
input_data = np.expand_dims(input_data, axis=0)

output = model.predict(input_data)
print(output)
print(type(output))

# print("Nutrient Mix System ON")
# print("pH: ")