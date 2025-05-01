import serial
import time
import os

def serial_monitor(device_path, baudrate=9600, timeout=1):
    print(f"🔁 Serial monitor watching {device_path} (baud: {baudrate})")
    print("Unplug/replug your device anytime. Press Ctrl+C to quit.\n")

    while True:
        # Wait for device
        while not os.path.exists(device_path):
            #print(f"[{time.strftime('%H:%M:%S')}] 🔌 Waiting for {device_path}...")
            time.sleep(1)

        try:
            with serial.Serial(device_path, baudrate, timeout=timeout) as ser:
                print(f"[{time.strftime('%H:%M:%S')}] ✅ Connected to {device_path}")
                while True:
                    try:
                        if ser.in_waiting > 0:
                            data = ser.readline().decode('utf-8', errors='replace').strip()
                            if data:
                                print(f"[{time.strftime('%H:%M:%S')}] {data}")
                        else:
                            time.sleep(0.1)
                    except (serial.SerialException, OSError) as e:
                        print(f"[{time.strftime('%H:%M:%S')}] ⚠️  Device lost: {e}")
                        break  # Device was unplugged; break inner loop and wait again
        except (serial.SerialException, OSError) as e:
            print(f"[{time.strftime('%H:%M:%S')}] ❌ Could not open serial port: {e}")
            time.sleep(1)
        except KeyboardInterrupt:
            print("\n🛑 Exiting serial monitor.")
            break

if __name__ == "__main__":
    serial_port = '/dev/ttyUSB0'  # Change as needed
    serial_monitor(serial_port, baudrate=115200)

