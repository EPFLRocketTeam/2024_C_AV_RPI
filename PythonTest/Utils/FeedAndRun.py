import pandas as pd
import subprocess
import pickle
import datetime
import sys
import os
import socket

# Sample DataFrame

print(f"current working directory: {os.getcwd()}")
df = pd.read_csv('../data/combined3.csv')
startTime = datetime.datetime.now()
print(f"Start time: {startTime}")
print(f"Dataframe: {df.columns}")
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('localhost', 8888))
server_socket.listen(1)
print(f"Server {server_socket} is listening for requests")
print("Server is listening for requests")
print(df['Time(ms)'].max())
maxTime = df['Time(ms)'].max()
client_socket, client_address = server_socket.accept()

# Function to handle requests
#default infini§te time
def handle_request(maxTime = sys.maxsize ):
    # Get current time
    currentTime = datetime.datetime.now()
    elapsedTime = currentTime - startTime
    elapsedTime = elapsedTime.total_seconds()
    elapsedTime = elapsedTime*1000
    elapsedTime = int(elapsedTime)%maxTime
    
    # Get the current row from the DataFrame
    #if elapsed time is not in the dataframe, use the just before the value eg: 1:00:00 is not in the dataframe, use 0:59:59
    print(f"Elapsed time: {elapsedTime}")
    currentRow = df[df['Time(ms)'] <= elapsedTime].tail(1)
    caps = ""
    for i in df.columns:
        caps += f"{i}:{currentRow[i].values[0]};"

    #remove the last comma
    caps = caps[:-1]
        

    return caps

while True:
    print("Waiting for connection")
    
    print("Connection from:", client_address)
    request = client_socket.recv(1024).decode('utf-8')
   
    
    if request.strip() == "request":
        # Get info based on current time
        print(f"Request received")
        
        info = handle_request(maxTime)
        print(f"Info: {info}")
        client_socket.send(info.encode('utf-8'))
        print(f"Info sent")
        
    
