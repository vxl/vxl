// This is brl/bbas/bxml/bxml_write.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   October 5, 2006

#include "bxml_write.h"
#include <vcl_fstream.h>


//: Write the entire contents of \p filepath into an XML document class
void bxml_write(const vcl_string& filepath, const bxml_document& doc)
{
  vcl_ofstream file(filepath.c_str());
  bxml_write(file, doc);
}


//: Write the entire data stream \p is into an XML document class
void bxml_write(vcl_ostream& os, const bxml_document& doc)
{
  bxml_write_declaration(os, doc);
  bxml_write_data(os, doc.root_element());
}


//: Write the document declaration (header)
void bxml_write_declaration(vcl_ostream& os, const bxml_document& doc)
{
  os  << "<?xml version=\"" << doc.version() << '"'
      << " encoding=\"" << doc.encoding() << '"'
      << " standalone=\"" << (doc.standalone() ? "yes" : "no") << "\"?>\n";
}


//: Write the data (element or text)
void bxml_write_data(vcl_ostream& os, const bxml_data_sptr& data)
{
  if (!data)
    return;

  if (data->type() == bxml_data::TEXT) {
    bxml_text* text = static_cast<bxml_text*>(data.ptr());
    os << text->data();
  }

  if (data->type() == bxml_data::ELEMENT) {
    bxml_element* element = static_cast<bxml_element*>(data.ptr());
    bxml_write_element(os, *element);
  }
}


//: Write the XML element
void bxml_write_element(vcl_ostream& os, const bxml_element& element)
{
  // open the start tag
  os << '<' << element.name();

  // write the attributes
  for (bxml_element::const_attr_iterator i = element.attr_begin();
       i != element.attr_end();  ++i)
  {
    os << ' '<<i->first<<"=\""<<i->second<<'"';
  }

  // check if this should be an emtpy tag
  if (element.data_begin() == element.data_end()) {
    os << " />";
    return;
  }
  os << '>';

  // write the data within the tag
  for (bxml_element::const_data_iterator i = element.data_begin();
       i != element.data_end();  ++i)
  {
    bxml_write_data(os, *i);
  }

  // end tag
  os << "</" << element.name() << '>';
}
