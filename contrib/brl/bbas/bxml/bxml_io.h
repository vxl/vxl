//this-sets-emacs-to-*-c++-*-mode
#ifndef bxml_io_h_
#define bxml_io_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A xml process class for converting aggregates of objects
//        
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 8, 2002    Initial version.
//   Based on the original TargetJr design 
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_string.h>
#include <vcl_vector.h>
#include <dom/DOM.hpp>
#include <bxml/dll.h>
#include <bxml/bxml_generic_ptr.h>
#include <bxml/bxml_input_converter_sptr.h>
class bxml_io 
{
 public:
  bxml_io();
  ~bxml_io();

  // input methods
  static bxml_input_converter_sptr find_converter_from_tag(vcl_string tag_name);
  static bxml_input_converter_sptr find_input_converter(vcl_string class_name);
  static void register_input_converter(bxml_input_converter_sptr conv);

  static void register_input_converters();

  static bool get_xml_root(vcl_string fname,DOM_Element& root);
  static bool parse_xml(vcl_string fname,vcl_vector<bxml_generic_ptr>& objs);

 protected:
  static BXML_DLL_DATA int _current_object_index;

  //members
  static BXML_DLL_DATA vcl_vector<bxml_input_converter_sptr> _input_converters;
};

#endif // bxml_io_h_
