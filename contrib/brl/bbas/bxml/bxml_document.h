// This is brl/bbas/bxml/bxml_document.h
#ifndef bxml_document_h_
#define bxml_document_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief An XML document representation
// \author Matt Leotta (Brown)
// \date   October 5, 2006
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>


//: A block of character data found within XML tags
// This is a base class and can be either plain text or
// an XML element
class bxml_data : public vbl_ref_count
{
 public:
  enum datatype {TEXT, ELEMENT};

  virtual ~bxml_data() {}

  //: Return the type of XML data
  virtual datatype type() const = 0;
};

//: compare two XML data objects
bool operator==(const bxml_data& d1, const bxml_data& d2);


typedef vbl_smart_ptr<bxml_data> bxml_data_sptr;


//: text data within XML
class bxml_text : public bxml_data
{
 public:
  //: Constructor
  bxml_text(const vcl_string& data) : data_(data) {}

  //: Destructor
  virtual ~bxml_text() {}

  //: Return the type of XML data
  datatype type() const { return TEXT; }

  //: Access the text data
  vcl_string data() const { return data_; }

  //: Set the text data
  void set_data(const vcl_string& data) { data_ = data; }

 private:
  vcl_string data_;
};


//: An XML element
class bxml_element : public bxml_data
{
 public:
  typedef vcl_vector<bxml_data_sptr>::const_iterator const_data_iterator;
  typedef vcl_map<vcl_string,vcl_string>::const_iterator const_attr_iterator;

  //: Constructor - default
  bxml_element() {}

  //: Constructor
  bxml_element(const vcl_string& name) : name_(name) {}

  //: Destructor
  virtual ~bxml_element() {}

  //: Return the type of XML data
  datatype type() const { return ELEMENT; }

  //: Return the name of the element
  vcl_string name() const { return name_; }

  //: Return the value of an attribute
  vcl_string attribute(const vcl_string& attr_name) const;

  //: Return the value of an attribute
  template <class T>
  bool get_attribute(const vcl_string& attr_name, T& value) const
  {
    vcl_stringstream s(this->attribute(attr_name));
    if (s.str() == "")
      return false;
    s >> value;
    return true;
  }

  //: Return the number of attributes
  unsigned int num_attributes() const { return attributes_.size(); }

  //: An iterator to the beginning of the attributes
  const_attr_iterator attr_begin() const { return attributes_.begin(); }

  //: An iterator to the end of the attributes
  const_attr_iterator attr_end() const { return attributes_.end(); }

  //: Return the number of data nodes
  unsigned int num_data() const { return data_.size(); }

  //: An iterator to the beginning of the data
  const_data_iterator data_begin() const { return data_.begin(); }

  //: An iterator to the end of the data
  const_data_iterator data_end() const { return data_.end(); }

  //: Append text in this element
  void append_text(const vcl_string& text);

  //: Append data (typically another element) in this element
  void append_data(const bxml_data_sptr& el)
  { data_.push_back(el); }

  void set_attribute(const vcl_string& attr_name, const vcl_string& attr_value)
  { attributes_[attr_name] = attr_value; }

  template <class T>
  void set_attribute(const vcl_string& attr_name, const T& attr_value)
  {
    vcl_stringstream s;
    s << attr_value;
    attributes_[attr_name] = s.str();
  }

 private:
  //: The name of the element
  vcl_string name_;

  //: The map of attributes to values
  vcl_map<vcl_string,vcl_string> attributes_;

  //: The character data.
  vcl_vector<bxml_data_sptr> data_;
};


//: compare two XML element objects
bool operator==(const bxml_element& e1, const bxml_element& e2);


//: Represents a full XML document stored as a tree
class bxml_document
{
 public:
  //: Constructor - default
  bxml_document();

  //: Destructor
  ~bxml_document(){}

  //: Return the root element
  bxml_data_sptr root_element() const {return root_element_;}

  //: Return the version string
  vcl_string version() const { return version_; }

  //: Return the encoding string
  vcl_string encoding() const { return encoding_; }

  //: Return the standalone bit
  bool standalone() const { return standalone_; }

  //: set the root element
  void set_root_element(const bxml_data_sptr& root)
  { root_element_ = root; }

  //: Set the version string
  void set_version(const vcl_string& version) { version_ = version; }

  //: Set the encoding string
  void set_encoding(const vcl_string& encoding) { encoding_ = encoding; }

  //: Set the standalone bit
  void set_standalone(bool standalone) { standalone_ = standalone; }

 private:
  //: The root element
  bxml_data_sptr root_element_;

  vcl_string version_;
  vcl_string encoding_;
  bool standalone_;
};


//: compare two XML documents
bool operator==(const bxml_document& d1, const bxml_document& d2);


#endif // bxml_document_h_
