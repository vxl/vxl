//-*- c++ -*-------------------------------------------------------------------
#ifndef BaseCube_h_
#define BaseCube_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 11 May 99
//
//-----------------------------------------------------------------------------

class BaseCube {
public:
  // Constructors/Destructors--------------------------------------------------

  BaseCube( double, double, double, double);

protected:
  // Data Members--------------------------------------------------------------

  double bases;
  double basex;
  double basey;
  double basez;
};

#endif // BaseCube_h_
