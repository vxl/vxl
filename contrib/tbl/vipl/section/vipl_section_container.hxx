// This is tbl/vipl/section/vipl_section_container.hxx
#ifndef vipl_section_container_hxx_
#define vipl_section_container_hxx_

#include <iostream>
#include "vipl_section_container.h"
#include <vipl/filter/vipl_filter_abs.h> // for filter_abs::X_Axis()
#include <vipl/section/vipl_section_iterator.hxx>
#include <vipl/section/vipl_section_descriptor.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// The pointer ``the'' is just assigned
template < class DataType >
  vipl_section_container< DataType > ::vipl_section_container( vipl_section_container< DataType >* pt)
  : hsthe(pt),
    hsimgsz (2,0),
    hsimgstart (2,0),
    hssecsz (2,0),
    hsoverlap (2,0),
    hsrawdata (nullptr),
    hsimgptr (nullptr),
    refcount_ (1)
{}

// The pointer is deep copied
template < class DataType >
  vipl_section_container< DataType > ::vipl_section_container( const vipl_section_container< DataType >* pt , int t)
  : hsthe(0),
    hsimgsz (2,0),
    hsimgstart (2,0),
    hssecsz (2,0),
    hsoverlap (2,0),
    hsrawdata (0),
    hsimgptr (nullptr),
    refcount_ (1)
{
  hsthe = pt->virtual_copy();
}

template < class DataType >
  vipl_section_container< DataType > ::~vipl_section_container()
{
// if we have initialized meta_class, free soft slots. Do Not delete this
  if (hsthe && (hsthe != this)) FILTER_IMPTR_DEC_REFCOUNT(hsthe);
}

template < class DataType >
  vipl_section_container< DataType > ::vipl_section_container()
  : hsthe(this),
    hsimgsz (2,0),
    hsimgstart (2,0),
    hssecsz (2,0),
    hsoverlap (2,0),
    hsrawdata (0),
    hsimgptr (nullptr),
    refcount_ (1)
// C++ auto-generated low-level constructor
{}

template < class DataType >
  vipl_section_container< DataType > ::vipl_section_container(const vipl_section_container< DataType > &t)
  : hsthe(0),
    hsimgsz(t.hsimgsz),
    hsimgstart(t.hsimgstart),
    hssecsz(t.hssecsz),
    hsoverlap(t.hsoverlap),
    hsrawdata(t.hsrawdata),
    hsimgptr(t.hsimgptr),
    refcount_ (1)
// C++ auto-generated low-level copy constructor
{
  // you can fill special ``copy constructor'' stuff here.
  // All dynamic/soft attributes are copied. Thus your
  //code should not change any soft attributes (if you
  //want to change it here is should be hard because it
  //is always changed!) So don't change things without
  //knowing their form.
  hsthe = t.hsthe->virtual_copy();
}

template < class DataType >
   vipl_section_iterator< DataType > vipl_section_container< DataType > ::begin()
{
  vipl_section_descriptor<DataType> *ptr = new vipl_section_descriptor<DataType>(nullptr, virtual_copy());
  ptr->put_real_descriptor(ptr);
  ptr->ref_i_curr_sec_start()[0] = 0;
  ptr->ref_i_curr_sec_start()[1] = 0;
  ptr->ref_i_curr_sec_end()[0] = secsz()[0] + overlap()[0];
  ptr->ref_i_curr_sec_end()[1] = secsz()[1] + overlap()[1];
  if (ptr->ref_i_curr_sec_end()[0] > image_size(0))
    ptr->ref_i_curr_sec_end()[0] = image_size(0);
  if (ptr->ref_i_curr_sec_end()[1] > image_size(1))
    ptr->ref_i_curr_sec_end()[1] = image_size(1);
  ptr->ref_i_curr_sec_size()[0] = ptr->ref_i_curr_sec_end()[0];
  ptr->ref_i_curr_sec_size()[1] = ptr->ref_i_curr_sec_end()[1];
  vipl_section_iterator<DataType> i(ptr, virtual_copy());
  return i;
}

template < class DataType >
   vipl_section_iterator< DataType > vipl_section_container< DataType > ::end()
{
  // the "end" iterator has 0 as the real_descriptor
  vipl_section_iterator<DataType> i(nullptr, virtual_copy());
  return i;
}

