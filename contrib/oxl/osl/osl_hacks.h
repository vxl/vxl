// This is oxl/osl/osl_hacks.h
#ifndef osl_hacks_h_
#define osl_hacks_h_
// .NAME osl_hacks
// .INCLUDE osl/osl_hacks.h
// .FILE osl_hacks.cxx
// \author fsm

// FIXME FIXME FIXME FIXME FIXME
#define osl_IUDelete(p) /* delete p */

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

#endif // osl_hacks_h_
