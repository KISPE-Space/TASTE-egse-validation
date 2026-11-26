import socket
import serial
import threading
import time
from threading import Thread
from time import sleep

baud = 115200
port = "/dev/ttyACM1"

ser = serial.Serial(port, baud, timeout = 1, write_timeout=1)
serialBuffer = bytearray()

UDP_IP = "127.0.0.1"    #IP of YAMCS
IP_ADDRESS = "0.0.0.0"

# Need to update this for the CAN interface
UDP_TX_PORT = 10011        # Port to send to
UDP_RX_PORT = 10022        # Port to receuve from 


tx_sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
rx_sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
rx_sock.bind((IP_ADDRESS, UDP_RX_PORT))
rx_sock.settimeout(0.1)

def readSerial():
        while True:
            # The value in .read(X) determines the number of bytes to read
            SerialData = ser.read(2048)

            if SerialData: 
                print("Serial -> UDP:") 
                print(' '.join(f'{b:02X}' for b in SerialData)) 
                print()  
                tx_sock.sendto(SerialData, (UDP_IP, UDP_TX_PORT))
# END of readSerial()

def readSocket():
    while True: 
        try: 
            data, addr = rx_sock.recvfrom(2048) 
        except socket.timeout: 
            continue
         
        if data: print("UDP -> Serial:") 
        print(' '.join(f'{b:02X}' for b in data)) 
        print() 
        ser.write(data)

#END of readSocket()

# Starting Threads
threading.Thread(target=readSerial, daemon=True).start()
threading.Thread(target=readSocket, daemon=True).start()

# Main Loop
try:

    while True:
        sleep(1)

except KeyboardInterrupt:
    print("\n Keyboard Interrupt! Stopping \n")
finally:
    ser.close()
    tx_sock.close() 
    rx_sock.close()

# END of Loop/Main code