// This is core/vpgl/file_formats/vpgl_replacement_sensor_model_tres.h
#ifndef vpgl_replacement_sensor_model_tres_h_
#define vpgl_replacement_sensor_model_tres_h_
//:
// \file
// \brief: Define nitf2.1 tagged record extenstions for the replacement sensor model (RSM)
// \author J. L. Mundy
// \date October 2023
// Added RSMECB - April 2025
//
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/file_formats/vil_nitf2_image.h>
#include <iostream>
template <class T>
struct nitf_tre
{
  nitf_tre(const std::string & head, std::ostream & ostr)
    : head_(head)
  {
    ostr << "===== " << head << " =====" << std::endl;
  }
  nitf_tre(std::string tag, bool optional = false, bool possibly_blank = false)
    : tag_(tag)
    , optional_(optional)
    , possibly_blank_(possibly_blank)
    , is_blank_(false)
    , is_missing_(false)
    , type_("scalar")
  {}

  nitf_tre(std::string tag, std::string type, bool optional = false, bool possibly_blank = false)
    : tag_(tag)
    , type_(type)
    , optional_(optional)
    , possibly_blank_(possibly_blank)
    , is_blank_(false)
    , is_missing_(false)
  {}

  bool
  get(vil_nitf2_tagged_record_sequence::const_iterator & tres_itr, bool verbose = false);
  bool
  get(vil_nitf2_tagged_record_sequence::const_iterator & tres_itr, T & value);
  bool
  get(vil_nitf2_tagged_record_sequence::const_iterator & tres_itr, std::vector<T> & values);
  bool
  append(std::ostream & ostr);
  bool
  get_append(vil_nitf2_tagged_record_sequence::const_iterator & tres_itr, std::ostream & os, bool verbose = false)
  {
      
    bool good = get(tres_itr, verbose);
    if (!good)
    {
      os << tag_ + " required and failed" << std::endl;
      return good;
    }

    good = append(os);
    if (!good)
    {
      std::cout << "append failed for TRE " + tag_ << std::endl;
      return good;
    }
    return false;
  }
  bool optional_;
  bool possibly_blank_;
  bool valid_;
  bool is_blank_;
  bool is_missing_;
  std::string type_;
  std::string head_;
  std::string tag_;
  std::string blank_;
  T value_;
  std::vector<T> values_;
};
class vpgl_replacement_sensor_model_tres
{
public:
  static void
  define_RSMIDA();
  static void
  define_RSMPCA();
  static void
  define_RSMPIA();
  static void
  define_RSMGIA();
  static void
  define_RSMDCA();
  static void
  define_RSMDCB();
  static void
  define_RSMECA();
  static void
  define_RSMECB();
  static void
  define_RSMAPA();
  static void
  define_RSMAPB();
  static void
  define_RSMGGA();
  static void
  define()
  {
    define_RSMIDA();
    define_RSMPCA();
    define_RSMPIA();
    define_RSMGIA();
    define_RSMDCA();
    define_RSMDCB();
    define_RSMECA();
    define_RSMECB();
    define_RSMAPA();
    define_RSMAPB();
    define_RSMGGA();
  }

private:
  vpgl_replacement_sensor_model_tres();
  ~vpgl_replacement_sensor_model_tres();
};
template <class T>
bool
nitf_tre<T>::get(vil_nitf2_tagged_record_sequence::const_iterator & tres_itr, T & value)
{
  bool good = get(tres_itr);
  if (good)
    value = value_;
  return good;
}
template <class T>
bool
nitf_tre<T>::get(vil_nitf2_tagged_record_sequence::const_iterator & tres_itr, std::vector<T> & values)
{
  type_ = "vector";
  values.clear();
  bool good = get(tres_itr);
  if (good)
  {
    values = values_;
    return good;
  }
  return false;
}

template <class T>
bool
nitf_tre<T>::get(vil_nitf2_tagged_record_sequence::const_iterator & tres_itr, bool verbose)
{
  bool ret = true;
  if (type_ == "scalar")
    ret = (*tres_itr)->get_value(tag_, value_);
  else if (type_ == "vector")
  {
    ret = (*tres_itr)->get_values(tag_, values_);
  }
  if (ret)
  {
    valid_ = true;
    return true;
  }
  else if (type_ == "scalar" && !ret && possibly_blank_)
  {
    ret = (*tres_itr)->get_value(tag_, blank_);
    if (blank_ != "")
      is_blank_ = true;
    return true;
  }
  else if (type_ == "scalar" && !ret && optional_)
  {
    is_missing_ = true;
    valid_ = false;
    return true;
  }
  if (verbose)
    std::cout << tag_ + " property failed" << std::endl;
  return false;
}

template <class T>
bool
nitf_tre<T>::append(std::ostream & ostr)
{
  if (!ostr)
    return false;
  if (type_ == "scalar" && valid_)
  {
    ostr << tag_ << ' ' << value_ << std::endl;
    return true;
  }
  else if (type_ == "vector" && valid_)
  {
    size_t n = values_.size();
    for (size_t i = 0; i < n; ++i)
      ostr << tag_ + " index " << i << ' ' << values_[i] << std::endl;
    return true;
  }
  else if (type_ == "scalar" && possibly_blank_ && is_blank_)
  {
    ostr << tag_ << ' ' << "can be blank and is blank" << std::endl;
    return true;
  }
  else if (type_ == "scalar" && optional_ && is_missing_)
  {
    ostr << tag_ << ' ' << "is missing but optional" << std::endl;
    return true;
  }
  else if (is_missing_)
  {
    ostr << tag_ << ' ' << "is required and is missing" << std::endl;
    return false;
  }
  return false;
}
#endif // vpgl_replacement_sensor_model_tres_h_
