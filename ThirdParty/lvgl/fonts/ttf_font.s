    .global _ttf_font_start
_ttf_font_start:
    .incbin "ThirdParty/lvgl/fonts/UbuntuMono-Regular.ttf"

    .global _ttf_font_end
_ttf_font_end:

    .global _ttf_font_size
_ttf_font_size:
    .int _ttf_font_end - _ttf_font_start