template < class DataType >
  const vipl_section_iterator< DataType > vipl_section_container< DataType > ::begin() const
{
  vipl_section_descriptor<DataType> *ptr =
    new vipl_section_descriptor<DataType>(nullptr, virtual_copy());
  ptr->put_real_descriptor(ptr);
  ptr->ref_i_curr_sec_start()[0] = 0;
  ptr->ref_i_curr_sec_start()[1] = 0;
  if (ptr->ref_i_curr_sec_end()[0] > image_size(0))
    ptr->ref_i_curr_sec_end()[0] = image_size(0);
  if (ptr->ref_i_curr_sec_end()[1] > image_size(1))
    ptr->ref_i_curr_sec_end()[1] = image_size(1);
  ptr->ref_i_curr_sec_size()[0] = ptr->ref_i_curr_sec_end()[0];
  ptr->ref_i_curr_sec_size()[1] = ptr->ref_i_curr_sec_end()[1];
  vipl_section_iterator<DataType> i(ptr, virtual_copy());
  return i;
}

template < class DataType >
  const vipl_section_iterator< DataType > vipl_section_container< DataType > ::end() const
{
  // the "end" iterator has 0 as the real_descriptor
  vipl_section_iterator<DataType> i(nullptr, virtual_copy());
  return i;
}

// True only if the internal filterable image associated with b is the same as for this.
template < class DataType >
  bool vipl_section_container< DataType > ::operator==( const vipl_section_container< DataType >& b) const
{
  if (this == &b) return true;
  // silly, the only time the test below is true is the test above is true!
  // if (hsthe == b.hsthe) return true;
  // FIXME below
  return hsimgptr == b.hsimgptr;
}

template < class DataType >
  bool vipl_section_container< DataType > ::operator!=( const vipl_section_container< DataType >& b) const
{
  return !(*this == b);
}

template < class DataType >
  vipl_section_container< DataType >& vipl_section_container< DataType > ::operator=( const vipl_section_container< DataType >& b)
{
  if (this != &b) {
    if (hsthe != this) FILTER_IMPTR_DEC_REFCOUNT(hsthe);
    hsthe = b.hsthe->virtual_copy();
    // FIXME - the following is a protoDataType hack
    put_imgsz(b.imgsz());
    put_secsz(b.secsz());
  }
  return *this;
}

template < class DataType >
  int vipl_section_container< DataType > ::size() const
{
  return ((imgsz()[0] / secsz()[0]) + ((imgsz()[0] % secsz()[0]) ? 1 : 0)) *
    ((imgsz()[1] / secsz()[1]) + ((imgsz()[1] % secsz()[1]) ? 1 : 0));
}

template < class DataType >
  int vipl_section_container< DataType > ::max_size() const
{ return size(); }

template < class DataType >
  bool vipl_section_container< DataType > ::empty()
{ return false; }

// None of the comparison operators are implemented yet. FIXME

template < class DataType >
  bool vipl_section_container< DataType > ::operator<( const vipl_section_container< DataType >& ) const
{
  std::cerr << "Warning: called unimplemented method vipl_section_container::operator<\n";
  return false;
}

template < class DataType >
  bool vipl_section_container< DataType > ::operator>( const vipl_section_container< DataType >& ) const
{
  std::cerr << "Warning: called unimplemented method vipl_section_container::operator>\n";
  return false;
}

template < class DataType >
  bool vipl_section_container< DataType > ::operator<=( const vipl_section_container< DataType >& ) const
{
  std::cerr << "Warning: called unimplemented method vipl_section_container::operator<=\n";
  return false;
}

template < class DataType >
  bool vipl_section_container< DataType > ::operator>=( const vipl_section_container< DataType >& ) const
{
  std::cerr << "Warning: called unimplemented method vipl_section_container::operator>=\n";
  return false;
}

template < class DataType >
  void vipl_section_container< DataType > ::swap( const vipl_section_container< DataType >& ) const
{
  std::cerr << "Warning: called unimplemented method vipl_section_container::swap\n";
}

