#!/usr/bin/env python3
"""
Convert NYC TLC Parquet data to CSV format
Usage: python convert_parquet_to_csv.py input.parquet output.csv
"""

import sys
import pandas as pd

def convert_parquet_to_csv(input_file, output_file):
    """Convert Parquet file to CSV format for NYC taxi data"""
    print(f"Reading Parquet file: {input_file}")
    
    try:
        # Read parquet file
        df = pd.read_parquet(input_file)
        
        print(f"Loaded {len(df)} rows with columns: {list(df.columns)}")
        
        # For NYC taxi data, we typically need:
        # - pickup_longitude, pickup_latitude (or pickup location)
        # - pickup_datetime
        # - Other optional fields
        
        # Check for common column name variations
        lon_cols = [col for col in df.columns if 'lon' in col.lower() or 'longitude' in col.lower()]
        lat_cols = [col for col in df.columns if 'lat' in col.lower() and 'longitude' not in col.lower()]
        
        if lon_cols and lat_cols:
            print(f"Found longitude column: {lon_cols[0]}")
            print(f"Found latitude column: {lat_cols[0]}")
        
        # Save to CSV
        print(f"Writing CSV file: {output_file}")
        df.to_csv(output_file, index=False)
        
        print(f"Successfully converted {len(df)} rows to {output_file}")
        return True
        
    except Exception as e:
        print(f"Error: {e}")
        print("\nMake sure you have pandas and pyarrow installed:")
        print("  pip install pandas pyarrow")
        return False

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python convert_parquet_to_csv.py <input.parquet> <output.csv>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    success = convert_parquet_to_csv(input_file, output_file)
    sys.exit(0 if success else 1)



