// This is oxl/mvl/TriTensorCompute.cxx
//:
// \file
// Ported from TargetJr's MViewCompute by Peter Vanroose, 17 June 2002.

#include "TriTensorCompute.h"
#include <mvl/TriTensor.h>

// Constructor.
BaseTriTensorCompute::BaseTriTensorCompute ()
{
  // zero length list.
  matchlineseg_list_ptr_ = new std::list<HomgMatchLineSeg2D2D2D*> ();

  // zero length list.
  matchpoint_list_ptr_ = new std::list<HomgMatchPoint2D2D2D*> ();
}

// Destructor.
BaseTriTensorCompute::~BaseTriTensorCompute() = default;

//-----------------------------------------------------------------------------
//
//: append match_list to the list of matched linesegments

void
BaseTriTensorCompute::add_matches (std::list<HomgMatchLineSeg2D2D2D*> *match_list)
{
  delete matchlineseg_list_ptr_;
  matchlineseg_list_ptr_ = match_list;
}

//-----------------------------------------------------------------------------
//
//: append match_list to the list of matched points

void
BaseTriTensorCompute::add_matches (std::list<HomgMatchPoint2D2D2D*> *match_list)
{
  delete matchpoint_list_ptr_;
  matchpoint_list_ptr_ = match_list;
}


//-----------------------------------------------------------------------------
//
//: clear all entries from the list of matched linesegments
void
BaseTriTensorCompute::clear_matches_line (void)
{
  delete matchlineseg_list_ptr_;
}

//-----------------------------------------------------------------------------
//
//: clear all entries from the list of matched points
void
BaseTriTensorCompute::clear_matches_point (void)
{
  delete matchpoint_list_ptr_;
}
