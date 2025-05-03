import serial
import re
from collections import defaultdict
import time
# Configure serial port (adjust '/dev/ttyUSB0' and baud rate as needed)
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

packet_regex = re.compile(r'PKT_ID(\d+)PT:(\d+)SF:(\d+)CR:(\d+)txPower:(\d+)BW:([\d\.]+)RSSI(-?\d+)SNR(-?\d+)LEN(\d+)Timestamp(\d+)')
# Regular expression to parse valid packets

# Data structures for storing packets
packets = defaultdict(list)

# Mapping packet types
packet_type_map = {"1": "small", "2": "medium", "3": "big", "4": "sync"}
print("Starting LoRa Packet Logger...")
expected_packet_count = 10
last_data_time = time.time()

while True:
    try:
        line = ser.readline().decode('utf-8').strip()

        match = packet_regex.match(line)
        if (time.time() - last_data_time) > 60:
            for group_type in ['small', 'medium', 'big']:
                group_packets = packets[group_type]
                if group_packets:
                    received_count = len(group_packets)
                    pdr = (received_count / expected_packet_count) * 100

                    # Assume params are same within group; take first packet's params
                    params = group_packets[0]

                    # Write to corresponding file
                    filename = f"{group_type}_packets.txt"
                    with open(filename, 'a') as file:
                        file.write(f"Packet group ({group_type.upper()}):\n")
                        file.write(f"Params: SF={params['SF']} CR={params['CR']} txPower={params['txPower']} BW={params['BW']}\n")
                        file.write(f"Received packets: {received_count}/10 (PDR={pdr}%)\n")
                            #file.write("Packet IDs: " + ', '.join([p['pkt_id'] for p in group_packets]) + "\n")
                        file.write("Packets (ID: Timestamp):\n")
                        for p in group_packets:
                            file.write(f"{p['pkt_id']}: {p['Timestamp']}\n")
                        file.write("-" * 50 + "\n")

                # Reset groups
                packets = defaultdict(list)


        if match:
            last_data_time = time.time()
            pkt_id, pkt_type, sf, cr, tx_power, bw, rssi, snr, length, timestamp = match.groups()

            packet_info = {
                'pkt_id': pkt_id,
                'SF': sf,
                'CR': cr,
                'txPower': tx_power,
                'BW': bw,
                'RSSI': rssi,
                'SNR': snr,
                'LEN': length,
                'Timestamp': timestamp
            }
 
            type_str = packet_type_map.get(pkt_type, 'unknown')
            print(type_str)
            if type_str == 'sync':
                # On sync, process groups
                for group_type in ['small', 'medium', 'big']:
                    group_packets = packets[group_type]
                    if group_packets:
                        received_count = len(group_packets)
                        pdr = (received_count / expected_packet_count) * 100

                        # Assume params are same within group; take first packet's params
                        params = group_packets[0]

                        # Write to corresponding file
                        filename = f"{group_type}_packets.txt"
                        with open(filename, 'a') as file:
                            file.write(f"Packet group ({group_type.upper()}):\n")
                            file.write(f"Params: SF={params['SF']} CR={params['CR']} txPower={params['txPower']} BW={params['BW']}\n")
                            file.write(f"Received packets: {received_count}/10 (PDR={pdr}%)\n")
                            #file.write("Packet IDs: " + ', '.join([p['pkt_id'] for p in group_packets]) + "\n")
                            file.write("Packets (ID: Timestamp):\n")
                            for p in group_packets:
                                file.write(f"{p['pkt_id']}: {p['Timestamp']}\n")
                            file.write("-" * 50 + "\n")

                # Reset groups
                packets = defaultdict(list)

            elif type_str in ['small', 'medium', 'big']:
                packets[type_str].append(packet_info)

            # Ignore unknown types or handle them accordingly

    except KeyboardInterrupt:
        print("Script terminated by user.")
        break
    except Exception as e: 
        print(f"Error: {e}")
        continue
print("Ending")
ser.close()

