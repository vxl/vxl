// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_INDEX_VECTOR_H
#define VIL_NITF2_INDEX_VECTOR_H

#include <ostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// A wrapper for a vector of ints that represents an index into
// a repeating field. This class serves two simple purposes: it
// provides some convenient constructors from scalars (remember the
// lisp function "list"?), and it defines operator<< (std::ostream&).

class vil_nitf2_index_vector : public std::vector<int>
{
 public:
  // Convenience constructors: empty vector, 1 element, etc.
  vil_nitf2_index_vector() : std::vector<int>() {}
  vil_nitf2_index_vector(int i): std::vector<int>(1) {
    (*this)[0] = i; }
  vil_nitf2_index_vector(int i, int j) : std::vector<int>(2) {
    (*this)[0] = i; (*this)[1] = j; }
  vil_nitf2_index_vector(int i, int j, int k) : std::vector<int>(3) {
    (*this)[0] = i; (*this)[1] = j; (*this)[2] = k; }
  vil_nitf2_index_vector(int i, int j, int k, int l) : std::vector<int>(4) {
    (*this)[0] = i; (*this)[1] = j; (*this)[2] = k; (*this)[3] = l; }

  // General-purpose constructor
  vil_nitf2_index_vector(const std::vector<int>& v) : std::vector<int>(v) {}

  // Destructor
  virtual ~vil_nitf2_index_vector() = default;
};

inline std::ostream& operator << (std::ostream& os, const vil_nitf2_index_vector& vec)
{
  os << '(';
  for (vil_nitf2_index_vector::const_iterator it = vec.begin(); it != vec.end(); ++it) {
    if (it != vec.begin()) os << ", ";
    os << *it;
  }
  os << ')';
  return os;
}

#endif // VIL_NITF2_INDEX_VECTOR_H
