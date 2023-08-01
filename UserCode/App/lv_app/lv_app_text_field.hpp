#pragma once

#include "lvgl/lvgl.h"
#include "lv_app.hpp"
#include "fonts/lvgl_ttf.h"
#include <string>

class LvTextField
{
private:
    lv_obj_t *main_frame_;
    lv_obj_t *obj_msg_border_;
    lv_obj_t *label_msg_;
    lv_obj_t *label_title_;
    lv_obj_t *obj_title_frame_;

public:
    LvTextField(lv_obj_t *parent, const std::string &title = "Untitled",
                lv_coord_t width = LV_PCT(100), lv_coord_t height = 70,
                const lv_font_t *msg_font   = LvglTTF_GetLargeFont(),
                const lv_font_t *title_font = LvglTTF_GetFont())
    {
        main_frame_ = lv_obj_create(parent);
        lv_obj_set_style_pad_all(main_frame_, 0, 0);
        lv_obj_add_style(main_frame_, &lv_app::kStyleNoBorder, 0);
        lv_obj_set_size(main_frame_, width, height);

        obj_msg_border_ = lv_obj_create(main_frame_);
        lv_obj_set_size(obj_msg_border_, lv_pct(100), height - 15);
        lv_obj_align(obj_msg_border_, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_pad_hor(obj_msg_border_, 10, 0);
        lv_obj_set_style_pad_ver(obj_msg_border_, 0, 0);
        lv_obj_set_style_border_color(obj_msg_border_, lv_color_make(0, 0, 0), 0);
        lv_obj_set_style_border_width(obj_msg_border_, 1, 0);

        obj_title_frame_ = lv_obj_create(main_frame_);
        lv_obj_set_pos(obj_title_frame_, 10, 0);
        lv_obj_set_size(obj_title_frame_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_hor(obj_title_frame_, 2, 0);
        lv_obj_set_style_pad_ver(obj_title_frame_, 0, 0);
        lv_obj_add_style(obj_title_frame_, &lv_app::kStyleNoBorder, 0);

        label_title_ = lv_label_create(obj_title_frame_);
        lv_obj_set_style_text_font(label_title_, title_font, 0);
        lv_label_set_text(label_title_, title.c_str());

        lv_obj_move_background(obj_msg_border_);

        label_msg_ = lv_label_create(main_frame_);
        lv_obj_set_width(label_msg_, lv_pct(96));
        lv_obj_align(label_msg_, LV_ALIGN_CENTER, 0, 15 / 2 + 1);
        lv_obj_set_style_text_font(label_msg_, msg_font, 0);
        lv_label_set_long_mode(label_msg_, LV_LABEL_LONG_SCROLL);
        // lv_obj_set_style_text_align(label_msg_, LV_TEXT_ALIGN_CENTER, 0);
        // lv_label_set_text(label_msg_, "Hello World!");
    }

    void SetTitle(const char *text)
    {
        lv_label_set_text(label_title_, text);
    }

    void SetTitle(const std::string &text)
    {
        lv_label_set_text(label_title_, text.c_str());
    }

    void SetMsg(const std::string &text)
    {
        lv_label_set_text(label_msg_, text.c_str());
    }

    lv_obj_t *GetMsgLabel() const
    {
        return label_msg_;
    }

    ~LvTextField()
    {
        lv_obj_del(main_frame_);
    }
};

class LvSimpleTextField
{
private:
    lv_obj_t *main_frame_;
    lv_obj_t *label_msg_;
    lv_obj_t *label_title_;

public:
    LvSimpleTextField(lv_obj_t *parent, const std::string &title = "Untitled",
                      lv_coord_t width = LV_PCT(100), lv_coord_t height = 35,
                      const lv_font_t *msg_font   = LvglTTF_GetFont(),
                      const lv_font_t *title_font = LvglTTF_GetFont())
    {
        main_frame_ = lv_obj_create(parent);
        lv_obj_set_style_pad_all(main_frame_, 4, 0);
        lv_obj_set_style_border_width(main_frame_, 1, 0);
        lv_obj_set_style_border_color(main_frame_, lv_color_make(0, 0, 0), 0);
        lv_obj_set_size(main_frame_, width, height);

        label_title_ = lv_label_create(main_frame_);
        lv_obj_set_style_text_font(label_title_, title_font, 0);
        lv_label_set_text(label_title_, title.c_str());
        lv_obj_align(label_title_, LV_ALIGN_LEFT_MID, 0, 0);

        label_msg_ = lv_label_create(main_frame_);
        lv_obj_align(label_msg_, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_style_text_font(label_msg_, msg_font, 0);
    }

    void SetTitle(const char *text)
    {
        lv_label_set_text(label_title_, text);
    }

    void SetTitle(const std::string &text)
    {
        lv_label_set_text(label_title_, text.c_str());
    }

    void SetMsg(const std::string &text)
    {
        lv_label_set_text(label_msg_, text.c_str());
    }

    lv_obj_t *GetMsgLabel() const
    {
        return label_msg_;
    }

    ~LvSimpleTextField()
    {
        lv_obj_del(main_frame_);
    }
};
