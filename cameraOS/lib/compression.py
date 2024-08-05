import os
import time
from PIL import Image

def compress(imgPath, compressLevel):
    startTime = time.time()
    img = Image.open(imgPath)
    nameImg = os.path.splitext(os.path.basename(imgPath))[0]

    outputPath = "cameraOS/imagens/compressed"
    if not os.path.exists(outputPath):
        os.makedirs(outputPath)

    img.save(f"{outputPath}/{nameImg}.jp2", mct=1, irreversible=True, quality_mode="rates", quality_layers=[compressLevel])

    # img.save(f"{outputPath}/{nameImg}.jpg", quality=compressLevel, optimize=True)

    print(time.time() - startTime)