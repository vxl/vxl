#include "bxml_io.h"
#include <bxml/bxml_input_converter.h>
#include <bxml/bxml_vsol_point_2d_input_converter.h>
#include <bxml/bxml_vdgl_digital_curve_input_converter.h>
#include <bxml/bxml_vtol_vertex_2d_input_converter.h>
#include <bxml/bxml_vtol_zero_chain_2d_input_converter.h>
#include <bxml/bxml_vtol_edge_2d_input_converter.h>
#include <vcl_iostream.h>

// From XERCES:
#if !VCL_HAS_EXCEPTIONS
#include <util/XMLException.hpp>
#undef ThrowXML
#define ThrowXML(type,code) /* empty */
#endif
#include <util/PlatformUtils.hpp>
#include <parsers/DOMParser.hpp>
#include <dom/DOM_Document.hpp>
#include <dom/DOM_Element.hpp>

int bxml_io::current_object_index_ = 1;
vcl_vector<bxml_input_converter_sptr> bxml_io::input_converters_;

bxml_io::bxml_io()
{
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

bxml_input_converter_sptr bxml_io::find_input_converter(vcl_string class_name)
{
  for (vcl_vector<bxml_input_converter_sptr>::iterator cit=input_converters_.begin();
       cit != input_converters_.end(); cit++) {
    vcl_string conv_name = (*cit)->get_class_name();
    if (conv_name == class_name)
      return *cit;
  }
  return 0;
}

bxml_input_converter_sptr bxml_io::find_converter_from_tag(vcl_string tag_name)
{
  for (vcl_vector<bxml_input_converter_sptr>::iterator cit=input_converters_.begin();
       cit != input_converters_.end(); cit++) {
    vcl_string tname = (*cit)->get_tag_name();
    vcl_string ref_tname = (*cit)->get_ref_tag_name();
    if ( (tname == tag_name) || (ref_tname == tag_name) )
      return *cit;
  }
  return 0;
}

void bxml_io::register_input_converter(bxml_input_converter_sptr conv)
{
  if (!conv) {
    vcl_cout << "Can not register null converter\n";
    return;
  }

  //see if converter already exists
  vcl_string class_name = conv->get_class_name();
  if (bxml_io::find_input_converter(class_name))
   return;

  //if not found then add it
#ifdef DEBUG
  vcl_cout << "registering " << conv->get_class_name() << " converter\n";
#endif
  input_converters_.push_back(conv);
}

bool bxml_io::get_xml_root(vcl_string fname,DOM_Element& root)
{
  DOMParser *parser = new DOMParser;
  //  ErrorHandler *errReporter = new xml_error_handler();
  //  parser->setErrorHandler(errReporter);
  parser->setToCreateXMLDeclTypeNode(true);
#ifdef DEBUG
  vcl_cout << "parsing file " << fname << vcl_endl;
#endif
  parser->parse(fname.c_str());
#ifdef DEBUG
  vcl_cout << "done parsing file " << fname << vcl_endl;
#endif
  DOM_Node doc = parser->getDocument();
  delete parser;
  //  delete errReporter;

#ifdef DEBUG
  vcl_cout << "node_name=" << doc.getNodeName().transcode() << vcl_endl;
#endif
  int node_type = doc.getNodeType();
  if (node_type != DOM_Node::DOCUMENT_NODE) {
    vcl_cout << "Error: node_type=" << node_type << vcl_endl;
    return false;
  }

  root = ((DOM_Document*)&doc)->getDocumentElement();
  if (root==0)
  {
    vcl_cout << "Error: document has null root\n";
    return false;
  }
#ifdef DEBUG
  vcl_cout << "root_name=" << root.getTagName().transcode() << vcl_endl;
#endif
  return true;
}

bool bxml_io::parse_xml(vcl_string fname,vcl_vector<bxml_generic_ptr>& objs)
{
  XMLPlatformUtils::Initialize();

  DOM_Element root;
  bool good = bxml_io::get_xml_root(fname,root);

  if (!good) {
    XMLPlatformUtils::Terminate();
    return false;
  }

  bxml_input_converter_sptr conv;

  DOM_Node child = root.getFirstChild();
  while ( child != 0 )
  {
    int cnode_type = child.getNodeType();

    if (cnode_type == DOM_Node::ELEMENT_NODE)
    {
      vcl_string ctname = ((DOM_Element*)&child)->getTagName().transcode();
      conv = find_converter_from_tag(ctname);
      if (conv)
      {
#ifdef DEBUG
        vcl_cout << "calling " << conv->get_class_name() << vcl_endl;
#endif
        if (conv->extract_from_dom(child)) {
          bxml_generic_ptr gp = conv->construct_object();
          objs.push_back(gp);
        }
        else
          vcl_cout << "Error: unable to parse tag =" << ctname << vcl_endl;
      }
      else
        vcl_cout << "Unrecognized tag =" << ctname << vcl_endl;
    }
    child = child.getNextSibling();
  }
  XMLPlatformUtils::Terminate();
  return true;
}

void bxml_io::register_input_converters()
{
  bxml_io::register_input_converter(new bxml_vsol_point_2d_input_converter());
  bxml_io::register_input_converter(new bxml_vdgl_digital_curve_input_converter());
  bxml_io::register_input_converter(new bxml_vtol_vertex_2d_input_converter());
  bxml_io::register_input_converter(new bxml_vtol_zero_chain_2d_input_converter());
  bxml_io::register_input_converter(new bxml_vtol_edge_2d_input_converter());
}
