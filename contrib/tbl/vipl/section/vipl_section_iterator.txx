// This is tbl/vipl/section/vipl_section_iterator.txx
#ifndef vipl_section_iterator_txx_
#define vipl_section_iterator_txx_
//:
//  \file

#include "vipl_section_iterator.h"
#include <vipl/section/vipl_section_container.h>
#include <vipl/section/vipl_section_descriptor.h>
#include <vipl/filter/vipl_filter_abs.h> // for default def of FILTER_IMPTR_INC_REFCOUNT
#if 0
#include <vcl_iostream.h> // for error message stuff
#endif

#ifdef VCL_VC
// Disable complaints about empty controlled statements (from blank macro expansion)
# pragma warning( push )
# pragma warning( disable : 4390 )
#endif

// Assigns the pointer directly. Does not deep copy them.
template < class DataType >
  vipl_section_iterator< DataType > ::vipl_section_iterator(
                   vipl_section_descriptor< DataType >* desc ,
                 vipl_section_container< DataType >* containr)
  : hsreal_descriptor(desc),
    hscontainer(containr),
    hsincr_count(0)
{
  if (desc) FILTER_IMPTR_INC_REFCOUNT(desc);
  if (containr) FILTER_IMPTR_INC_REFCOUNT(containr);
#if 0
  vcl_cerr << "Warning: called unimplemented constructor with signature "
           << "vipl_section_descriptor< DataType >* desc, vipl_section_container< DataType >* containr\n";
#endif
}

#ifdef VCL_VC
// Disable complaints about empty controlled statements (from blank macro expansion)
# pragma warning( pop )
# pragma warning( disable : 4390 )
#endif


// Deep copies the pointers.
template < class DataType >
vipl_section_iterator< DataType > ::vipl_section_iterator(
                 vipl_section_descriptor< DataType >* desc ,
                 vipl_section_container< DataType >* containr ,
                int t)
  : hsreal_descriptor(0),
    hscontainer(0),
    hsincr_count(0)
{
  hsreal_descriptor = desc->virtual_copy();
  hscontainer = containr->virtual_copy();
}

template < class DataType >
  vipl_section_iterator< DataType > ::~vipl_section_iterator()
{
  if (ref_container()) FILTER_IMPTR_DEC_REFCOUNT(ref_container());
  if (ref_real_descriptor()) FILTER_IMPTR_DEC_REFCOUNT(ref_real_descriptor());
#if 0
  vcl_cerr << "Warning: called unimplemented vipl_section_iterator destructor\n";
#endif
}

template < class DataType >
  vipl_section_iterator< DataType > ::vipl_section_iterator()
  : hsreal_descriptor(0),
    hscontainer(0),
    hsincr_count(0)
// C++ auto-generated low-level constructor
{
}

template < class DataType >
  vipl_section_iterator< DataType > ::vipl_section_iterator(const vipl_section_iterator< DataType > &t)
  : hsreal_descriptor(0),
    hscontainer(0),
    hsincr_count(t.hsincr_count)
// C++ auto-generated low-level copy constructor
{
  // you can fill special ``copy constructor'' stuff here.
  // All dynamic/soft attributes are copied. Thus your
  //want to change it here is should be hard because it
  //is always changed! So don't change things without
  //knowing their form.
  if (t.hsreal_descriptor)
    hsreal_descriptor = t.hsreal_descriptor->virtual_copy();
  if (t.hscontainer)
    hscontainer = t.hscontainer->virtual_copy();
}

template < class DataType >
   vipl_section_iterator< DataType >& vipl_section_iterator< DataType > ::operator++()
{
  // for the time being, since section-size is the same as the image-size
  if (ref_container()->next_section(*ref_real_descriptor()))
    ++ref_incr_count();
  else {
    // delete the real_descriptor and set that slot to zero so that it will
    // equal the end iterator
    FILTER_IMPTR_DEC_REFCOUNT(ref_real_descriptor());
  }
  return *this;
}

// postfix
template < class DataType >
vipl_section_iterator< DataType > vipl_section_iterator< DataType > ::operator++(int )
{
  vipl_section_iterator<DataType> rtn(*this);
  ++(*this);
  return rtn;
}

// Get the descriptor for the current section
template < class DataType >
   vipl_section_descriptor< DataType > vipl_section_iterator< DataType > ::operator*()
{
  return *real_descriptor();
}

//:
// The equality test is true if this and p have the same address, or
// if this and p both have null descriptors. If this XOR p have null descriptor
// it is false. Finally, if this and p have linked containers which are equal,
// AND the iterator's ++ counts are the same, it is true, otherwise false.
template < class DataType >
  bool vipl_section_iterator< DataType > ::operator==( const vipl_section_iterator< DataType >& p) const
{
  if (this == &p) return true;
  // the following is equivalent to testing for NULL, as that is the only time
  // that the descriptor pointers will have the same value.
  if (real_descriptor() == p.real_descriptor()) return true;
  else if ((! real_descriptor()) ^ (! p.real_descriptor()))
    return false;
  if (*p.container() == *container() &&
      p.incr_count() == incr_count()) return true;
  return false;
}

template < class DataType >
  bool vipl_section_iterator< DataType > ::operator!=( const vipl_section_iterator< DataType >& p) const
{
  return !(*this == p);
}

template < class DataType >
   vipl_section_iterator< DataType >& vipl_section_iterator< DataType > ::operator=(
                   const
  vipl_section_iterator< DataType >& p)
{
  if (&p != this) {
    if (hscontainer) FILTER_IMPTR_DEC_REFCOUNT(hscontainer);
    if (hsreal_descriptor) FILTER_IMPTR_DEC_REFCOUNT(hsreal_descriptor);
    if (p.hsreal_descriptor) // end itr has 0 as real_descriptor
      hsreal_descriptor = p.hsreal_descriptor->virtual_copy();
    hscontainer = p.hscontainer->virtual_copy();
  }
  return *this;
}

#endif // vipl_section_iterator_txx_
