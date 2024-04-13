import socket
import threading
import pandas as pd
import time

# Sample DataFrame

df = pd.read_csv('data.csv')

def find_closest_row(df, arbitrary_timestamp):
    df_sorted = df.sort_values('Time(ms)')
    closest_index = df_sorted['Time(ms)'].searchsorted(arbitrary_timestamp, side='right') - 1
    closest_row = df_sorted.iloc[closest_index]
    return closest_row

def handle_client(client_socket, df,start_time=time.time()  ):
    data = client_socket.recv(1024)
    if data:
        arbitrary_timestamp = int(time.time() - start_time) * 1000
        closest_row = find_closest_row(df, arbitrary_timestamp)
        response = closest_row.to_json()
        print(f"Sending response: {response}")
        client_socket.sendall(response.encode())
        
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
        print(f"Connection from {client_address}")
        data = client_socket.recv(1024)
        print(f"Received data: {data}")
        if data:
            arbitrary_timestamp = int(time.time() - start_time) * 1000
            closest_row = find_closest_row(df, arbitrary_timestamp)
            response = closest_row.to_json()
            print(f"Sending response: {response}")
            client_socket.sendall(response.encode())
        client_socket.close()


if __name__ == "__main__":
    HOST = '127.0.0.1'
    PORT = 12345
    start_server(HOST, PORT, df)