//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_vector_fixed_h_
#define vnl_vector_fixed_h_
// define the following for IUE compatibility
#define _vnl_vector_h_INCLUDED 
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_cstring.h> // memcpy()
#include <vcl/vcl_cassert.h>
#include <vnl/vnl_vector_ref.h>
#include <vnl/vnl_c_vector.h>	

//----------------------------------------------------------------------
// Class : vnl_vector_fixed
//
// .SECTION Description
//    vnl_vector_fixed is a fixed-length, stack storage vnl_vector.
//    See the docs for vnl_matrix_ref
//
// .NAME        vnl_vector_fixed - Fixed length stack-stored vnl_vector.
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_vector_fixed.h
// .FILE        vnl/vnl_vector_fixed.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//.cg_iue
//.cg_dexable
//
//.cg_tdataInstMacro	VNL_VECTOR_FIXED_INSTANTIATE(T,n)
//.cg_tdataDeclPost
//// Include _Helper file in template instance files
//#include <MathDex/vnl_vector_fixed_Helper.h>
//.cg_tdataImplPost
//.cg_customOSConvert
//// Include _Helper file in template instance files
//#include <MathDex/vnl_vector_fixed_Helper.h>
//-----------------------------------------------------------------------------

//: fixed length  stack-stored vnl_vector.

template <class T, int n>
class vnl_vector_fixed : public vnl_vector_ref<T> {
  typedef vnl_vector_ref<T> Base;
public:
  
  // Constructors/Destructors--------------------------------------------------

// -- Construct an uninitialized n-vector
  vnl_vector_fixed():Base(n, space) {}

// -- Construct an n-vector copy of rhs.  Does not check that rhs
// is the right size.
  vnl_vector_fixed(const vnl_vector<T>& rhs):Base(n, space) {
    if (rhs.size() != n)    
      vnl_error_vector_dimension ("vnl_vector_fixed(const vnl_vector&) ", n, rhs.size());
    memcpy(space, rhs.data_block(), sizeof space);
  }
  
  // GCC generates (and calls) this even though above should do...
  vnl_vector_fixed(const vnl_vector_fixed<T,n>& rhs):Base(n, space) {
    memcpy(space, rhs.space, sizeof space);
  }

  vnl_vector_fixed (const T& v): Base(n,space) {
    for(int i = 0; i < n; ++i)
      data[i] = v;
  }
  
  vnl_vector_fixed (const T& px, const T& py, const T& pz): Base(n,space) { // 3D vector (px,py,pz)
    if (n != 3) vnl_error_vector_dimension ("constructor (x,y,z): n != 3", n, 3);
    data[0] = px;
    data[1] = py;
    data[2] = pz;
  }

  vnl_vector_fixed (const T& px, const T& py): Base(n,space) { // 2D vector (px,py)
    if (n != 2) vnl_error_vector_dimension ("constructor (x,y): n != 2", n, 2);
    data[0] = px;
    data[1] = py;
 }

  vnl_vector_fixed<T,n>& operator=(const vnl_vector_fixed<T,n>& rhs) {
    memcpy(space, rhs.space, sizeof space);
    return *this;
  }

  vnl_vector_fixed<T,n>& operator=(const vnl_vector<T>& rhs) {
    if (rhs.size() != n)    
      vnl_error_vector_dimension ("operator=", n, rhs.size());
    memcpy(space, rhs.data_block(), rhs.size() * sizeof(T) );
    return *this;
  }

  vnl_vector_fixed<T,n>& operator= (const T& t) 
    { vnl_vector<T>::operator= (t); return *this; };

  vnl_vector_fixed<T,n>& operator+= (const T t) 
    { vnl_vector<T>::operator+= (t); return *this; }; // binary operation and assignment
  vnl_vector_fixed<T,n>& operator*= (const T t) // Mutate vector data
    { vnl_vector<T>::operator*= (t); return *this; };
  vnl_vector_fixed<T,n>& operator/= (const T t)
    { vnl_vector<T>::operator/= (t); return *this; };
  vnl_vector_fixed<T,n>& operator-= (const T t)
    { vnl_vector<T>::operator-= (t); return *this; };

  vnl_vector_fixed<T,n>& operator+= (const vnl_vector<T>& rhs)
    { vnl_vector<T>::operator+= (rhs); return *this; };
  vnl_vector_fixed<T,n>& operator-= (const vnl_vector<T>& rhs)
    { vnl_vector<T>::operator-= (rhs); return *this; };

