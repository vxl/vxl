//: This is lemsvxlsrc/brcv/shp/bmsh3d/vis/bmsh3d_vis_vertex.h
//  MingChing Chang
//  May 03, 2005.

#ifndef _bmsh3d_vis_vertex_h_
#define _bmsh3d_vis_vertex_h_

#include <bmsh3d/bmsh3d_vertex.h>
#include <bmsh3d/bmsh3d_mesh.h>

class SoSeparator;
class SbColor;

SoSeparator* draw_vertex_geom_SoPoint (const bmsh3d_vertex* V);
SoSeparator* draw_vertex_geom_vispt_SoPoint (const bmsh3d_vertex* V);

SoSeparator* draw_vertex_vispt_SoPoint (const bmsh3d_vertex* V,                                             
                                        const SbColor& color, const float size);

SoSeparator* draw_vertex_geom_SoCube (const bmsh3d_vertex* V, 
                                      const float size);
SoSeparator* draw_vertex_geom_vispt_SoCube (const bmsh3d_vertex* V, 
                                            const float size);

SoSeparator* draw_vertex_SoCube (const bmsh3d_vertex* V, 
                                 const SbColor& color, const float size);
SoSeparator* draw_vertex_vispt_SoCube (const bmsh3d_vertex* V, 
                                       const SbColor& color, const float size,
                                       const bool show_id = false);

#endif


