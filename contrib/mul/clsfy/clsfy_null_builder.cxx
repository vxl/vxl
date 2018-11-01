// This is mul/clsfy/clsfy_null_builder.cxx
//:
// \file
// \brief Implement a null classifier builder
// \author Ian Scott
// \date 2009-08-27

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include "clsfy_null_builder.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <clsfy/clsfy_null_classifier.h>


//=======================================================================

std::string clsfy_null_builder::is_a() const
{
  return std::string("clsfy_null_builder");
}

//=======================================================================

bool clsfy_null_builder::is_class(std::string const& s) const
{
  return s == clsfy_null_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_null_builder::clone() const
{
  return new clsfy_null_builder(*this);
}

//=======================================================================

void clsfy_null_builder::print_summary(std::ostream& os) const
{
  os << "default_class: " << default_class_;
}

//=======================================================================

void clsfy_null_builder::b_write(vsl_b_ostream& bfs) const
{
  constexpr short version_no = 1;
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
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_null_builder&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_null_builder::build(clsfy_classifier_base& model,
                                 mbl_data_wrapper<vnl_vector<double> >&  /*inputs*/,
                                 unsigned /* nClasses */,
                                 const std::vector<unsigned> &outputs) const
{
  auto &classifier = dynamic_cast<clsfy_null_classifier&>(model);

  classifier.set_default_class(default_class_);

  return static_cast<double>(
    std::count(outputs.begin(), outputs.end(), default_class_) )
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
void clsfy_null_builder::config(std::istream &as)
{
 std::string s = mbl_parse_block(as);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  {
    default_class_ = vul_string_atoi(
      props.get_optional_property("default_class", "0") );
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
    "clsfy_null_builder::config", props, mbl_read_props_type());
}
