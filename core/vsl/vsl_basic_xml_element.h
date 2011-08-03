// This is core/vsl/vsl_basic_xml_element.h
#ifndef vsl_basic_xml_element_h_
#define vsl_basic_xml_element_h_
//:
// \file
// \brief  creates basic xml nodes and writes them out to the stream
// A basic node contains only text content, and optionally has attribute(s).
// \author Gamze Tunali (gamze@lems.brown.edu)
// \date   Dec 21, 2005

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h> // for vcl_fixed

template<typename T> vcl_string toString(const T& t)
{
  vcl_stringstream strm;

  strm << vcl_fixed << t;
  return strm.str();
}

class vsl_basic_xml_element
{
 public:
  //: constructs with a name
  vsl_basic_xml_element(vcl_string tag)
    : tag_(tag) {}

  //: constructs with a name and a list of (attribute,value) pair
  vsl_basic_xml_element(vcl_string tag, vcl_vector<vcl_pair<vcl_string, vcl_string> > attrs)
    : tag_(tag), attrs_(attrs) {}

  //destructor
  ~vsl_basic_xml_element() {}

  //: overloaded methods to add attribute values
  void add_attribute(vcl_string attr_name, vcl_string value);
  void add_attribute(vcl_string attr_name, double value);
  void add_attribute(vcl_string attr_name, float value) { add_attribute(attr_name, (double)value); }
  void add_attribute(vcl_string attr_name, long value);
  void add_attribute(vcl_string attr_name, int value) { add_attribute(attr_name, (long)value); }
  void add_attribute(vcl_string attr_name, unsigned long value) { add_attribute(attr_name, (long)value); }
  void add_attribute(vcl_string attr_name, unsigned int value) { add_attribute(attr_name, (long)value); }

  void add_attribute_list(vcl_vector<vcl_pair<vcl_string, vcl_string> > attrs);
#if 0
  bool delete_attribute(vcl_string attr_name);
#endif
  void append_cdata(vcl_string cdata);
  void append_cdata(double cdata);
  void append_cdata(int cdata);

  void x_write(vcl_ostream& ostr);
  //: writes the opening tag for this node to the stream
  void x_write_open(vcl_ostream& ostr);

  //: writes the closing tag for this node to the stream
  void x_write_close(vcl_ostream& ostr);

 protected:
  vcl_string tag_; //!< the node name
  vcl_vector<vcl_pair<vcl_string, vcl_string> > attrs_; //!< the node attributes
  vcl_string cdata_; //!< the text() content of this node
};

#endif // vsl_basic_xml_element_h_
