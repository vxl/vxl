// This is oxl/osl/osl_edgel_chain.h
#ifndef osl_edgel_chain_h_
#define osl_edgel_chain_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME osl_edgel_chain
// .INCLUDE osl/osl_edgel_chain.h
// .FILE osl_edgel_chain.cxx
// \author fsm

#include <vcl_iosfwd.h>

struct osl_edgel_chain
{
  unsigned int n;
  float *x, *y, *grad, *theta;

  osl_edgel_chain();
  osl_edgel_chain(unsigned int n_);
  osl_edgel_chain(osl_edgel_chain const &);
  void operator=(osl_edgel_chain const &);
  ~osl_edgel_chain();
  void SetLength(unsigned int);

  float  GetGrad(unsigned int i) const;
  float *GetGrad() const;
  float  GetTheta(unsigned int i) const;
  float *GetTheta() const;
  float  GetX(unsigned int i) const;
  float *GetX() const;
  float  GetY(unsigned int i) const;
  float *GetY() const;
  void SetGrad(float v, unsigned int i);
  void SetTheta(float v, unsigned int i);
  void SetX(float v, unsigned int i);
  void SetY(float v, unsigned int i);
  unsigned int size() const;
  //
  void write_ascii(vcl_ostream &) const;
  void read_ascii(vcl_istream &);
};

#endif // osl_edgel_chain_h_
