#ifndef ge3d_mtl_h
#define ge3d_mtl_h

#if 0
//<copyright>
//
// Copyright (c) 1995,96
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
 * File:     mtl.h
 *
 * Author:   Michael Pichler
 *
 * Created:  29 Jun 95
 *
 * Changed:  24 Jul 96
 *
 *
 */


#include "color.h"


/* definition of multiple materials (for facesets) */

typedef struct
{
  int
    num_base,
    num_ambient,
    num_diffuse,
    num_specular,
    num_emissive,
    num_shininess,
    num_transparency;
  const colorRGB
    *rgb_base;
  colorRGB
    *rgb_ambient,
    *rgb_diffuse,
    *rgb_specular,
    *rgb_emissive;
  float
    *val_shininess,
    *val_transparency;
  int multival;  /* multiple values on other than diffuse */
} materialsGE3D;


#define initmtl3D(m)  \
  m->num_base = m->num_ambient = m->num_diffuse = m->num_specular = m->num_emissive = 0,  \
  m->num_shininess = m->num_transparency = 0,  \
  m->rgb_base = m->rgb_ambient = m->rgb_diffuse = m->rgb_specular = m->rgb_emissive = NULL,  \
  m->val_shininess = m->val_transparency = NULL,  \
  m->multival = 0

/* material bindings */

enum ge3d_matbinding_t
{
  matb_default,
  matb_overall,
  matb_perpart,
  matb_perpartindexed,
  matb_perface,
  matb_perfaceindexed,
  matb_pervertex,
  matb_pervertexindexed
};


/* material scope definition */

enum ge3d_material_t  /* constants for ge3dMaterial */
{
  mat_front,
  mat_back,
  mat_front_and_back
};


#endif // ge3d_mtl_h
