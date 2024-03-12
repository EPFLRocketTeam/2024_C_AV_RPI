

import os
import sys
#add site-packages to the path

sys.path.append("/home/marin/.local/lib/python3.10/site-packages")



import csv 
import matplotlib.pyplot as plt

import pandas as pd
import tqdm

import numpy as np
import plotly.graph_objects as go
parent_dir = os.path.dirname(__file__)
print(parent_dir)




# There are multiple folders in the parent directory. We need to get csv file each folder 
# there is an unknown number of levels till we reach the csv files.
# do a recursive search for the csv files
csvList = list()
def get_files(extension ):

    for root, dirs, files in os.walk(parent_dir):
        for file in files:
            
            if file.endswith(extension):
                csvList.append(os.path.join(root, file))
    return csvList
    
    
    


def get_first_row(path):
    #print(f"Reading the first row of the file: {path}")
    with open(path, 'r') as file:
        data = file.read()
    # Split the data into lines
    lines = data.split('\n')
    var = lines[0].split(',')

    if len(var) < 2:
        var = lines[0].split(';')
    return var


def from_text_to_csv(path):
    new_path = path.replace('.txt', '.csv')
    last_var = path.split('/')[-1].replace('.txt', '')
    with open(new_path, 'w') as file:
        writer = csv.writer(file)
        writer.writerow(['Date', 'Time', last_var])
    
    # Read the text file
    with open(path, 'r') as file:
        data = file.read()
    # Split the data into lines
    lines = data.split('\n')
   
    # Create a progress bar
    progress_bar = tqdm.tqdm(total=len(lines), desc='Converting lines')
    
    for line in lines:
        elements = line.split(':')
        last_elem = elements[-1]
        tempLine = line.replace(last_elem, '')
       
        try:
            date, time = tempLine.split(' ')
        except ValueError:
            # Handle the error here
            print("Error occurred while splitting the line:", line)
            continue
        date,time = tempLine.split(' ')
        with open(new_path, 'a') as file:
            writer = csv.writer(file)
            writer.writerow([date, time, last_elem])

        
        # Update the progress bar
        progress_bar.update(1)
    

    # Close the progress bar
    progress_bar.close()

def var_Mapping():
    text_files = get_files('.csv')
    varMap = dict()
    for file in text_files:
        #add the file to the dictionary i
        tempVar = get_first_row(file)
        for i in tempVar:
            if i not in varMap:
                varMap[i] = [file]
            else:
                varMap[i].append(file)
    #for i in varMap:
    # make the variables green in color in the text
        #print(f"\033[92m{i}\033[0m :{varMap[i]} ")
    return varMap





def calculate_norm(acceleration):
    # Parse acceleration values from the CSV file
    accel_values = acceleration.strip('(').strip(')').split(',')
    accel_values = [value.strip().strip('(') for value in accel_values]

    accel_values = [float(value) for value in accel_values]
    
    # Calculate the norm of the acceleration vector
    norm = np.linalg.norm(accel_values)
    return norm






def plot_var(variables):
    varMap = var_Mapping()
    # Get the acceleration data from the CSV file
    acceleration_file = varMap[variables]
    # Read the CSV file
    data = pd.read_csv(acceleration_file)
    # Calculate the norm of the acceleration vector
    data['norm'] = data[variables].apply(calculate_norm)
    # Plot the norm of the acceleration vector with row position on the x-axis
    plt.plot(data['norm'])
    plt.xlabel('Row position')
    plt.ylabel(f'{variables}')
    plt.title(f'{variables} vector over time')

    #plt.show()
    #save in folder -> "plots" with the name of the file being the name of the variables
    if not os.path.exists('plots'):
        os.makedirs('plots')
    print(f"Saving plot as {variables}.png")
    plt.savefig(f'plots/{variables}.png')
    
    # Create a new plot
    fig = go.Figure()
    # Add a line to the plot
    fig.add_trace(go.Scatter(x=data.index, y=data['norm'], mode='lines', name=variables))
    # Update the layout of the plot
    fig.update_layout(title=f'{variables} vector over time', xaxis_title='Row position', yaxis_title=f'{variables} vector')
    # Save the plot as an HTML file
    fig.write_html(f'plots/{variables}.html')
    # Show the plot

