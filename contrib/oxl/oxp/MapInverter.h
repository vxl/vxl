//-*- c++ -*-------------------------------------------------------------------
#ifndef MapInverter_h_
#define MapInverter_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : MapInverter
//
// .SECTION Description
//    MapInverter is a class that awf hasn't documented properly. FIXME
//
// .NAME        MapInverter - Undocumented class FIXME
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/MapInverter.h
// .FILE        MapInverter.h
// .FILE        MapInverter.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 13 Jul 98
//
//-----------------------------------------------------------------------------

#include <oxp/Mapping_2d_2d.h>

class MapInverter : public Mapping_2d_2d {
public:
  MapInverter(Mapping_2d_2d* themap): _themap(themap) {}

protected: // implementation of Mapping_2d_2d
  void implement_map(double x1, double y1, double* x2, double* y2) {
    _themap->map(x1, y1, x2, y2);
  }
  void implement_inverse_map(double x2, double y2, double* x1, double* y1) {
    _themap->inverse_map(x2, y2, x1, y1);
  }
  
protected:
  Mapping_2d_2d* _themap;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS MapInverter.

