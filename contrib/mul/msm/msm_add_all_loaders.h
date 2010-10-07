#ifndef msm_add_all_loaders_h_
#define msm_add_all_loaders_h_
//:
// \file

//: Add all binary loaders and factory objects available in msm
//  Necessary for binary loading of derived classes by
//  base class pointer.  One instance of each possible
//  derived class must be added to the binary loader.
void msm_add_all_loaders();

#endif // msm_add_all_loaders_h_
