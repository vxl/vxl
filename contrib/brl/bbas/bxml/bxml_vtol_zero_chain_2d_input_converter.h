#ifndef bxml_vtol_zero_chain_2d_input_converter_h
#define bxml_vtol_zero_chain_2d_input_converter_h
//-----------------------------------------------------------------------------
//:
// \file
// \brief A converter from TargetJr ZeroChain to vtol_zero_chain_2d
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 08, 2002    Initial version.
//   Adapted from the original by R. Kaucic
// \endverbatim
//-----------------------------------------------------------------------------

#include <vtol/vtol_vertex_sptr.h>
#include <bxml/bxml_input_converter.h>
#include <vcl_vector.h>

class bxml_vtol_zero_chain_2d_input_converter : public bxml_input_converter
{
 public:
  bxml_vtol_zero_chain_2d_input_converter();
  virtual ~bxml_vtol_zero_chain_2d_input_converter();

  virtual bool extract_object_atrs(DOM_Node& node);
  virtual bool extract_from_dom(DOM_Node& node);

  virtual vcl_string get_id(DOM_Node& node);

  // construct new object
  bxml_generic_ptr construct_object();

 protected:
  int new_or_ref;
  vcl_string id_;
  vcl_vector<vtol_vertex_sptr> verts_;
};

#endif
