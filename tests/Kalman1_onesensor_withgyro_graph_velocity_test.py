import pandas as pd
import matplotlib.pyplot as plt

# Load the datasets with nrows parameter
output_file = pd.read_csv('output_Kalman1_onesensor_withgyro_test.csv')
av_training_data = pd.read_csv('Test_AVTrainingData.csv')

# Renaming columns for clarity
output_file.rename(columns={
    'Estimated Velocity x': 'Velocity x (m/s) Output_file',
    'Estimated Velocity y': 'Velocity y (m/s) Output_file',
    'Estimated Velocity z': 'Velocity z (m/s) Output_file'}, inplace=True)
av_training_data.rename(columns={
    'vx(m/s)': 'Velocity x (m/s) AVTrainingData',
    'vy(m/s)': 'Velocity y (m/s) AVTrainingData',
    'vz(m/s)': 'Velocity z (m/s) AVTrainingData'}, inplace=True)

# Plotting
plt.figure(figsize=(10, 6))

# Plot velocity from the first file
plt.plot(output_file['Time'], output_file['Velocity x (m/s) Output_file'], label='Velocity x Output_file', color='blue')
plt.plot(output_file['Time'], output_file['Velocity y (m/s) Output_file'], label='Velocity y Output_file', color='green')
plt.plot(output_file['Time'], output_file['Velocity z (m/s) Output_file'], label='Velocity z Output_file', color='red')

# Plot velocity from the second file
plt.plot(av_training_data['time(s)'], av_training_data['Velocity x (m/s) AVTrainingData'], label='Velocity x AVTrainingData', color='cyan', linestyle='--')
plt.plot(av_training_data['time(s)'], av_training_data['Velocity y (m/s) AVTrainingData'], label='Velocity y AVTrainingData', color='magenta', linestyle='--')
plt.plot(av_training_data['time(s)'], av_training_data['Velocity z (m/s) AVTrainingData'], label='Velocity z AVTrainingData', color='orange', linestyle='--')

# Adding titles and labels
plt.title('Velocity Comparison')
plt.xlabel('Time (seconds)')
plt.ylabel('Velocity (m/s)')
plt.legend()

# Display the plot
plt.show()