#ifndef bxml_input_converter_h_
#define bxml_input_converter_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief The base class for xml input conversion.
//
// The current clear operation explicitly uses vsol_spatial_object_2d
// class methods.  These clear operations should be lower in the
// class hierarchies.  When more class roots are needed the clear should
// be abstracted.
//
// \verbatim
//  Initial version December 07, 2002
//  Based on the TargetJr design by R. Kaucic
// \endverbatim
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <dom/DOM_Document.hpp>
#include <bxml/bxml_generic_ptr.h>
#include <bxml/dll.h>

class bxml_input_converter : public vbl_ref_count
{
 public:
  bxml_input_converter();
  bxml_input_converter(bxml_input_converter const& x)
    : vbl_ref_count(), debug_(x.debug_), null_id_(x.null_id_),
      class_name_(x.class_name_), tag_name_(x.tag_name_),
      ref_tag_name_(x.ref_tag_name_) {}
  ~bxml_input_converter();

  //:specific sub_class string names
  vcl_string get_tag_name() {return tag_name_;}
  vcl_string get_class_name() {return class_name_;}
  vcl_string get_ref_tag_name() {return ref_tag_name_;}

  //:helper functions
  static vcl_string get_DOM_tag(DOM_Node& node);
  int check_tag(DOM_Node& node);
  bool getNextElementSibling(DOM_Node& node);
  DOM_Node getChild(DOM_Node& node,vcl_string tname);


  //:attribute access and conversion
  bool has_attr(DOM_Node& node, vcl_string attr_name);
  vcl_string get_string_attr(DOM_Node& node,vcl_string attr_name);
  int    get_int_attr(DOM_Node& node,vcl_string attr_name);
  double get_double_attr(DOM_Node& node,vcl_string attr_name);
  bool   get_bool_attr(DOM_Node& node,vcl_string attr_name);

  //:basic conversion methods
  virtual bool extract_ref_object_atrs(DOM_Node& /*node*/){return false;}
  virtual bool extract_object_atrs(DOM_Node& /*node*/){return false;}
  virtual bool extract_from_dom(DOM_Node& /*node*/) = 0;
  virtual bxml_generic_ptr construct_object()=0;
  //:utilities
  void set_debug(bool debug){debug_=debug;}
  //:clear spatial object instances in the table. move lower in hierarchy JLM
  static void clear();
 protected:
  bool debug_;
  vcl_string  null_id_;
  static BXML_DLL_DATA vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> >  obj_table_;
  vcl_string class_name_;
  vcl_string tag_name_;
  vcl_string ref_tag_name_;
};

#endif
