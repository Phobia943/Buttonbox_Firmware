import serial
from time import sleep


COM_PORT = "COM8"

# FILENAME = "config_alt.json"
FILENAME = "config_buttonbox_2funky.json"


def print_all(ser_obj: serial.Serial):
    last_good_line = b""
    while True:
        line_read = ser_obj.readline()
        if line_read:
            last_good_line = line_read
            print(f"ser: {line_read}")
        else:
            return last_good_line


with serial.Serial(COM_PORT, 115200, timeout=0.1) as ser:
    print("Starting to send config file")
    with open(FILENAME, "r") as conf:
        ser.write(b"__CHANGE_CONFIG_FILE__\n")
        sleep(0.1)
        ser.write(f"{FILENAME}\n".encode('utf-8'))
        sleep(0.1)
        ser.write(b"__CHANGE_DEFAULT_CONFIG__\n")
        sleep(0.1)
        ser.write(f"{FILENAME}\n".encode('utf-8'))
        sleep(0.1)
        ser.write(b"__NEW_CONFIG__\n")
        sleep(0.1)
        ser.write(b"__START__\n")
        print_all(ser)
        sleep(0.1)
        for line in conf:
            ser.write(line.encode('utf-8'))
            # print_all(ser)
            # sleep(0.1)
        ser.write(b"\n")
        ser.write(b"__END__\n")
        ser.write(b"__RELOAD__\n")
        while True:
            print_all(ser)
            sleep(0.1)
