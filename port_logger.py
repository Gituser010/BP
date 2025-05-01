import serial
import time
import sys
import os

def usage():
    print("Usage: python port_logger.py <serial_port> <baud_rate>")
    print("Example: python port_logger.py /dev/ttyACM1 115200")
    sys.exit(1)

if len(sys.argv) != 3:
    usage()

port = sys.argv[1]
try:
    baudrate = int(sys.argv[2])
except ValueError:
    print("Invalid baud rate.")
    usage()

log_filename = f"serial_log_{time.strftime('%Y%m%d_%H%M%S')}.txt"
print(f"Logging to {log_filename}...")

def log_serial_data():
    with open(log_filename, 'a') as logfile:
        while True:
            try:
                with serial.Serial(port, baudrate, timeout=1) as ser:
                    print(f"Connected to {port} at {baudrate} baud.")
                    while True:
                        if ser.in_waiting > 0:
                            line = ser.readline().decode(errors='replace').strip()
                            print(line)
                            logfile.write(line + '\n')
                            logfile.flush()
            except serial.SerialException as e:
                print(f"[{time.strftime('%H:%M:%S')}] Serial error: {e}")
                print("Waiting for device to become available...")
                time.sleep(5)
            except Exception as e:
                print(f"[{time.strftime('%H:%M:%S')}] Unexpected error: {e}")
                time.sleep(5)

if __name__ == "__main__":
    log_serial_data()

