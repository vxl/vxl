#ifndef osl_chamfer_h_
#define osl_chamfer_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

//: Chamfer transform routines taken from osl_edge_detector.
void osl_chamfer_Forward (int,int,int**,float**);
void osl_chamfer_Backward(int,int,int**,float**);
void osl_chamfer_Alt1(int,int,int**,float**);
void osl_chamfer_Alt2(int,int,int**,float**);

// don't use this.
int osl_Minimum4(int, int, int, int);

#endif // osl_chamfer_h_
