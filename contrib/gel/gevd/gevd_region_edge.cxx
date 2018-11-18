// This is gel/gevd/gevd_region_edge.cxx
#include "gevd_region_edge.h"
//:
// \file
#include <vtol/vtol_edge_2d.h>

gevd_region_edge::gevd_region_edge(const vtol_edge_2d_sptr& e)
: edge_(e)
{
}

gevd_region_edge::~gevd_region_edge() = default;

bool gevd_region_edge::is_vertex() const
{
  return !edge_; // returns true if edge_ is a null (smart) pointer
}

bool gevd_region_edge::SetNewLabel(unsigned int label)
{
  if (label == 0) return false;
  labels_.push_back(label);
  return true;
}

//--------------------------------------------------------
//: Conditionally propagate the labels of a gevd_region_edge.
//  In this approach we don't differentiate between Right
//  and Left.  Here we allow an unlimited number of labels
//  to be assigned to an edge, which is necessary due to
//  small regions which don't have proper closure but
//  whose edges are on the boundary of larger legitimate
//  regions.
void gevd_region_edge::Prop(gevd_region_edge const* re, unsigned int label)
{
  //First try to propagate the labels
  //Do not allow propagation to occur
  //between different edges
  vtol_edge_2d_sptr source = re->get_edge();
  vtol_edge_2d_sptr target = this->get_edge();
  if (source&&target&&*source==*target)
    for (unsigned int i = 0; i<re->NumLabels(); ++i)
      this->SetNewLabel(re->GetLabel(i));
  this->SetNewLabel(label);
}
