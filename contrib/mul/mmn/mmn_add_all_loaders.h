#ifndef mmn_add_all_loaders_h_
#define mmn_add_all_loaders_h_
//:
// \file

//: Add all binary loaders and factory objects available in mmn
//  Necessary for binary loading of derived classes by
//  base class pointer.  One instance of each possible
//  derived class must be added to the binary loader.
void mmn_add_all_loaders();

#endif // mmn_add_all_loaders_h_
