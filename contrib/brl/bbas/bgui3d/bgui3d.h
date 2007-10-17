// This is basic/bgui3d/bgui3d.h
#ifndef bgui3d_h_
#define bgui3d_h_

//:
// \file
// \brief  Initialize Coin3d for bgui3d
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   5/25/04
//
// \verbatim
//  Modifications
// \endverbatim

///#include <Inventor/SoDB.h>
///#include <Inventor/SoInteraction.h>
///#include <Inventor/nodekits/SoNodeKit.h>

class SoDB;
class SoNodeKit;
class SoInteraction;

//: Initialize Coin3d for bgui3d use
void bgui3d_init();

#endif // bgui3d_h_
