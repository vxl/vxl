#ifndef vipl_section_descriptor_2d_hxx_
#define vipl_section_descriptor_2d_hxx_
//:
//  \file

#include <iostream>
#include "vipl_section_descriptor_2d.h"
#include <vipl/section/vipl_section_container.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Ctor useful to define 2D section descriptors for ROA's. If you.
// Once constructed there is no way to change the data in the section descriptor.
template < class DataType >
vipl_section_descriptor_2d< DataType > ::vipl_section_descriptor_2d(
                int startx ,
                int endx ,
                int starty ,
                int endy)
 : hsreal_descriptor (0),
   hsreal_container (0),
   hsi_data_ptr (0),
   hsi_data_offsets ( std::vector< int > () ),
   hsi_curr_sec_start ( std::vector< int > () ),
   hsi_curr_sec_end ( std::vector< int > () ),
   hsi_curr_sec_size ( std::vector< int > () )
{
  std::cerr << "Warning: called unimplemented vipl_section_descriptor_2d constructor with signature "
           << "int startx, int endx, int starty, int endy\n";
}

//: Assigns the pointers directly. Does not attempt to deep copy them.
template < class DataType >
  vipl_section_descriptor_2d< DataType > ::vipl_section_descriptor_2d(
                   vipl_section_descriptor< DataType >* desc ,
                 vipl_section_container< DataType >* container): hsreal_descriptor (0),
                 hsreal_container (0),
                 hsi_data_ptr (0),
                 hsi_data_offsets ( std::vector< int > () ),
                 hsi_curr_sec_start ( std::vector< int > () ),
                 hsi_curr_sec_end ( std::vector< int > () ),
                 hsi_curr_sec_size ( std::vector< int > () )
{
  std::cerr << "Warning: called unimplemented constructor with signature "
           << "vipl_section_descriptor< DataType >* desc, vipl_section_container< DataType >* container\n";
}

//:  Deep-copies the pointers
template < class DataType >
vipl_section_descriptor_2d< DataType > ::vipl_section_descriptor_2d(
          const vipl_section_descriptor< DataType >* desc ,
          const vipl_section_container< DataType >* container ,
          int t): hsreal_descriptor (0),
          hsreal_container (0),
          hsi_data_ptr (0),
          hsi_data_offsets ( std::vector< int > () ),
          hsi_curr_sec_start ( std::vector< int > () ),
          hsi_curr_sec_end ( std::vector< int > () ),
          hsi_curr_sec_size ( std::vector< int > () )
{
  std::cerr << "Warning: called unimplemented constructor with signature "
           << "const vipl_section_descriptor< DataType >* desc, const vipl_section_container< DataType >* container, int t\n";
}

template < class DataType >
vipl_section_descriptor_2d< DataType > ::~vipl_section_descriptor_2d() = default;

template < class DataType >
vipl_section_descriptor_2d< DataType > ::vipl_section_descriptor_2d(): hsreal_descriptor (0),
                 hsreal_container (0),
                 hsi_data_ptr (0),
                 hsi_data_offsets ( std::vector< int > () ),
                 hsi_curr_sec_start ( std::vector< int > () ),
                 hsi_curr_sec_end ( std::vector< int > () ),
                 hsi_curr_sec_size ( std::vector< int > () )
// C++ auto-generated low-level constructor
{
}

template < class DataType >
vipl_section_descriptor_2d< DataType > ::vipl_section_descriptor_2d(const vipl_section_descriptor_2d< DataType > &t)
              :
  vipl_section_descriptor< DataType > (t),
                 hsreal_descriptor(t.hsreal_descriptor),
                 hsreal_container(t.hsreal_container),
                 hsi_data_ptr(t.hsi_data_ptr),
                 hsi_data_offsets(t.hsi_data_offsets),
                 hsi_curr_sec_start(t.hsi_curr_sec_start),
                 hsi_curr_sec_end(t.hsi_curr_sec_end),
                 hsi_curr_sec_size(t.hsi_curr_sec_size)
    // C++ auto-generated low-level copy constructor
{
      // you can fill special ``copy constructor'' stuff here.
      // All dynamic/soft attributes are copied. Thus your
      //want to change it here is should be hard because it
      //is always changed!) So don't change things without
      //knowing their form.
}

#if 0 // not implemented
//:
// This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
// 1 means ``y'' axis etc...) and returns an integer which describes the start
// coordinate value for ``x'' (or ``y'' etc..) with respect to the ``image''
// coordinate system
template < class DataType >
int vipl_section_descriptor_2d< DataType > ::curr_sec_start( int axis) const
{
}

//:
// This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
// 1 means ``y'' axis etc...) and returns an integer which describes the end
// coordinate value for ``x'' (or ``y'' etc..) with respect to the ``image''
// coordinate system
template < class DataType >
int vipl_section_descriptor_2d< DataType > ::curr_sec_end( int axis) const
{
}

//:
// This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
// 1 means ``y'' axis etc...) and returns an integer which describes the size
// of the axis (end minus start) for ``x'' (or ``y'' etc..) with respect to the
// ``image'' coordinate system
template < class DataType >
int vipl_section_descriptor_2d< DataType > ::curr_sec_size( int axis) const
{
}

//:
// This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
// 1 means ``y'' axis etc...) and returns an integer which describes the offset
// of the next (i.e. associated with the higher coordinate value) data item
// along the axis.
template < class DataType >
int vipl_section_descriptor_2d< DataType > ::data_offsets( int axis) const
{
}

//: Returns a referable pointer to the first data item in the current section.
// If the value returned is null, then the address is not available to the filter.
template < class DataType >
DataType* vipl_section_descriptor_2d< DataType > ::data_ptr()
{
}

//: Returns a const pointer to the first data item in the current section.
// If the value returned is null, then the address is not available to the filter.
template < class DataType >
const DataType* vipl_section_descriptor_2d< DataType > ::data_ptr() const
{
}

//: Returns a writable pointer to the ``real'' section descriptor.
// If this method is called on a concrete instance, it should return 0.
template < class DataType >
vipl_section_descriptor< DataType >* vipl_section_descriptor_2d< DataType > ::inner_descriptor()
{
}

//: Returns a const pointer to the ``real'' section descriptor.
// If this method is called on a concrete instance, it should return 0.
template < class DataType >
const vipl_section_descriptor< DataType >* vipl_section_descriptor_2d< DataType > ::inner_descriptor() const
{
}

//: Makes a new correct copy. It's just a bit more tricky because descriptors have pointers to its ``real instance''.
template < class DataType >
vipl_section_descriptor< DataType >* vipl_section_descriptor_2d< DataType > ::virtual_copy() const
{
}
#endif

#endif // vipl_section_descriptor_2d_hxx_
