// This is core/vsl/vsl_basic_xml_element.h
#ifndef vsl_basic_xml_element_h_
#define vsl_basic_xml_element_h_
//:
// \file
// \brief  creates basic xml nodes and writes them out to the stream
// A basic node contains only text content, and optionally has attribute(s).
// \author Gamze Tunali (gamze@lems.brown.edu)
// \date   Dec 21, 2005

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <vxl_config.h>

template<typename T> std::string toString(const T& t)
{
  std::stringstream strm;

  strm << std::fixed << t;
  return strm.str();
}

class vsl_basic_xml_element
{
 public:
  //: constructs with a name
  vsl_basic_xml_element(std::string tag)
    : tag_(std::move(tag)) {}

  //: constructs with a name and a list of (attribute,value) pair
  vsl_basic_xml_element(std::string tag, std::vector<std::pair<std::string, std::string> > attrs)
    : tag_(std::move(tag)), attrs_(std::move(attrs)) {}

  //destructor
  ~vsl_basic_xml_element() = default;

  //: overloaded methods to add attribute values
  void add_attribute(std::string attr_name, std::string value);
  void add_attribute(std::string attr_name, double value);
  void add_attribute(std::string attr_name, float value) { add_attribute(attr_name, (double)value); }
  void add_attribute(std::string attr_name, long value);
  void add_attribute(std::string attr_name, int value) { add_attribute(attr_name, (long)value); }
  void add_attribute(std::string attr_name, size_t value) { add_attribute(attr_name, (long)value); }
#if VXL_ADDRESS_BITS != 32   //defined(_WIN32) && defined(_MSC_VER) && !defined(_WIN64) )  // Cannot have this overloading for MSVC or GCC 32-bit build as it collides with size_t
  void add_attribute(std::string attr_name, unsigned int value) { add_attribute(attr_name, (long)value); }
#endif
  void add_attribute_list(std::vector<std::pair<std::string, std::string> > attrs);
#if 0
  bool delete_attribute(std::string attr_name);
#endif
  void append_cdata(const std::string& cdata);
  void append_cdata(double cdata);
  void append_cdata(int cdata);

  void x_write(std::ostream& ostr);
  //: writes the opening tag for this node to the stream
  void x_write_open(std::ostream& ostr);

  //: writes the closing tag for this node to the stream
  void x_write_close(std::ostream& ostr);

 protected:
  std::string tag_; //!< the node name
  std::vector<std::pair<std::string, std::string> > attrs_; //!< the node attributes
  std::string cdata_; //!< the text() content of this node
};

#endif // vsl_basic_xml_element_h_
