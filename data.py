import csv

# Read the CSV data
input_file = 'partition.csv'
output_file = 'partition2.csv'

with open(input_file, mode='r') as infile, open(output_file, mode='w', newline='') as outfile:
    reader = csv.reader(infile)
    writer = csv.writer(outfile)
    
    # Write the header
    header = next(reader)
    new_header = header[:2] + ['speedup']
    writer.writerow(new_header)
    
    # Process each row
    for row in reader:
        partition, array_size, sequential_time, parallel_time = row[:4]
        speed = float(sequential_time) / float(parallel_time)
        new_row = [partition, array_size, speed]
        writer.writerow(new_row)
