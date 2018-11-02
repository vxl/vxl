#ifndef rsdl_point_h_
#define rsdl_point_h_
//:
// \file
// \brief A point with mixed Cartesian and angular coordinates.
// \author Chuck Stewart
// \date June 2001
//
//  This is a class to represent points with mixed Cartesian and
//  angular coordinates.  The size of each dimension must be provided
//  to the constructor either explicitly or implicitly.  The code acts
//  as though the cartesian and angular values are represented in two
//  separate arrays.


#include <iostream>
#include <iosfwd>
#include <vector>
#include <vnl/vnl_vector.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rsdl_point
{
 public:
  //: Used until we support member templates.
  typedef std::vector<double>::const_iterator const_iterator;

 public:
  //: Provided only for STL.
  //  Creates a point without any data.
  rsdl_point( );

  //: Ctor to set the dimensions without setting the values.
  //  \a Nc and \a Na are the number of cartesian and angular
  //  coordinates, respectively.
  rsdl_point( unsigned int Nc, unsigned int Na=0 );

  //: Ctor from Cartesian and angular value vector iterators.
  rsdl_point( const_iterator c_begin, const_iterator c_end,
              const_iterator a_begin, const_iterator a_end );

  //: Ctor from Cartesian and angular value vnl_vectors.
  rsdl_point( const vnl_vector<double>& c, const vnl_vector<double>& a );

  //: Copy constructor.
  rsdl_point( const rsdl_point& old );

  //: Ctor from cartesian and the angular values, in order, in a single vnl_vector.
  //  The angular data is taken as the last \a Na values in the vector.
  rsdl_point( const vnl_vector<double>& all, unsigned int Na=0 );

  //: Destructor.
  ~rsdl_point();

  //: Number of cartesian dimensions
  unsigned int num_cartesian() const { return Nc_; }

  //: Number of angular dimensions
  unsigned int num_angular() const { return Na_; }

  //: Establish the cartesian values from a vnl_vector.
  void set_cartesian( const vnl_vector<double>& c );

  //: Establish the cartesian values from an iterator.
  //  No checking is done: \a Nc elements are expected.
  void set_cartesian( const_iterator c );

  //: Establish the angular values from a vnl_vector.
  void set_angular( const vnl_vector<double>& a );

  //: Establish the cartesian and angular values from a vnl_vector.
  void set_all( const vnl_vector<double>& all );

  //: Establish the angular values from an iterator.
  //  No checking is done: \a Nc elements are expected.
  void set_angular( const_iterator a );

  //: Constant access to the indexed cartesian coordinate.
  inline double cartesian( unsigned int i ) const { return data_[i]; }

  //: Mutable access to the indexed cartesian coordinate.
  inline double& cartesian( unsigned int i ) { return data_[i]; }

  //: Constant access to the indexed angular coordinate.
  inline double angular( unsigned int i ) const { return data_[Nc_+i]; }

  //: Mutable access to the indexed angular coordinate.
  inline double& angular ( unsigned int i ) { return data_[Nc_+i]; }

  //: Assignment operator.
  rsdl_point&  operator= ( const rsdl_point& old );

  //: Resize both the cartesian and angular dimensions.
  void resize( unsigned int Nc, unsigned int Na );

 private:
  unsigned int Nc_;
  unsigned int Na_;
  double *data_;
};

std::ostream& operator<< ( std::ostream& ostr, const rsdl_point& pt );

#endif
