#ifndef rsdl_bounding_box_h_
#define rsdl_bounding_box_h_
//:
// \file
// \brief Bounds on a rectangular region over a  mix Cartesian and angular dimensions.
// \author Chuck Stewart
// \date June 2001
//
//  A very simple class to represent a rectangular region that is
//  defined on coordinates that mix Cartesian and angular dimensions.
//  This is used in the k-d tree algorithm.  Nc and Na must be
//  non-negative and Nc+Na>0.
//
//  This class is effectively "read-only" (except for an assignment
//  operator).


#include <iostream>
#include <iosfwd>
#include <rsdl/rsdl_point.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rsdl_bounding_box
{
 private:
  //: default ctor is private!
  rsdl_bounding_box() = default;

 public:

  //: Construct a region to enclose two points.
  //  Note that if the numerical value of a min angle is
  //  greater than that of a max angle, this is effectively be
  //  treated as "wrapping around".  The cartesian values of the
  //  min_point and max_point are checked and reordered if inconsistent.
  rsdl_bounding_box( const rsdl_point& min_point,
                     const rsdl_point& max_point );

  //: Copy constructor
  rsdl_bounding_box( const rsdl_bounding_box& old );

  //: Assignment operator
  rsdl_bounding_box& operator= ( const rsdl_bounding_box& old );

  //: Mutable access to lower bound cartesian value in dimension i.
  inline double& min_cartesian( unsigned int i ) { return min_point_.cartesian(i); }

  //: Number of cartesian dimensions.
  inline unsigned int num_cartesian( ) const { return min_point_.num_cartesian(); }

  //: Number of angular dimensions.
  inline unsigned int num_angular( ) const { return min_point_.num_angular(); }

  //: Constant access to lower bound cartesian value in dimension i.
  inline double min_cartesian( unsigned int i ) const { return min_point_.cartesian(i); }

  //: Mutable access to upper bound cartesian value in dimension i.
  inline double& max_cartesian( unsigned int i ) { return max_point_.cartesian(i); }

  //: Constant access to upper bound cartesian value in dimension i
  inline double max_cartesian( unsigned int i ) const { return max_point_.cartesian(i); }

  //: Mutable access to lower bound angular value in dimension i
  inline double& min_angular( unsigned int i ) { return min_point_.angular(i); }

  //: Constant access to lower bound angular value in dimension i
  inline double min_angular( unsigned int i ) const { return min_point_.angular(i); }

  //: Mutable access to upper bound angular value in dimension i
  inline double& max_angular( unsigned int i ) { return max_point_.angular(i); }

  //: Constant access to upper bound angular value in dimension i
  inline double max_angular( unsigned int i ) const { return max_point_.angular(i); }

 private:
  rsdl_point min_point_;
  rsdl_point max_point_;
};

std::ostream& operator<< ( std::ostream& ostr, const rsdl_bounding_box& box );

#endif
