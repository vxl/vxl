#ifndef my_map_h_
#define my_map_h_

#include <vmap/vmap_2_map.h>
#include <vmap/vmap_2_tmap.h>

struct my_map_vertex : public vmap_2_tmap_vertex
{
  int _id ;
  int id() const
  {
    return  _id ;
  }

  int & id()
  {
    return _id ;
  }
};

struct my_map_edge : public vmap_2_tmap_edge
{
  int _id ;
  int id() const
  {
    return  _id ;
  }

  int & id()
  {
    return _id ;
  }
};

struct my_map_face : public vmap_2_tmap_face
{
  int _id ;
  int id() const
  {
    return  _id ;
  }

  int & id()
  {
    return _id ;
  }
};

struct my_map_dart : public vmap_2_tmap_dart
{
  int _id ;
  int id() const
  {
    return  _id ;
  }

  int & id()
  {
    return _id ;
  }
};

typedef vmap_2_map<> my_map ;
typedef vmap_2_tmap<my_map_vertex,my_map_edge,my_map_face,my_map_dart> my_tmap ;

#endif
