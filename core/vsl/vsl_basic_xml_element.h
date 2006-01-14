// This is core/vsl/vsl_basic_xml_element.h
#ifndef vsl_basic_xml_element_h_
#define vsl_basic_xml_element_h_
//:
// \file
// \brief  creates basic xml nodes and writes them out to the stream
// \author Gamze Tunali (gamze@lems.brown.edu)
// \date   12/21/2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>

template<typename T> vcl_string toString(const T& t)
{
  vcl_stringstream strm;

  strm << fixed << t;
  vcl_string str(strm.str());
  return str;
}

class vsl_basic_xml_element
{
 public:
  //: constructs with a name
  vsl_basic_xml_element(vcl_string tag)
    : tag_(tag) {}

  //: constructs with a name and a list of (attibute,value) pair
  vsl_basic_xml_element(vcl_string tag, vcl_vector<vcl_pair<vcl_string, vcl_string> > attrs)
    : tag_(tag), attrs_(attrs) {}

  //destructor
  ~vsl_basic_xml_element() {}

  //: overloaded add_attribute methods to add different types of values as string
  void add_attribute(vcl_string attr_name, double value);
  void add_attribute(vcl_string attr_name, int value);
  void add_attribute(vcl_string attr_name, vcl_string value);
#if 0
  void add_attribute(vcl_pair<vcl_string, vcl_string> attr);
#endif

  void add_attribute_list(vcl_vector<vcl_pair<vcl_string, vcl_string> > attrs);

  bool delete_attribute(vcl_string attr_name);

  void append_cdata(vcl_string cdata);
  void append_cdata(double cdata);
  void append_cdata(int cdata);

  void x_write(vcl_ostream& ostr);
  //: writes the opening tag to the stream
  void x_write_open(vcl_ostream& ostr);

  //: writes the closing tag to the stream
  void x_write_close(vcl_ostream& ostr); 

 protected:
  vcl_string tag_;
  vcl_vector<vcl_pair<vcl_string, vcl_string> > attrs_;
  vcl_string cdata_;
};

#endif // vsl_basic_xml_element_h_
