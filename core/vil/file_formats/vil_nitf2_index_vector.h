// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_INDEX_VECTOR_H
#define VIL_NITF2_INDEX_VECTOR_H

#include <vcl_ostream.h>

// A wrapper for a vector of ints that represents an index into
// a repeating field. This class serves two simple purposes: it 
// provides some convenient constructors from scalars (remember the
// lisp function "list"?), and it defines operator<< (vcl_ostream&).

class vil_nitf2_index_vector : public vcl_vector<int>
{
public:
  // Convenience constructors: empty vector, 1 element, etc.
  vil_nitf2_index_vector() : vcl_vector<int>() {}
  vil_nitf2_index_vector(int i): vcl_vector<int>(1) {
    this->at(0) = i; }
  vil_nitf2_index_vector(int i, int j) : vcl_vector<int>(2) {
    this->at(0) = i; this->at(1) = j; }
  vil_nitf2_index_vector(int i, int j, int k) : vcl_vector<int>(3) {
    this->at(0) = i; this->at(1) = j; this->at(2) = k; }
  vil_nitf2_index_vector(int i, int j, int k, int l) : vcl_vector<int>(4) {
    this->at(0) = i; this->at(1) = j; this->at(2) = k; this->at(3) = l; }

  // General-purpose constructor 
  vil_nitf2_index_vector(const vcl_vector<int>& v) : vcl_vector<int>(v) {};

  // Destructor
  virtual ~vil_nitf2_index_vector() {};
};

inline vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_index_vector& vec)
{
  os << "(";
  for (vil_nitf2_index_vector::const_iterator it = vec.begin(); it != vec.end(); ++it) {
    if (it != vec.begin()) os << ", ";
    os << *it;
  }
  os << ")";
  return os;
}

#endif // VIL_NITF2_INDEX_VECTOR_H
