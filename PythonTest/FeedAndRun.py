import csv

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