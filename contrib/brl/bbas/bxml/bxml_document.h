// This is brl/bbas/bxml/bxml_document.h
#ifndef bxml_document_h_
#define bxml_document_h_
//:
// \file
// \brief An XML document representation
// \author Matt Leotta (Brown)
// \date   October 5, 2006
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli (Brown) - July 15, 2009 - enabled smart pointer for bxml_document
//   Ricardo Fabbri (Brown) - October 18, 2009 - specialized get_attribute for strings
// \endverbatim

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

//: A block of character data found within XML tags
// This is a base class and can be either plain text or
// an XML element
class bxml_data : public vbl_ref_count
{
 public:
  enum datatype {TEXT, ELEMENT};

  ~bxml_data() override = default;

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
  bxml_text(std::string  data) : data_(std::move(data)) {}

  //: Destructor
  ~bxml_text() override = default;

  //: Return the type of XML data
  datatype type() const override { return TEXT; }

  //: Access the text data
  std::string data() const { return data_; }

  //: Set the text data
  void set_data(const std::string& data) { data_ = data; }

 private:
  std::string data_;
};


//: An XML element
class bxml_element : public bxml_data
{
 public:
  typedef std::vector<bxml_data_sptr>::const_iterator const_data_iterator;
  typedef std::map<std::string,std::string>::const_iterator const_attr_iterator;

  //: Constructor - default
  bxml_element() = default;

  //: Constructor
  bxml_element(std::string  name) : name_(std::move(name)) {}

  //: Destructor
  ~bxml_element() override = default;

  //: Return the type of XML data
  datatype type() const override { return ELEMENT; }

  //: Return the name of the element
  std::string name() const { return name_; }

  //: Return the value of an attribute
  std::string attribute(const std::string& attr_name) const;

  //: Specialization for std::string.
  bool get_attribute(const std::string& attr_name, std::string& value) const
  {
    value = this->attribute(attr_name);
    return true;
  }

  //: Return the value of an attribute.
  // \see specialization for std::string.
  template <class T>
  bool get_attribute(const std::string& attr_name, T& value) const
  {
    std::stringstream s(this->attribute(attr_name));
    if (s.str() == "")
      return false;
    s >> value;
    return true;
  }


  //: Return the values of all attributes with a given name
  std::vector<std::string> attributes(const std::string& attr_name) const;

  //: Specialization for std::string.
  bool get_attributea(const std::string& attr_name, std::vector<std::string>& values) const
  {
    values = this->attributes(attr_name);
    return true;
  }

  //: Return the value of an attribute.
  // \see specialization for std::string.
  template <class T>
  bool get_attributes(const std::string& attr_name, std::vector<T>& values) const
  {
    values.clear();
    std::vector<std::string> values_str = attributes(attr_name);
    for (const auto & vi : values_str) {
      std::stringstream s(vi);
      if (s.str() == "")
        return false;
      T value_t;
      s >> value_t;
      values.push_back(value_t);

    }
    return true;
  }

  //: Return the number of attributes
  unsigned int num_attributes() const { return (unsigned int)attributes_.size(); }

  //: An iterator to the beginning of the attributes
  const_attr_iterator attr_begin() const { return attributes_.begin(); }

  //: An iterator to the end of the attributes
  const_attr_iterator attr_end() const { return attributes_.end(); }

  //: Return the number of data nodes
  unsigned int num_data() const { return (unsigned int)data_.size(); }

  //: An iterator to the beginning of the data
  const_data_iterator data_begin() const { return data_.begin(); }

  //: An iterator to the end of the data
  const_data_iterator data_end() const { return data_.end(); }

  //: Append text in this element
  void append_text(const std::string& text);

  //: Append data (typically another element) in this element
  void append_data(const bxml_data_sptr& el)
  { data_.push_back(el); }

  //: Set attribute with and optional precision.
  template <class T>
  void set_attribute(const std::string& attr_name, const T& attr_value, unsigned p = 5)
  {
    std::stringstream s;
    s.precision(p);
    s << attr_value;
    attributes_[attr_name] = s.str();
  }
  //: Specialization for std::string below.
  void set_attribute(const std::string& attr_name, const std::string& attr_value)
  { attributes_[attr_name] = attr_value; }

 private:
  //: The name of the element
  std::string name_;

  //: The map of attributes to values
  std::map<std::string,std::string> attributes_;

  //: The character data.
  std::vector<bxml_data_sptr> data_;
};


//: compare two XML element objects
bool operator==(const bxml_element& e1, const bxml_element& e2);


//: Represents a full XML document stored as a tree
class bxml_document : public vbl_ref_count
{
 public:
  //: Constructor - default
  bxml_document();

  //: Destructor
  ~bxml_document() override= default;

  //: Return the root element
  bxml_data_sptr root_element() const {return root_element_;}

  //: Return the version string
  std::string version() const { return version_; }

  //: Return the encoding string
  std::string encoding() const { return encoding_; }

  //: Return the standalone bit
  bool standalone() const { return standalone_; }

  //: set the root element
  void set_root_element(const bxml_data_sptr& root)
  { root_element_ = root; }

  //: Set the version string
  void set_version(const std::string& version) { version_ = version; }

  //: Set the encoding string
  void set_encoding(const std::string& encoding) { encoding_ = encoding; }

  //: Set the standalone bit
  void set_standalone(bool standalone) { standalone_ = standalone; }

 private:
  //: The root element
  bxml_data_sptr root_element_;

  std::string version_;
  std::string encoding_;
  bool standalone_;
};



typedef vbl_smart_ptr<bxml_document> bxml_document_sptr;

//: compare two XML documents
bool operator==(const bxml_document& d1, const bxml_document& d2);

// Binary io, NOT IMPLEMENTED, signatures defined to use bxml_document as a brdb_value
void vsl_b_write(vsl_b_ostream & os, bxml_document const &ph);
void vsl_b_read(vsl_b_istream & is, bxml_document &ph);
void vsl_b_read(vsl_b_istream& is, bxml_document* ph);
void vsl_b_write(vsl_b_ostream& os, const bxml_document* &ph);


#endif // bxml_document_h_
