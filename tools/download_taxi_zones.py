#!/usr/bin/env python3
"""
Download NYC Taxi Zone Lookup Table and convert to CSV format.
This table maps LocationID to zone center coordinates.
"""

import pandas as pd
import requests
import os
import sys

def download_taxi_zone_lookup(output_file="data/nyc_taxi/taxi_zone_lookup.csv"):
    """
    Download taxi zone lookup table from NYC TLC.
    This table contains LocationID, Borough, Zone, and approximate coordinates.
    """
    
    # NYC TLC Taxi Zone Lookup Table URL
    url = "https://d37ci6vzurychx.cloudfront.net/misc/taxi+_zone_lookup.csv"
    
    print(f"Downloading taxi zone lookup table from NYC TLC...")
    print(f"URL: {url}")
    
    try:
        # Download the file
        response = requests.get(url, timeout=30)
        response.raise_for_status()
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        
        # Save to file
        with open(output_file, 'wb') as f:
            f.write(response.content)
        
        print(f"Downloaded to: {output_file}")
        
        # Read and display summary
        df = pd.read_csv(output_file)
        print(f"\nLoaded {len(df)} taxi zones")
        print(f"Columns: {df.columns.tolist()}")
        print(f"\nFirst few rows:")
        print(df.head())
        
        # Check if we have coordinates
        if 'Lat' in df.columns and 'Lon' in df.columns:
            print(f"\n✅ Found Lat/Lon columns - coordinates available!")
        elif 'latitude' in df.columns and 'longitude' in df.columns:
            print(f"\n✅ Found latitude/longitude columns - coordinates available!")
        else:
            print(f"\n⚠️  No coordinate columns found. Columns: {df.columns.tolist()}")
            print("We may need to use zone centroids from shapefile.")
        
        return output_file
        
    except Exception as e:
        print(f"Error downloading taxi zone lookup: {e}")
        print("\nTrying alternative: creating lookup from known zone centers...")
        return create_basic_lookup_table(output_file)

def create_basic_lookup_table(output_file="data/nyc_taxi/taxi_zone_lookup.csv"):
    """
    Create a basic lookup table with common taxi zone IDs and approximate coordinates.
    This is a fallback if download fails.
    """
    
    # Common NYC taxi zones with approximate center coordinates
    # These are approximate - for production use, download the official lookup table
    zones = [
        # Manhattan zones (approximate centers)
        {"LocationID": 1, "Zone": "Newark Airport", "Borough": "EWR", "Lat": 40.6895, "Lon": -74.1745},
        {"LocationID": 2, "Zone": "JFK Airport", "Borough": "Queens", "Lat": 40.6413, "Lon": -73.7781},
        {"LocationID": 3, "Zone": "LaGuardia Airport", "Borough": "Queens", "Lat": 40.7769, "Lon": -73.8740},
        # Add more zones as needed
        # For now, we'll use a more comprehensive approach: download the actual file
    ]
    
    print(f"\nCreating basic lookup table with {len(zones)} zones...")
    print("⚠️  This is a limited fallback. Please download the official lookup table.")
    
    df = pd.DataFrame(zones)
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    df.to_csv(output_file, index=False)
    
    return output_file

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Download NYC Taxi Zone Lookup Table")
    parser.add_argument("-o", "--output", default="data/nyc_taxi/taxi_zone_lookup.csv",
                       help="Output CSV file path")
    
    args = parser.parse_args()
    
    # Install requests if needed
    try:
        import requests
    except ImportError:
        print("Installing requests package...")
        os.system("pip install requests")
        import requests
    
    download_taxi_zone_lookup(args.output)



