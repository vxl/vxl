// This is tbl/vipl/section/vipl_section_descriptor.h
#ifndef vipl_section_descriptor_h_
#define vipl_section_descriptor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file

#include <vcl_vector.h>

template < class DataType > class vipl_section_container; //template forward reference
template < class DataType > class vipl_section_iterator; //template forward reference

template < class DataType >
class vipl_section_descriptor
{
  friend class vipl_section_container< DataType > ; //declare a friend class
  friend class vipl_section_iterator< DataType > ; //declare a friend class

  vipl_section_descriptor< DataType >* hsreal_descriptor;
  vipl_section_container< DataType >* hsreal_container;
  DataType* hsi_data_ptr;
  vcl_vector< int > hsi_data_offsets;
  vcl_vector< int > hsi_curr_sec_start;
  vcl_vector< int > hsi_curr_sec_end;
  vcl_vector< int > hsi_curr_sec_size;

 protected:
  //: Assigns the pointers directly. Does not attempt to deep copy them.
  vipl_section_descriptor( vipl_section_descriptor< DataType >* desc ,
                           vipl_section_container< DataType >* container) ;

  //: Deep-copies the pointers
  vipl_section_descriptor(
      const vipl_section_descriptor< DataType >* desc ,
      const vipl_section_container< DataType >* container , int t) ;

 public:
  //:
  // A simple section_descriptor useful for filter
  // Regions_of_Application. it is not associated with any
  // container or ``real'' descriptor. It cannot verify that the
  // start/end points are meaningful for a particular image (there
  // is none associated with it), but if used for the ROA of a
  // filter this can be used to limit its operation to only a small
  // window within the image.
  vipl_section_descriptor( vcl_vector< int >& startpts , vcl_vector< int >& endpts);

  virtual ~vipl_section_descriptor();//low_level destructor
  vipl_section_descriptor();//low_level c++ constructor
  //user accessed low_level c++ copy constructor
  vipl_section_descriptor(const vipl_section_descriptor< DataType > &);

  //:
  // This method takes in an integer argument called axis (i.e. 0
  // means the ``x'' axis, 1 means ``y'' axis etc...) and returns
  // an integer which describes the start coordinate value for ``x''
  // (or ``y'' etc..) with respect to the ``image'' coordinate system.
  virtual int curr_sec_start( int axis) const { return hsi_curr_sec_start[axis]; }

  //:
  // This method takes in an integer argument called axis (i.e. 0
  // means the ``x'' axis, 1 means ``y'' axis etc...) and returns
  // an integer which describes the end coordinate value for ``x''
  // (or ``y'' etc..) with respect to the ``image'' coordinate system.
  virtual int curr_sec_end( int axis) const { return hsi_curr_sec_end[axis]; }

  //:
  // This method takes in an integer argument called axis (i.e. 0
  // means the ``x'' axis, 1 means ``y'' axis etc...) and returns
  // an integer which describes the size of the axis (end minus
  // start) for ``x'' (or ``y'' etc..) with respect to the
  // ``image'' coordinate system.
  virtual int curr_sec_size( int axis) const { return hsi_curr_sec_size[axis]; }

  //:
  // This method takes in an integer argument called axis (i.e. 0
  // means the ``x'' axis, 1 means ``y'' axis etc...) and returns
  // an integer which describes the offset of the next
  // (i.e. associated with the higher coordinate value) data item
  // along the axis.
  virtual int data_offsets( int axis) const ;

  //: Returns a referable pointer to the first data item in the current section.
  // If the value returned is null, then the
  // address is not available to the filter.
  virtual DataType* data_ptr() ;

  //:
  // Returns a const pointer to the first data item in the current
  // section. If the value returned is null, then the address is
  // not available to the filter.
  virtual const DataType* data_ptr() const ;

  //: Returns a writable pointer to the ``real'' section descriptor.
  // If this method is called on a concrete instance,
  // it should return 0.
  virtual vipl_section_descriptor< DataType >* inner_descriptor(){return hsreal_descriptor;}

  //: Returns a const pointer to the ``real'' section descriptor.
  // If this method is called on a concrete instance,
  // it should return 0.
  virtual const vipl_section_descriptor< DataType >* inner_descriptor()const{return hsreal_descriptor;}

  //:
  // This method takes in a section_descriptor (which can be thought
  // of as a filter's ROA) and updates this section to be the
  // intersection of the ROA and the original section. It returns 0
  // if the region is empty, 1 if nothing changed and 2 if there
  // was really a change in the section.
  int restrict( const vipl_section_descriptor< DataType >& ROA) ;

  //:
  // Makes a new correct copy. It's just a bit more tricky because
  // descriptors have pointers to its ``real instance.''
  virtual vipl_section_descriptor< DataType >* virtual_copy() const ;

 public:
  vipl_section_descriptor< DataType >* real_descriptor() const{ return hsreal_descriptor;}
  vipl_section_descriptor< DataType >* & ref_real_descriptor(){ return hsreal_descriptor;}
  void put_real_descriptor( vipl_section_descriptor< DataType >* v){ hsreal_descriptor = v;}
  vipl_section_container< DataType >* real_container() const{ return hsreal_container;}
  vipl_section_container< DataType >* & ref_real_container(){ return hsreal_container;}
  void put_real_container( vipl_section_container< DataType >* v){ hsreal_container = v;}
  DataType* i_data_ptr() const{ return hsi_data_ptr;}
  DataType* & ref_i_data_ptr(){ return hsi_data_ptr;}
  void put_i_data_ptr( DataType* v){ hsi_data_ptr = v;}
  vcl_vector< int > const & i_data_offsets() const{ return hsi_data_offsets;}
  vcl_vector< int > & ref_i_data_offsets(){ return hsi_data_offsets;}
  void put_i_data_offsets( vcl_vector< int > const & v){ hsi_data_offsets = v;}
  vcl_vector< int > const & i_curr_sec_start() const{ return hsi_curr_sec_start;}
  vcl_vector< int > & ref_i_curr_sec_start(){ return hsi_curr_sec_start;}
  void put_i_curr_sec_start( vcl_vector< int > const & v){ hsi_curr_sec_start = v;}
  //:
  // This method takes in an integer argument called axis (i.e. 0
  // means the ``x'' axis, 1 means ``y'' axis etc...) and returns
  // an integer which describes the end coordinate value for ``x''
  // (or ``y'' etc..) with respect to the ``image'' coordinate system.
  vcl_vector< int > const & i_curr_sec_end() const{ return hsi_curr_sec_end;}
  vcl_vector< int > & ref_i_curr_sec_end(){ return hsi_curr_sec_end;}
  void put_i_curr_sec_end( vcl_vector< int > const & v){ hsi_curr_sec_end = v;}
  vcl_vector< int > const & i_curr_sec_size() const{ return hsi_curr_sec_size;}
  vcl_vector< int > & ref_i_curr_sec_size(){ return hsi_curr_sec_size;}
  void put_i_curr_sec_size( vcl_vector< int > const & v){ hsi_curr_sec_size = v;}

  // refcounting:
 private: int refcount_;
 public:  int refcount() const { return refcount_; }
 public:  int inc_refcount() { return ++refcount_; }
 public:  int dec_refcount() { if (refcount_<=1) { delete this; return 0; } return --refcount_; }

}; // end of class definition

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_section_descriptor.txx"
#endif

#endif // vipl_section_descriptor_h_