#19289
    
def find_row_value(variables, row):
    varMap = var_Mapping()
    # Get the acceleration data from the CSV file
    acceleration_file = varMap[variables]
    # Read the CSV file
    data = pd.read_csv(acceleration_file)
    # Calculate the norm of the acceleration vector
    
    return data['Time(ms)'][row]





#plot_var('acceleration')

def reevaluate_timeStamp(std_time, paths):

    print (f"Standard time: {std_time}")
    print(f"Paths: {paths}")


    for path in paths:
        print(f"Reading the file: {path}")
        #read the csv file and change Time(s) to Time(ms) with panda
        data = pd.read_csv(path)
        #get the time column
        time = data['Time(ms)']
        #convert the time to ms
        time_ms = time.apply(lambda x: from_shitStamp_to_ms(x))
        #subtract the standard time from the time_ms

        new_time = time_ms - std_time
        #add the new time to the data
        data['Time(ms)'] = new_time
      
        #save the new data to a new file
        new_path = path.replace('.csv', '_new.csv')
        data.to_csv(new_path, index=False)
        print(f"New file created: {new_path}")

        


def from_shitStamp_to_ms(shitStamp):
    # Split the timestamp into its components
    components = shitStamp.split(':')
    # Remove the last empty string
    components.pop()
 
    # Convert the components to integers
    components = [float(component) for component in components]
    # Calculate the time in milliseconds
    try:
        time_ms = components[0] * 3600000 + components[1] * 60000 + components[2] * 1000
        return int(time_ms)
    except IndexError:
        print(f"Error occurred while converting the timestamp: {shitStamp},{components}")



def reavgnss_timeStamp(std_time, path):

    #std_time is in s

    std_time = std_time * 1000

    #multiply by 1000 to get the time in ms all elements in column Time(s) using panda 

    #read the csv file
    data = pd.read_csv(path)

    #get the time column
    print(f"Columns: {data.columns}")
    time = data['Time(s)']

    #convert the time to ms
    time_ms = time * 1000

    #subtract the standard time from the time_ms
    new_time = time_ms - std_time

    #add the new time to the data
    data['Time(ms)'] = new_time
    #remove the old time column
    data = data.drop(columns=['Time(s)'])





    #save the new data to a new file
    new_path = path.replace('.csv', '_new.csv')
    data.to_csv(new_path, index=False)
    print(f"New file created: {new_path}")

def mean_cycle(path):
    #read the csv file
    data = pd.read_csv(path)
   
    if 'Time(ms)' not in data.columns:
        print(f"Time(ms) not in {path}")
        return -1
    #get the time column
    time = data['Time(ms)']

    #between each n and n+1, calculate the difference and add it to a list
    time_0 = np.array(time)
    time_1 = np.roll(time, 1)

    #remove the first element of time_1 and the first element of time_0
    time_1 = time_1[1:]
    time_0 = time_0[1:]

    time_diff = time_0 - time_1
    print (f"Time diff: {time_diff[:10]}")
    #calculate the mean of the list
    mean_time_diff = np.mean(time_diff)
    

    return mean_time_diff




    
    

#from_shitStamp_to_ms('12:02:47.993748:')


#43367993
shitPaths = "/mnt/c/Users/marin/Documents/Ma2/2024_C_AV_RPI-1/PythonTest/ERT_Wildhorn_Flight_Data/Team12_PFR_Payload_Data_EuRoC2022/PFR_Payload_Data/SensorData/accelerometer.csv;/mnt/c/Users/marin/Documents/Ma2/2024_C_AV_RPI-1/PythonTest/ERT_Wildhorn_Flight_Data/Team12_PFR_Payload_Data_EuRoC2022/PFR_Payload_Data/SensorData/barometer.csv".split(';')

#reevaluate_timeStamp(43367993, shitPaths)