// Modifies the passed in descriptor to point to the next section . This is
// used by the default iterators for the operator++ method. Returns TRUE if
// successfully incremented the variable. Used so we don't have to copy
// descriptors a zillion times. Unfortunately STL usage implies we do copy
// more often then we'd like.
// This might seem like a method for the iterator but we want to keep that
// class more "pure" (it would need too much state)
template < class DataType >
  bool vipl_section_container< DataType > ::next_section(
  vipl_section_descriptor< DataType >& in_out)
  const
{
  // alg: if section is at the right edge, then go to the next row
  // next-row if section is at the bottom edge call
  // bogus prototype alert! FIXME
  // This function should really be pure
  bool incremented_Y=false, incremented_X=false;
#if 0 // commented out
  int xi = image_size(vipl_filter_abs::X_Axis()) - in_out.curr_sec_end(vipl_filter_abs::X_Axis());
  int yi = image_size(vipl_filter_abs::Y_Axis()) - in_out.curr_sec_end(vipl_filter_abs::Y_Axis());
  if (xi > section_size(vipl_filter_abs::X_Axis())) xi = section_size(vipl_filter_abs::X_Axis());
  if (yi > section_size(vipl_filter_abs::Y_Axis())) yi = section_size(vipl_filter_abs::Y_Axis());
  // this method should really be defined at a more concrete class
  // as is, this just a hack to have sections up and running with
  // scalar_image_2d_of<FOO>
  // first increment X if possible
  if (xi > 0) {
    incremented_X = 1;
    in_out.ref_i_curr_sec_start()[vipl_filter_abs::X_Axis()] += xi;
    in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()] += xi;
    in_out.ref_i_curr_sec_size()[vipl_filter_abs::X_Axis()] = xi;
  }
  else if (yi > 0) {
    incremented_Y = 1;
    // typewriter "spring"
    in_out.ref_i_curr_sec_start()[vipl_filter_abs::X_Axis()] = 0;
    in_out.ref_i_curr_sec_start()[vipl_filter_abs::Y_Axis()] += yi;
    in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()] = xi;
    in_out.ref_i_curr_sec_end()[vipl_filter_abs::Y_Axis()] += yi;
    in_out.ref_i_curr_sec_size()[vipl_filter_abs::X_Axis()] = xi;
    in_out.ref_i_curr_sec_size()[vipl_filter_abs::Y_Axis()] = yi;
  }
#endif
  //int xs = in_out.curr_sec_start(vipl_filter_abs::X_Axis());
  //int ys = in_out.curr_sec_start(vipl_filter_abs::Y_Axis());
  int xi = image_size(vipl_filter_abs::X_Axis())
           - in_out.curr_sec_end(vipl_filter_abs::X_Axis());
  int yi = image_size(vipl_filter_abs::Y_Axis())
           - in_out.curr_sec_end(vipl_filter_abs::Y_Axis());
  if (xi > section_size(vipl_filter_abs::X_Axis()))
    xi = section_size(vipl_filter_abs::X_Axis());
  if (yi > section_size(vipl_filter_abs::Y_Axis()))
    yi = section_size(vipl_filter_abs::Y_Axis());
  // this method should really be defined at a more concrete class
  // as is, this just a hack to have sections up and running with
  // scalar_image_2d_of<FOO>
  // first increment X if possible
  if (xi > 0) {
    incremented_X = true;
    // if start != 0 we just increment it by size, else we add overlap
    if (in_out.ref_i_curr_sec_start()[vipl_filter_abs::X_Axis()] > 0){
      in_out.ref_i_curr_sec_start()[vipl_filter_abs::X_Axis()]
        += section_size(vipl_filter_abs::X_Axis());
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()]
        += section_size(vipl_filter_abs::X_Axis());
    }
    else {
      // we are in the "first" block so we have to include overlap stuff
      in_out.ref_i_curr_sec_start()[vipl_filter_abs::X_Axis()]
        += section_size(vipl_filter_abs::X_Axis())
        - overlap()[vipl_filter_abs::X_Axis()];
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()]
        = in_out.ref_i_curr_sec_start()[vipl_filter_abs::X_Axis()]
        + section_size(vipl_filter_abs::X_Axis())
        + 2*overlap()[vipl_filter_abs::X_Axis()];
    }
    if (in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()]
        > image_size(vipl_filter_abs::X_Axis()))
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()] =
        image_size(vipl_filter_abs::X_Axis());
    in_out.ref_i_curr_sec_size()[vipl_filter_abs::X_Axis()] =
      in_out.i_curr_sec_end()[vipl_filter_abs::X_Axis()]
      - in_out.i_curr_sec_start()[vipl_filter_abs::X_Axis()];
  }
  else if (yi > 0) {
    incremented_Y = true;
    // typewriter "spring"
    in_out.ref_i_curr_sec_start()[vipl_filter_abs::X_Axis()] = 0;
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()]
        = section_size(vipl_filter_abs::X_Axis())
        + overlap()[vipl_filter_abs::X_Axis()];
    // if y != 0 we increment, else we must include overlap as well
    if (in_out.ref_i_curr_sec_start()[vipl_filter_abs::Y_Axis()] > 0 ){
      in_out.ref_i_curr_sec_start()[vipl_filter_abs::Y_Axis()] +=
        section_size(vipl_filter_abs::Y_Axis());
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::Y_Axis()] +=
        section_size(vipl_filter_abs::Y_Axis());
    }
    else {
      in_out.ref_i_curr_sec_start()[vipl_filter_abs::Y_Axis()] =
        section_size(vipl_filter_abs::Y_Axis())
        - overlap()[vipl_filter_abs::Y_Axis()];
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::Y_Axis()] =
        in_out.ref_i_curr_sec_start()[vipl_filter_abs::Y_Axis()]
        + section_size(vipl_filter_abs::Y_Axis())
        + 2*overlap()[vipl_filter_abs::Y_Axis()];
    }
    in_out.ref_i_curr_sec_size()[vipl_filter_abs::X_Axis()] =
      in_out.i_curr_sec_end()[vipl_filter_abs::X_Axis()]
      - in_out.i_curr_sec_start()[vipl_filter_abs::X_Axis()];
    in_out.ref_i_curr_sec_size()[vipl_filter_abs::Y_Axis()] =
      in_out.i_curr_sec_end()[vipl_filter_abs::Y_Axis()]
      - in_out.i_curr_sec_start()[vipl_filter_abs::Y_Axis()];
    if (in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()]
        > image_size(vipl_filter_abs::X_Axis()))
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::X_Axis()]
        = image_size(vipl_filter_abs::X_Axis());
    if (in_out.ref_i_curr_sec_end()[vipl_filter_abs::Y_Axis()]
        > image_size(vipl_filter_abs::Y_Axis()))
      in_out.ref_i_curr_sec_end()[vipl_filter_abs::Y_Axis()] =
        image_size(vipl_filter_abs::Y_Axis());
  }
  if (in_out.real_container() == the()) {
    std::cout << "next_section() for 0x" << (void *)&in_out << std::endl;
  }
  return incremented_Y | incremented_X;
}

