#include "my_top_level.h"

void initialise_ids(my_top_level & p)
{
  // Value inits
  int i=1 ;
  for (my_top_level::vertex_iterator v=p.begin_vertex(); v!=p.end_vertex(); ++v)
  {
    v->id()=i ;
    i++ ;
  }

  i=1 ;
  for (my_top_level::edge_iterator e=p.begin_edge(); e!=p.end_edge(); ++e)
  {
    e->id()=i ;
    i++ ;
  }
  i=1 ;
  for (my_top_level::face_iterator f=p.begin_face(); f!=p.end_face(); ++f)
  {
    f->id()=i ;
    i++ ;
  }
  i=1 ;
  for (my_top_level::dart_iterator d=p.begin_dart(); d!=p.end_dart(); ++d)
  {
    d->id()=i ;
    i++ ;
  }
}

