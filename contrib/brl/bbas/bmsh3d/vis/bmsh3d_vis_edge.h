// This is brl/bbas/bmsh3d/vis/bmsh3d_vis_edge.h
#ifndef bmsh3d_vis_edge_h_
#define bmsh3d_vis_edge_h_
//:
// \file
// \author MingChing Chang
// \date May 03, 2005.

#include <Inventor/SbColor.h>
class SoSeparator;
class bmsh3d_edge;

void draw_edge_geom (SoSeparator* root, const bmsh3d_edge *E,
                     const bool user_defined_class = true);

SoSeparator* draw_edge (bmsh3d_edge* E, const SbColor& color, const float width = 1.0f,
                        const bool user_defined_class = true);

#endif
