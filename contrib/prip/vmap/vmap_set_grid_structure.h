// This is contrib/prip/vmap/vmap_set_grid_structure.h
#ifndef vmap_set_grid_structure_h_
#define vmap_set_grid_structure_h_
//:
// \file
// \brief function to build Maps that have a grid shape.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vmap_grid_structure.h"


template < class TMap >
void vmap_set_grid_structure(TMap & arg, const vmap_grid_structure& grid, vmap_2_tmap_tag)
{
  vcl_cout<<"Building TopologicalGrid"<<vcl_endl ;
  arg.initialise(grid.nb_vertices(),
								grid.nb_edges(),
								grid.nb_faces()) ;
  arg.set_edge_cycles();
  
	int i,j;

  for (j=1; j<grid.height(); j++)
    for (i=1; i<grid.width(); i++)
    {
      arg.set_dart(grid.down_dart(i,j),grid.left_dart(i,j),grid.vertex(i,j),grid.face(i,j));
      arg.set_dart(grid.right_dart(i,j),grid.down_dart(i,j),grid.vertex(i,j),grid.face(i,j-1));
      arg.set_dart(grid.up_dart(i,j),grid.right_dart(i,j),grid.vertex(i,j),grid.face(i-1,j-1));
      arg.set_dart(grid.left_dart(i,j),grid.up_dart(i,j),grid.vertex(i,j),grid.face(i-1,j));
    }
  int background=grid.background_index() ;
  for (i=1; i<grid.width(); i++)
  {
    arg.set_dart(grid.left_dart(i,0),grid.right_dart(i,0),grid.vertex(i,0),grid.face(i-1,0));
    arg.set_dart(grid.right_dart(i,0),grid.down_dart(i,0),grid.vertex(i,0),background);
    arg.set_dart(grid.down_dart(i,0),grid.left_dart(i,0),grid.vertex(i,0),grid.face(i,0));
    arg.set_dart(grid.right_dart(i,grid.height()),grid.left_dart(i,grid.height()),grid.vertex(i,grid.height()),grid.face(i,grid.height()-1));
    arg.set_dart(grid.left_dart(i,grid.height()),grid.up_dart(i,grid.height()),grid.vertex(i,grid.height()),background);
    arg.set_dart(grid.up_dart(i,grid.height()),grid.right_dart(i,grid.height()),grid.vertex(i,grid.height()),grid.face(i-1,grid.height()-1));
  }
  for (j=1; j<grid.height(); j++)
  {
    arg.set_dart(grid.up_dart(0,j),grid.right_dart(0,j),grid.vertex(0,j),background);
    arg.set_dart(grid.right_dart(0,j),grid.down_dart(0,j),grid.vertex(0,j),grid.face(0,j-1));
    arg.set_dart(grid.down_dart(0,j),grid.up_dart(0,j),grid.vertex(0,j),grid.face(0,j));
    arg.set_dart(grid.up_dart(grid.width(),j),grid.down_dart(grid.width(),j),grid.vertex(grid.width(),j),grid.face(grid.width()-1,j-1));
    arg.set_dart(grid.down_dart(grid.width(),j),grid.left_dart(grid.width(),j),grid.vertex(grid.width(),j),background);
    arg.set_dart(grid.left_dart(grid.width(),j),grid.up_dart(grid.width(),j),grid.vertex(grid.width(),j),grid.face(grid.width()-1,j));

  }

  { // corner 1
    arg.set_dart(grid.down_dart(0,0),grid.right_dart(0,0),0,0);
    arg.set_dart(grid.right_dart(0,0),grid.down_dart(0,0),0,background);
  }
  { // corner 2
    arg.set_dart(grid.down_dart(grid.width(),0),grid.left_dart(grid.width(),0),grid.vertex(grid.width(),0),background);
    arg.set_dart(grid.left_dart(grid.width(),0),grid.down_dart(grid.width(),0),grid.vertex(grid.width(),0),grid.face(grid.width()-1,0));
  }
  { // corner 3
    arg.set_dart(grid.up_dart(0,grid.height()),grid.right_dart(0,grid.height()),grid.vertex(0,grid.height()),background);
    arg.set_dart(grid.right_dart(0,grid.height()),grid.up_dart(0,grid.height()),grid.vertex(0,grid.height()),grid.face(0,grid.height()-1));
  }
  { // corner 4
    arg.set_dart(grid.left_dart(grid.width(),grid.height()),grid.up_dart(grid.width(),grid.height()),grid.vertex(grid.width(),grid.height()),background);
    arg.set_dart(grid.up_dart(grid.width(),grid.height()),grid.left_dart(grid.width(),grid.height()),grid.vertex(grid.width(),grid.height()),grid.face(grid.width()-1,grid.height()-1));
  }
}
template < class TMap >
void vmap_set_grid_structure(TMap & arg, const vmap_grid_structure& grid, vmap_2_map_tag)
{
	vcl_cout<<"Building CombinatorialGrid"<<vcl_endl ;
  arg.initialise_darts(grid.nb_darts()) ;

  int i,j;

  for (j=1; j<grid.height(); j++)
    for (i=1; i<grid.width(); i++)
    {
      arg.set_sigma(grid.down_dart(i,j),grid.left_dart(i,j));
      arg.set_sigma(grid.right_dart(i,j),grid.down_dart(i,j));
      arg.set_sigma(grid.up_dart(i,j),grid.right_dart(i,j));
      arg.set_sigma(grid.left_dart(i,j),grid.up_dart(i,j));
    }
  for (i=1; i<grid.width(); i++)
  {
    arg.set_sigma(grid.left_dart(i,0),grid.right_dart(i,0));
    arg.set_sigma(grid.right_dart(i,0),grid.down_dart(i,0));
    arg.set_sigma(grid.down_dart(i,0),grid.left_dart(i,0));
    arg.set_sigma(grid.right_dart(i,grid.height()),grid.left_dart(i,grid.height()));
    arg.set_sigma(grid.left_dart(i,grid.height()),grid.up_dart(i,grid.height()));
    arg.set_sigma(grid.up_dart(i,grid.height()),grid.right_dart(i,grid.height()));
  }
  for (j=1; j<grid.height(); j++)
  {
    arg.set_sigma(grid.up_dart(0,j),grid.right_dart(0,j));
    arg.set_sigma(grid.right_dart(0,j),grid.down_dart(0,j));
    arg.set_sigma(grid.down_dart(0,j),grid.up_dart(0,j));
    arg.set_sigma(grid.up_dart(grid.width(),j),grid.down_dart(grid.width(),j));
    arg.set_sigma(grid.down_dart(grid.width(),j),grid.left_dart(grid.width(),j));
    arg.set_sigma(grid.left_dart(grid.width(),j),grid.up_dart(grid.width(),j));

  }

  { // corner 1
    arg.set_sigma(grid.down_dart(0,0),grid.right_dart(0,0));
    arg.set_sigma(grid.right_dart(0,0),grid.down_dart(0,0));
  }
  { // corner 2
    arg.set_sigma(grid.down_dart(grid.width(),0),grid.left_dart(grid.width(),0));
    arg.set_sigma(grid.left_dart(grid.width(),0),grid.down_dart(grid.width(),0));
  }
  { // corner 3
    arg.set_sigma(grid.up_dart(0,grid.height()),grid.right_dart(0,grid.height()));
    arg.set_sigma(grid.right_dart(0,grid.height()),grid.up_dart(0,grid.height()));
  }
  { // corner 4
    arg.set_sigma(grid.left_dart(grid.width(),grid.height()),grid.up_dart(grid.width(),grid.height()));
    arg.set_sigma(grid.up_dart(grid.width(),grid.height()),grid.left_dart(grid.width(),grid.height()));
  }
}

template < class TMap >
inline void vmap_set_grid_structure(TMap & arg, int width, int height)
{
	 vmap_grid_structure grid(width,height) ;
	 vmap_set_grid_structure(arg, grid, TMap::tag ) ;
}
template < class TMap >
inline void vmap_set_grid_structure(TMap & arg, const vmap_grid_structure& grid)
{
	vmap_set_grid_structure(arg, grid, TMap::tag ) ;
}

#endif
