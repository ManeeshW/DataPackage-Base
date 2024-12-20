import time
import board
import adafruit_bno055

from datetime import datetime


i2c = board.I2C()
sensor = adafruit_bno055.BNO055_I2C(i2c)

# If you are going to use UART uncomment these lines
# uart = board.UART()
# sensor = adafruit_bno055.BNO055_UART(uart)

last_val = 0xFFFF


def temperature():
    global last_val  # pylint: disable=global-statement
    result = sensor.temperature
    if abs(result - last_val) == 128:
        result = sensor.temperature
        if abs(result - last_val) == 128:
            return 0b00111111 & result
    last_val = result
    return result


t = datetime.now()
t_pre = datetime.now()

freq = 100.0
avg_number = 100

while True:
    t = datetime.now()
    dt = (t - t_pre).total_seconds()

    # print("Temperature: {} degrees C".format(sensor.temperature))
    # """
    # print(
    #     "Temperature: {} degrees C".format(temperature())
    # )  # Uncomment if using a Raspberry Pi
    # """
    # print("Accelerometer (m/s^2): {}".format(sensor.acceleration))
    # print("Magnetometer (microteslas): {}".format(sensor.magnetic))
    # print("Gyroscope (rad/sec): {}".format(sensor.gyro))
    print("Euler angle: {}".format(sensor.euler))
    # print("Quaternion: {}".format(sensor.quaternion))
    # print("Linear acceleration (m/s^2): {}".format(sensor.linear_acceleration))
    # print("Gravity (m/s^2): {}".format(sensor.gravity))
    print()

    t_pre = t

    freq = int((freq * (avg_number - 1) + (1.0 / dt)) / avg_number)
    print(freq)

    time.sleep(0.01)
