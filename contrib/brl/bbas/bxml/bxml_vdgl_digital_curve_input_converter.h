#ifndef bxml_vdgl_digital_curve_input_converter_h
#define bxml_vdgl_digital_curve_input_converter_h
//-----------------------------------------------------------------------------
//:
// \file
// \brief A converter from TargetJr XML for a digital curve
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 09, 2002    Initial version.
//   Based on the TargetJr design by R. Kaucic
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_vector.h>
#include <bxml/bxml_input_converter.h>

class bxml_vdgl_digital_curve_input_converter : public bxml_input_converter
{
 public:
  bxml_vdgl_digital_curve_input_converter();
  virtual ~bxml_vdgl_digital_curve_input_converter();

  virtual bool extract_ref_object_atrs(DOM_Node& node);
  virtual bool extract_object_atrs(DOM_Node& node);
  virtual bool extract_from_dom(DOM_Node& node);

  // construct new object
  bxml_generic_ptr construct_object();

 protected:
  int new_or_ref;
  vcl_string id_;
  int n_points_;
  vcl_vector<float> xs_;
  vcl_vector<float> ys_;
};

#endif
