// This is prip/vdtop/vdtop_set_8_veinerization_structure.h
#ifndef vdtop_set_8_veinerization_structure_h_
#define vdtop_set_8_veinerization_structure_h_
//:
// \file
// \brief .
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vil/vil_image_view.h>
#include <vmap/vmap_types.h>
#include <vdtop/vdtop_set_structure_from_digital_graph.h>

//: The 3 in 1 function.
// It computes upper masks, removes non maximal directions, and compute the symmetric.
// "masks" is then a digital graph of the 4-veinerization of the tabulated function [arg(i,j),j,i]. 
template <class T>
void vdtop_compute_8_veinerization_mask(vil_image_view<T> & img,
                                        vil_image_view<vdtop_8_neighborhood_mask> & masks,
                                        int & nb_vertices, int & nb_edges) ;

//: Set a map from vmap so that it has the structure of a 8-veinerization.
//  See  Marchadier Jocelyn, Walter Kropatsch, Allan Hanbury "Homotopic transformations of combinatorial maps",
//  DGCI2003,  Lectures Notes On Computer Science no. 2886, page 134-143, 2003 for details and motivations.
template <class T,  class TMap >
inline void vdtop_set_8_veinerization_structure(TMap & arg, vil_image_view<T> & img)
{
  int nb_vertices,nb_edges ;
  vil_image_view<vdtop_8_neighborhood_mask> mask ;
  // build upper mask from grey map 
  vdtop_compute_8_veinerization_mask(img, mask,nb_vertices,nb_edges) ;
  vdtop_set_structure_from_digital_graph(arg, mask, nb_vertices,nb_edges, TMap::tag ) ;
}

#include "vdtop_set_8_veinerization_structure.txx"

#endif

