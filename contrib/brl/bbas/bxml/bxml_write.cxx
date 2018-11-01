// This is brl/bbas/bxml/bxml_write.cxx
//:
// \file
// \author Matt Leotta
// \date   October 5, 2006

#include <iostream>
#include <fstream>
#include "bxml_write.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Write the entire contents of \p filepath into an XML document class
void bxml_write(const std::string& filepath, const bxml_document& doc)
{
  std::ofstream file(filepath.c_str());
  bxml_write(file, doc);
}


//: Write the entire data stream \p is into an XML document class
void bxml_write(std::ostream& os, const bxml_document& doc)
{
  bxml_write_declaration(os, doc);
  bxml_write_data(os, doc.root_element());
}


//: Write the document declaration (header)
void bxml_write_declaration(std::ostream& os, const bxml_document& doc)
{
  os  << "<?xml version=\"" << doc.version() << '"'
      << " encoding=\"" << doc.encoding() << '"'
      << " standalone=\"" << (doc.standalone() ? "yes" : "no") << "\"?>\n";
}


//: Write the data (element or text)
void bxml_write_data(std::ostream& os, const bxml_data_sptr& data)
{
  if (!data)
    return;

  if (data->type() == bxml_data::TEXT) {
    auto* text = static_cast<bxml_text*>(data.ptr());
    os << text->data();
  }

  if (data->type() == bxml_data::ELEMENT) {
    auto* element = static_cast<bxml_element*>(data.ptr());
    bxml_write_element(os, *element);
  }
}


//: Write the XML element
void bxml_write_element(std::ostream& os, const bxml_element& element)
{
  // open the start tag
  os << '<' << element.name();

  // write the attributes
  for (auto i = element.attr_begin();
       i != element.attr_end();  ++i)
  {
    os << ' '<<i->first<<"=\""<<i->second<<'"';
  }

  // check if this should be an empty tag
  if (element.data_begin() == element.data_end()) {
    os << " />";
    return;
  }
  os << '>';

  // write the data within the tag
  for (auto i = element.data_begin();
       i != element.data_end();  ++i)
  {
    bxml_write_data(os, *i);
  }

  // end tag
  os << "</" << element.name() << '>';
}
