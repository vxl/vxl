#ifndef vipl_section_descriptor_hxx_
#define vipl_section_descriptor_hxx_
//:
// \file

#include <iostream>
#include "vipl_section_descriptor.h"
#include <vipl/section/vipl_section_container.h>
#include <vipl/filter/vipl_filter_abs.h> // for default def of FILTER_IMPTR_INC_REFCOUNT
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifdef _MSC_VER
// Disable complaints about empty controlled statements (from blank macro expansion)
# pragma warning( push )
# pragma warning( disable : 4390 )
#endif

//: Assigns the pointers directly. Does not attempt to deep copy them.
template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(
                 vipl_section_descriptor< DataType >* desc ,
                 vipl_section_container< DataType >* container)
  : hsreal_descriptor(desc),
    hsreal_container(container),
    hsi_data_ptr(nullptr),
    hsi_data_offsets(2,0),
    hsi_curr_sec_start(2,0),
    hsi_curr_sec_end(2,0),
    hsi_curr_sec_size(2,0),
    refcount_ (1)
{
  if (desc) { FILTER_IMPTR_INC_REFCOUNT(desc); }
  if (container) { FILTER_IMPTR_INC_REFCOUNT(container); }
#ifdef DEBUG
  std::cerr << "Warning: called unimplemented vipl_section_descriptor constructor with signature "
           << "vipl_section_descriptor<DataType>*, vipl_section_container<DataType>*\n";
#endif
}

#ifdef _MSC_VER
# pragma warning( pop )
#endif

//: Deep-copies the pointers
template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(
                const vipl_section_descriptor< DataType >* desc ,
                const vipl_section_container< DataType >* container ,
                int t)
 : hsreal_descriptor(0),
   hsreal_container(0),
   hsi_data_ptr(0),
   hsi_data_offsets(2,0),
   hsi_curr_sec_start(2,0),
   hsi_curr_sec_end(2,0),
   hsi_curr_sec_size(2,0),
   refcount_ (1)
{
  hsreal_descriptor = desc->virtual_copy();
  hsreal_container = container->virtual_copy();
#ifdef DEBUG
  std::cerr << "Warning: called unimplemented vipl_section_descriptor constructor with signature "
           << "const vipl_section_descriptor<DataType>*, const vipl_section_container<DataType>*, int\n";
#endif
}

//: A simple section_descriptor useful for filter Regions_of_Application.
// It is not associated with any container or ``real'' descriptor.
// It cannot verify that the start/end points are meaningful for a particular
// image (there is none associated with it), but if used for the ROA of a
// filter this can be used to limit its operation to only a small window
// within the image.
template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(
                 std::vector< int >& startpts ,
                 std::vector< int >& endpts)
  : hsreal_descriptor (0),
    hsreal_container (0),
    hsi_data_ptr (0),
    hsi_data_offsets (2,0),
    hsi_curr_sec_start (2,0),
    hsi_curr_sec_end (2,0),
    hsi_curr_sec_size (2,0),
    refcount_ (1)
{
#ifdef DEBUG
  std::cerr << "Warning: called unimplemented vipl_section_descriptor constructor with signature "
           << "std::vector<int>&, std::vector<int>&\n";
#endif
}

template < class DataType >
vipl_section_descriptor< DataType > ::~vipl_section_descriptor()
{
  if (this != hsreal_descriptor && hsreal_descriptor && (hsreal_descriptor->refcount() > 1) )
    FILTER_IMPTR_DEC_REFCOUNT(hsreal_descriptor);
  if (hsreal_container && hsreal_container->refcount()>1)
    FILTER_IMPTR_DEC_REFCOUNT(hsreal_container);
#ifdef DEBUG
  std::cerr << "Warning: called unfinished destructor\n";
#endif
}

template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor()
  : hsreal_descriptor(this),
    hsreal_container(nullptr),
    hsi_data_ptr(nullptr),
    hsi_data_offsets(2,0),
    hsi_curr_sec_start(2,0),
    hsi_curr_sec_end(2,0),
    hsi_curr_sec_size(2,0),
    refcount_ (1)
// C++ auto-generated low-level constructor
{
  hsreal_descriptor = this;
}

template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(const vipl_section_descriptor< DataType > &t)
  : hsreal_descriptor(t.hsreal_descriptor),
    hsreal_container(t.hsreal_container),
    hsi_data_ptr(t.hsi_data_ptr),
    hsi_data_offsets(t.hsi_data_offsets),
    hsi_curr_sec_start(t.hsi_curr_sec_start),
    hsi_curr_sec_end(t.hsi_curr_sec_end),
    hsi_curr_sec_size(t.hsi_curr_sec_size),
    refcount_ (1)
// C++ auto-generated low-level copy constructor
{
  // you can fill special ``copy constructor'' stuff here.
  // All dynamic/soft attributes are copied. Thus your
  //want to change it here is should be hard because it
  //is always changed!) So don't change things without
  //knowing their form.
  if (t.hsreal_descriptor)
    hsreal_descriptor = t.hsreal_descriptor->virtual_copy();
  if (t.hsreal_container)
    hsreal_container = t.hsreal_container->virtual_copy();
}

//:
// This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
// 1 means ``y'' axis etc...) and returns an integer which describes the offset
// of the next (i.e. associated with the higher coordinate value) data item
// along the axis.
template < class DataType >
int vipl_section_descriptor< DataType > ::data_offsets( int /*axis*/ ) const
{
  std::cerr << "Warning: called unimplemented method vipl_section_descriptor::data_offsets\n";
  return 0;
}

//: Returns a referable pointer to the first data item in the current section.
// If the value returned is 0, then the address is not available to the filter.
template < class DataType >
DataType* vipl_section_descriptor< DataType > ::data_ptr()
{
  std::cerr << "Warning: called unimplemented method vipl_section_descriptor::data_ptr\n";
  return nullptr;
}

//: Returns a const pointer to the first data item in the current section.
// If the value returned is 0, then the address is not available to the filter.
template < class DataType >
const DataType* vipl_section_descriptor< DataType > ::data_ptr() const
{
  std::cerr << "Warning: called unimplemented method vipl_section_descriptor::data_ptr\n";
  return nullptr;
}

//:
// This method takes in a section_descriptor (which can be thought of as a
// filter's ROA) and updates this section to be the intersection of the ROA
// and the original section. It returns 0 if the region is empty, 1 if nothing
// changed and 2 if there was really a change in the section.
template < class DataType >
int vipl_section_descriptor< DataType > ::restrict( const vipl_section_descriptor< DataType >& ROA)
{
  return 0;
}

//: Makes a new correct copy. It's just a bit more tricky because descriptors have pointers to its ``real instance''.
template < class DataType >
vipl_section_descriptor< DataType >* vipl_section_descriptor< DataType > ::virtual_copy() const
{
  vipl_section_descriptor< DataType > *rtn = new vipl_section_descriptor< DataType >;
  rtn->put_real_descriptor(rtn);
  if (real_container())
    rtn->put_real_container(real_container()->virtual_copy());
  else
    rtn->put_real_container(nullptr);
  rtn->put_i_curr_sec_start(i_curr_sec_start());
  rtn->put_i_curr_sec_end(i_curr_sec_end());
  rtn->put_i_curr_sec_size(i_curr_sec_size());
  return rtn;
}

#endif // vipl_section_descriptor_hxx_
