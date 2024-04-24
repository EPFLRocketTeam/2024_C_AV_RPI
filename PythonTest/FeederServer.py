import socket
import threading
import pandas as pd
import time
import sys


df =pd.read_csv('./Test_AVTrainingData.csv')



def find_closest_row(df, arbitrary_timestamp):
    df_sorted = df.sort_values('time(s)')
    closest_index = df_sorted['time(s)'].searchsorted(arbitrary_timestamp, side='right') - 1
    closest_row = df_sorted.iloc[closest_index]
    return closest_row

def handle_client(client_socket, df,start_time=time.time()  ):
    data = client_socket.recv(1024)
    if data:
        arbitrary_timestamp = int(time.time() - start_time)
        closest_row = find_closest_row(df, arbitrary_timestamp)
        # response = "10.93692, -135.6988, 141.0303, 1194.959, -28.595, 32.00462, 214.5993, -2.50537, 3.041237, 9.567583, 2.711033e-09, 0.005679535, 0.006876037, 86402.2, 2.0, nan, nan"
        response = ','.join(map(str, closest_row.values))
        print(f"Sending response: {response}")
        client_socket.sendall(response.encode())
        client_socket.close()
        
    #client_socket.close()
    

def start_server(host, port, df):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(5)
    print(f"Server listening on {host}:{port}...")

    start_time = time.time()
    while True:
        print("Waiting for connection...")
        client_socket, client_address = server_socket.accept()
        #new thread for each client
        client_socket.settimeout(1)
        client_thread = threading.Thread(target=handle_client, args=(client_socket, df, start_time))
        client_thread.start()
        print(f"Connection from {client_address} has been established!")
    
    
        


if __name__ == "__main__":
    HOST = '127.0.0.1'
    PORT = 12345
    start_server(HOST, PORT, df)