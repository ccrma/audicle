/*----------------------------------------------------------------------------
    Audicle Context-sensitive, On-the-fly Audio Programming Environment
      for the ChucK Programming Language

    Copyright (c) 2005 Ge Wang, Perry R. Cook, Ananya Misra, Philip Davidson.
      All rights reserved.
      http://audicle.cs.princeton.edu/
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
    U.S.A.
-----------------------------------------------------------------------------*/

/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Rgbhsl is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Rgbhsl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// This library converts between colors defined with RGB values and HSL
// values.  It also finds in-between colors by moving linearly through
// HSL space.
// All functions take values for r, g, b, h, s, and l between 0.0 and 1.0
// (RGB = red, green, blue;  HSL = hue, saturation, luminosity)

#ifndef RGBHSL_H
#define RGBHSL_H


void rgb2hsl(double r, double g, double b, double &h, double &s, double &l);

void hsl2rgb(double h, double s, double l, double &r, double &g, double &b);

// For these 'tween functions, a tween value of 0.0 will output the first
// color while a tween value of 1.0 will output the second color.
// A value of 0 for direction indicates a positive progression around
// the color wheel (i.e. red -> yellow -> green -> cyan...).  A value of
// 1 does the opposite.
void hslTween(double h1, double s1, double l1,
                     double h2, double s2, double l2, double tween, int direction,
                     double &outh, double &outs, double &outl);

void rgbTween(double r1, double g1, double b1,
                     double r2, double g2, double b2, double tween, int direction,
                     double &outr, double &outg, double &outb);


#endif // RGBHSL_H