// Given the axis, returns the starting coordinate of the related image in
// the specified axis. The values of \usearg {axis} increase from
// 0. Rationale for this is the span of the image in pixels in the first
// (i.e. 0) axis, the second (i.e. 1) axis etc... The Axes have const values
// in the class filter, e.g. vipl_filter::X_Axis
template < class DataType >
  int vipl_section_container< DataType > ::image_start(int /*axis*/) const
{
  return 0;
}

// Given the axis, returns the size of the related image in the specified
// axis. The values of \usearg {axis} increase from 0. Rationale for this is
// the span of the image in pixels in the first (i.e. 0) axis, the second
// (i.e. 1) axis etc...
template < class DataType >
  int vipl_section_container< DataType > ::image_size( int axis) const
{
  return imgsz()[axis];
}

// Given the axis, returns the end coordinate of the related image in the
// specified axis. The values of \usearg {axis} increase from 0. Rationale
// for this is the span of the image in pixels in the first (i.e. 0) axis,
// the second (i.e. 1) axis etc...
template < class DataType >
  int vipl_section_container< DataType > ::image_end( int axis) const
{
  return imgsz()[axis] +
   imgstart()[axis];
 }

// Given the axis, returns the size of the related section size in the
// specified axis. The values of \usearg {axis} increase from 0. Rationale
// for this is the span of the image in pixels in the first (i.e. 0) axis,
// the second (i.e. 1) axis etc... If the section container does not use
// fixed sized sections then this should return -1

template < class DataType >
  int vipl_section_container< DataType > ::section_size( int axis) const
{
  return secsz()[axis];
}

// Returns true if the \useclass {section_descriptor}s will have valid
// pointer values. By default they are if and only if the raw_data_pointer is
// set.
template < class DataType >
  bool vipl_section_container< DataType > ::is_pointer_safe() const
{
  return false;
}

// Does the correct copy. It's a bit tricky due to the fact that an instance
// has a pointer to its ``real instance''.
template < class DataType >
   vipl_section_container< DataType >* vipl_section_container< DataType > ::virtual_copy() const
{
  vipl_section_container< DataType >*rtn = new vipl_section_container<DataType> (nullptr);
  rtn->put_the(rtn);
  rtn->put_imgsz(imgsz());
  rtn->put_secsz(secsz());
  rtn->put_imgptr(imgptr());
  rtn->put_overlap(overlap());
  return rtn;
}

#endif // vipl_section_container_hxx_
