#ifndef bxml_vtol_vertex_2d_input_converter_h
#define bxml_vtol_vertex_2d_input_converter_h

//-----------------------------------------------------------------------------
//:
// \file
// \brief A converter from TargetJr Vertex to vtol_vertex_2d vtol objects.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 08, 2002    Initial version.
//   Adapted from the original by R. Kaucic
// \endverbatim
//-----------------------------------------------------------------------------

#include <vsol/vsol_point_2d_sptr.h>
#include <bxml/bxml_input_converter.h>

class bxml_vtol_vertex_2d_input_converter : public bxml_input_converter
{
 public:
  bxml_vtol_vertex_2d_input_converter();
  virtual ~bxml_vtol_vertex_2d_input_converter();

  virtual bool extract_object_atrs(DOM_Node& node);
  virtual bool extract_from_dom(DOM_Node& node);

  virtual vcl_string get_id(DOM_Node& node);

  // construct new object
  bxml_generic_ptr construct_object();

 protected:
  int new_or_ref;
  vcl_string id_;
  vsol_point_2d_sptr pt_;
};

#endif
