// This is contrib/brl/bbas/bxml/bxml_document.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   October 5, 2006

#include "bxml_document.h"



//: Return the value of an attribute
vcl_string
bxml_element::attribute(const vcl_string& attr_name) const
{
  vcl_map<vcl_string,vcl_string>::const_iterator result = attributes_.find(attr_name);
  if(result != attributes_.end())
    return result->second;
  return "";
}


//: Constructor - default
bxml_document::bxml_document()
  : root_element_(NULL),
    version_("1.0"),
    encoding_("UTF-8"),
    standalone_(true)
{
}


//: Append text in this element
void bxml_element::append_text(const vcl_string& text)
{
  bxml_text * last_text = NULL;
  if(!data_.empty()){
    bxml_data_sptr last_data = data_.back();
    if(last_data->type() == bxml_data::TEXT)
      last_text = static_cast<bxml_text*>(last_data.ptr());
  }

  if(last_text)
    last_text->set_data(last_text->data() + text);
  else
    data_.push_back(new bxml_text(text));
}


//: compare two XML data objects
bool operator==(const bxml_data& d1, const bxml_data& d2)
{
  if(d1.type() != d2.type())
    return false;

  if(d1.type() == bxml_data::TEXT){
    const bxml_text& text1 = static_cast<const bxml_text&>(d1);
    const bxml_text& text2 = static_cast<const bxml_text&>(d2);
    return text1.data() == text2.data();
  }

  if(d1.type() == bxml_data::ELEMENT){
    const bxml_element& element1 = static_cast<const bxml_element&>(d1);
    const bxml_element& element2 = static_cast<const bxml_element&>(d2);
    return element1 == element2;
  }

  return false;
}


//: compare two XML element objects
bool operator==(const bxml_element& e1, const bxml_element& e2)
{
  if(e1.name() != e2.name())
    return false;

  if(e1.num_attributes() != e2.num_attributes())
    return false;

  bxml_element::const_attr_iterator a1 = e1.attr_begin();
  bxml_element::const_attr_iterator a2 = e2.attr_begin();
  for(; a1 != e1.attr_end();  ++a1, ++a2)
  {
    if(a1->first != a2->first || a1->second != a2->second)
      return false;
  }

  if(e1.num_data() != e2.num_data())
    return false;

  bxml_element::const_data_iterator d1 = e1.data_begin();
  bxml_element::const_data_iterator d2 = e2.data_begin();
  for(; d1 != e1.data_end();  ++d1, ++d2)
  {
    if(!(**d1 == **d2))
      return false;
  }
  return true;
}


//: compare two XML documents
bool operator==(const bxml_document& d1, const bxml_document& d2)
{
  if(d1.version() != d2.version() ||
     d1.encoding() != d2.encoding() ||
     d1.standalone() != d2.standalone())
    return false;

  return *d1.root_element() == *d2.root_element();
}
