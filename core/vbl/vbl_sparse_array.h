#ifndef vbl_sparse_array_h_
#define vbl_sparse_array_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_sparse_array.h

//:
// \file
// \brief Sparse array allowing space efficient access of the form s[3000]=2.
// \author  Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
// \verbatim
// Modifications:
//     230497 AWF Converted from vbl_sparse_array3D
// \endverbatim
//---------------------------------------------------------------------------

#include <vcl_functional.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>

template <class T>
//: Sparse array allowing space efficient access of the form s[3000] = 2;
class vbl_sparse_array {
public:
  typedef vcl_map<unsigned, T, vcl_less<unsigned> > Map;

  typedef typename Map::const_iterator const_iterator;

  //: Construct a vbl_sparse_array which can hold up to MAXINT elements.
  vbl_sparse_array() {}
  // ~vbl_sparse_array() {} - defaults will do
  //vbl_sparse_array(const vbl_sparse_array<T>&);
  //vbl_sparse_array& operator=(const vbl_sparse_array<T>&);

  // Operations----------------------------------------------------------------
  //: Return contents at (i)
  T      & operator [] (unsigned i) { return storage_[i]; }
  //: Return contents at (i).  Assertion failure if not yet filled.
  T const& operator [] (unsigned i) const;

  //: Return true if location (i) has been filled.
  bool fullp(unsigned i) const;
  //: Put a value into location (i).
  bool put(unsigned i, const T& t);
  //: Return the address of location (i).  0 if not yet filled.
  T* get_addr(unsigned);

  // Computations--------------------------------------------------------------
  //: Return number of locations that have been assigned a value using "put".
  unsigned count_nonempty() const { return storage_.size(); }

  const_iterator begin() const { return storage_.begin(); }
  const_iterator end() const { return storage_.end(); }

  // Data Control--------------------------------------------------------------
  //: Print the Array to a stream in "(i): value" format.
  vcl_ostream& print(vcl_ostream&) const;
  //  friend ostream& operator >> (ostream&, const vbl_sparse_array<T>& );

protected:
  // Data Members--------------------------------------------------------------
  Map storage_;
};

template <class T>
inline vcl_ostream& operator << (vcl_ostream& s, const vbl_sparse_array<T>& a)
{
  return a.print(s);
}

#define VBL_SPARSE_ARRAY_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array.txx instead"

#endif // vbl_sparse_array_h_
