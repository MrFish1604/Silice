import matplotlib.pyplot as plt
from math import sqrt, log2, log10
filename = "music.raw"
# filename = "soulsweeper.mp3.raw"

BLOCK_SIZE = 512

with open(filename, "rb") as f:
    data = f.read()

data = bytearray(data)
for i in range(0, len(data)):
    data[i] = data[i] & 0x7F

rms = []
l2 = []
l10 = []
for i in range(0, len(data), BLOCK_SIZE):
    v = sum(data[i:i+BLOCK_SIZE]) >> 9
    rms.append(v)
    v = (v-48) >> 2 if v > 48 else 0
    l2.append(v)


X = [x for x in range(0, len(rms))]
plt.subplot(2, 1, 1)
plt.bar(X, rms)
plt.grid()
plt.subplot(2, 1, 2)
plt.bar(X, l2)
plt.grid()
plt.show()
