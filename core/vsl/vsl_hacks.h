#ifndef vsl_hacks_h_
#define vsl_hacks_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_hacks
// .INCLUDE vsl/vsl_hacks.h
// .FILE vsl_hacks.cxx
// \author fsm@robots.ox.ac.uk

// FIXME FIXME FIXME FIXME FIXME
#define IUDelete(p) /* delete p */

//#define fsm_delete 0 == 
#define fsm_delete delete

//#define fsm_delete_array 0 == 
#define fsm_delete_array delete []

// Set this to 1 if you think it can avoid heap corruption, and
// you don't mind leaking some core.
#define ALLOW_CORELEAKS 0

// If you think your segfault is due to an out-of-bounds array
// access, this one may be the thing for you.
//#define fsm_pad +50
#define fsm_pad /* */

#endif
