import pandas as pd
import matplotlib.pyplot as plt

# Load the datasets with nrows parameter
output_file = pd.read_csv('new_output_Kalman_test.csv')
av_training_data = pd.read_csv('Test_AVTrainingData.csv')

# Renaming columns for clarity
output_file.rename(columns={'Estimated Altitude': 'Altitude (m) Output_file'}, inplace=True)
av_training_data.rename(columns={'posz(m)': 'Altitude (m) AVTrainingData'}, inplace=True)

# Plotting
plt.figure(figsize=(10, 6))

# Plot velocity from the first file
plt.plot(output_file['Time'], output_file['Altitude (m) Output_file'], label='Altitude (m) Output_file', color='blue')

# Plot velocity from the second file
plt.plot(av_training_data['time(s)'], av_training_data['Altitude (m) AVTrainingData'], label='Altitude (m) AVTrainingData', color='red')

# Adding titles and labels
plt.title('Altitude Comparison')
plt.xlabel('Time (seconds)')
plt.ylabel('Altitude (m)')
plt.legend()

# Display the plot
plt.show()
