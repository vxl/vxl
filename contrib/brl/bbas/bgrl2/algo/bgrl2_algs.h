// This is contrib/brl/bbas/bgrl2/algo/bgrl2_algs.h
#ifndef bgrl2_algs_h_
#define bgrl2_algs_h_
//:
// \file
// \brief Basic graph algorithms
// \author Ozge C. Ozcanli
//
// \verbatim
//  Modifications
//   O.C.Ozcanli  02/25/2007   added euler tour algorithm
//   Ozge C. Ozcanli 11/15/08  Moved up to vxl
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "../bgrl2_graph.h"

//: return the euler tour of the graph starting from the given edge and in the direction towards the given node
//  (the given node should be either source or target of the given node)
//  ALG:
//  starting from the "next edge" of the given node wrt the given edge, add the next edges of successor nodes as a chain
//  stop when the initial edge is re-encountered
template <class G, class E, class V>
void euler_tour(vbl_smart_ptr<G> g, vbl_smart_ptr<E> e, vbl_smart_ptr<V> n, std::vector<vbl_smart_ptr<E> >& edges);

//: return the depth of the graph starting from the given node (!!!assumes there are no loops)
template <class G, class E, class V>
int depth_no_loop(vbl_smart_ptr<G> g, vbl_smart_ptr<E> e, vbl_smart_ptr<V> n);


#endif // bgrl2_algs_h_
