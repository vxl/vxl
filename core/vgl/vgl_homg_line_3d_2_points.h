//*****************************************************************************
// File name: vgl_homg_point_3d.h
// Description: Represents a homogeneous 3D line using two points.
//              A class to hold a homogeneous representation of a 3D Line.  The
//              line is stored as a pair of homogeneous 3d points.
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/05| François BERTEL          |Change default constructor
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/02/15| Don HAMILTON, Peter TU   |Creation
//*****************************************************************************
#ifndef vgl_homg_line_3d_2_points_h
#define vgl_homg_line_3d_2_points_h

#ifdef __GNUC__
#pragma interface
#endif

template <class Type>
class vgl_homg_line_3d_2_points;

#include <vcl/vcl_iostream.h>
#include <vgl/vgl_homg_point_3d.h>

template <class Type>
class vgl_homg_line_3d_2_points
{

  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
   
  //---------------------------------------------------------------------------
  // Name: vgl_homg_line_3d_2_points
  // Task: Default constructor with (0,0,0,1) and (1,0,0,0)
  //---------------------------------------------------------------------------
  explicit vgl_homg_line_3d_2_points(void);
  
  //: -- Copy constructor  
  vgl_homg_line_3d_2_points(const vgl_homg_line_3d_2_points<Type> &that);  
  
  //: Construct from two points
  vgl_homg_line_3d_2_points(const vgl_homg_point_3d<Type> & point_finite,
                            const vgl_homg_point_3d<Type> & point_infinite);
  
  //: -- Destructor
  ~vgl_homg_line_3d_2_points ();

  //: Data Access-------------------------------------------------------------
  
  vgl_homg_point_3d<Type> const & get_point_finite() const { return point_finite_; }
  vgl_homg_point_3d<Type>       & get_point_finite() { return point_finite_; }
  vgl_homg_point_3d<Type> const & get_point_infinite() const { return point_infinite_; } 
  vgl_homg_point_3d<Type>       & get_point_infinite() { return point_infinite_; }
  

  // Double3 dir() const;
  
  // Utility Methods---------------------------------------------------------
  
  //: force a point to infinity
  void force_point2_infinite();
  
  // INTERNALS---------------------------------------------------------------
protected:
  // Data Members------------------------------------------------------------
  // any finite point on the line
  vgl_homg_point_3d<Type> point_finite_;
  vgl_homg_point_3d<Type> point_infinite_;
};

  // stream operators 
template <class Type> 
ostream&  operator<<(ostream& s, const vgl_homg_line_3d_2_points<Type>& p);

template <class Type>
istream&  operator>>(istream& is,  vgl_homg_line_3d_2_points<Type>& p);

#endif 
