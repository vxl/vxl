#ifndef vbl_trace_h_
#define vbl_trace_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

void vbl_trace_function(char const *file, int line);

// recompile with DEFINES=-DVBL_TRACE
#if defined(VBL_TRACE)
# define vbl_trace vbl_trace_function(__FILE__, __LINE__)
#else
# define vbl_trace /* */
#endif

#endif
