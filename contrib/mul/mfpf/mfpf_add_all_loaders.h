#ifndef mfpf_add_all_loaders_h_
#define mfpf_add_all_loaders_h_

//: Add all binary loaders and factory objects available in mfpf
//  Necessary for binary loading of derived classes by
//  base class pointer.  One instance of each possible
//  derived class must be added to the binary loader.
void mfpf_add_all_loaders();

#endif // mfpf_add_all_loaders_h_
