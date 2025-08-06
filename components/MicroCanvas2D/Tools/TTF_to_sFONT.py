from PIL import Image, ImageFont, ImageDraw
import sys
import os

def convert_font_to_sfont(ttf_path, font_size):
    font = ImageFont.truetype(ttf_path, font_size)

    # Use a known-wide char for size reference
    test_char = "M"
    # bbox = font.getbbox(test_char)
    # width = bbox[2] - bbox[0]
    # height = bbox[3] - bbox[1]

    ascent, descent = font.getmetrics()
    height = ascent + descent
    width = font.getmask("M").size[0]  # safe width guess

    chars = [chr(i) for i in range(32, 127)]  # Printable ASCII
    font_data = []

    for ch in chars:
        image = Image.new("1", (width, height), 0)
        draw = ImageDraw.Draw(image)
        draw.text((0, 0), ch, font=font, fill=1)

        char_bytes = []
        for y in range(height):
            byte = 0
            bit_count = 0
            for x in range(width):
                pixel = image.getpixel((x, y))
                byte = (byte << 1) | (1 if pixel else 0)
                bit_count += 1
                if bit_count == 8:
                    char_bytes.append(byte)
                    byte = 0
                    bit_count = 0
            if bit_count != 0:
                byte = byte << (8 - bit_count)
                char_bytes.append(byte)

        font_data.append(char_bytes)

    return font_data, width, height

def generate_c_file(font_data, width, height, font_name, out_file):
    bytes_per_char = len(font_data[0])
    with open(out_file, "w") as f:
        f.write(f'#include "fonts.h"\n\n')
        f.write(f'const unsigned char {font_name}_table[] = {{\n')
        for i, char_bytes in enumerate(font_data):
            ascii_code = i + 32
            f.write(f'  // {chr(ascii_code)} (0x{ascii_code:02X})\n  ')
            f.write(', '.join(f'0x{b:02X}' for b in char_bytes))
            f.write(',\n')
        f.write('};\n\n')

        f.write(f'sFONT {font_name} = {{\n')
        f.write(f'  {width}, // Width\n')
        f.write(f'  {height}, // Height\n')
        f.write(f'  {font_name}_table\n')
        f.write('};\n')

# Main entry
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python ttf_to_sfont.py source.ttf font_size")
        sys.exit(1)

    ttf_path = sys.argv[1]
    font_size = int(sys.argv[2])

    font_basename = os.path.splitext(os.path.basename(ttf_path))[0]
    font_name = f"{font_basename}_{font_size}".replace("-", "_")
    out_file = f"{font_name}.c"

    font_data, width, height = convert_font_to_sfont(ttf_path, font_size)
    generate_c_file(font_data, width, height, font_name, out_file)

    print(f"[✓] Generated: {out_file}")
