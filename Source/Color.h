/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Color.h                                          *
* Usage:  Define ascii color codes                         *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* ANSI color                                               *
* Command format: Esc[x;ym                                 *
*                                                          *
* In C, Escape (Esc) is coded as \x1B                      *
* so the command becomes \x1B[x;ym                         *
* The m at the end is a constant, leaving just the x and y *
* values to be specified. The are many possible            *
* combinations, but 'x' = 1 and 'y' = 31-36 is common.     *
*                                                          *
* These codes changes the color and the color remains      *
* the same until it is changed again. So be sure to        *
* 'reset' the color to 'normal' as appropriate.            *
*                                                          *
* ASNI color codes:                                        *
*    Control                                               *
*  0 - normal                                              *
*  1 - bold (bright)                                       *
*  4 - underline                                           *
*  5 - blink                                               *
*  7 - reverse video                                       *
*  8 - invisible                                           *
*    Forground colors                                      *
* 30 - black                                               *
* 31 - red                                                 *
* 32 - green                                               *
* 33 - yellow                                              *
* 34 - blue                                                *
* 35 - magenta                                             *
* 36 - cyan                                                *
* 37 - white                                               *
*    Background colors                                     *
* 40 - black                                               *
* 41 - red                                                 *
* 42 - green                                               *
* 43 - yellow                                              *
* 44 - blue                                                *
* 45 - magenta                                             *
* 46 - cyan                                                *
* 47 - white                                               *
************************************************************/

#ifndef COLOR_H
#define COLOR_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"

/***********************************************************
* Define color codes                                       *
************************************************************/

#define NORMAL    "\x1B[0;m"
#define BBLACK    "\x1B[1;30m";
#define BRED      "\x1B[1;31m";
#define BGREEN    "\x1B[1;32m";
#define BYELLOW   "\x1B[1;33m";
#define BBLUE     "\x1B[1;34m";
#define BMAGENTA  "\x1B[1;35m";
#define BCYAN     "\x1B[1;36m";
#define BWHITE    "\x1B[1;37m";

#endif