#!/usr/bin/env python3
# import os
# import time
# from picamera import PiCamera
# from lib import compression

import os.path
import sys
from moviepy.editor import VideoFileClip

# try:
    # camera = PiCamera()
    # camera.resolution = (1024, 720)
    # path = "cameraOS/imagens"
    #
    # if not os.path.exists(path):
    #     os.makedirs(path)
    #
    # ts = time.strftime("%Y-%m-%d-%H %M %S", time.gmtime())
    # imgPath = f"{path}/foto_{ts}.jpg"
    # print(f"{path}/foto_{ts}.jpg")
    # camera.capture(imgPath)
    # compression.compress(imgPath, 50)

# videoPath = os.listdir("record")
# print(videoPath[len(videoPath)-1])
# cap = cv2.VideoCapture(f"record/{videoPath[len(videoPath)-1]}")
#
# # Pule para o último frame
# total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
# cap.set(cv2.CAP_PROP_POS_FRAMES, total_frames - 1)
#
# # Leia o último frame
# ret, frame = cap.read()
#
# # Salve o último frame como uma imagem
# if ret:
#     cv2.imwrite('ultimo_frame.jpg', frame)
#     print('Último frame salvo como "ultimo_frame.jpg"')
# else:
#     print('Erro ao ler o último frame')
#
# cap.release()


# Carregue o vídeo
videoPath = os.listdir("record")
print(f"record/{videoPath[len(videoPath)-1]}")
clip = VideoFileClip(f"record/{videoPath[len(videoPath)-1]}")

# Obtenha o último frame
last_frame = clip.get_frame(clip.duration - 0.01)  # 0.01 segundos antes do final

# Salve o último frame como uma imagem
last_frame.save_frame('ultimo_frame_moviepy.jpg')

print('Último frame salvo como "ultimo_frame_moviepy.jpg"')

#
# except KeyboardInterrupt:
#     sys.exit()