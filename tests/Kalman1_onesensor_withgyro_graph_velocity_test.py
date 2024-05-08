import pandas as pd
import matplotlib.pyplot as plt

# Load the datasets with nrows parameter
output_file = pd.read_csv('output_Kalman1_onesensor_withgyro_test.csv')
av_training_data = pd.read_csv('Test_AVTrainingData.csv')

# Renaming columns for clarity
output_file.rename(columns={'Estimated Velocity': 'Velocity (m/s) Output_file'}, inplace=True)
av_training_data.rename(columns={'vz(m/s)': 'Velocity (z) (m/s) AVTrainingData'}, inplace=True)

# Plotting
plt.figure(figsize=(10, 6))

# Plot velocity from the first file
plt.plot(output_file['Time'], output_file['Velocity (m/s) Output_file'], label='Velocity (m/s) Output_file', color='blue')

# Plot velocity from the second file
plt.plot(av_training_data['time(s)'], av_training_data['Velocity (z) (m/s) AVTrainingData'], label='Velocity (z) (m/s) AVTrainingData', color='red')

# Adding titles and labels
plt.title('Velocity Comparison')
plt.xlabel('Time (seconds)')
plt.ylabel('Velocity (m/s)')
plt.legend()

# Display the plot
plt.show()