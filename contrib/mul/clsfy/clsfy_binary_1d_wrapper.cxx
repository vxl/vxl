// Copyright: (C) 2009 Imorphics PLC
#include "clsfy_binary_1d_wrapper.h"
//:
// \file
// \brief Wrap a classifier_1d in general classifier_base derivative.
// \author Ian Scott
// \date 2 Sep 2009

//=======================================================================

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vsl/vsl_binary_io.h>
#include <clsfy/clsfy_binary_threshold_1d.h>


//=======================================================================


clsfy_binary_1d_wrapper::clsfy_binary_1d_wrapper():
  classifier_1d_(new clsfy_binary_threshold_1d())
{}

vcl_string clsfy_binary_1d_wrapper::is_a() const
{
  return vcl_string("clsfy_binary_1d_wrapper");
}

//=======================================================================

bool clsfy_binary_1d_wrapper::is_class(vcl_string const& s) const
{
  return s == clsfy_binary_1d_wrapper::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

//:
// required if data is present in this class
void clsfy_binary_1d_wrapper::print_summary(vcl_ostream& os) const
{
  os << "underlying classifier: ";
  vsl_print_summary(os, classifier_1d_);
}

//=======================================================================

void clsfy_binary_1d_wrapper::b_write(vsl_b_ostream& bfs) const
{
  short version_no=1;
  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,classifier_1d_);
}

//=======================================================================

void clsfy_binary_1d_wrapper::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,classifier_1d_);
      break;
    default:
      vcl_cerr << "I/O ERROR: clsfy_binary_1d_wrapper::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

