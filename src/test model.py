import numpy as np
import matplotlib.pyplot as plt
from tensorflow import keras

model_file = "env_model" # name of the saved model file
model = keras.models.load_model(model_file)
input_data = np.array([[16.7, -3.4]])
output = model.predict(input_data)
print(output)