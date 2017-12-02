// This is prip/vdtop/vdtop_set_structure_from_digital_graph.h
#ifndef vdtop_set_structure_from_digital_graph_h_
#define vdtop_set_structure_from_digital_graph_h_
//:
// \file
// \author Jocelyn Marchadier
// \date 06 May 2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vil/vil_image_view.h>
#include <vmap/vmap_types.h>
#include <vdtop/vdtop_8_neighborhood_mask.h>

//: Sets a map structure from a digital graph
template <class TMap>
void vdtop_set_structure_from_digital_graph(TMap & res, const vil_image_view<vdtop_8_neighborhood_mask> & arg,
                                            int nb_vertices,int nb_edges, vmap_2_map_tag) ;

//: Sets a tmap structure from a digital graph
template <class TMap>
void vdtop_set_structure_from_digital_graph(TMap & res, const vil_image_view<vdtop_8_neighborhood_mask> & arg,
                                            int nb_vertices,int nb_edges, vmap_2_tmap_tag) ;

#include "vdtop_set_structure_from_digital_graph.hxx"

#endif
