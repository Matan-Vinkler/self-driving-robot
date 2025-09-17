import spidev
import time


class SpiController:
    def __init__(self, spi_bus: int, spi_device: int):
        self.spi_bus = spi_bus
        self.spi_device = spi_device
        self.spi = spidev.SpiDev()
    
    def open(self, max_speed_hz: int = 1000000):
        self.spi.open(self.spi_bus, self.spi_device)
        self.spi.max_speed_hz = max_speed_hz

    def send_message(self, message) -> bool:
        msg_bytes = message.as_bytes()
        for i in msg_bytes:
            rcv_byte = self.spi.xfer2([i])
            data_recv = rcv_byte[0]
        time.sleep(0.05)
