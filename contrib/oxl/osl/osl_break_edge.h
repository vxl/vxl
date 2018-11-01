// This is oxl/osl/osl_break_edge.h
#ifndef osl_break_edge_h_
#define osl_break_edge_h_
//:
// \file
// \author fsm

class osl_edge;
#include <iostream>
#include <list>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Break the given edge at the positions indicated.
//
// The first element of 'where' must be 0 and the last
// must be in->size()-1.
//
// The output edges will be in the order of the edgelchain
// and the first (resp. last) output edge will share its
// start (resp. end) vertex with the start (resp. end)
// vertex of the given edge.
//
// The input edge is not modified in any way, though its
// endpoints will obviously have the reference counts
// increased.
void osl_break_edge(osl_edge const *in,
                    std::vector<unsigned> const &where,
                    std::list<osl_edge*> *broken);

//: Break the given edge at points of "high curvature".
//
// This means edgels where the rms line fitting error over
// a neighbourhood of the given size exceeds the given threshold.
//
// For a neighbourhood size of k,the number of edgels used in
// each line fit is 2*k+1 (so the default is 5 edgels). The new
// edges are pushed onto the end of the given std::list in the
// obvious order.
void osl_break_edge(osl_edge const *in,
                    std::list<osl_edge*> *broken,
                    double threshold = 0.2,
                    unsigned nbhd_size = 2);

#endif // osl_break_edge_h_
