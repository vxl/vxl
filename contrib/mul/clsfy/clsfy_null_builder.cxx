// This is mul/clsfy/clsfy_null_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Implement a null classifier builder
// \author Ian Scott
// \date 2009-08-27

#include "clsfy_null_builder.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <clsfy/clsfy_null_classifier.h>


//=======================================================================

vcl_string clsfy_null_builder::is_a() const
{
  return vcl_string("clsfy_null_builder");
}

//=======================================================================

bool clsfy_null_builder::is_class(vcl_string const& s) const
{
  return s == clsfy_null_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_null_builder::clone() const
{
  return new clsfy_null_builder(*this);
}

//=======================================================================

void clsfy_null_builder::print_summary(vcl_ostream& os) const
{
  os << "default_class: " << default_class_;
}

//=======================================================================

void clsfy_null_builder::b_write(vsl_b_ostream& bfs) const
{
  const short version_no = 1;
  vsl_b_write(bfs, version_no);
  vsl_b_write(bfs, default_class_);
}

//=======================================================================

void clsfy_null_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case 1:
    vsl_b_read(bfs, default_class_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_null_builder&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_null_builder::build(clsfy_classifier_base& model,
                                 mbl_data_wrapper<vnl_vector<double> >& inputs,
                                 unsigned /* nClasses */,
                                 const vcl_vector<unsigned> &outputs) const
{
  clsfy_null_classifier &classifier = dynamic_cast<clsfy_null_classifier&>(model);

  classifier.set_default_class(default_class_);

  return static_cast<double>(
    vcl_count(outputs.begin(), outputs.end(), default_class_) )
    / outputs.size();
}

//=======================================================================
//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_null_builder::new_classifier() const
{
  return new clsfy_null_classifier();
}

//=======================================================================
//: Initialise the parameters from a text stream.
// The next non-ws character in the stream should be a '{'
// \verbatim
// {
//   default_class: 1  (default 0)
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void clsfy_null_builder::config(vcl_istream &as)
{
 vcl_string s = mbl_parse_block(as);

  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  {
    default_class_ = vul_string_atoi(
      props.get_optional_property("default_class", "0") );
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
    "clsfy_null_builder::config", props, mbl_read_props_type());
}
