#ifndef HomgMap2D_h_
#define HomgMap2D_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME    HomgMap2D - General 2D projective map
// .LIBRARY MViewBasics
// .HEADER  MultiView Package
// .INCLUDE mvl/HomgMap2D.h
// .FILE    HomgMap2D.cxx
//
// .SECTION Description
//    HomgMap2D represents a mapping between two 2D spaces.
//    It is used to represent a variety of such mappings, including
//    camera calibration and numerical conditioning.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 06 Nov 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

class HomgMap2D {
public:
  virtual ~HomgMap2D() = 0;
//: Transform a point from space 1 to space 2.
  virtual HomgPoint2D transform(const HomgPoint2D& p) = 0;
//: Transform a point from space 2 to space 1.
  virtual HomgPoint2D inverse_transform(const HomgPoint2D& p) = 0;
};

#endif // HomgMap2D_h_
