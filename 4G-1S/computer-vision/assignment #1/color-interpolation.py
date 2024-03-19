import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt


def color_rect():
    # Color rectangle 생성
    g = np.zeros((256, 256, 3), dtype=np.uint8)

    for i in range(256):
        g[i, :, :] = (0, i, 255 - i)

    return g


g = color_rect()

# cv.imshow('Org', g)
col = cv.cvtColor(g, cv.COLOR_BGR2RGB)
plt.imshow(col)
plt.show()