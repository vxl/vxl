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
#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include <vtol/vtol_edge.h>
#include <gevd/gevd_region_edge.h>




gevd_region_edge::gevd_region_edge(vtol_edge_2d * e)
{
  _left_region = 0;
  _right_region = 0;
  _edge = e;
  //  if(e)
  //    e->Protect();
}

gevd_region_edge::~gevd_region_edge() 
{
  //  if(_edge)
  //_edge->UnProtect();
}

void gevd_region_edge::SetLeftRegion(unsigned int label)
{
  if(label!=_right_region)
    _left_region=label;
}

void gevd_region_edge::SetRightRegion(unsigned int label)
{
  if(label!=_left_region)
    _right_region=label;
}

bool gevd_region_edge::SetNewLabel(unsigned int label)
{
  if(!label) return false;
  _labels.push_back(label);
  return true ;
}

//--------------------------------------------------------
//  -- Conditionally propagate the labels of a gevd_region_edge.
//     In this approach we don't differentiate between Right 
//     and Left.  Here we allow an unlimited number of labels
//     to be assigned to an edge, which is necessary due to
//     small regions which don't have proper closure but 
//     whose edges are on the boundary of larger legitimate
//     regions.
void gevd_region_edge::Prop(gevd_region_edge* re, unsigned int label)
{
  //First try to propagate the labels
  //Do not allow propagation to occur 
  //between different edges
  vtol_edge* source = re->get_edge();
  vtol_edge* target = this->get_edge();
  if(source&&source==target)
    for(int i = 0; i<re->GetNumLabels(); i++)
      this->SetNewLabel(re->GetLabel(i));
  this->SetNewLabel(label);
}

