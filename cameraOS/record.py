import os
import time
from picamera import PiCamera

outputPath = "cameraOS/record"
ts = time.strftime("%Y-%m-%d-%H-%M-%S", time.gmtime())
fileName = f"record_{ts}.h264"

pathImg = "cameraOS/imagens"

if not os.path.exists(outputPath):
    os.makedirs(outputPath)

if not os.path.exists(pathImg):
    os.makedirs(pathImg)

camera = PiCamera()
camera.framerate = 25
camera.resolution = (1920, 1080)
time.sleep(2)

try:
    camera.start_recording(f"{outputPath}/{fileName}")
    print("Gravando...")

    while True:
        time.sleep(1)
        timestamp = time.strftime("%Y%m%d-%H%M%S")
        photo_filename = f"{pathImg}/foto_{timestamp}.jpg"
        camera.capture(photo_filename, use_video_port=True)
        print(f"Tirada foto: {photo_filename}")

except:
    print("ERROR")
    camera.stop_recording()