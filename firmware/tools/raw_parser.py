"""
This script parses the raw data from the pico tracker.
At the very bottom the data is printed. Feel free to change
the print statement to suit the data you're interested in.
The parsed data is a list of dicts, with keys 'time', 'coords',
'battery', 'solar', 'temperature' and 'satellites'.

Data is expected in the form of a series of lines containing
'/A={6 digits} {6 digits}z.{18 chars}{11 chars}\n'. Anything
else will be ignored.
"""

import re
import sys
from ukhas_format import *
from habitat_upload import *
from datetime import datetime
from math import log, exp

"""
Decodes a 'base 91' encoded string
"""
def base91_decode(enc_str):
    enc_ints = [ord(x) - 33 for x in enc_str]
    powers = range(len(enc_ints))[::-1]
    return sum(x * pow(91, i) for x, i in zip(enc_ints, powers))

"""
Takes a parsed telemetry line and returns a datetime
Assumes data is from the last month, as per the current machine's time
"""
def extract_time(line):
    # Capture a 6 digit string
    p = re.compile(r'(\d{6})z\S{20}')
    match = p.match(line)

    if match == None:
        return None
    else:
        # Get a datetime object
        dt = datetime.strptime(match.group(1), '%d%H%M')
        now = datetime.now()

        if dt.day > now.day: # from last month
            now = now - timedelta(months = 1)

        # fill in month and year
        dt = dt.replace(year=now.year, month=now.month)

        return dt

"""
Takes a parsed telemetry line and returns latitude, longitude and
altitude. It decodes from base 91 along the way
"""
def extract_lat_long_alt(line):
    # Capture a 4 char encoded latitude
    p = re.compile(r'\d{6}z(\S{4})(\S{4})(\S{2})\S{10}')
    match = p.match(line)

    if match == None:
        return None
    else:
        enc_lat, enc_long, enc_alt = match.groups()

        # Lat/long in fractional degrees, alt in metres
        latitude =  90.0 - (base91_decode(enc_lat) / 380926.0)
        longitude = -180.0 + (base91_decode(enc_long) / 190463.0)
        altitude = exp(log(1.002) * base91_decode(enc_alt)) / 3.2808

        return (latitude, longitude, altitude)

"""
Takes a parsed telemetry line and returns readings on battery,
temperature_external, temperature_internal, satellites and ttff.
It decodes from base91 along the way
"""
def extract_telemetry(line):
    # Capture an 10 char encoded telemetry segment
    p = re.compile(r'\d{6}z\S{10}(\S{10})')
    match = p.match(line)

    if match == None:
        return None
    else:
        tel = match.group(1)

        # Split into 2 char chunks
        parts = [tel[i:i+2] for i in range(0, 10, 2)]
        batt_enc, temp_e_enc, temp_i_enc, sat_enc, ttff_enc = tuple(parts)

        # Reverse aprs conversions
        battery = base91_decode(batt_enc) / 1000.0
        temperature_e = (base91_decode(temp_e_enc) / 10) - 273.2
        temperature_i = (base91_decode(temp_i_enc) / 10) - 273.2
        satellite_count = base91_decode(sat_enc)
        ttff = base91_decode(ttff_enc)

        return (battery, temperature_e, temperature_i, satellite_count, ttff)

"""
Exracts the 'raw data' segment from a line of data; this is the 20
character section after \d{6}z
"""
def extract_raw_data(line):
    # Capture the raw data segment
    p = re.compile(r'(\d{6}z\S{20})\|')
    match = p.search(line)

    if match == None:
        return None
    else:
        return match.group(1)

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

# Get the name of the input file
if len(sys.argv) >= 2:
    file_name = sys.argv[1]
else:
    file_name = raw_input("File to read (rawdata.txt): ") or "rawdata.txt"

# Get the flight number
if len(sys.argv) >= 3:
    flight_number = sys.argv[2]
else:
    flight_number = raw_input("Flight Number (xx): ") or "xx"

# Callsign
callsign = "UBSEDS"+flight_number

with open(file_name, 'r') as data_file:
    data = []

    for line in data_file:
        # Extract raw data string
        raw = extract_raw_data(line)

        if raw == None:
            continue
        else:
            tele = extract_telemetry(raw)

            datum = {
                'time': extract_time(raw),
                'coords': extract_lat_long_alt(raw),
                'battery': tele[0],
                'temperature_e': tele[1],
                'temperature_i': tele[2],
                'satellites': tele[3],
                'ttff': tele[4]
            }

            if datum not in data: # unique values only
                data.append(datum)

    # Sort data lines by time
    data = sorted(data, key=lambda x: x['time'])

    # Print data
    for datum in data:
        print "{}: {:.6f} {:.6f}, {}m {}V {}C {}C sats {} ttff {}".format(
            str(datum['time']),
            datum['coords'][0], datum['coords'][1], int(round(datum['coords'][2])),
            datum['battery'], datum['temperature_e'], datum['temperature_i'],
            datum['satellites'], datum['ttff'])

    # Upload data to habitat
    for datum in data:
        ukhas_str = ukhas_format(datum, callsign)
        try:
            print ukhas_str
            print habitat_upload(datum['time'], ukhas_str)
        except:
            None
