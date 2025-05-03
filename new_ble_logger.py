
import serial
import time
import re
from collections import defaultdict
import os

# Configuration
PORT = "/dev/ttyACM1"  # Update this to match your system
BAUDRATE = 115200
LOG_DIR = "logs"
os.makedirs(LOG_DIR, exist_ok=True)

# Packet types
SMALL_TYPE = 1
MEDIUM_TYPE = 2
BIG_TYPE = 3

EXPECTED_PACKETS = 10

# Output files
file_map = {
    SMALL_TYPE: open(os.path.join(LOG_DIR, "small_messages.log"), "w"),
    MEDIUM_TYPE: open(os.path.join(LOG_DIR, "medium_messages.log"), "w"),
    BIG_TYPE: open(os.path.join(LOG_DIR, "big_messages.log"), "w"),
}

# Regex to parse packet lines
packet_regex = re.compile(
    r"pktId: (\d+); type:(\d+), size:(\d+), timestamp:(\d+) phy:(\d+) (\d+) RSSI:(-?\d+)"
)

# Variables
connected = False
packets = defaultdict(list)
connection_start_time = None
connection_end_time = None

def calculate_pdr(received, expected):
    return (received / expected) * 100 if expected > 0 else 0

def process_connection_data():
    global packets, connection_start_time, connection_end_time

    for packet_type, logs in packets.items():
        if not logs:
            continue
        logs = sorted(logs, key=lambda x: x['timestamp'])
        first_ts = logs[0]['timestamp']
        last_ts = logs[-1]['timestamp']
        pdr = calculate_pdr(len(logs), EXPECTED_PACKETS)
        ids = [str(log['pktId']) for log in logs]
        line = (
            f"Packet Type: {packet_type}\n"
            f"Packet IDs: {', '.join(ids)}\n"
            f"PHY: {logs[0]['phy_tx']} {logs[0]['phy_rx']}\n"
            f"Sizes: {', '.join(str(log['size']) for log in logs)}\n"
            f"Time to Deliver: {last_ts - first_ts} ms\n"
            f"RSSI Values: {', '.join(str(log['rssi']) for log in logs)}\n"
            f"PDR: {pdr:.2f}%\n"
            f"Timestamps: {first_ts} - {last_ts}\n{'-'*50}\n"
        )
        print(line)
        file_map[packet_type].write(line)
        file_map[packet_type].flush()

    packets.clear()
    connection_start_time = None
    connection_end_time = None

# Main loop
with serial.Serial(PORT, BAUDRATE, timeout=1) as ser:
    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if not line:
                continue

            if "Connected" in line:
                connected = True
                connection_start_time = int(time.time() * 1000)
                packets.clear()

            elif "Disconnected" in line:
                connected = False
                connection_end_time = int(time.time() * 1000)
                process_connection_data()

            elif "PACKET_HANDLER: pktId" in line:
                match = packet_regex.search(line)
                if match:
                    pktId, pktType, size, timestamp, phy_tx, phy_rx, rssi = match.groups()
                    pktType = int(pktType)
                    if pktType in [SMALL_TYPE, MEDIUM_TYPE, BIG_TYPE]:
                        packets[pktType].append({
                            'pktId': int(pktId),
                            'type': pktType,
                            'size': int(size),
                            'timestamp': int(timestamp),
                            'phy_tx': int(phy_tx),
                            'phy_rx': int(phy_rx),
                            'rssi': int(rssi)
                        })

        except KeyboardInterrupt:
            print("Interrupted by user. Exiting...")
            break

# Cleanup
for f in file_map.values():
    f.close()