  vnl_vector_fixed<T,n> operator- () const	// negation and
    { return  (vnl_vector_fixed<T,n> (*this) *= -1); };
  vnl_vector_fixed<T,n> operator+ (const T t) const // all binary operations
    { return  (vnl_vector_fixed<T,n> (*this) += t); };
  vnl_vector_fixed<T,n> operator* (const T t) const // return by values.
    { return  (vnl_vector_fixed<T,n> (*this) *= t); };
  vnl_vector_fixed<T,n> operator/ (const T t) const
    { return  (vnl_vector_fixed<T,n> (*this) /= t); };
 inline vnl_vector_fixed<T,n> operator- (const T t) const
    { return  (vnl_vector_fixed<T,n> (*this) -= t); };

friend inline vnl_vector_fixed<T,n> operator+(const T t,
					 const vnl_vector_fixed<T,n>& rhs)
    { return  (vnl_vector_fixed<T,n> (rhs) += t); };
friend inline vnl_vector_fixed<T,n> operator-(const T t,
					 const vnl_vector_fixed<T,n>& rhs)
    { return  (( - vnl_vector_fixed<T,n> (rhs)) += t); };
friend inline vnl_vector_fixed<T,n> operator*(const T t,
					 const vnl_vector_fixed<T,n>& rhs)
    { return  (vnl_vector_fixed<T,n> (rhs) *= t); };

  vnl_vector_fixed<T,n> operator+ (const vnl_vector<T>& rhs) const
    { return  (vnl_vector_fixed<T,n> (*this) += rhs); };
  vnl_vector_fixed<T,n> operator- (const vnl_vector<T>& rhs) const
    { return  (vnl_vector_fixed<T,n> (*this) -= rhs); };

  vnl_vector_fixed<T,n> apply(T (*f)(T)) 
    {
      vnl_vector_fixed<T,n> ret;
      vnl_c_vector<T>::apply(this->data, num_elmts, f, ret.data);
      return ret;
    }
  vnl_vector_fixed<T,n> apply(T (*f)(const T&))
    {
      vnl_vector_fixed<T,n> ret;
      vnl_c_vector<T>::apply(this->data, num_elmts, f, ret.data);
      return ret;
    }
// do not specialize operations with vnl_matrix, since the size of the result
// vector is not known at compile time
//  vnl_vector_fixed<T,n> operator* (const vnl_matrix<T>& m) const;
//friend vnl_vector_fixed<T,n> operator* (const vnl_matrix<T>& m, const vnl_vector_fixed<T,n>& v);
// do not specialize extract for the same reason as above
//  vnl_vector_fixed<T,n> extract (unsigned int len, unsigned int start=0) const; // subvector
  vnl_vector_fixed<T,n>& update (const vnl_vector<T>& v, unsigned int start=0)
    { return (vnl_vector_fixed<T,n>&) vnl_vector<T>::update (v, start); };

friend vnl_vector_fixed<T,n> element_product (const vnl_vector_fixed<T,n>& a,
					 const vnl_vector_fixed<T,n>& b)// v[i] = a[i]*b[i]
  { vnl_vector_fixed<T,n> ret (a);
    for (int i=0; i<n; i++) ret.data[i] *= b.data[i];
    return ret;
  }

friend vnl_vector_fixed<T,n> element_quotient (const vnl_vector_fixed<T,n>& a, // v[i] = a[i]/b[i]
					  const vnl_vector_fixed<T,n>& b)
  { vnl_vector_fixed<T,n> ret (a);
    for (int i=0; i<n; i++) ret.data[i] /= b.data[i];
    return ret;
  }

  inline vnl_vector_fixed<T,n>& normalize()	 // v /= sqrt(dot(v,v))
    { return (vnl_vector_fixed<T,n>&) vnl_vector<T>::normalize(); };

public:
  // void these methods on vnl_vector_fixed, since they deallocate the underlying
  // storage
  vnl_vector<T>& pre_multiply (const vnl_matrix<T>&); // v = m * v
  vnl_vector<T>& post_multiply (const vnl_matrix<T>&); // v = v * m
  vnl_vector<T>& operator*= (const vnl_matrix<T>&);

private:
  // Data Members--------------------------------------------------------------
  T space[n];

};

#if !defined (VCL_SUNPRO_CC) || ! defined (_ODI_OSSG_)
vnl_vector_fixed<double,3> cross_3d (const vnl_vector_fixed<double,3>& vect1, 
				     const vnl_vector_fixed<double,3>& vect2);
vnl_vector_fixed<float,3> cross_3d (const vnl_vector_fixed<float,3>& vect1, 
				    const vnl_vector_fixed<float,3>& vect2);
vnl_vector_fixed<int,3> cross_3d (const vnl_vector_fixed<int,3>& vect1, 
				  const vnl_vector_fixed<int,3>& vect2);
#endif

// #ifdef IUE
// // Overloads of global IUEg_getTypeId, etc. (if using the full IUE)
// # include<MathDex/vnl_vector_fixed_Helper.h>
// #endif


// // fix GNU bug: when g++ sees a typedef using a templated class with only the
// // forward declaration of the templated class, it sometimes cannot match it
// // to the definition when that is seen
// #ifdef __GNUC__
// # if (__GNUC__ > 2) || (__GNUC_MINOR__ <= 7)
// // only if iue_vector.h has been included
// #  ifdef vnl_vector_h_
// // include iue_vector.h again
// #   undef vnl_vector_h_
// #   include <math/iue_vector.h>
// #  endif
// # endif
// #endif


#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_vector_fixed.

