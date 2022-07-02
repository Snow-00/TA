import numpy as np
import matplotlib.pyplot as plt
from tensorflow import keras
from pandas import read_csv

def create_model(x, y, z):
    inputs = keras.Input(shape=(12))
    layer = keras.layers.Dense(16, activation='relu')(inputs)
    layer = keras.layers.Dense(32, activation='relu')(layer)
    layer = keras.layers.Dense(64, activation='relu')(layer)
    layer = keras.layers.Dense(128, activation='relu')(layer)
    out1 = keras.layers.Dense(1)(layer)
    out2 = keras.layers.Dense(1)(layer)

    model = keras.Model(inputs=inputs, outputs=[out1, out2])
    print((model.weights))
    model.summary()
    model.compile(optimizer='adam', loss='mse')
    model.fit(x,
              [y, z],
              epochs=10,
              batch_size=32)
    
    return model

dataframe = read_csv("housing.csv", delim_whitespace=True, header=None)
dataset = dataframe.values

train_data = int(len(dataset) * 0.8)
x_train = dataset[:train_data,:12]
y_train = dataset[:train_data,12]
z_train = dataset[:train_data,13]
a_train = dataset[:train_data,12:]

x_test = dataset[train_data:,:12]
x_test = np.expand_dims(x_test, axis=1)

model = create_model(x_train, y_train, z_train)
output1, output2 = model.predict(x_test[0])
print((output1[0][0]))


# y_pred = model.predict(x_test).flatten()

# a = plt.axes(aspect='equal')
# plt.scatter(y_test, y_pred)
# plt.xlabel('True values')
# plt.ylabel('Predicted values')
# plt.title('A plot that shows the true and predicted values')
# plt.xlim([0, 60])
# plt.ylim([0, 60])
# plt.plot([0, 60], [0, 60])
# plt.show()