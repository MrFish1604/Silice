import matplotlib.pyplot as plt
from math import sqrt, log2, log10
filename = "music.raw"
# filename = "soulsweeper.mp3.raw"

with open(filename, "rb") as f:
    data = f.read()

data = bytearray(data)
for i in range(0, len(data)):
    data[i] = data[i] & 0x7F

rms = []
l2 = []
l10 = []
for i in range(0, len(data), 512):
    v = sum(data[i:i+512]) >> 9
    # v = (v - 48)  if v > 48 else 0
    rms.append(v)
    v = (v-48) >> 2 if v > 48 else 0
    l2.append(v)
    # l2.append(int(log2(rms[-1])) if rms[-1] > 0 else 0)
    # l10.append(log10(rms[-1]) if rms[-1] > 0 else 0)


X = [x for x in range(0, len(rms))]
plt.subplot(2, 1, 1)
plt.bar(X, rms)
plt.grid()
plt.subplot(2, 1, 2)
plt.bar(X, l2)
plt.grid()
plt.show()
