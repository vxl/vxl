// This is core/vsl/vsl_basic_xml_element.cxx
#include "vsl_basic_xml_element.h"
//:
// \file

void vsl_basic_xml_element::add_attribute_list(vcl_vector<vcl_pair<vcl_string, vcl_string> > attrs)
{
  for (unsigned int i=0; i<attrs.size(); i++) {
    attrs_.push_back(attrs[i]);
  }
}

void vsl_basic_xml_element::add_attribute(vcl_string attr_name, double value)
{
  vcl_string value_str = toString(value);
  vcl_pair<vcl_string, vcl_string> attr(attr_name, value_str.data());
  attrs_.push_back(attr);
}

void vsl_basic_xml_element::add_attribute(vcl_string attr_name, int value)
{
  vcl_string value_str = toString(value); //"" + value;
  vcl_pair<vcl_string, vcl_string> attr(attr_name, value_str);
  attrs_.push_back(attr);
}

void vsl_basic_xml_element::add_attribute(vcl_string attr_name, vcl_string value)
{
  vcl_pair<vcl_string, vcl_string> attr(attr_name, value);
  attrs_.push_back(attr);
}

void vsl_basic_xml_element::append_cdata(vcl_string cdata)
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

bool vsl_basic_xml_element::delete_attribute(vcl_string attr_name)
{
  return false;
}

void vsl_basic_xml_element::x_write(vcl_ostream& ostr)
{
  // put the initial bracket with element name and the attribute-value list
  x_write_open(ostr);

  // put the character data between the tags
  if (cdata_.size() > 0)
    ostr << cdata_ << '\n';

  // close the element
  x_write_close(ostr);
}

void vsl_basic_xml_element::x_write_open(vcl_ostream& ostr)
{
  ostr << '<' << tag_;
  for (unsigned int i=0; i<attrs_.size(); i++) {
    ostr << ' ' << attrs_[i].first << "=\"" << attrs_[i].second << '"';
  }
  ostr << ">\n";
}

//: writes the closing tag to the stream
void vsl_basic_xml_element::x_write_close(vcl_ostream& ostr)
{
  ostr << "</" << tag_ << ">\n";
}
