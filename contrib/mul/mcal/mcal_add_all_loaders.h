#ifndef mcal_add_all_loaders_h_
#define mcal_add_all_loaders_h_

//: Add all binary loaders and factory objects available in mcal
//  Necessary for binary loading of derived classes by
//  base class pointer.  One instance of each possible
//  derived class must be added to the binary loader.
void mcal_add_all_loaders();

#endif // mcal_add_all_loaders_h_
