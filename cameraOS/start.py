import os
import time
from picamera import PiCamera
from lib import compression
from lib import burnDetection
import threading

outputPath = "cameraOS/record"
ts = time.strftime("%Y-%m-%d-%H-%M-%S", time.gmtime())
fileName = f"record_{ts}.h264"

pathImg = "cameraOS/imagens"

if not os.path.exists(outputPath):
    os.makedirs(outputPath)

if not os.path.exists(pathImg):
    os.makedirs(pathImg)

camera = PiCamera()
camera.framerate = 30
camera.resolution = (1280, 720)
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
        burnDetection.burnDetection(photo_filename, 16).start()

        thred = threading.Thread(target=compression.compress, args=(f"{pathImg}/foto_{timestamp}.jpg", 250))
        thred.start()

except:
    print("ERROR")
    camera.stop_recording()