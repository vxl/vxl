#include <vipl/section/vipl_section_descriptor.h>
#include <vipl/section/vipl_section_iterator.h>
#include <vipl/section/vipl_section_container.h>
#include <stdio.h> // for some of the error messages

// Assigns the pointers directly. Does not attempt to deep copy them.
template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(
                 vipl_section_descriptor< DataType >* desc ,
                 vipl_section_container< DataType >* container)
  : hsreal_descriptor(desc),
    hsreal_container(container),
    hsi_data_ptr(0),
    hsi_data_offsets(1,0),
    hsi_curr_sec_start(1,0),
    hsi_curr_sec_end(1,0),
    hsi_curr_sec_size(1,0),
    refcount_ (1)
{
  if(desc) FILTER_IMPTR_INC_REFCOUNT(desc);
  if(container) FILTER_IMPTR_INC_REFCOUNT(container);
  // cerr << "Warning: called unimplemented constructor with sig vipl_section_descriptor< DataType >* desc, vipl_section_container< DataType >* container\n";
}

//  Deep-copies the pointers
template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(
                const vipl_section_descriptor< DataType >* desc ,
                const vipl_section_container< DataType >* container ,
                int t)
 : hsreal_descriptor(0),
   hsreal_container(0),
   hsi_data_ptr(0),
   hsi_data_offsets(1,0),
   hsi_curr_sec_start(1,0),
   hsi_curr_sec_end(1,0),
   hsi_curr_sec_size(1,0),
   refcount_ (1)
{
  hsreal_descriptor = desc->virtual_copy();
  hsreal_container =
  container->virtual_copy();
  // cerr << "Warning: called unimplemented constructor with sig const vipl_section_descriptor< DataType >* desc, const vipl_section_container< DataType >* container, int t\n";
}

// A simple section_descriptor useful for filter Regions_of_Application. it is not associated with any container or ``real'' descriptor. It cannot verify that the start/end points are meaningful for a particular image (there is none associated with it), but if used for the ROA of a filter this can be used to limit its operation to only a small window within the image. 
template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(
                 vcl_vector< int >& startpts ,
                 vcl_vector< int >& endpts)
  : hsreal_descriptor (0),
    hsreal_container (0),
    hsi_data_ptr (0),
    hsi_data_offsets (1,0),
    hsi_curr_sec_start (1,0),
    hsi_curr_sec_end (1,0),
    hsi_curr_sec_size (1,0),
    refcount_ (1)
{
  cerr << "Warning: called unimplemented constructor with sig vcl_vector< int >& startpts, vcl_vector< int >& endpts\n";
}

template < class DataType >
vipl_section_descriptor< DataType > ::~vipl_section_descriptor()
{
  if (this != hsreal_descriptor && hsreal_descriptor && (hsreal_descriptor->refcount() > 1) )
    FILTER_IMPTR_DEC_REFCOUNT(hsreal_descriptor);
  if (hsreal_container && hsreal_container->refcount()>1)
    FILTER_IMPTR_DEC_REFCOUNT(hsreal_container);
  // cerr << "Warning: called unfinished destructor\n";
}

template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor()
  : hsreal_descriptor(this),
    hsreal_container(0),
    hsi_data_ptr(0),
    hsi_data_offsets(1,0),
    hsi_curr_sec_start(1,0),
    hsi_curr_sec_end(1,0),
    hsi_curr_sec_size(1,0),
    refcount_ (1)
// C++ auto-generated low-level constructor
{
  hsreal_descriptor = this;
}

template < class DataType >
vipl_section_descriptor< DataType > ::vipl_section_descriptor(const vipl_section_descriptor< DataType > &t)
  : hsreal_descriptor(0),
    hsreal_container(0),
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

// This method takes in an argument called axis (i.e. 0 means the ``x'' axis, 1 means ``y'' axis) etc... and returns an integer which describes the start coordinate value for ``x'' (or ``y'' etc..) with respect to the ``image'' coordinate system
template < class DataType >
int vipl_section_descriptor< DataType > ::curr_sec_start( int axis) const
{
  return i_curr_sec_start()[axis];
}

// This method takes in an argument called axis (i.e. 0 means the ``x'' axis, 1 means ``y'' axis) etc... and returns an integer which describes the end coordinate value for ``x'' (or ``y'' etc..) with respect to the ``image'' coordinate system

template < class DataType >
int vipl_section_descriptor< DataType > ::curr_sec_end( int axis) const
{
  return i_curr_sec_end()[axis];
}

// This method takes in an argument called axis (i.e. 0 means the ``x'' axis, 1 means ``y'' axis) etc... and returns an integer which describes the size of the axis (end minus start) for ``x'' (or ``y'' etc..) with respect to the ``image'' coordinate system
template < class DataType >
int vipl_section_descriptor< DataType > ::curr_sec_size( int axis) const
{
  return i_curr_sec_size()[axis];
}

// This method takes in an argument called axis (i.e. 0 means the ``x'' axis, 1 means ``y'' axis) etc... and returns an integer which describes the offset of the next (i.e. associated with the higher coordinate value) data item along the axis.
template < class DataType >
int vipl_section_descriptor< DataType > ::data_offsets( int axis) const
{
  cerr << "Warning: called unimplemented method data_offsets\n";
  return 0;
}

// Returns a referable pointer to the first data item in the current section. If the value returned is null, then the address is not available to the filter
template < class DataType >
DataType* vipl_section_descriptor< DataType > ::data_ptr()
{
  cerr << "Warning: called unimplemented method data_ptr\n";
  return 0;
}

// Returns a const pointer to the first data item in the current section. If the value returned is null, then the address is not available to the filter
template < class DataType >
const DataType* vipl_section_descriptor< DataType > ::data_ptr() const
{
  cerr << "Warning: called unimplemented method data_ptr\n";
  return 0;
}

// Returns a writable pointer to the ``real'' section descriptor. If this method is called on a concrete instance, it should return 0. 
template < class DataType >
vipl_section_descriptor< DataType >* vipl_section_descriptor< DataType > ::inner_descriptor()
{
  return ref_real_descriptor();
}

// Returns a const pointer to the ``real'' section descriptor. If this method is called on a concrete instance, it should return 0. 
template < class DataType >
const vipl_section_descriptor< DataType >* vipl_section_descriptor< DataType > ::inner_descriptor() const
{
  return real_descriptor();
}

// This method takes in a section_descriptor (which can be thought of as a filter's ROA) and updates this section to be the intersection of the ROA and the original section. It returns 0 if the region is empty, 1 if nothing changed and 2 if there was really a change in the section.
template < class DataType >
int vipl_section_descriptor< DataType > ::restrict( const vipl_section_descriptor< DataType >& ROA)
{
  return 0;
}

// Makes a new correct copy. It's just a bit more tricky because descriptors have pointers to its ``real instance.''
template < class DataType >
vipl_section_descriptor< DataType >* vipl_section_descriptor< DataType > ::virtual_copy() const
{
  vipl_section_descriptor< DataType > *rtn = new vipl_section_descriptor< DataType >;
  rtn->put_real_descriptor(rtn);
  if (real_container())
    rtn->put_real_container(real_container()->virtual_copy());
  else
    rtn->put_real_container(0);
  rtn->put_i_curr_sec_start(i_curr_sec_start());
  rtn->put_i_curr_sec_end(i_curr_sec_end());
  rtn->put_i_curr_sec_size(i_curr_sec_size());
  // cerr << "Warning: called possibly method virtual_copy\n";
  return rtn;
}
