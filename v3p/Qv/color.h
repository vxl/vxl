#ifndef ge3d_color_h
#define ge3d_color_h

#if 0
//<copyright>
//
// Copyright (c) 1994,95
// Institute for Information Processing and Computer Supported New Media (IICM),
// Graz University of Technology, Austria.
//
// This file is part of VRweb.
//
// VRweb is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VRweb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VRweb; see the file LICENCE. If not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
//</copyright>
#endif

/*
 * File:     color.h
 *
 * Author:   Michael Pichler
 *
 * Created:  24 Jan 94
 *
 * Changed:  24 Jan 94
 *
 *
 */


typedef struct
{ float R, G, B;
} colorRGB;


/* initialisation/assignment.  c := (r, g, b)  */
#define initRGB(c, r, g, b) \
  (c).R = (r),  (c).G = (g),  (c).B = (b)


#endif // ge3d_color_h
