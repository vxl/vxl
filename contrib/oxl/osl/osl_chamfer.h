// This is oxl/osl/osl_chamfer.h
#ifndef osl_chamfer_h_
#define osl_chamfer_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

//: Chamfer transform routines taken from osl_edge_detector.
void osl_chamfer_Forward (int,int,int**,float**);
void osl_chamfer_Backward(int,int,int**,float**);
void osl_chamfer_Alt1(int,int,int**,float**);
void osl_chamfer_Alt2(int,int,int**,float**);

// don't use this.
int osl_Minimum4(int, int, int, int);

#endif // osl_chamfer_h_
