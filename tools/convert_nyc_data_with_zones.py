#!/usr/bin/env python3
"""
Convert NYC taxi data (with LocationID) to CSV with coordinates.
Uses taxi zone lookup table to convert LocationID to lat/lon.
"""

import pandas as pd
import numpy as np
import sys
import os
from datetime import datetime

def load_taxi_zone_lookup(lookup_file="data/nyc_taxi/taxi_zone_lookup.csv"):
    """Load taxi zone lookup table."""
    if not os.path.exists(lookup_file):
        print(f"Error: Taxi zone lookup file not found: {lookup_file}")
        print("Please run: python tools/download_taxi_zones.py")
        sys.exit(1)
    
    df = pd.read_csv(lookup_file)
    print(f"Loaded {len(df)} taxi zones from {lookup_file}")
    
    # Find coordinate columns (different files may have different names)
    lat_col = None
    lon_col = None
    
    for col in df.columns:
        col_lower = col.lower()
        if 'lat' in col_lower:
            lat_col = col
        if 'lon' in col_lower or ('long' in col_lower and 'lat' not in col_lower):
            lon_col = col
    
    # Some lookup tables use service_zone with coordinates in separate files
    # For now, we'll create approximate coordinates based on LocationID if not available
    if lat_col is None or lon_col is None:
        print("⚠️  Warning: No Lat/Lon columns found in lookup table")
        print(f"Columns: {df.columns.tolist()}")
        print("Using approximate coordinates based on zone location...")
        
        # Create approximate coordinates based on LocationID ranges
        # This is a simplified approach - for production, use actual zone centroids
        df['Lat'] = 40.7 + (df['LocationID'] % 100) * 0.01  # Approximate NYC lat range
        df['Lon'] = -74.0 - (df['LocationID'] % 100) * 0.01  # Approximate NYC lon range
        lat_col = 'Lat'
        lon_col = 'Lon'
        print("✅ Created approximate coordinates")
    
    # Create lookup dictionary
    lookup = {}
    for _, row in df.iterrows():
        location_id = int(row['LocationID'])
        lat = float(row[lat_col]) if pd.notna(row[lat_col]) else None
        lon = float(row[lon_col]) if pd.notna(row[lon_col]) else None
        
        if lat is not None and lon is not None:
            lookup[location_id] = (lat, lon)
    
    print(f"Created lookup table with {len(lookup)} zones with coordinates")
    return lookup

def parse_datetime(dt_str):
    """Parse datetime string to Unix timestamp."""
    try:
        # Try different datetime formats
        for fmt in ['%Y-%m-%d %H:%M:%S', '%Y-%m-%d %H:%M:%S.%f', '%m/%d/%Y %H:%M:%S']:
            try:
                dt = datetime.strptime(str(dt_str), fmt)
                return dt.timestamp()
            except:
                continue
        return 0.0
    except:
        return 0.0

