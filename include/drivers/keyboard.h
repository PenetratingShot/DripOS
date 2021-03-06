/*
    keyboard.h
    Copyright Menotdan and cfenollosa 2018-2019

    Keyboard driver

    MIT License
*/

#pragma once

#include <stdbool.h>
#include "../cpu/types.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define KEYUPOFFSET 0x80

#define SC_MAX 57

//extern int keytimeout[256];
void init_keyboard();