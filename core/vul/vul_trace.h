// This is vxl/vul/vul_trace.h
#ifndef vul_trace_h_
#define vul_trace_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief writes out file, line to stderr.
// \author fsm@robots.ox.ac.uk
//

//: writes out file, line to stderr and flushes.
void vul_trace_function(char const *file, int line);

// recompile with DEFINES=-DVBL_TRACE
#if defined(VBL_TRACE)
# define vul_trace vul_trace_function(__FILE__, __LINE__)
#else
# define vul_trace /* */
#endif

#endif // vul_trace_h_
