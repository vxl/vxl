#ifndef my_pyramidamid_h_
#define my_pyramidamid_h_

#include <vpyr/vpyr_2_pyramid.h>
#include <vpyr/vpyr_2_tpyramid.h>

struct IdData
{
  int id ;
};

struct my_pyramid_vertex : public vmap_2_tpdl_vertex< vmap_simple_data<IdData> >
{
  int id() const
  {
    return data().id ;
  }

  int & id()
  {
    return data().id ;
  }
};

struct my_pyramid_edge : public vmap_2_tpdl_edge< vmap_simple_data<IdData> >
{
  int id() const
  {
    return data().id ;
  }

  int & id()
  {
    return data().id ;
  }
};

struct my_pyramid_face : public vmap_2_tpdl_face< vmap_simple_data<IdData> >
{
  int id() const
  {
    return data().id ;
  }

  int & id()
  {
    return data().id ;
  }
};

struct my_pyramid_dart : public vmap_2_tpdl_dart< vmap_simple_data<IdData> >
{
  int id() const
  {
    return data().id ;
  }

  int & id()
  {
    return data().id ;
  }
};

//typedef vmap_2_pyramid<> Pyramid1 ;
typedef vpyr_2_tpyramid_level<my_pyramid_vertex,my_pyramid_edge,my_pyramid_face,my_pyramid_dart> my_pyramid_level ;
//class my_pyramid_level : public vmap_2_tpyramid_level<my_pyramid_vertex,my_pyramid_edge,my_pyramid_face,my_pyramid_dart>
//{
//};
typedef vpyr_2_tpyramid<my_pyramid_level> my_tpyramid ;


void build_base_grid(my_tpyramid & p) ;

#endif
