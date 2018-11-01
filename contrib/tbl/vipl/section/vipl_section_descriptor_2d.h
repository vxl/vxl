// This is tbl/vipl/section/vipl_section_descriptor_2d.h
#ifndef vipl_section_descriptor_2d_h_
#define vipl_section_descriptor_2d_h_
//:
//  \file

#include <vipl/section/vipl_section_descriptor.h>

template < class DataType > class vipl_section_container_2d; //template forward reference
template < class DataType > class vipl_section_iterator; //template forward reference

template < class DataType >
class vipl_section_descriptor_2d : public vipl_section_descriptor< DataType >
{
  friend class vipl_section_container< DataType > ; //declare a friend class
  friend class vipl_section_iterator< DataType > ; //declare a friend class

  // since STL does things by values we play a few games so we can have subclasses

  vipl_section_descriptor< DataType >* hsreal_descriptor;
  vipl_section_container< DataType >* hsreal_container;
  // pointer to the real data
  DataType* hsi_data_ptr;
  // "pointer offsets" to move 1 unit in any dimen
  std::vector< int > hsi_data_offsets;
  // describe the start,end and size of the section
  std::vector< int > hsi_curr_sec_start;
  std::vector< int > hsi_curr_sec_end;
  std::vector< int > hsi_curr_sec_size;
 public:
  //: Ctor useful to define 2D section descriptors for ROA's. If you.
  //  Once constructed there is no way to change the data in the section descriptor.
  vipl_section_descriptor_2d( int startx , int endx , int starty , int endy);
 protected:
  // Assigns the pointers directly. Does not attempt to deep copy them.
  vipl_section_descriptor_2d(
                 vipl_section_descriptor< DataType >* desc ,
                 vipl_section_container< DataType >* container);
  //  Deep-copies the pointers
  vipl_section_descriptor_2d(
                const vipl_section_descriptor< DataType >* desc ,
                const vipl_section_container< DataType >* container ,
                int t);
 public:
  virtual ~vipl_section_descriptor_2d(); //low_level destructor
  vipl_section_descriptor_2d(); //low_level c++ constructor
  //user accessed low_level c++ copy constructor
  vipl_section_descriptor_2d(const vipl_section_descriptor_2d< DataType > &);
#if 0 // not implemented at this level
  //:
  // This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
  // 1 means ``y'' axis) etc... and returns an integer which describes the start
  // coordinate value for ``x'' (or ``y'' etc..) with respect to the ``image''
  // coordinate system.
  virtual int curr_sec_start( int axis) const;
  //:
  // This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
  // 1 means ``y'' axis) etc... and returns an integer which describes the end
  // coordinate value for ``x'' (or ``y'' etc..) with respect to the ``image''
  // coordinate system.
  virtual int curr_sec_end( int axis) const;
  //:
  // This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
  // 1 means ``y'' axis) etc... and returns an integer which describes the size
  // of the axis (end minus start) for ``x'' (or ``y'' etc..) with respect to
  // the ``image'' coordinate system.
  virtual int curr_sec_size( int axis) const;
  //:
  // This method takes in an argument called axis (i.e. 0 means the ``x'' axis,
  // 1 means ``y'' axis) etc... and returns an integer which describes the
  // offset of the next (i.e. associated with the higher coordinate value) data
  // item along the axis.
  virtual int data_offsets( int axis) const;
  //: Returns a referable pointer to the first data item in the current section.
  // If the value returned is null, then the address is not available to the
  // filter.
  virtual DataType* data_ptr();
  //: Returns a const pointer to the first data item in the current section.
  // If the value returned is null, then the address is not available to the
  // filter.
  virtual const DataType* data_ptr() const;
  //: Returns a writable pointer to the ``real'' section descriptor.
  // If this method is called on a concrete instance, it should return 0.
  virtual vipl_section_descriptor< DataType >* inner_descriptor();
  //:
  // Returns a const pointer to the ``real'' section descriptor. If this method
  // is called on a concrete instance, it should return 0.
  virtual const vipl_section_descriptor< DataType >* inner_descriptor() const;
  //:
  // Makes a new correct copy. It's just a bit more tricky because descriptors
  // have pointers to its ``real instance.''
 protected:
  virtual vipl_section_descriptor< DataType >* virtual_copy() const;
#endif
 public:
  vipl_section_descriptor< DataType >* real_descriptor() const { return hsreal_descriptor; }
  vipl_section_descriptor< DataType >* & ref_real_descriptor() { return hsreal_descriptor; }
  void put_real_descriptor( vipl_section_descriptor< DataType >* v) { hsreal_descriptor = v; }
 public: vipl_section_container< DataType >* real_container() const { return hsreal_container; }
 public: vipl_section_container< DataType >* & ref_real_container() { return hsreal_container; }
 public: void put_real_container( vipl_section_container< DataType >* v) { hsreal_container = v; }
 public: DataType* i_data_ptr() const { return hsi_data_ptr; }
 public: DataType* & ref_i_data_ptr() { return hsi_data_ptr; }
 public: void put_i_data_ptr( DataType* v) { hsi_data_ptr = v; }
 public: std::vector< int > const & i_data_offsets() const { return hsi_data_offsets; }
 public: std::vector< int > & ref_i_data_offsets() { return hsi_data_offsets; }
 public: void put_i_data_offsets( std::vector< int > const & v) { hsi_data_offsets = v; }
 public: std::vector< int > const & i_curr_sec_start() const { return hsi_curr_sec_start; }
 public: std::vector< int > & ref_i_curr_sec_start() { return hsi_curr_sec_start; }
 public: void put_i_curr_sec_start( std::vector< int > const & v) { hsi_curr_sec_start = v; }
 public: std::vector< int > const & i_curr_sec_end() const { return hsi_curr_sec_end; }
 public: std::vector< int > & ref_i_curr_sec_end() { return hsi_curr_sec_end; }
 public: void put_i_curr_sec_end( std::vector< int > const & v) { hsi_curr_sec_end = v; }
 public: std::vector< int > const & i_curr_sec_size() const { return hsi_curr_sec_size; }
 public: std::vector< int > & ref_i_curr_sec_size() { return hsi_curr_sec_size; }
 public: void put_i_curr_sec_size( std::vector< int > const & v) { hsi_curr_sec_size = v; }
}; // end of class definition

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_section_descriptor_2d.hxx"
#endif

#endif // vipl_section_descriptor_2d_h_
