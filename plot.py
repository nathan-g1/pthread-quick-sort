import pandas as pd
import matplotlib.pyplot as plt

# Assuming your CSV data is in a file named 'your_data.csv'
data = pd.read_csv("threshold_v_time.csv")

filtered_data = data[data['threshold'] <= 2000000]

# Group the data by array size and create separate dataframes
array_sizes = filtered_data['array_size'].unique()
dataframes = {size: filtered_data[data['array_size'] == size] for size in array_sizes}

# Create the plot with a wider x-axis
plt.figure(figsize=(20, 10))  # Increased the width from 20 to 30

# Plot each array size's data
for size, df in dataframes.items():
    plt.plot(df['threshold'], df['time'], label=f'Array Size: {size} (line)')

plt.xlabel("Threshold")
plt.ylabel("Time")
plt.title("Time vs. Threshold for Different Array Sizes")
plt.grid(True)

# Adjust y-axis scale if needed
# plt.yscale('log')

# Add a legend
plt.legend()

plt.xticks(filtered_data['threshold'].unique())

plt.show()