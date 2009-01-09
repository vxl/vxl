// This is brl/bbas/vidl2/vidl2_color.cxx
#include "vidl2_color.h"
//:
// \file
// \author Matt Leotta
//

#include <vcl_iostream.h>

//: The total number of pixel datatypes
const unsigned int num_types = 5;

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
#define vidl2_type_index_mac(T,NUM)\
VCL_DEFINE_SPECIALIZATION \
struct type_index<T> \
{\
enum { index = NUM };\
}

vidl2_type_index_mac(vxl_byte, 0);
vidl2_type_index_mac(bool, 1);
vidl2_type_index_mac(vxl_uint_16, 2);
vidl2_type_index_mac(vxl_ieee_32, 3);
vidl2_type_index_mac(vxl_ieee_64, 4);
  
#undef vidl2_type_index_mac

//: Generates an entry into the table of color conversions functions
// This is called for every pair of color formats and function pointers
// that exist between all pairs of pixel formats
template <vidl2_pixel_color in_color, vidl2_pixel_color out_color, class fptr_T>
struct table_entry_init
{
  static inline void set_entry(vidl2_color_conv_fptr& table_entry)
  {
    if(!table_entry){
      fptr_T fp = &vidl2_color_converter<in_color,out_color>::convert;
      table_entry = reinterpret_cast<vidl2_color_conv_fptr>(fp);
    }
  }
};

//: Recursive template metaprogram to generate table entries for each pair of pixel types
template <int Fmt_Code>
struct table_init
{
  static inline void populate(vidl2_color_conv_fptr table[VIDL2_PIXEL_COLOR_ENUM_END][VIDL2_PIXEL_COLOR_ENUM_END]
                                                         [num_types][num_types])
  {
    const vidl2_pixel_format in_fmt = vidl2_pixel_format(Fmt_Code/VIDL2_PIXEL_FORMAT_ENUM_END);
    const vidl2_pixel_format out_fmt = vidl2_pixel_format(Fmt_Code%VIDL2_PIXEL_FORMAT_ENUM_END);
    vidl2_pixel_color in_color = static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<in_fmt>::color_idx);
    vidl2_pixel_color out_color = static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<out_fmt>::color_idx);
    typedef typename vidl2_pixel_traits_of<in_fmt>::type in_type;
    typedef typename vidl2_pixel_traits_of<out_fmt>::type out_type;
    unsigned in_type_num = type_index<in_type>::index;
    unsigned out_type_num = type_index<out_type>::index;
    typedef typename func_ptr<in_type,out_type>::type fptr;
    table_entry_init<static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<in_fmt>::color_idx),
                     static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<out_fmt>::color_idx),
                     fptr>::set_entry(table[in_color][out_color][in_type_num][out_type_num]);
    table_init<Fmt_Code-1>::populate(table);
  }
};


//: The base case
VCL_DEFINE_SPECIALIZATION
struct table_init<0>
{
  static inline void populate(vidl2_color_conv_fptr table[VIDL2_PIXEL_COLOR_ENUM_END][VIDL2_PIXEL_COLOR_ENUM_END]
                                                         [num_types][num_types])
  {
    const vidl2_pixel_format in_fmt = vidl2_pixel_format(0);
    const vidl2_pixel_format out_fmt = vidl2_pixel_format(0);
    vidl2_pixel_color in_color = static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<in_fmt>::color_idx);
    vidl2_pixel_color out_color = static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<out_fmt>::color_idx);
    typedef vidl2_pixel_traits_of<in_fmt>::type in_type;
    typedef vidl2_pixel_traits_of<out_fmt>::type out_type;
    unsigned in_type_num = type_index<in_type>::index;
    unsigned out_type_num = type_index<out_type>::index;
    typedef func_ptr<in_type,out_type>::type fptr;
    table_entry_init<static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<in_fmt>::color_idx),
    static_cast<vidl2_pixel_color>(vidl2_pixel_traits_of<out_fmt>::color_idx),
    fptr>::set_entry(table[in_color][out_color][in_type_num][out_type_num]);
  }
};
  

//: Recursive template metaprogram to generate type table entries for each pixel data type
// all pixel formats are scanned, some data types will be initialized multiple times
template <int Fmt>
struct type_table_init
{
  static inline void populate(const vcl_type_info* type_table[num_types])
  {
    typedef typename vidl2_pixel_traits_of<static_cast<vidl2_pixel_format>(Fmt)>::type dtype;
    type_table[type_index<dtype>::index] = &typeid(dtype);
    type_table_init<Fmt-1>::populate(type_table);
  }
};
  
  
//: The base case
VCL_DEFINE_SPECIALIZATION
struct type_table_init<0>
{
  static inline void populate(const vcl_type_info* type_table[num_types])
  {
    typedef vidl2_pixel_traits_of<static_cast<vidl2_pixel_format>(0)>::type dtype;
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
      table_init<VIDL2_PIXEL_FORMAT_ENUM_END*VIDL2_PIXEL_FORMAT_ENUM_END-1>::populate(table);
      type_table_init<VIDL2_PIXEL_FORMAT_ENUM_END-1>::populate(type_table);
      for(unsigned int i=0; i<num_types; ++i){
        if(type_table[i])
          vcl_cout << "type "<<i<<" is "<<type_table[i]->name() << vcl_endl;
      }
    }
    
    //: Apply the conversion
    vidl2_color_conv_fptr operator()(vidl2_pixel_color in_C, const vcl_type_info& in_type,
                                     vidl2_pixel_color out_C, const vcl_type_info& out_type) const
    {
      unsigned int in_idx = type_index(in_type);
      unsigned int out_idx = type_index(out_type);
      return table[in_C][out_C][in_idx][out_idx];
    }
    
    unsigned int type_index(const vcl_type_info& t) const
    {
      for(unsigned int i=0; i<num_types; ++i)
      {
        if( type_table[i] && t == *type_table[i] )
          return i;
      }
      vcl_cerr << "error: unregistered pixel data type - "<<t.name()<<vcl_endl;
      return -1;
    }
  private:
    //: Table of color conversion functions
    vidl2_color_conv_fptr table[VIDL2_PIXEL_COLOR_ENUM_END][VIDL2_PIXEL_COLOR_ENUM_END][num_types][num_types];
    const vcl_type_info* type_table[num_types];
    
  };

//: Instantiate a global conversion function table
converter conversion_table;

}



//: Returns a color conversion function based on runtime values
// The function returned is always a vidl2_color_conv_fptr which
// converts const vxl_byte* to vxl_byte*.  Some of these function
// may actually reinterpret the data as other types (i.e. bool* or
// vxl_uint_16*) via reinterpret_cast
vidl2_color_conv_fptr
vidl2_color_converter_func( vidl2_pixel_color in_C, const vcl_type_info& in_type,
                            vidl2_pixel_color out_C, const vcl_type_info& out_type)
{
  return conversion_table(in_C, in_type, out_C, out_type);
}
