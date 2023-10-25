from queue import Queue
import platform
import serial

if platform.system() == 'Darwin':
    arduino_serial_port = '/dev/cu.usbmodem14101'
elif platform.system() == 'Windows':
    arduino_serial_port = 'COM3'

arduino_serial = serial.Serial(arduino_serial_port, 9600)
serial_out_queue = Queue()

def serial_in():
    global serial_in_buffer
    while arduino_serial.in_waiting > 0:
        try:
            msg = arduino_serial.read(arduino_serial.in_waiting).decode()
        except serial.SerialException as e:
            print(f"Errore nella comunicazione seriale: {str(e)}")
        #print(f'SERIAL received {msg}')
        if '\n' in msg:
            return msg
    return None

#send data to Arduino serial port
def serial_out():
    if not serial_out_queue.empty():
        #print(f'SERIAL sending {list(serial_out_queue.queue)}')
        while not serial_out_queue.empty():
            msg = serial_out_queue.get()
            arduino_serial.write(str.encode(msg))
        arduino_serial.flush()

def serial_set_light(val):
    serial_out_queue.put(f'set_light:{str(val)}\n')
    serial_out()

def serial_set_roller_blind(pos):
    serial_out_queue.put(f'set_roller_blind:{str(pos)}\n')
    serial_out()