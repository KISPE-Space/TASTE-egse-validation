import socket
import serial
import threading
from time import sleep

baud = 115200
port = "/dev/ttyUSB0"

ser = serial.Serial(port, baud, timeout=1, write_timeout=1)

UDP_IP = "127.0.0.1"
IP_ADDRESS = "0.0.0.0"

UDP_TX_PORT = 10011
UDP_RX_PORT = 10022

START_BYTE = 0xAA
END_BYTE = 0x55
ESC_BYTE = 0xA0

tx_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
rx_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

rx_sock.bind((IP_ADDRESS, UDP_RX_PORT))
rx_sock.settimeout(0.1)


def encode_packet(data):
    packet = bytearray([START_BYTE])

    for byte in data:
        if byte == START_BYTE or byte == END_BYTE:
            packet.append(ESC_BYTE)

        packet.append(byte)

    packet.append(END_BYTE)

    return packet


def readSerial():
    while True:
        SerialData = ser.read(2048)

        if SerialData:

            # Forward serial bytes to UDP unchanged
            tx_sock.sendto(SerialData, (UDP_IP, UDP_TX_PORT))


def readSocket():
    while True:
        try:
            data, addr = rx_sock.recvfrom(2048)
        except socket.timeout:
            continue

        if data:
            # Encode UDP payload
            packet = encode_packet(data)

            # Send encoded packet over serial
            ser.write(packet)


# Start threads
threading.Thread(target=readSerial, daemon=True).start()
threading.Thread(target=readSocket, daemon=True).start()

try:
    while True:
        sleep(1)

finally:
    ser.close()
    tx_sock.close()
    rx_sock.close()