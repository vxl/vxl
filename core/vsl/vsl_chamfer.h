#ifndef vsl_chamfer_h_
#define vsl_chamfer_h_
#ifdef __GNUC__
#pragma interface "vsl_chamfer"
#endif
/*
  fsm@robots.ox.ac.uk
*/

//: Chamfer transform routines taken from vsl_edge_detector.
void vsl_chamfer_Forward (int,int,int**,float**);
void vsl_chamfer_Backward(int,int,int**,float**);
void vsl_chamfer_Alt1(int,int,int**,float**);
void vsl_chamfer_Alt2(int,int,int**,float**);

// don't use this.
int vsl_Minimum4(int, int, int, int);

#endif
