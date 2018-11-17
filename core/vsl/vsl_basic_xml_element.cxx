// This is core/vsl/vsl_basic_xml_element.cxx
#include "vsl_basic_xml_element.h"
//:
// \file

void vsl_basic_xml_element::add_attribute_list(std::vector<std::pair<std::string, std::string> > attrs)
{
  for (const auto & attr : attrs) {
    attrs_.push_back(attr);
  }
}

void vsl_basic_xml_element::add_attribute(std::string attr_name, std::string value)
{
  std::pair<std::string, std::string> attr(attr_name, value);
  attrs_.push_back(attr);
}

void vsl_basic_xml_element::add_attribute(std::string attr_name, double value)
{
  std::string value_str = toString(value);
  std::pair<std::string, std::string> attr(attr_name, value_str.data());
  attrs_.push_back(attr);
}

void vsl_basic_xml_element::add_attribute(std::string attr_name, long value)
{
  std::string value_str = toString(value);
  std::pair<std::string, std::string> attr(attr_name, value_str);
  attrs_.push_back(attr);
}

void vsl_basic_xml_element::append_cdata(const std::string& cdata)
{
  if (cdata_.size() > 0)
    cdata_.append(" ");
  cdata_.append(cdata);
}

void vsl_basic_xml_element::append_cdata(double cdata)
{
  if (cdata_.size() > 0)
    cdata_.append(" ");
  cdata_.append(toString(cdata));
}

void vsl_basic_xml_element::append_cdata(int cdata)
{
  if (cdata_.size() > 0)
    cdata_.append(" ");
  cdata_.append(toString(cdata));
}

#if 0
bool vsl_basic_xml_element::delete_attribute(std::string /*attr_name*/)
{
  std::cerr << "vsl_basic_xml_element::delete_attribute() not yet implemented\n";
  return false;
}
#endif

void vsl_basic_xml_element::x_write(std::ostream& ostr)
{
  // put the initial bracket with element name and the attribute-value list
  x_write_open(ostr);

  // put the character data between the tags
  if (cdata_.size() > 0)
    ostr << cdata_ << '\n';

  // close the element
  x_write_close(ostr);
}

void vsl_basic_xml_element::x_write_open(std::ostream& ostr)
{
  ostr << '<' << tag_;
  for (auto & attr : attrs_) {
    ostr << ' ' << attr.first << "=\"" << attr.second << '"';
  }
  ostr << ">\n";
}

//: writes the closing tag to the stream
void vsl_basic_xml_element::x_write_close(std::ostream& ostr)
{
  ostr << "</" << tag_ << ">\n";
}
