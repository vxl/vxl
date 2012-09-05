// This is core/vil/vil_color_table.h
#ifndef vil_color_table_h_
#define vil_color_table_h_
//:
// \file
// \brief Color tables for applying false color to greyscale images.
// \author brooksby@research.ge.com
// \date 08 Nov 2005
// \verbatim
//  Modifications
//   8 Nov 2005 Initial port from TargetJr.
// \endverbatim
//
// This file provides a way to load color tables directly, rather than computing them.
// It was created from the predefined color tables in IDL. It provides a way of adding
// pseudo-color to data for visualization purposes.
//
// These tables are provided courtesy of Research Systems Incorporated (RSI),
// Boulder, CO and are used with permission.  RSI is the developer of IDL.
//
// The number (ie REDx, GREENx, BLUEx) refer to the color table numbers from IDL.
// The IDL descriptions are as follows. I don't claim the names make sense, but
// at least they are unique and give a hint as to how they may display.
//
// - Nr Name
// -  0 Black & White Linear
// -  1 Blue/White Linear
// -  2 Green-Red-Blue-White
// -  3 Red Temperature
// -  4 Blue-Green-Red-Yellow
// -  5 Standard Gamma-II
// -  6 Prism
// -  7 Red-Purple
// -  8 Green/White Linear
// -  9 Green/White Exponential
// - 10 Green-Pink
// - 11 Blue-Red
// - 12 16 Level
// - 13 Rainbow
// - 14 Steps
// - 15 Stern Special
// - 16 Haze
// - 17 Blue-Pastel
// - 18 Red-Pastel
// - 19 Hue Sat Lightness 1
// - 20 Hue Sat Lightness 2
// - 21 Hue Sat Value 1
// - 22 Hue Sat Value 2
// - 23 Purple-Red + Stripes
// - 24 Beach
// - 25 Mac Style
// - 26 Eos A
// - 27 Eos B
// - 28 Hardcandy
// - 29 Nature
// - 30 Ocean
// - 31 Peppermint
// - 32 Plasma
// - 33 Blue-Red 2
// - 34 Rainbow 2
// - 35 Blue Waves
// - 36 Volcano
// - 37 Waves
//
// To understand how these color maps appear, compile and run the example
// program vil_color_tables.cxx.  This executable will produce a color image
// for each of these tables representing a ramp from 0 to 255.

#include <vil/vil_rgb.h>
#include <vxl_config.h> // for vxl_byte

enum color_theme {BWLIN1=0, BWLIN2, GRBW, REDTEMP, BGRY, GAMMA_II, PRISM, RED_PURPLE,
                  GWLIN, GWEXP, GREENPINK, BLUERED, SIXTEENLEVEL, RAINBOW1, STEPS, STERN,
                  HAZE, BLUE_PASTEL, RED_PASTEL, HSL1, HSL2, HSV1, HSV2, PURPLE_RED_STRIPES,
                  BEACH, MAC, EOS_A, EOS_B, HARD_CANDY, NATURE, OCEAN, PEPPERMINT, PLASMA,
                  BLUERED2, RAINBOW2, BLUE_WAVES, VOLCANO1, WAVES1};

vil_rgb<vxl_byte> color_value(int theme, int color_index);

#endif
