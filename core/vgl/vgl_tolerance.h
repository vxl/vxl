#ifndef vgl_tolerance_h_
#define vgl_tolerance_h_

//! \file
//  \author Kieran O'Mahony
//  \date 21 August 2007
//  \brief Tolerances used throughout vgl when performing comparisons 


template <typename T>
class vgl_tolerance
{
  public:
    //! Tolerance for judging 4 points to be coplanar
    static const T point_3d_coplanarity;

    //! Tolerance for judging positions to be equal
    static const T position;
};

#endif
