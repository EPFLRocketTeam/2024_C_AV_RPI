import csv
<<<<<<< HEAD

def get_data_from_csv():
    # Read data from a CSV file
    # This is a sample implementation, you should adjust it according to your CSV format
    with open('data.csv', 'r') as csvfile:
        reader = csv.reader(csvfile)
        data = next(reader)  # Assuming the CSV file contains one row of data
    return ','.join(data)

def main():
    while True:
        # Wait for a request from the C++ program
        request = input()

        if request.strip() == 'request':
            # Get data from CSV
            data = get_data_from_csv()

            # Send data to the C++ program
            print(data)
            # Add a condition to break the loop if needed
            # For instance, if all data from the CSV has been sent, you can break the loop

if __name__ == "__main__":
    main()
=======
import subprocess

# Function to read data from CSV
def read_data_from_csv(filename):
    data = []
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            data.append(row)
    return data

# Function to write data to CSV
def write_data_to_csv(filename, data):
    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerows(data)

# Main function
if __name__ == "__main__":
    while True:
        # Read data from input CSV
        input_data = read_data_from_csv('input.csv')

        # Assuming input.csv has int, float, string values
        int_value, float_value, string_value = input_data[0]

        # Call the C program and pass input
        process = subprocess.Popen(['./c_program'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)

        # Send input to the C program
        process.stdin.write(f"{int_value} {float_value} {string_value}\n")
        process.stdin.flush()

        # Get output from the C program
        c_output = process.stdout.readline().strip()

        # Process the output (if needed)
        print("Output from C program:", c_output)

        # Update the input CSV file (if needed)
        # For simplicity, let's just remove the first row
        input_data.pop(0)
        write_data_to_csv('input.csv', input_data)
>>>>>>> d1acead (correction Avstate and CMake creation)
