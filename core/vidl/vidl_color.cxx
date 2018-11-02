// This is core/vidl/vidl_color.cxx
#include <iostream>
#include "vidl_color.h"
//:
// \file
// \author Matt Leotta
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: The total number of pixel datatypes
constexpr unsigned int num_types = 5;

namespace {

//: Define the function pointer type for each pair of data types
template <class T1, class T2>
struct func_ptr
{
  typedef void (*type)(const T1* in, T2* out);
};


// Assign an index to each of the pixel data types
template <class T>
struct type_index;
#define vidl_type_index_mac(T,NUM)\
template <> \
struct type_index<T> \
{\
enum { index = (NUM) };\
}

vidl_type_index_mac(vxl_byte, 0);
vidl_type_index_mac(bool, 1);
vidl_type_index_mac(vxl_uint_16, 2);
vidl_type_index_mac(vxl_ieee_32, 3);
vidl_type_index_mac(vxl_ieee_64, 4);

#undef vidl_type_index_mac

//: Generates an entry into the table of color conversions functions
// This is called for every pair of color formats and function pointers
// that exist between all pairs of pixel formats
template <vidl_pixel_color in_color, vidl_pixel_color out_color, class fptr_T>
struct table_entry_init
{
  static inline void set_entry(vidl_color_conv_fptr& table_entry)
  {
    if (!table_entry) {
      fptr_T fp = &vidl_color_converter<in_color,out_color>::convert;
      table_entry = reinterpret_cast<vidl_color_conv_fptr>(fp);
    }
  }
};

//: Recursive template metaprogram to generate table entries for each pair of pixel types
template <int Fmt_Code>
struct table_init
{
  static inline void populate(vidl_color_conv_fptr table[VIDL_PIXEL_COLOR_ENUM_END][VIDL_PIXEL_COLOR_ENUM_END]
                                                        [num_types][num_types])
  {
    const auto in_fmt = vidl_pixel_format(Fmt_Code/VIDL_PIXEL_FORMAT_ENUM_END);
    const auto out_fmt = vidl_pixel_format(Fmt_Code%VIDL_PIXEL_FORMAT_ENUM_END);
    auto in_color = static_cast<vidl_pixel_color>(vidl_pixel_traits_of<in_fmt>::color_idx);
    auto out_color = static_cast<vidl_pixel_color>(vidl_pixel_traits_of<out_fmt>::color_idx);
    typedef typename vidl_pixel_traits_of<in_fmt>::type in_type;
    typedef typename vidl_pixel_traits_of<out_fmt>::type out_type;
    unsigned in_type_num = type_index<in_type>::index;
    unsigned out_type_num = type_index<out_type>::index;
    typedef typename func_ptr<in_type,out_type>::type fptr;
    table_entry_init<static_cast<vidl_pixel_color>(vidl_pixel_traits_of<in_fmt>::color_idx),
                     static_cast<vidl_pixel_color>(vidl_pixel_traits_of<out_fmt>::color_idx),
                     fptr>::set_entry(table[in_color][out_color][in_type_num][out_type_num]);
    table_init<Fmt_Code-1>::populate(table);
  }
};


//: The base case
template <>
struct table_init<0>
{
  static inline void populate(vidl_color_conv_fptr table[VIDL_PIXEL_COLOR_ENUM_END][VIDL_PIXEL_COLOR_ENUM_END]
                                                        [num_types][num_types])
  {
    const auto in_fmt = vidl_pixel_format(0);
    const auto out_fmt = vidl_pixel_format(0);
    auto in_color = static_cast<vidl_pixel_color>(vidl_pixel_traits_of<in_fmt>::color_idx);
    auto out_color = static_cast<vidl_pixel_color>(vidl_pixel_traits_of<out_fmt>::color_idx);
    typedef vidl_pixel_traits_of<in_fmt>::type in_type;
    typedef vidl_pixel_traits_of<out_fmt>::type out_type;
    unsigned in_type_num = type_index<in_type>::index;
    unsigned out_type_num = type_index<out_type>::index;
    typedef func_ptr<in_type,out_type>::type fptr;
    table_entry_init<static_cast<vidl_pixel_color>(vidl_pixel_traits_of<in_fmt>::color_idx),
    static_cast<vidl_pixel_color>(vidl_pixel_traits_of<out_fmt>::color_idx),
    fptr>::set_entry(table[in_color][out_color][in_type_num][out_type_num]);
  }
};


//: Recursive template metaprogram to generate type table entries for each pixel data type
//  All pixel formats are scanned, some data types will be initialized multiple times
template <int Fmt>
struct type_table_init
{
  static inline void populate(const std::type_info* type_table[num_types])
  {
    typedef typename vidl_pixel_traits_of<static_cast<vidl_pixel_format>(Fmt)>::type dtype;
    type_table[type_index<dtype>::index] = &typeid(dtype);
    type_table_init<Fmt-1>::populate(type_table);
  }
};


//: The base case
template <>
struct type_table_init<0>
{
  static inline void populate(const std::type_info* type_table[num_types])
  {
    typedef vidl_pixel_traits_of<static_cast<vidl_pixel_format>(0)>::type dtype;
    type_table[type_index<dtype>::index] = &typeid(dtype);
  }
};


//: A table of all conversion functions
class converter
{
 public:
  //: Constructor - generate the table
  converter()
  {
    // generate the table of function pointers
    table_init<VIDL_PIXEL_FORMAT_ENUM_END*VIDL_PIXEL_FORMAT_ENUM_END-1>::populate(table);
    type_table_init<VIDL_PIXEL_FORMAT_ENUM_END-1>::populate(type_table);
#if 0
    for (unsigned int i=0; i<num_types; ++i){
      if (type_table[i])
        std::cout << "type "<<i<<" is "<<type_table[i]->name() << std::endl;
    }
#endif // 0
  }

  //: Apply the conversion
  vidl_color_conv_fptr operator()(vidl_pixel_color in_C, const std::type_info& in_type,
                                  vidl_pixel_color out_C, const std::type_info& out_type) const
  {
    unsigned int in_idx = type_index(in_type);
    unsigned int out_idx = type_index(out_type);
    return table[in_C][out_C][in_idx][out_idx];
  }

  unsigned int type_index(const std::type_info& t) const
  {
    for (unsigned int i=0; i<num_types; ++i)
    {
      if ( type_table[i] && t == *type_table[i] )
        return i;
    }
    std::cerr << "error: unregistered pixel data type - "<<t.name()<<std::endl;
    return static_cast<unsigned int>(-1);
  }
 private:
  //: Table of color conversion functions
  vidl_color_conv_fptr table[VIDL_PIXEL_COLOR_ENUM_END][VIDL_PIXEL_COLOR_ENUM_END][num_types][num_types];
  const std::type_info* type_table[num_types];
};

//: Instantiate a global conversion function table
converter conversion_table;

} // namespace


//: Returns a color conversion function based on runtime values
// The function returned is always a vidl_color_conv_fptr which
// converts const vxl_byte* to vxl_byte*.  Some of these function
// may actually reinterpret the data as other types (i.e. bool* or
// vxl_uint_16*) via reinterpret_cast
vidl_color_conv_fptr
vidl_color_converter_func( vidl_pixel_color in_C, const std::type_info& in_type,
                           vidl_pixel_color out_C, const std::type_info& out_type)
{
  return conversion_table(in_C, in_type, out_C, out_type);
}
