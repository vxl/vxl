#include "my_map.h"
#include "my_map_functions.txx"
#include <vmap/vmap_set_grid_structure.h>

#include <vmap/vmap_non_oriented_kernel.h>
#include <vmap/vmap_oriented_kernel.h>

#include <vcl_iostream.h>


void tmap_test()
{
  my_tmap map ;
  vcl_cout<<vcl_endl<<"----------------"<<vcl_endl
          <<"TopologialMap test."<<vcl_endl
          <<"----------------"<<vcl_endl

          <<"Initialization of a 2X2 grid."<<vcl_endl ;
  vmap_set_grid_structure(map,2,2) ;
  if (!map.valid_permutations())
  {
    vcl_cerr<<"Problem !!!!"<<vcl_endl ;
  }
  // write its structure to vcl_cout
  map.write_structure(vcl_cout) ;

  // setting its elements
  vcl_cout<<map.nb_vertices()<<" vertices,"
          <<map.nb_faces()<<" faces and "
          <<map.nb_edges()<<" edges."<<vcl_endl ;

  // Value inits
  int i=1 ;
  for (my_tmap::vertex_iterator v=map.begin_vertex(); v!=map.end_vertex(); ++v, ++i)
  {
    v->id()=i ;
  }

  i=1 ;
  for (my_tmap::edge_iterator e=map.begin_edge(); e!=map.end_edge(); ++e, ++i)
  {
    e->id()=i ;
  }

  i=1 ;
  for (my_tmap::face_iterator f=map.begin_face(); f!=map.end_face(); ++f, ++i)
  {
    f->id()=i ;
  }
  i=1 ;
  for (my_tmap::dart_iterator d=map.begin_dart(); d!=map.end_dart(); ++d, ++i)
  {
    d->id()=i ;
  }

  display_tmap(map) ;

  vcl_cout<<"Contraction of {1,7}/{"<<map.alpha(0)<<','<<map.alpha(12)<<'}'<<vcl_endl ;

  my_tmap::contraction_kernel K(map) ;
  K.add(map.alpha(0)) ;
  K.add(map.alpha(12)) ;
  vcl_cout<<K.size()<<vcl_endl ;
  map.contraction(K) ;
  display_tmap(map) ;

  vcl_cout<<"Contraction of {8,4} :"<<vcl_endl ;
  my_tmap::removal_kernel K2(map) ;

  K2.add(find_dart(map,8)) ;
  K2.add(find_dart(map,4).alpha()) ;
  vcl_cout<<K2.size()<<vcl_endl ;
  map.removal(K2) ;
  display_tmap(map) ;

  my_tmap copy_map(map) ;
  if (!copy_map.valid_permutations())
  {
    vcl_cerr<<"Problem !!!!"<<vcl_endl ;
  }
  display_tmap(copy_map) ;
}

void map_test()
{
  my_map map ;
  vmap_set_grid_structure(map,2,2) ;
}


int main()
{
  map_test() ;
  tmap_test() ;
  return 0 ;
}
