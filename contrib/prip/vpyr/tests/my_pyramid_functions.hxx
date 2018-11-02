#ifndef my_pyramid_functions_hxx_
#define my_pyramid_functions_hxx_

#include <iostream>
#include <vector>
#include <vmap/tests/my_map_functions.hxx>
#include <vmap/vmap_types.h> // for vmap_vertex_index & vmap_face_index
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class P>
void display_tpyramid(const P & pyramid)
{
  typename P::const_level_iterator i=pyramid.begin_level();

  for (int j=0; i!=pyramid.end_level(); ++i, ++j)
  {
    std::cout<<"\n----------------\nlevel_type 0x"
            <<std::hex<<(*i).index()<<std::dec
            <<"\n----------------\n" ;
    display_tmap(*i) ;
    std::vector<vmap_vertex_index> vertexFather;
    std::cout<<"Base vertex correspondences for 0x"<<std::hex<<(*i).index()<<std::dec<<':' ;
    pyramid.down_projection_vertices((*i).index(),vertexFather) ;
    for (unsigned int j=0; j<vertexFather.size(); ++j)
    {
      std::cout<<" ("<<pyramid.base_level().vertex(j).id()<<','<<(*i).vertex(vertexFather[j]).id()<<')' << std::flush;
    }
    std::cout<<std::endl ;

    std::vector<vmap_face_index> faceFather;
    std::cout<<"Base face correspondences for 0x"<<std::hex<<(*i).index()<<std::dec<<':' ;
    pyramid.down_projection_faces((*i).index(),faceFather) ;
    for (unsigned int j=0; j<faceFather.size(); ++j)
    {
      std::cout<<" ("<<pyramid.base_level().face(j).id()<<','<<(*i).face(faceFather[j]).id()<<')' << std::flush;
    }
    std::cout<<std::endl ;
  }
}

#endif
