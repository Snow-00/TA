import serial

arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 9600 #arduino uno runs at 9600 baud
fileName="dataset_pH.csv" #name of the CSV file generated

ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)
samples = 10 #how many samples to collect
print_labels = False
line = 0

while True:
    # incoming = ser.read(9999)
    # if len(incoming) > 0:
    # if print_labels:
    #     if line==0:
    #         print("Printing Column Headers")
    #     else:
    #         print("Line " + str(line) + ": writing...")
    getData=str(ser.readline())
    data=getData[0:][:-2]
    print(data)

    file = open(fileName, "a")
    file.write(data + "\n") #write data with a newline
    line = line+1

print("Data collection complete!")
file.close()