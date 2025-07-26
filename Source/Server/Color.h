/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Color.h                                          *
* Usage:  Define ascii color codes                         *
* Author: Steve Bryant                                     *
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
* Define color codes                                       *
************************************************************/

#define NORMAL    "\x1B[0;m";   // &N
#define BBLACK    "\x1B[1;30m"; // &K
#define BRED      "\x1B[1;31m"; // &R
#define BGREEN    "\x1B[1;32m"; // &G
#define BYELLOW   "\x1B[1;33m"; // &Y
#define BBLUE     "\x1B[1;34m"; // &B
#define BMAGENTA  "\x1B[1;35m"; // &M
#define BCYAN     "\x1B[1;36m"; // &C
#define BWHITE    "\x1B[1;37m"; // &W

/***********************************************************
* Color constants                                          *
************************************************************/

const string Normal        = NORMAL;
const string BrightBlack   = BBLACK;
const string BrightRed     = BRED;
const string BrightGreen   = BGREEN;
const string BrightYellow  = BYELLOW;
const string BrightBlue    = BBLUE;
const string BrightMagenta = BMAGENTA;
const string BrightCyan    = BCYAN;
const string BrightWhite   = BWHITE;

#endif