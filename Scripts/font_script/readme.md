# 字体构建脚本

## 裁剪字体

在这里裁剪 ttf 字体: [在线字体裁剪 (disidu.com)](https://font-subset.disidu.com/)

建议勾选 半角字母、半角标点、全角数字、全角标点

然后把 `裁剪字库字符集.txt` 中的内容粘贴到网页的输入框里，可以自己补充字符

## 构建字体

将 ttf 拖到 build_font.py 上，生成 bin 文件

## 烧录字体

将生成的 bin 文件拖到 flash_stlink.py 上
