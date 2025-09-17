import cv2, numpy as np
from picamera2 import Picamera2
from libcamera import Transform
from hailo_platform import VDevice, HailoSchedulingAlgorithm, FormatType
from spi_control.backend.spi_controller import SpiController
from spi_control.common.messages import ForwardRight, SetSpeedRight, OffRight, ForwardLeft, OffLeft, SetSpeedLeft, BackwardRight, BackwardLeft

HEF_PATH   = "model/nvidia_model.hef"
TIMEOUT_MS = 1000

import numpy as np

def angle_to_pwm(angle, cruise=0.3, k=0.6, min_pwm=70, max_pwm=200, invert=False):
    a = float(np.clip(angle, -1, 1))
    if invert: a = -a
    a = np.sign(a) * (abs(a)**3)          # gentler near center

    left_n  = np.clip(cruise - k*a, 0, 1)
    right_n = np.clip(cruise + k*a, 0, 1)

    scale = max_pwm - min_pwm
    left_pwm  = int(round(min_pwm + left_n  * scale))
    right_pwm = int(round(min_pwm + right_n * scale))
    return left_pwm, right_pwm


def init_spi_controller() -> SpiController:
    spi = SpiController(0, 0)
    spi.open()

    spi.send_message(OffLeft())
    spi.send_message(OffRight())

    return spi

def turn_off_motors():
    spi.send_message(OffLeft())
    spi.send_message(OffRight())

def send_to_arduino(spi: SpiController, left_pwm: int, right_pwm: int):
    spi.send_message(OffLeft())
    spi.send_message(OffRight())

    spi.send_message(SetSpeedLeft(left_pwm))
    spi.send_message(SetSpeedRight(right_pwm))

    spi.send_message(ForwardLeft())
    spi.send_message(ForwardRight())

def preprocess_from_rgb(rgb, target_hw=(66,200)):  # (H,W)
    rgb = cv2.resize(rgb, (320,160), interpolation=cv2.INTER_AREA)
    crop = rgb[60:135,:,:]
    yuv_u8 = cv2.cvtColor(crop, cv2.COLOR_RGB2YUV)
    yuv_u8 = cv2.GaussianBlur(yuv_u8, (3,3), 0)
    yuv_u8 = cv2.resize(yuv_u8, (target_hw[1], target_hw[0]), interpolation=cv2.INTER_AREA)
    x = yuv_u8.astype(np.float32) / 255.0                 # (H,W,3) float32
    pre_bgr = cv2.cvtColor(yuv_u8, cv2.COLOR_YUV2BGR)     # for display only
    return x, pre_bgr


spi = init_spi_controller()

# Camera: RGB888 frames so no BGR<->RGB juggling needed
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(
    main={"format": "RGB888", "size": (1280, 720)},
    transform=Transform(hflip=1, vflip=1)   # rot180
))
picam2.start()

params = VDevice.create_params()
params.scheduling_algorithm = HailoSchedulingAlgorithm.ROUND_ROBIN

with VDevice(params) as vdev:
    infer_model = vdev.create_infer_model(HEF_PATH)
    infer_model.input().set_format_type(FormatType.FLOAT32)
    infer_model.output().set_format_type(FormatType.FLOAT32)

    in_shape  = list(infer_model.input().shape)   # [66,200,3] or [1,66,200,3]
    out_shape = list(infer_model.output().shape)  # [1] or [1,1]

    if len(in_shape) == 3:
        H,W,C = in_shape
        x_buf = np.empty((H,W,C), dtype=np.float32)
    else:
        _,H,W,C = in_shape
        x_buf = np.empty((1,H,W,C), dtype=np.float32)
    y_buf = np.empty(tuple(out_shape), dtype=np.float32)

    with infer_model.configure() as cfg:
        bindings = cfg.create_bindings()
        bindings.input().set_buffer(x_buf)
        bindings.output().set_buffer(y_buf)

        frame_count = 0
        while True:
            try:
                rgb = picam2.capture_array()  # (H,W,3) RGB
                x, pre_bgr = preprocess_from_rgb(rgb, (H,W))
                if x_buf.ndim == 3: x_buf[:] = x
                else:               x_buf[0] = x

                job = cfg.run_async([bindings])
                job.wait(TIMEOUT_MS)
                angle = float(y_buf.reshape(-1)[0]) / 3.0

                left_pwm, right_pwm = angle_to_pwm(angle)
                print(angle, left_pwm, right_pwm)

                if frame_count == 50:
                    print(f"Sending {left_pwm, right_pwm}")
                    send_to_arduino(spi, left_pwm, right_pwm)
                    frame_count = 0

                frame_count += 1

                disp = cv2.resize(pre_bgr, (W*3, H*3), interpolation=cv2.INTER_NEAREST)
                cv2.putText(disp, f"steer: {(left_pwm, right_pwm)}", (8,22), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,255,255), 2)
                cv2.imshow("Preprocessed (display only)", disp)
                if cv2.waitKey(1) & 0xFF == 27: break
            except KeyboardInterrupt:
                print("keyboard interrupt")
                turn_off_motors()
                break

cv2.destroyAllWindows()
picam2.stop()
turn_off_motors()