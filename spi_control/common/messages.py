from dataclasses import dataclass, field
import dataclasses


@dataclass
class SpiMessageBase:
    startByte: int = field(default=0x42, init=False)

    def as_bytes(self) -> list:
        byte_list = list(dataclasses.astuple(self))
        checksum = 0
        for b in byte_list:
            checksum ^= b
        byte_list.append(checksum)
        return byte_list


@dataclass
class SetSpeedRight(SpiMessageBase):
    opCode: int = field(default=0x01, init=False)
    speed: int


@dataclass
class SetSpeedLeft(SpiMessageBase):
    opCode: int = field(default=0x02, init=False)
    speed: int


@dataclass
class ForwardRight(SpiMessageBase):
    opCode: int = field(default=0x03, init=False)


@dataclass
class ForwardLeft(SpiMessageBase):
    opCode: int = field(default=0x04, init=False)


@dataclass
class BackwardRight(SpiMessageBase):
    opCode: int = field(default=0x05, init=False)


@dataclass
class BackwardLeft(SpiMessageBase):
    opCode: int = field(default=0x06, init=False)


@dataclass
class OffRight(SpiMessageBase):
    opCode: int = field(default=0x07, init=False)


@dataclass
class OffLeft(SpiMessageBase):
    opCode: int = field(default=0x08, init=False)