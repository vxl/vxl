#ifndef vbl_sparse_array_h_
#define vbl_sparse_array_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vbl_sparse_array - Sparse array allowing space efficient access of the form s[3000] = 2;
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_sparse_array.h
// .FILE	vbl_sparse_array.txx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
//
// .SECTION Modifications:
//     230497 AWF Converted from vbl_sparse_array3D
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_functional.h>
#include <vcl/vcl_map.h>
#include <vcl/vcl_utility.h>
#include <vcl/vcl_iostream.h>

//
template <class T>
class vbl_sparse_array {
public:
  typedef vcl_map<unsigned, T, vcl_less<unsigned> > Map;

  typedef typename Map::const_iterator const_iterator;

// -- Construct a vbl_sparse_array which can hold up to MAXINT elements.
  vbl_sparse_array() {}
  // ~vbl_sparse_array() {} - defaults will do
  //vbl_sparse_array(const vbl_sparse_array<T>&);
  //vbl_sparse_array& operator=(const vbl_sparse_array<T>&);

  // Operations----------------------------------------------------------------
  T      & operator [] (unsigned i) { return storage_[i]; }
  T const& operator [] (unsigned) const;

  bool fullp(unsigned) const;
  bool put(unsigned, const T&);
  T* get_addr(unsigned);

  // Computations--------------------------------------------------------------
// -- Return number of locations that have been assigned a value using "put".
  unsigned count_nonempty() const { return storage_.size(); }

  const_iterator begin() const { return storage_.begin(); }
  const_iterator end() const { return storage_.end(); }

  // Data Control--------------------------------------------------------------
  vcl_ostream& print(vcl_ostream&) const;
  //  friend ostream& operator >> (ostream&, const vbl_sparse_array<T>& );

protected:
  // Data Members--------------------------------------------------------------
  Map storage_;
};

template <class T>
inline ostream& operator << (ostream& s, const vbl_sparse_array<T>& a)
{
  return a.print(s);
}

#define VBL_SPARSE_ARRAY_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array.txx instead"

#endif // vbl_sparse_array_h_
