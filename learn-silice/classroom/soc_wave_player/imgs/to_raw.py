# import matplotlib.pyplot as plt
from PIL import Image
from sys import argv

if len(argv) < 2:
    print("Usage: python3 test.py <filename>")
    exit()
filename = argv[1]
name = filename.split(".")[0]

L = 128

print(f"Resizing {filename} to {L}x{L} pixels")
img = Image.open(filename)
img = img.resize((L, L))

print(f"Converting {filename} to {name}.raw")
with open(name + ".raw", "wb") as f:
    for i in range(L):
        for j in range(L):
            pixel = img.getpixel((i, j))
            f.write(bytes(pixel))


# plt.imshow(img)
# plt.show()
