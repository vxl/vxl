//: This is lemsvxlsrc/brcv/shp/bmsh3d/vis/bmsh3d_vis_mesh.h
//  MingChing Chang
//  May 03, 2005.

#ifndef _bmsh3d_vis_edge_h_
#define _bmsh3d_vis_edge_h_

#include <Inventor/SbColor.h>
class SoSeparator;

void draw_edge_geom (SoSeparator* root, const bmsh3d_edge *E,
                     const bool user_defined_class = true);

SoSeparator* draw_edge (bmsh3d_edge* E, const SbColor& color, const float width = 1.0f,
                        const bool user_defined_class = true);


#endif


