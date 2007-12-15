#include "bgui3d.h"
//:
// \file

#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoNodeKit.h>


//: Initialize Coin3d for bgui3d use
void bgui3d_init()
{
  // initialize the Coin3d database
  SoDB::init();

  // initialize the Coin3d nodekits
  SoNodeKit::init();

// initialize Coin3d interaction
  SoInteraction::init();
}
