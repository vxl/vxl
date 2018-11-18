// This is brl/bbas/bxml/bxml_document.cxx
//:
// \file
// \author Matt Leotta
// \date   October 5, 2006

#include "bxml_document.h"


//: Return the value of an attribute
std::string
bxml_element::attribute(const std::string& attr_name) const
{
  auto result = attributes_.find(attr_name);
  if (result != attributes_.end())
    return result->second;
  return "";
}


//: Constructor - default
bxml_document::bxml_document()
  : root_element_(nullptr),
    version_("1.0"),
    encoding_("UTF-8"),
    standalone_(true)
{
}


//: Append text in this element
void bxml_element::append_text(const std::string& text)
{
  bxml_text * last_text = nullptr;
  if (!data_.empty()){
    bxml_data_sptr last_data = data_.back();
    if (last_data->type() == bxml_data::TEXT)
      last_text = static_cast<bxml_text*>(last_data.ptr());
  }

  if (last_text)
    last_text->set_data(last_text->data() + text);
  else
    data_.push_back(new bxml_text(text));
}


//: compare two XML data objects
bool operator==(const bxml_data& d1, const bxml_data& d2)
{
  if (d1.type() != d2.type())
    return false;

  if (d1.type() == bxml_data::TEXT){
    const auto& text1 = static_cast<const bxml_text&>(d1);
    const auto& text2 = static_cast<const bxml_text&>(d2);
    return text1.data() == text2.data();
  }

  if (d1.type() == bxml_data::ELEMENT){
    const auto& element1 = static_cast<const bxml_element&>(d1);
    const auto& element2 = static_cast<const bxml_element&>(d2);
    return element1 == element2;
  }

  return false;
}


//: compare two XML element objects
bool operator==(const bxml_element& e1, const bxml_element& e2)
{
  if (e1.name() != e2.name())
    return false;

  if (e1.num_attributes() != e2.num_attributes())
    return false;

  auto a1 = e1.attr_begin();
  auto a2 = e2.attr_begin();
  for (; a1 != e1.attr_end();  ++a1, ++a2)
  {
    if (a1->first != a2->first || a1->second != a2->second)
      return false;
  }

  if (e1.num_data() != e2.num_data())
    return false;

  auto d1 = e1.data_begin();
  auto d2 = e2.data_begin();
  for (; d1 != e1.data_end();  ++d1, ++d2)
  {
    if (!(**d1 == **d2))
      return false;
  }
  return true;
}


//: compare two XML documents
bool operator==(const bxml_document& d1, const bxml_document& d2)
{
  if (d1.version() != d2.version() ||
      d1.encoding() != d2.encoding() ||
      d1.standalone() != d2.standalone())
    return false;

  return *d1.root_element() == *d2.root_element();
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use bxml_document as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, bxml_document const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use bxml_document as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & /*is*/, bxml_document & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use bxml_document as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, bxml_document* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new bxml_document();
    vsl_b_read(is, *ph);
  }
  else
    ph = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bxml_document* &ph)
{
  if (ph==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
