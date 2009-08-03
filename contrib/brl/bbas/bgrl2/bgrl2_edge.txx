// This is brl/bbas/bgrl2/bgrl2_edge.txx
#ifndef bgrl2_edge_txx_
#define bgrl2_edge_txx_
//:
// \file

#include "bgrl2_edge.h"

//: Returns target(edge) if v = source(edge) and source(edge) otherwise.
template <class V>
typename bgrl2_edge<V>::V_sptr bgrl2_edge<V>::opposite(V_sptr v)
{
  if (v==source_)
    return target_;
  else
    return source_;
}

//: Determine if ``this'' edge shares a vertex with other edge
// Return the shared node if so, otherwise return 0
template <class V>
typename bgrl2_edge<V>::V_sptr bgrl2_edge<V>::
shared_vertex(const bgrl2_edge<V >& other) const
{
  if (this->source() == other.source() || this->source() == other.target())
    return this->source();
  if (this->target() == other.source() || this->target() == other.target())
    return this->target();
  return 0;
}


//: Print an ascii summary to the stream
template <class V>
void bgrl2_edge<V>::print_summary( vcl_ostream& os ) const
{
  os << vcl_endl;
}

#define BGRL2_EDGE_INSTANTIATE(E) \
template class bgrl2_edge<E >

#endif // bgrl2_edge_txx_
