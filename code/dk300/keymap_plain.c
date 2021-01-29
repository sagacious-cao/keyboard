/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"

static const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    {
     {KC_ENTER, KC_DOT, KC_COMM,   KC_M,   KC_V,  KC_APP, KC_PAST, KC_PSLS, KC_NLCK,   KC_P8,   KC_NO,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO},
     {    KC_X,   KC_D,   KC_S,    KC_A, KC_TAB,    KC_W,    KC_1,    KC_5,    KC_C,    KC_4,    KC_2,   KC_3,    KC_Q,    KC_E, KC_LSFT,    KC_Z,  KC_SPC, KC_LCTL,   KC_NO,   KC_NO},
     { KC_BSPC,  KC_F7, KC_RBRC,   KC_Y,   KC_T, KC_CAPS,   KC_P6,   KC_P5,   KC_P4,   KC_NO,   KC_NO,  KC_NO, KC_PAUS,   KC_F3,   KC_NO, KC_LBRC,   KC_NO,   KC_NO,   KC_NO,   KC_NO},
     {  KC_F12,  KC_NO,   KC_NO,   KC_N,   KC_B,   KC_NO, KC_PMNS, KC_RGHT, KC_DOWN,   KC_P9, KC_LEFT,  KC_NO,   KC_NO,   KC_NO,   KC_NO, KC_SLSH,   KC_NO,   KC_NO,   KC_NO, KC_RALT},
     {  KC_F11,  KC_NO,   KC_F6,   KC_H,   KC_G, KC_BSLS,  KC_DEL,   KC_P0,   KC_NO,   KC_P7,   KC_UP,  KC_NO,  KC_ESC,   KC_F4,   KC_NO, KC_SCLN,   KC_NO,   KC_NO,   KC_NO, KC_LALT},
     {  KC_F10,   KC_9,    KC_8,   KC_7,   KC_O,    KC_U, KC_PGDN, KC_SLCK, KC_PSCR,   KC_F5,  KC_END,  KC_NO,    KC_R,    KC_I,   KC_NO,    KC_0,   KC_NO, KC_RCTL,   KC_NO,   KC_NO},
     {   KC_F9,  KC_F8,  KC_EQL,   KC_6,   KC_P,   KC_F1, KC_PGUP,  KC_INS, KC_PDOT, KC_PPLS, KC_HOME,  KC_NO,  KC_GRV,   KC_F2,   KC_NO, KC_MINS,   KC_NO,   KC_NO, KC_LGUI,   KC_NO},
     {   KC_NO,   KC_L,    KC_K,   KC_J,   KC_F,   KC_NO,   KC_P3,   KC_P2,   KC_P1,   KC_NO, KC_PENT,  KC_NO,   KC_NO,   KC_NO, KC_RSFT, KC_QUOT,   KC_NO,   KC_NO,   KC_NO,  KC_FN0}
    },
};

static const action_t fn_actions[] = {
};

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    return keymaps[(layer)][(key.row)][(key.col)];
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    return fn_actions[FN_INDEX(keycode)];
}
