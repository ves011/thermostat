/*
 * styles.c
 *
 *  Created on: Jan 10, 2026
 *      Author: viorel_serbu
 */

#include <stdio.h>
#include "lvgl.h"

lv_style_t btn_norm, btn_sel, btn_press, cell_style, cell_style_left;

void init_styles()
	{
	lv_style_init(&cell_style);
    lv_style_reset(&cell_style);
    lv_style_set_text_color(&cell_style, lv_color_hex(0xc0c0c0));
    lv_style_set_text_font(&cell_style, &lv_font_montserrat_14);
    lv_style_set_text_align(&cell_style, LV_TEXT_ALIGN_RIGHT);

    lv_style_init(&cell_style_left);
    lv_style_reset(&cell_style_left);
    lv_style_set_text_color(&cell_style_left, lv_color_hex(0xc0c0c0));
    lv_style_set_text_font(&cell_style_left, &lv_font_montserrat_14);
    lv_style_set_text_align(&cell_style_left, LV_TEXT_ALIGN_LEFT);

    lv_style_init(&btn_sel);
    lv_style_init(&btn_norm);

    lv_style_set_text_color(&btn_norm, lv_color_hex(0xc0c0c0));
    //lv_style_set_text_font(&btn_norm, &seg_black_20);

    lv_style_set_translate_y(&btn_sel, 5);
    lv_style_set_shadow_ofs_y(&btn_sel, 10);
    lv_style_set_shadow_ofs_x(&btn_sel, 10);
    lv_style_set_text_color(&btn_sel, lv_color_hex(0xffc040));
    //lv_style_set_text_font(&btn_sel, &seg_black_20);

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = { LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0 };
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);
    lv_style_set_transition(&btn_sel, &trans);

    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);
    lv_style_set_transition(&btn_sel, &trans);

    /*Init the pressed style*/
    lv_style_init(&btn_press);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&btn_press, 30);
    lv_style_set_outline_opa(&btn_press, LV_OPA_TRANSP);

    lv_style_set_translate_y(&btn_press, 5);
    lv_style_set_shadow_ofs_y(&btn_press, 3);
    lv_style_set_bg_color(&btn_press, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&btn_press, lv_palette_darken(LV_PALETTE_BLUE, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t transp;
    static lv_style_prop_t propsp[] = { LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0 };
    lv_style_transition_dsc_init(&transp, propsp, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&btn_press, &trans);
	}

