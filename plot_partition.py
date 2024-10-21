import pandas as pd
import matplotlib.pyplot as plt

# Create a DataFrame from the data
data = {
    "partition": [
        "partition_lomuto",
        "partition_lomuto",
        "partition_lomuto",
        "partition_lomuto",
        "partition_lomuto",
        "partition_median_of_three",
        "partition_median_of_three",
        "partition_median_of_three",
        "partition_median_of_three",
        "partition_median_of_three",
        "partition_hoare",
        "partition_hoare",
        "partition_hoare",
        "partition_hoare",
        "partition_hoare"
    ],
    "array_size": [1024, 32768, 1048576, 33554432, 1073741824, 1024, 32768, 1048576, 33554432, 1073741824, 1024, 32768, 1048576, 33554432, 1073741824],
    "speedup": [1.1076923076923078, 0.993212669683258, 0.9838860652757471, 0.940361428314591, 0.8642569603578659, 1.0, 1.123706338939198, 1.0986275490359505, 0.9478567931382018, 0.900335894803734, 1.3257575757575757, 1.1376930660532367, 1.0915344817783843, 0.9354043613380846, 0.8952447858804718]
}

df = pd.DataFrame(data)

# Create a line chart with different colors for each partition type
plt.figure(figsize=(10, 6))
for partition_type in df["partition"].unique():
    partition_data = df[df["partition"] == partition_type]
    plt.plot(partition_data["array_size"], partition_data["speedup"], label=partition_type)

# Set labels and title
plt.xlabel("Array Size")
plt.ylabel("Speedup")
plt.title("Partition Type vs. Speedup")

# Add legend
plt.legend()

# Grid lines
plt.grid(True)

# Show the plot
plt.show()