#ifndef mbl_add_all_loaders_h_
#define mbl_add_all_loaders_h_
//:
// \file

//: Add all loaders available in mbl
//  Necessary for binary loading of derived classes by
//  base class pointer.  One instance of each possible
//  derived class must be added to the binary loader.
void mbl_add_all_loaders();

#endif // mbl_add_all_loaders_h_
