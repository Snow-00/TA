import numpy as np
import matplotlib.pyplot as plt
from tensorflow import keras
from pandas import read_csv

csv_file = "../dataset/refined_dataset.csv"
TOTAL_COL = 4
OUT_COL = 2
input_col = TOTAL_COL - OUT_COL

class myCallback(keras.callbacks.Callback):
    # checking accuracy if loss less than 83%
    def on_epoch_end(self, epoch, logs={}):
        threshold = 0.17
        if(logs.get('loss') < threshold):
            print(f"\nExceeds {threshold}, finish training model!")
            self.model.stop_training = True

def create_model(x, y):
    # callback = myCallback()
    model = keras.models.Sequential([
        keras.layers.Dense(8, activation='relu', input_shape=[input_col]),
        keras.layers.Dense(16, activation='relu'),
        keras.layers.Dense(32, activation='relu'),
        keras.layers.Dense(64, activation='relu'),
        # keras.layers.Dropout(0.5),
        keras.layers.Dense(128, activation='relu'),
        # keras.layers.Dropout(0.5),
        keras.layers.Dense(OUT_COL)
    ])

    model.compile(optimizer= keras.optimizers.Adam(learning_rate=0.001), loss='mse', metrics=[keras.metrics.RootMeanSquaredError()])
    model.fit(x,
              y,
              epochs=2500,
              batch_size=32)

    model.save('testModel')

    return model

dataframe = read_csv(csv_file, header=None)
dataset = dataframe.values

train_data = int(len(dataset) * 0.8)
x_train = dataset[:, :input_col]
y_train = dataset[:, input_col:]

# x_test = dataset[train_data:, :input_col]
# y_test = dataset[train_data:, input_col]

# model = create_model(x_train, y_train)
model = keras.models.load_model('NutrientModel')
# print((model.weights))
model.summary()

pH_data = []
TDS_data = []

pH_set = np.array([3.2, 5.2, 4, 0, 0])
TDS_set = np.array([6, 9, 5, 1, 2.9])

while True:
    cmd = input("Input data: ")
    if cmd == "stop":
        break
    
    cmd = cmd.split(",")
    x_test = np.array(cmd, dtype=float)
    x_test = np.expand_dims(x_test, axis=0)
    # print(x_test[0])
    # print(x_test[0].shape)
    output = np.array(model.predict(x_test))
    pH_data.append(round(output[0][0], 2))
    TDS_data.append(round(output[0][1], 2))
    print(output)


# a = plt.axes(aspect='equal')
# plt.scatter(y_test, y_pred)   
# plt.xlabel('True values')
# plt.ylabel('Predicted values')
# plt.title('A plot that shows the true and predicted values')
# plt.xlim([0, 60])
# plt.ylim([0, 60])
# plt.plot([0, 60], [0, 60])
# plt.show()