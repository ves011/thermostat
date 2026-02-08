/*
 * ui.c
 *
 *  Created on: Jan 17, 2026
 *      Author: viorel_serbu
 */

#include "ui.h"

void _ui_anim_callback_set_opacity(lv_anim_t * a, int32_t v)
	{
    ui_anim_user_data_t * usr = (ui_anim_user_data_t *)a->user_data;
    lv_obj_set_style_opa(usr->target, v, 0);
	}

int32_t _ui_anim_callback_get_opacity(lv_anim_t * a)
	{

    ui_anim_user_data_t * usr = (ui_anim_user_data_t *)a->user_data;
    return lv_obj_get_style_opa(usr->target, 0);
	}

void _ui_anim_callback_free_user_data(lv_anim_t * a)
	{
    lv_free(a->user_data);
    a->user_data = NULL;
	}


