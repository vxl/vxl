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
//-*- c++ -*-------------------------------------------------------------------
#ifndef _gevd_region_edge_h_
#define _gevd_region_edge_h_
//
// .NAME gevd_region_edge - An edge with extract information to support
//                    constructing Face regions from an gevd_vtol_edge_2d segmentation.
// .LIBRARY Detection
// .HEADER Segmentation package
// .INCLUDE Detection/gevd_region_edge.h
// .FILE gevd_region_edge.h
// .FILE gevd_region_edge.C
//
// .SECTION Description
//  This class supports tracing of region boundaries in conjuction with
//  the class gevd_vtol_edge_2dlIntensity. The gevd_region_edge maintains a list of region
//  labels which the edge is adjacent to. Ideally, the edge is adjacent to
//  only two regions, left and right, as shown below.
//
//                    _left_region
//           V1------------------------V2
//                    _right_region
//
//  In practice, this restriction is too limiting.  It is possible for 
//  an edge to be adjacent to more than two regions. For example:
//     c  c
//   o-----\ c
// E    b b \ c
//   o------ a \c  Note that edge E is adjacent to both regions
//      c c \a /c  a and b even though there is no intervening  
//         c -- c  vertex.  In order to preserve regions b and 
//           c c   c, it is necessary to allow more then two labels 
//                 for a given edge.
//   
// .SECTION Author J. L. Mundy - January 24, 1999
// .SECTION Modifications 
//       Feb 10, 1999  - JLM Extended the concept of a 
//       gevd_region_edge to represet a Vertex, where the gevd_vtol_edge_2d is NULL.
//       This extention permits the propagation of region labels
//       across junctions.
//       March 7, 1999 - JLM Allowed more than two regions per gevd_vtol_edge_2d.
//-----------------------------------------------------------------------------

#include <vtol/vtol_edge_2d.h>
#include <vbl/vbl_ref_count.h>

class gevd_region_edge : public vbl_ref_count
{
public:
  //Constructors/Destructors
  gevd_region_edge(vtol_edge_2d* e);
  ~gevd_region_edge();	  
  //Accessors
  unsigned int GetLeftRegion() {return _left_region;}//Now obsolete
  void SetLeftRegion(unsigned int region);//Now obsolete
  unsigned int GetRightRegion() {return _right_region;}//Now obsolete
  void SetRightRegion(unsigned int region); //Now obsolete
  void Prop(gevd_region_edge* re, unsigned int label);
  vtol_edge_2d* get_edge(){return _edge;}
  bool is_vertex(){return !(bool)_edge;}
  int GetNumLabels(){return _labels.size();}
  unsigned int GetLabel(int i){return _labels[i];}
  //Utitities (especially for testing)
protected:
  //Utilities
  bool SetNewLabel(unsigned int label);
  //members
  vtol_edge_2d* _edge;
  unsigned int _left_region;
  unsigned int _right_region;
  vcl_vector<unsigned int> _labels;
};

#endif
