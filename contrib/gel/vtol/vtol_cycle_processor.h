// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

#ifndef _cycle_processor_h
#define _cycle_processor_h
//-----------------------------------------------------------------------------
//
// Class : cycle_processor
//
// .SECTION Description: A class for tracing boundaries and forming 
// nested one_cycles. The input is a set of vtol_edge(s) which may or may not
// define one or more closed paths, or cycles. The output is a set of 
// vtol_one_chain(s) which define a nested set of cycles.  That is, there is 
// an outer, bounding, cycle and zero or more interior hole boundaries.     
// The algorithm requires a tolerance which is used in the process of 
// deciding if one boundary is enclosed by another.
// Man page information:
//
// .NAME cycle_processor
// .LIBRARY RegistrationBasics
// .HEADER
// .INCLUDE APEXView/cycle_processor.h
// .FILE cycle_processor.h cycle_processor.C
//
// .SECTION Author:
//	        J.L. Mundy August 13, 2000
//          GE Corporate Research and Development
//
// Modifications : <none yet>
//
//-----------------------------------------------------------------------------
//#include <list>
//#include <vcl_vector>
//#include <cool/decls.h>
//#include <cool/ListP.h>
//#include <Topology/vtol_vertex_ref.h>
//#include <Topology/vtol_edge_ref.h>
//#include <Topology/vtol_one_chain_ref.h>
//#include <Topology/Face_ref.h>


#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vcl_vector.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_sptr.h>

class vtol_cycle_processor
{
 public:
  vtol_cycle_processor(vcl_vector<vtol_edge_sptr>& edges);
  vtol_cycle_processor(vcl_vector<vtol_edge*>& edges);
  ~vtol_cycle_processor(){};

  // PUBLIC INTERFACE----------------------------------------------------------
  bool nested_one_cycles(vcl_vector<vtol_one_chain_sptr>& one_chains,
                         const float& tolerance = 1e-03);
 protected:
  //internal utilites
  void set_bridge_vars();
  void init(vcl_vector<vtol_edge_sptr>& edges);
  vtol_edge_sptr search_for_next_edge(vcl_vector<vtol_edge_sptr>& edges_at_last);
  void add_edge_to_path();
  bool classify_path(vcl_vector<vtol_edge_sptr>& path_edges, vtol_one_chain_sptr& chain);
  void compute_cycles();
  void sort_one_cycles();
  void process();
  //members
  bool _valid;
  float _tolerance;
  bool _cycle;
  bool _found_next_edge;
  vtol_vertex_sptr _first;
  vtol_vertex_sptr _last;
  vtol_edge_sptr _l;
  vtol_edge_sptr _next_edge;
  vcl_vector<vtol_edge_sptr> _edges;
  vcl_vector<vtol_edge_sptr> _e_stack;
  vcl_vector<vtol_vertex_sptr> _v_stack;
  vcl_vector<vtol_one_chain_sptr> _chains;
  vcl_vector<vtol_one_chain_sptr> _nested_one_cycles;
};

#endif
// _vtol_cycle_processor_h
