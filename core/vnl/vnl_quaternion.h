//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_quaternion_h_
#define vnl_quaternion_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 16 Mar 00

//: Unit quaternion represents rotation in 3D.
//
// Quaternion is a 4-element vector with 1 real and 3 imaginary
// components:
// <math>
//    q = r + (i*x + j*y + k*z)
//    r = cos(theta/2)
//    (x, y, z) = sin(theta/2) (kx, ky, kz) 
// </math>
// where theta and k are  respectively the angle and axis of rotation. 
// 3D vectors can be  thought  of  as  imaginary  quaternions, and  so  a
// quaternion is represented as a Vector<FLOAT> with the imaginary
// part before the real part for 1-1 alignment.
// <skip>
// Unit quaternion provides a more efficient representation for
// rotation, than  the usual orthonormal matrix that has nine
// parameters  and  six  orthonormal  constraints.   The   unit
// quaternion  has only one unit magnitude constraint. Composing
// rotations with quaternions results in fewer multiplications
// and less error. To insure valid rotation result, the
// nearest unit quaternion is computed, and this is much easier
// than  finding  the  nearest orthonormal matrix. Transforming
// vectors with a quaternion requires more operations  compared
// to multiplication with the equivalent orthonormal matrix.
//
// .SECTION See also
// Vector<Type> and Matrix<Type> for basic operations on vectors and matrices.
// <skip>
// Transform for coordinate transformations.
// <skip>
// Envelope for envelope-letter scheme that avoids deep copy on
// return by value in arithmetic expressions like: q1 * q2 * q3 *...


#include <vcl/vcl_compiler.h>
#include <vcl/vcl_functional.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

template <class FLOAT>
class vnl_quaternion : public vnl_vector_fixed<FLOAT, 4> { // public for IS-A relation
  typedef vnl_vector_fixed<FLOAT, 4> Base;
public:
  vnl_quaternion (FLOAT x=0, FLOAT y=0, FLOAT z=0, FLOAT r=1); // null quat
  vnl_quaternion (const vnl_vector<FLOAT>& axis, FLOAT angle); 
  vnl_quaternion (const vnl_matrix<FLOAT>& transform); // from 3-4 square row-major
  vnl_quaternion (const vnl_vector<FLOAT>& vec);     // from 3-4D vector
  inline vnl_quaternion (const vnl_quaternion<FLOAT>& from);  // copy constructor
  inline ~vnl_quaternion();			       // free internal array
  inline vnl_quaternion& operator= (const vnl_quaternion<FLOAT>& rhs); // q1 = q2
  
  inline FLOAT& x ();			// imaginary component
  inline FLOAT& y ();			// parallel to axis of rotation
  inline FLOAT& z ();
  inline FLOAT& r ();			// real component
  inline FLOAT x () const;
  inline FLOAT y () const;
  inline FLOAT z () const;
  inline FLOAT r () const;
  inline FLOAT real () const;    
  inline vnl_vector<FLOAT> imaginary () const; // imaginary vector part

  vnl_vector<FLOAT> axis () const;		// Axis of rotation
  FLOAT angle () const;				// Angle of rotation
  
  vnl_matrix_fixed<FLOAT,3,3> rotation_matrix () const; // to 3 rot matrix
  vnl_matrix_fixed<FLOAT,4,4> rotation_matrix_4 () const; // to 4 rot matrix
  
  vnl_quaternion<FLOAT> conjugate () const;	// same real, opposite img part
  vnl_quaternion<FLOAT> inverse () const;	// inverse for nonzero quat
  
  vnl_quaternion<FLOAT> operator* (const vnl_quaternion<FLOAT>&) const; 
  vnl_vector<FLOAT> rotate (const vnl_vector<FLOAT>& v) const; // rotate 3D v
  
  inline friend ostream& operator<< (ostream& os, const vnl_quaternion<FLOAT>& q);
};


// Quaternion -- Creates a copy of from quaternion.
template <class FLOAT>
inline
vnl_quaternion<FLOAT>::vnl_quaternion (const vnl_quaternion<FLOAT>& from) :
  Base(from)
{			// 1-1 layout between vector&quat
}

// ~Quaternion -- Frees space allocated for quaternion.

template <class FLOAT>
inline
vnl_quaternion<FLOAT>::~vnl_quaternion () {} // Vector will free data array


// x -- 

template <class FLOAT>
inline FLOAT& vnl_quaternion<FLOAT>::x () {
  return this->operator()(0);
}

// y -- 

template <class FLOAT>
inline FLOAT& vnl_quaternion<FLOAT>::y () {
  return this->operator()(1);
}

// z -- 

template <class FLOAT>
inline FLOAT& vnl_quaternion<FLOAT>::z () {
  return this->operator()(2);
}

// r -- Accessors for the imaginary and real components of  the
//      quaternion. Use these accessors to both get
//      and set the components.

template <class FLOAT>
inline FLOAT& vnl_quaternion<FLOAT>::r () {
  return this->operator()(3);
}


// x -- 

template <class FLOAT>
inline FLOAT vnl_quaternion<FLOAT>::x () const {
  return this->operator()(0);
}

// y -- 

template <class FLOAT>
inline FLOAT vnl_quaternion<FLOAT>::y () const {
  return this->operator()(1);
}

// z -- 

template <class FLOAT>
inline FLOAT vnl_quaternion<FLOAT>::z () const {
  return this->operator()(2);
}

// r -- Accessors for the imaginary and real components of  the
//      quaternion. Use these accessors to both get
//      and set the components.

template <class FLOAT>
inline FLOAT vnl_quaternion<FLOAT>::r () const {
  return this->operator()(3);
}

// imaginary -- Copies and returns the imaginary part.

template <class FLOAT>
inline vnl_vector<FLOAT> vnl_quaternion<FLOAT>::imaginary () const {
  return this->extract(3,0);
}

// real -- Copies and returns the real part.

template <class FLOAT>
inline FLOAT vnl_quaternion<FLOAT>::real () const {
  return this->get(3);
}

// operator=  -- Overloads assignment operator to  copy rhs quaternion
//      into lhs quaternion.

template <class FLOAT>
inline vnl_quaternion<FLOAT>& vnl_quaternion<FLOAT>::operator= (const vnl_quaternion<FLOAT>& rhs) {
  Base::operator=(rhs);		// same as copy vector part
  return *this;
}



// operator<<  --

template <class FLOAT>
inline ostream& operator<< (ostream& os, const vnl_quaternion<FLOAT>& q) {
  return os << *((vnl_vector<FLOAT>*) &q);
}

// operator<<  -- Print the components of Quaternion.
//  awf removed : pointers should never be printed dereffed.
// template <class FLOAT>
// inline ostream& operator<< (ostream& os, const vnl_quaternion<FLOAT>* q) {
//   return os << *((vnl_vector<FLOAT>*) q);
// }

#define VNL_QUATERNION_INSTANTIATE(FLOAT) extern "Error, include vnl/vnl_quaternion.txx";

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_quaternion.
