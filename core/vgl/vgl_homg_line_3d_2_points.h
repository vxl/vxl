#ifndef vgl_homg_line_3d_2_points_h_
#define vgl_homg_line_3d_2_points_h_
//-----------------------------------------------------------------------------
//
// .NAME vgl_homg_line_3d_2_points - Represents a homogeneous 3D line using
// two points
// .LIBRARY vgl
// .HEADER  vgl/vgl_homg_line_3d_2_points.h
// .INCLUDE vcl/vcl_iostream.h
// .INCLUDE vgl/vgl_homg_point_3d.h
// .FILE    vgl/vgl_homg_line_3d_2_points.txx
//
// .SECTION Description
// A class to hold a homogeneous representation of a 3D Line.  The line is
// stored as a pair of homogeneous 3d points.
//
// .SECTION Author
// Don HAMILTON
// Peter TU
// François BERTEL
//
// .SECTION Modifications
// 2000/05/05 François BERTEL        Change default constructor
// 2000/02/15 Don HAMILTON, Peter TU Creation
//-----------------------------------------------------------------------------

template <class Type>
class vgl_homg_line_3d_2_points;

#include <vcl/vcl_iostream.h>
#include <vgl/vgl_homg_point_3d.h>

template <class Type>
class vgl_homg_line_3d_2_points
{
  //***************************************************************************
  // Initialization
  //***************************************************************************
public:
  //---------------------------------------------------------------------------
  //: Default constructor with (0,0,0,1) and (1,0,0,0)
  //---------------------------------------------------------------------------
  explicit vgl_homg_line_3d_2_points(void);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vgl_homg_line_3d_2_points(const vgl_homg_line_3d_2_points<Type> &that);  

  //---------------------------------------------------------------------------
  //: Construct from two points
  //---------------------------------------------------------------------------
  vgl_homg_line_3d_2_points(const vgl_homg_point_3d<Type> &point_finite,
                            const vgl_homg_point_3d<Type> &point_infinite);
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vgl_homg_line_3d_2_points();

  //***************************************************************************
  //: Data access
  //***************************************************************************
  
  vgl_homg_point_3d<Type> const &get_point_finite(void) const
  {
    return point_finite_;
  }
  vgl_homg_point_3d<Type> &get_point_finite(void)
  {
    return point_finite_;
  }
  vgl_homg_point_3d<Type> const &get_point_infinite(void) const
  {
    return point_infinite_;
  } 
  vgl_homg_point_3d<Type> &get_point_infinite(void)
  {
    return point_infinite_;
  }

  //***************************************************************************
  // Utility methods
  //***************************************************************************
  
  //---------------------------------------------------------------------------
  //: force a point to infinity
  //---------------------------------------------------------------------------
  void force_point2_infinite(void);

  //***************************************************************************
  // Internals
  //***************************************************************************
protected:
  // Data Members------------------------------------------------------------
  // any finite point on the line
  vgl_homg_point_3d<Type> point_finite_;
  vgl_homg_point_3d<Type> point_infinite_;
};

//*****************************************************************************
// stream operators
//*****************************************************************************

template <class Type>
ostream &operator<<(ostream &s,
                    const vgl_homg_line_3d_2_points<Type> &p);

template <class Type>
istream &operator>>(istream &is,
                    vgl_homg_line_3d_2_points<Type> &p);

#endif // #ifndef vgl_homg_line_3d_2_points_h_
