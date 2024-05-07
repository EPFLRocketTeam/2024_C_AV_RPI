import pandas as pd
import matplotlib.pyplot as plt

# Specify the number of rows to read
rows_to_read = 60764-1

# Load the datasets with nrows parameter
output_file = pd.read_csv('output_Kalman_test.csv', nrows=rows_to_read)
combined_data = pd.read_csv('combined_data.csv', nrows=rows_to_read)

# Convert the second file's time to seconds from milliseconds
combined_data['Time(s)'] = (combined_data['Time(ms)'] + 2463759.0) / 1000.0

# Renaming columns for clarity
output_file.rename(columns={'Estimated Altitude': 'Altitude (m) Output_file'}, inplace=True)
combined_data.rename(columns={'altitude(m)': 'Altitude (m) Combined_data'}, inplace=True)

# Plotting
plt.figure(figsize=(10, 6))

# Plot velocity from the first file
plt.plot(output_file['Time'], output_file['Altitude (m) Output_file'], label='Altitude (m) Output_file', color='blue')

# Plot velocity from the second file
plt.plot(combined_data['Time(s)'], combined_data['Altitude (m) Combined_data'], label='Altitude (m) Combined_data', color='red')

# Adding titles and labels
plt.title('Altitude Comparison')
plt.xlabel('Time (seconds)')
plt.ylabel('Altitude (m)')
plt.legend()

# Display the plot
plt.show()