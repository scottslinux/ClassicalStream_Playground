# Converts any image (png/jpg/webp) into an RGB565 PROGMEM header for the TFT,
# in the same format as the other images in include/.
#
# Usage (from the project folder; needs Pillow):
#   python3 tools/img2header.py <image> <name> <width> <height> [crop|pad]
#
# Example - replace the oldies art with a photo of a record player:
#   python3 tools/img2header.py ~/Pictures/turntable.png oldies_150x165 150 165
#
# crop (default): scale to cover width x height and center crop. Good when the
#                 image is already the right shape (e.g. 3:4 photo -> 240x320 splash).
# pad:            trim the empty black border around the subject, scale it as big
#                 as it fits, and center it on black. Good for genre art so a
#                 tall trumpet or wide turntable is never cut off.
# Nothing is ever stretched. Near-black pixels become pure black (0x0000) so the art blends
# into the black screen like the original instrument photos.
# Writes include/<name>.h and tools/<name>_preview.png.
import os, sys
from PIL import Image, ImageOps

TOOLS = os.path.dirname(os.path.abspath(__file__))
INC = os.path.join(TOOLS, "..", "include")

def main():
    if len(sys.argv) not in (5, 6):
        print("usage: python3 tools/img2header.py <image> <name> <width> <height> [crop|pad]")
        sys.exit(1)

    src, name, w, h = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
    mode = sys.argv[5] if len(sys.argv) == 6 else "crop"

    img = Image.open(src)
    img = ImageOps.exif_transpose(img)          # respect phone photo rotation
    if img.mode in ("RGBA", "LA", "P"):
        img = img.convert("RGBA")
        black = Image.new("RGBA", img.size, (0, 0, 0, 255))
        img = Image.alpha_composite(black, img)  # transparent -> black
    img = img.convert("RGB")
    if mode == "pad":
        # find the subject: everything brighter than the near-black background
        bright = img.convert("L").point(lambda p: 255 if p > 24 else 0)
        box = bright.getbbox()
        if box:
            img = img.crop(box)
        img = ImageOps.contain(img, (w, h), Image.LANCZOS)
        canvas = Image.new("RGB", (w, h), (0, 0, 0))
        canvas.paste(img, ((w - img.width)//2, (h - img.height)//2))
        img = canvas
    else:
        img = ImageOps.fit(img, (w, h), Image.LANCZOS)

    path = os.path.join(INC, name + ".h")
    with open(path, "w") as f:
        f.write("#include <Arduino.h>\n")
        f.write("const uint16_t %s[] PROGMEM = {\n" % name)
        rows = []
        for y in range(h):
            vals = []
            for x in range(w):
                r, g, b = img.getpixel((x, y))
                if r < 12 and g < 12 and b < 12:
                    r = g = b = 0                    # clean black background
                vals.append("0x%04X" % (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)))
            rows.append("  " + ", ".join(vals))
        f.write(",\n".join(rows))
        f.write("\n};\n")

    img.resize((w*2, h*2), Image.NEAREST).save(os.path.join(TOOLS, name + "_preview.png"))
    print("wrote", os.path.normpath(path), "(%d bytes of flash)" % (w*h*2))

if __name__ == "__main__":
    main()
