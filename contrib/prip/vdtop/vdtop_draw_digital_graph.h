// This is contrib/prip/vdtop/vdtop_draw_digital_graph.h
#ifndef vdtop_draw_digital_graph_h_
#define vdtop_draw_digital_graph_h_
//:
// \file
// \brief Function to draw a gigital graph in a human readable form. For now, graps have the structure of vil_image_view<vdtop_8_neighborhood_mask>.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vxl_config.h>
#include "vil/vil_image_view.h"
#include "vdtop/vdtop_8_neighborhood_mask.h"

//: draws the digital graph in a human understable form.
//
void vdtop_draw_digital_graph(vil_image_view<vxl_byte> & img,  vil_image_view<vdtop_8_neighborhood_mask> & masks) 
{
  img.set_size(5*masks.ni(),5*masks.nj()) ;
  for (int j= 0 ;j<masks.nj(); ++j)
    for (int i = 0; i<masks.ni(); ++i)
      {
				img(i*5+3,j*5+3)=255 ;
				vdtop_freeman_code dir= masks(i,j).direction_8_neighbor(0) ;
				for (int k = 0 ; k<masks(i,j).nb_8_neighbors(); dir= masks(i,j).direction_8_neighbor(++k) )
				{
					for (int l=1;l<2;l++)
						img(i*5+3+l*dir.di(),j*5+3+l*dir.dj())=128 ;
				}
			}
}

#endif
