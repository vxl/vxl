#ifndef my_pyramid_functions_txx_
#define my_pyramid_functions_txx_

#include <vmap/tests/my_map_functions.txx>

template <class P>
void display_tpyramid(const P & pyramid)
{
  typename P::const_level_iterator i=pyramid.begin_level();
 
  for (int j=0; i!=pyramid.end_level(); ++i, ++j)
  {
    vcl_cout<<vcl_endl<<"----------------"<<vcl_endl
            <<"level_type "<<i->index()<<vcl_endl
            <<"----------------"<<vcl_endl ;
    display_tmap(*i) ;
    vcl_vector<vmap_vertex_index> vertexFather;
    vcl_cout<<"Base vertices correspondances :"<<i->index()<<vcl_endl ;
    pyramid.down_projection_vertices(i->index(),vertexFather) ;
    for (unsigned int j=0; j<vertexFather.size(); ++j)
    {
      vcl_cout<<'('<<pyramid.base_level().vertex(j).id()<<','<<i->vertex(vertexFather[j]).id()<<") " ;
    }
    vcl_cout<<vcl_endl ;

    vcl_vector<vmap_face_index> faceFather;
    vcl_cout<<"Base faces correspondances :"<<i->index()<<vcl_endl ;
    pyramid.down_projection_faces(i->index(),faceFather) ;
    for (unsigned int j=0; j<faceFather.size(); ++j)
    {
      vcl_cout<<'('<<pyramid.base_level().face(j).id()<<','<<i->face(faceFather[j]).id()<<") " ;
    }
    vcl_cout<<vcl_endl ;
  }
}

#endif
