//this-sets-emacs-to-*-c++-*-mode
#ifndef bxml_vsol_point_2d_input_converter_h_
#define bxml_vsol_point_2d_input_converter_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A converter for reading a TargetJR XML version of 
//        vsol_point_2d from an XML DOM Tree
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 07, 2002    Initial version.
//   Based on the TargetJr design by R. Kaucic
// \endverbatim
//--------------------------------------------------------------------------------
#include <bxml/bxml_input_converter.h>
#include <vsol/vsol_point_2d.h>

class DOM_Node;

class bxml_vsol_point_2d_input_converter : public bxml_input_converter
{
 public:
  bxml_vsol_point_2d_input_converter();
  ~bxml_vsol_point_2d_input_converter();
  vcl_string& id(){return _id;}
  //: virtual methods
  virtual bool extract_ref_object_atrs(DOM_Node& node);
  virtual bool extract_object_atrs(DOM_Node& node);
  virtual bool extract_from_dom(DOM_Node& node);

  //: construct a new object instance
virtual bxml_generic_ptr construct_object();

 protected:



 private:
  //utility functions

  //members
  int new_or_ref;
  vcl_string _id;
  double _x;
  double _y;
};

#endif // bxml_vsol_point_2d_input_converter_h_
