#include "my_pyramid.h"
#include "my_pyramid_functions.txx"
#include <vmap/tests/my_map.h>
#include <vmap/vmap_set_grid_structure.h>

void build_base_grid(my_tpyramid & p)
{
  vcl_cout<<"Initialization of a 2X2 grid."<<vcl_endl ;
  my_tmap map ;

  vmap_set_grid_structure(map,2,2) ;

  vcl_cout<<"Set the base of the pyramid."<<vcl_endl ;
  p.set_base_structure(map) ;
  // write its structure to cout

  // setting its elements
#if 0
  vcl_cout<<p.nb_vertices(p.base_level())<<" vertices,"
          <<p.nb_faces(p.base_level())<<" faces and "
          <<p.nb_edges(p.base_level())<<" edges."<<vcl_endl ;
#endif // 0

  // Value inits
  int i=1 ;
  for (my_pyramid_level::vertex_iterator v=p.base_level().begin_vertex(); v!=p.base_level().end_vertex(); ++v)
  {
    v->id()=i ;
    i++ ;
  }

  i=1 ;
  for (my_pyramid_level::edge_iterator e=p.base_level().begin_edge(); e!=p.base_level().end_edge(); ++e)
  {
    e->id()=i ;
    i++ ;
  }
  i=1 ;
  for (my_pyramid_level::face_iterator f=p.base_level().begin_face(); f!=p.base_level().end_face(); ++f)
  {
    f->id()=i ;
    i++ ;
  }
  i=1 ;
  for (my_pyramid_level::dart_iterator d=p.base_level().begin_dart(); d!=p.base_level().end_dart(); ++d)
  {
    d->id()=i ;
    i++ ;
  }

  display_tpyramid(p) ;
}

