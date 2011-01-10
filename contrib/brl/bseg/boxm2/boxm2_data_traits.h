#ifndef boxm2_data_traits_h_
#define boxm2_data_traits_h_
//:
// \file
// \brief traits for data types
//
// \author Vishal Jain
// \date nov 17, 2010

#include <vcl_string.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vnl/vnl_vector_fixed.h>
class boxm2_mog3_grey_processor;

enum boxm2_data_type
{
  BOXM2_ALPHA=0,
  BOXM2_MOG3_GREY,
  BOXM2_AUX,    
  BOXM2_UNKNOWN
};

//: Pixel properties for templates.
template <boxm2_data_type type>
class boxm2_data_traits;

//: traits for a mixture of gaussian appearance model of gray-scale images
template<>
class boxm2_data_traits<BOXM2_ALPHA>
{
 public:
  typedef float datatype;
  vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(){ return "alpha";}
};

template<>
class boxm2_data_traits<BOXM2_MOG3_GREY>
{
 public:
  typedef boxm2_mog3_grey_processor processor;
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(){ return "boxm2_mog3_grey";}
};

template<>
class boxm2_data_traits<BOXM2_AUX>
{
 public:
  typedef vnl_vector_fixed<int, 4> datatype;
  vcl_size_t datasize(){return sizeof(datatype);}
  static vcl_string prefix(){ return "aux";}
};
#endif
