// This is gel/octree/BaseCube.h
#ifndef BaseCube_h_
#define BaseCube_h_
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 11 May 99
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim
//
//-----------------------------------------------------------------------------

class BaseCube
{
 public:
  // Constructors/Destructors--------------------------------------------------

  BaseCube(double x, double y, double z, double s)
  : bases(s), basex(x), basey(y), basez(z) {}

 protected:
  // Data Members--------------------------------------------------------------

  double bases;
  double basex;
  double basey;
  double basez;
};

#endif // BaseCube_h_