#reavgnss_timeStamp(2379.4135,"/mnt/c/Users/marin/Documents/Ma2/2024_C_AV_RPI-1/PythonTest/ERT_Wildhorn_Flight_Data/Team12_PFR_Altitude_SRAD Avionics_Logging_Data_EuRoC2022/avionics_data/gnss_data.csv")

cycleMap = dict()
print(f"\033[94m{list(map(lambda x: x.split('/')[-1],var_Mapping()['Time(ms)']))}\033[0m")
#print (f"{var_Mapping()['Time(ms)']}")
#for i in var_Mapping()['Time(ms)']:
    #cycleMap[i] = mean_cycle(i)

#print(f"\033[94m{cycleMap}\033[0m")

def add_to_dic(key ,dic, value):
    if key not in dic.keys():
        dic[key] = [value]
    else:
        dic[key].append(value)
    
dataframeList =[]

def combine_csv_files(paths):
    # Create an empty DataFrame
  
    combined_data = pd.DataFrame()
    
    list_of_dataframes = []
    list_of_timstamps = []
    map_of_timeStamps = dict()
    current_path = 0
    map_of_current_values = dict()
    set_of_col = set()
    # Iterate over the paths
    for path in paths:
        # Read the CSV file
        #print in purple color the current file being read
        print (f" \033[94mReading the file: {path}\033[0m")
       
        tempData = pd.read_csv(path)
        list_of_dataframes.append(tempData)
       
        #print(f"Columns: {tempData['Time(ms)'][:10]}")
        tempData['Time(ms)'].apply(add_to_dic, args=(map_of_timeStamps, current_path))

        set_of_col |= set(tempData.columns)
        

        current_path += 1
    #print(f"Map of timeStamps: {map_of_timeStamps.keys()}")
    list_of_timstamps = list(map_of_timeStamps.keys())
    list_of_timstamps.sort()
    for i in set_of_col:
        combined_data[i] = 0
    
    #print(f"List of timeStamps: {list_of_timstamps}")
    # Create a progress bar for iterating over list_of_timestamps
    progress_bar = tqdm.tqdm(total=len(list_of_timstamps), desc='Combining data')
    
    for i in list_of_timstamps:
        
        where_to_get = map_of_timeStamps[i]
        for j in where_to_get:
            tempData = list_of_dataframes[j]
            for k in tempData.columns:
               # we are looking for the values of column k for time i
            
                if k == 'Time(ms)':
                    continue
                else:
                    #value at in column k for time i which is tempData['Time(ms)'][i]
                    
                    map_of_current_values[k] = tempData[tempData['Time(ms)'] == i][k].values[0]
        #add row to the combined dataframe

        for l in map_of_current_values.keys():
            
            #add row to the combined dataframe
            combined_data = combined_data._append({l:map_of_current_values[l]}, ignore_index=True)


        
        # Update the progress bar
        progress_bar.update(1)
    
    # Close the progress bar



    progress_bar.close()
    dataframeList = list_of_dataframes
    
    print(f"Combined data: {combined_data.columns}")
    #save to a csv file
    combined_data.to_csv('combined_data.csv', index=False)
    



   
paths_to_combine = ["/mnt/c/Users/marin/Documents/Ma2/2024_C_AV_RPI-1/PythonTest/ERT_Wildhorn_Flight_Data/Team12_PFR_Payload_Data_EuRoC2022/PFR_Payload_Data/SensorData/accelerometer_new.csv",
                     "/mnt/c/Users/marin/Documents/Ma2/2024_C_AV_RPI-1/PythonTest/ERT_Wildhorn_Flight_Data/Team12_PFR_Payload_Data_EuRoC2022/PFR_Payload_Data/SensorData/barometer_new.csv",
                    "/mnt/c/Users/marin/Documents/Ma2/2024_C_AV_RPI-1/PythonTest/ERT_Wildhorn_Flight_Data/Team12_PFR_Altitude_SRAD Avionics_Logging_Data_EuRoC2022/avionics_data/gnss_data_new.csv"]
print (f"Paths to combine: {len(paths_to_combine)}")
combine_csv_files(paths_to_combine)

   
    #from_text_to_csv(file)



    