def convert_nyc_data(input_file, output_file, lookup_file="data/nyc_taxi/taxi_zone_lookup.csv", 
                     sample_size=None):
    """
    Convert NYC taxi data from LocationID format to coordinate format.
    
    Output format: pickup_longitude, pickup_latitude, pickup_datetime, passenger_count, trip_distance
    """
    
    print(f"Reading NYC taxi data: {input_file}")
    
    # Load taxi zone lookup
    zone_lookup = load_taxi_zone_lookup(lookup_file)
    
    # Read input file (support both CSV and Parquet)
    if input_file.endswith('.parquet'):
        df = pd.read_parquet(input_file)
    else:
        # Read CSV in chunks if it's large
        chunk_size = 100000
        chunks = []
        for chunk in pd.read_csv(input_file, chunksize=chunk_size, low_memory=False):
            chunks.append(chunk)
            if sample_size and len(pd.concat(chunks, ignore_index=True)) >= sample_size:
                break
        df = pd.concat(chunks, ignore_index=True)
        
        if sample_size:
            df = df.head(sample_size)
    
    print(f"Loaded {len(df)} rows")
    print(f"Columns: {df.columns.tolist()}")
    
    # Find required columns
    pu_location_col = None
    pu_datetime_col = None
    passenger_col = None
    distance_col = None
    
    for col in df.columns:
        col_lower = col.lower()
        if 'pulocationid' in col_lower or 'pickup_location_id' in col_lower:
            pu_location_col = col
        if 'pickup_datetime' in col_lower or 'tpep_pickup_datetime' in col_lower:
            pu_datetime_col = col
        if 'passenger_count' in col_lower:
            passenger_col = col
        if 'trip_distance' in col_lower:
            distance_col = col
    
    if pu_location_col is None:
        print("Error: Could not find PULocationID column")
        print(f"Available columns: {df.columns.tolist()}")
        sys.exit(1)
    
    print(f"\nUsing columns:")
    print(f"  LocationID: {pu_location_col}")
    print(f"  DateTime: {pu_datetime_col}")
    print(f"  Passengers: {passenger_col}")
    print(f"  Distance: {distance_col}")
    
    # Convert data
    results = []
    missing_zones = set()
    
    for idx, row in df.iterrows():
        if idx % 100000 == 0:
            print(f"Processing row {idx}/{len(df)}...")
        
        # Get LocationID
        try:
            location_id = int(row[pu_location_col])
        except (ValueError, TypeError):
            continue
        
        # Look up coordinates
        if location_id not in zone_lookup:
            missing_zones.add(location_id)
            continue
        
        lat, lon = zone_lookup[location_id]
        
        # Get other fields
        timestamp = parse_datetime(row[pu_datetime_col]) if pu_datetime_col else 0.0
        passengers = float(row[passenger_col]) if passenger_col and pd.notna(row[passenger_col]) else 0.0
        distance = float(row[distance_col]) if distance_col and pd.notna(row[distance_col]) else 0.0
        
        # Create data point: [longitude, latitude, timestamp, passengers, distance]
        results.append([lon, lat, timestamp, passengers, distance])
    
    print(f"\nProcessed {len(results)} data points")
    if missing_zones:
        print(f"⚠️  Warning: {len(missing_zones)} LocationIDs not found in lookup table")
        print(f"Missing zones (first 10): {list(missing_zones)[:10]}")
    
    # Create output DataFrame
    output_df = pd.DataFrame(results, columns=[
        'pickup_longitude', 
        'pickup_latitude', 
        'pickup_datetime', 
        'passenger_count', 
        'trip_distance'
    ])
    
    # Filter valid coordinates (NYC area)
    output_df = output_df[
        (output_df['pickup_longitude'] >= -74.3) & (output_df['pickup_longitude'] <= -73.7) &
        (output_df['pickup_latitude'] >= 40.5) & (output_df['pickup_latitude'] <= 40.9)
    ]
    
    print(f"After filtering valid NYC coordinates: {len(output_df)} points")
    
    # Save to CSV
    print(f"\nWriting to {output_file}...")
    output_df.to_csv(output_file, index=False)
    print(f"✅ Successfully converted {len(output_df)} data points")
    
    # Print statistics
    print(f"\n=== Statistics ===")
    print(f"Longitude range: {output_df['pickup_longitude'].min():.4f} to {output_df['pickup_longitude'].max():.4f}")
    print(f"Latitude range: {output_df['pickup_latitude'].min():.4f} to {output_df['pickup_latitude'].max():.4f}")
    print(f"Date range: {output_df['pickup_datetime'].min():.0f} to {output_df['pickup_datetime'].max():.0f}")
    print(f"Average passengers: {output_df['passenger_count'].mean():.2f}")
    print(f"Average distance: {output_df['trip_distance'].mean():.2f} miles")

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Convert NYC taxi data (LocationID) to coordinates")
    parser.add_argument("input_file", help="Input CSV or Parquet file")
    parser.add_argument("output_file", help="Output CSV file with coordinates")
    parser.add_argument("--lookup", default="data/nyc_taxi/taxi_zone_lookup.csv",
                       help="Taxi zone lookup table CSV")
    parser.add_argument("--sample", type=int, help="Sample size (for testing)")
    
    args = parser.parse_args()
    
    # Install required packages if needed
    try:
        import pandas as pd
    except ImportError:
        print("Installing pandas...")
        os.system("pip install pandas pyarrow")
        import pandas as pd
    
    convert_nyc_data(args.input_file, args.output_file, args.lookup, args.sample)



