// This is brl/bseg/sdet/sdet_region_edge.cxx
#include "sdet_region_edge.h"
//:
// \file
#include <vcl_algorithm.h> // find
#include <vtol/vtol_edge_2d.h>

sdet_region_edge::sdet_region_edge(vtol_edge_2d_sptr e)
: edge_(e)
{
}

sdet_region_edge::~sdet_region_edge()
{
}

bool sdet_region_edge::is_vertex() const
{
  return !edge_; // returns true if edge_ is a null (smart) pointer
}

bool sdet_region_edge::SetNewLabel(unsigned int label)
{
  if (label == 0) return false;
  if (vcl_find(labels_.begin(), labels_.end(), label) == labels_.end())
    labels_.push_back(label);
  return true;
}

unsigned int sdet_region_edge::NumLabels(unsigned int max_label) const
{
  unsigned int n  = labels_.size();
  if (n<=max_label)
    return n;
  else
  {
    vcl_cout << "In sdet_region_edge::NumLabels(..) - # labels exceeds max label\n";
    return 0;
  }
}

//--------------------------------------------------------
//: Conditionally propagate the labels of a sdet_region_edge.
//  In this approach we don't differentiate between Right
//  and Left.  Here we allow an unlimited number of labels
//  to be assigned to an edge, which is necessary due to
//  small regions which don't have proper closure but
//  whose edges are on the boundary of larger legitimate
//  regions.
void sdet_region_edge::Prop(sdet_region_edge_sptr const& re, unsigned int label,
                            unsigned int max_label)
{
  //First try to propagate the labels
  //Do not allow propagation to occur
  //between different edges
  //re is the old edge whose information is being propagated to *this
  vtol_edge_2d_sptr source = re->get_edge();
  vtol_edge_2d_sptr target = this->get_edge();

  //copy the labels from source to the new target
  if (source&&target&&*source==*target)
    for (unsigned int i = 0; i<re->NumLabels(max_label); ++i)
      this->SetNewLabel(re->GetLabel(i, max_label));
  //set the new label onto this
  this->SetNewLabel(label);
}

