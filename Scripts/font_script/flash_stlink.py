import subprocess
import sys

# 想烧录的文件名和烧录地址
flash_file = "font.ttf.bin"
flash_address = "0x08080000"

if len(sys.argv) >= 2:
    flash_file = sys.argv[1]

if len(sys.argv) >= 3:
    flash_address = sys.argv[2]

confirm = input(
    "Flash %s at address %s? (y/n) "
    % (flash_file, flash_address)
)
if confirm != "y":
    sys.exit(0)

subprocess.run(
    [
        "STM32_Programmer_CLI",
        "-c",
        "port=SWD",
        "freq=4000",
        "--download",
        flash_file,
        flash_address,
        "-v",
        "--go",
    ]
)
