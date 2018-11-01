// This is tbl/vipl/section/vipl_section_container.h
#ifndef vipl_section_container_h_
#define vipl_section_container_h_

#include <vector>
#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <  class DataType > class vipl_section_descriptor; //template forward reference
template <  class DataType > class vipl_section_iterator; //template forward reference

template < class DataType >
class vipl_section_container
{
  friend class vipl_section_descriptor< DataType > ; //declare a friend class
  friend class vipl_section_iterator< DataType > ; //declare a friend class
   // STL container typedefs
 public:
  typedef vipl_section_descriptor< DataType > value_type;
  typedef vipl_section_descriptor< DataType >& reference;
  typedef const vipl_section_descriptor< DataType >& const_reference;
  typedef vipl_section_descriptor< DataType >* pointer;
  typedef vipl_section_iterator< DataType > iterator;
  typedef const vipl_section_iterator< DataType > const_iterator;
  typedef std::ptrdiff_t difference_type;
  typedef std::size_t size_type;
 protected:
  // declare data, accessors from codegen
  vipl_section_container< DataType >* hsthe;
  // allow for subclasses with pass-by-value
  std::vector< int > hsimgsz;
  // the dimensions of image
  std::vector< int > hsimgstart;
  // the starting index of image
  std::vector< int > hssecsz;
  // the size of a "normal" section
  std::vector< int > hsoverlap;
  // the amount to overlap sections
  DataType* hsrawdata;
  // pointer to raw block of data for section (may ==0)
  const void* hsimgptr;
  // (void) pointer to users "image", we just carry it

 public:
  // The pointer ``the'' is just assigned
  vipl_section_container(vipl_section_container< DataType >* pt);
  // The pointer is deep copied
  vipl_section_container(const vipl_section_container< DataType >* pt , int t);
  virtual ~vipl_section_container(); //low_level destructor
  vipl_section_container(); //low_level c++ constructor
  //user accessed low_level c++ copy constructor
  vipl_section_container(const vipl_section_container< DataType > &);
  virtual vipl_section_iterator< DataType > begin();
  virtual vipl_section_iterator< DataType > end();
  virtual const vipl_section_iterator< DataType > begin() const;
  virtual const vipl_section_iterator< DataType > end() const;

  // True only if the internal filterable image associated with
  // b is the same as for this.
  virtual bool operator==( const vipl_section_container< DataType >& b) const;

  // STL demands a few methods
  virtual bool operator!=( const vipl_section_container< DataType >& b) const;

  // STL demands this
  virtual vipl_section_container<DataType>& operator=( const vipl_section_container< DataType >& b);

  virtual int size() const ;
  virtual int max_size() const ;
  virtual bool empty() ;
  // None of the comparison operators are implemented yet. FIXME
  virtual bool operator<( const vipl_section_container< DataType >& b) const ;
  virtual bool operator>( const vipl_section_container< DataType >& b) const ;
  virtual bool operator<=( const vipl_section_container< DataType >& b) const ;
  virtual bool operator>=( const vipl_section_container< DataType >& b) const ;

  // not implemented
  virtual void swap( const vipl_section_container< DataType >& b) const ;

  // Modifies the passed in descriptor to point to the next section.
  // This is used by the default iterators for the operator++ method.
  // Returns TRUE if successfully incremented the variable.
  // Used so we don't have to copy descriptors a zillion times.
  // Unfortunately STL usage implies we do copy more often than we'd like.
 protected:
  virtual bool next_section( vipl_section_descriptor< DataType >& in_out) const ;

  // Given the axis, returns the starting coordinate of the related
  // image in the specified axis. The values of \usearg {axis}
  // increase from 0. Rationale for this is the span of the image
  // in pixels in the first (i.e. 0) axis, the second (i.e. 1) axis
  // etc... The Axes have const values in the class filter,
  // e.g. vipl_filter::X_Axis
 public:
  virtual int image_start( int axis) const ;

  // Given the axis, returns the size of the related image in the
  // specified axis. The values of \usearg {axis} increase from
  // 0. Rationale for this is the span of the image in pixels in
  // the first (i.e. 0) axis, the second (i.e. 1) axis etc...
  virtual int image_size( int axis) const ;

  // Given the axis, returns the end coordinate of the related
  // image in the specified axis. The values of \usearg {axis}
  // increase from 0. Rationale for this is the span of the image
  // in pixels in the first (i.e. 0) axis, the second (i.e. 1) axis
  // etc...
  virtual int image_end( int axis) const ;

  // Given the axis, returns the size of the related section size
  // in the specified axis. The values of \usearg {axis} increase
  // from 0. Rationale for this is the span of the image in pixels
  // in the first (i.e. 0) axis, the second (i.e. 1) axis etc... If
  // the section container does not use fixed sized sections then
  // this should return -1
  virtual int section_size( int axis) const ;

  // Returns true if the \useclass {section_descriptor}s will have
  // valid pointer values. By default they are if and only if the
  // raw_data_pointer is set.
  virtual bool is_pointer_safe() const ;

  // Does the correct copy. It's a bit tricky due to the fact that
  // an instance has a pointer to its ``real instance''.
 protected:
  virtual vipl_section_container< DataType >* virtual_copy() const ;
 public:
  vipl_section_container< DataType >* the() const{ return hsthe;}
  vipl_section_container< DataType >* & ref_the(){ return hsthe;}
  void put_the( vipl_section_container< DataType >* v){ hsthe = v;}
  std::vector< int > const & imgsz() const{ return hsimgsz;}
  std::vector< int > & ref_imgsz(){ return hsimgsz;}
  void put_imgsz( std::vector< int > const & v){ hsimgsz = v;}
  std::vector< int > const & imgstart() const{ return hsimgstart;}
  std::vector< int > & ref_imgstart(){ return hsimgstart;}
  void put_imgstart( std::vector< int > const & v){ hsimgstart = v;}
  std::vector< int > const & secsz() const{ return hssecsz;}
  std::vector< int > & ref_secsz(){ return hssecsz;}
  void put_secsz( std::vector< int > const & v){ hssecsz = v;}
  std::vector< int > const & overlap() const{ return hsoverlap;}
  std::vector< int > & ref_overlap(){ return hsoverlap;}
  void put_overlap( std::vector< int > const & v){ hsoverlap = v;}
  DataType* rawdata() const{ return hsrawdata;}
  DataType* & ref_rawdata(){ return hsrawdata;}
  void put_rawdata( DataType* v){ hsrawdata = v;}
  const void* imgptr() const{ return hsimgptr;}
  const void* & ref_imgptr(){ return hsimgptr;}
  void put_imgptr( const void* v){ hsimgptr = v;}

  // refcounting:
 private: int refcount_;
 public:  int refcount() const { return refcount_; }
 public:  int inc_refcount() { return ++refcount_; }
 public:  int dec_refcount() { if (refcount_<=1) { delete this; return 0; } return --refcount_; }

}; // end of class definition

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_section_container.hxx"
#endif

#endif // vipl_section_container_h_
