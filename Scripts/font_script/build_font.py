import struct
import sys


def BuildFont(file_path, output_name):
    # 打开原始文件并读取内容
    with open(file_path, "rb") as f:
        print("读取文件：", file_path)
        content = f.read()

    # 计算文件大小并将其转换为 uint32 类型
    size = len(content)
    size_uint32 = struct.pack("<I", size)

    print("文件大小：%d Bytes" % size)
    print("十六进制（小端序）：", size_uint32.hex())

    # 打开新文件并写入 uint32 数据和原始内容
    with open(output_name, "wb") as f:
        f.write(size_uint32)
        f.write(content)

    print("写入新文件：", output_name)
    print("其中前 4 字节是文件大小（十六进制小端序）")


def main():
    # 要转换的字体
    filename = "./font.ttf"

    if len(sys.argv) >= 2:
        filename = sys.argv[1]

    # 输出文件名
    new_filename = filename + ".bin"
    BuildFont(filename, new_filename)


if __name__ == "__main__":
    main()
