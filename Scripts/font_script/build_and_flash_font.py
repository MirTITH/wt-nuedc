import sys
import os

from build_font import *
from flash_stlink import *


def main():
    # 要转换的字体
    file_path = "./font.ttf"
    address = "0x08080000"

    if len(sys.argv) >= 2:
        file_path = sys.argv[1]

    if len(sys.argv) >= 3:
        address = sys.argv[2]

    new_file_folder = "__temp__"
    if not os.path.isdir(new_file_folder):
        os.makedirs(new_file_folder)

    # 输出文件名
    file_name = os.path.basename(file_path)
    new_filename = new_file_folder + "/" + file_name + ".bin"

    BuildFont(file_path, new_filename)
    FlashStm32(new_filename, address)


if __name__ == "__main__":
    main()
    os.system("pause")
