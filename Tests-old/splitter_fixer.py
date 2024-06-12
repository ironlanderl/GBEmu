import json
import os
from tqdm import tqdm

# Define the directory containing the JSON files
directory = 'C:\\Users\\fabri\\source\\GBEmu\\test_data\\'  # Ensure this path points to the correct directory

# Create a range for file names from CB00 to CBFF
file_range = [f'CB{i:02X}.json' for i in range(0x00, 0x100)]

# Use tqdm for progress indication
for filename in tqdm(file_range, desc="Processing files"):
    filepath = os.path.join(directory, filename)

    # Check if the file exists
    if os.path.isfile(filepath):
        tests = []

        # Open and read the JSON file
        with open(filepath, 'r') as file:
            for line in file:
                if line.strip():
                    tests.append(json.loads(line))

        # Write the combined JSON array back to the same file
        with open(filepath, 'w') as file:
            json.dump(tests, file, indent=2)

        tqdm.write(f"Merged JSON objects written back to {filepath}")
    else:
        tqdm.write(f"File not found: {filepath}")
