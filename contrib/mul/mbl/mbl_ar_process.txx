// This is mul/mbl/mbl_ar_process.txx
#ifndef mbl_ar_process_txx_
#define mbl_ar_process_txx_
//:
// \file

#include "mbl_ar_process.h"

#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_qr.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <mbl/mbl_mz_random.h>
#include <vcl_cmath.h>

//: Constructor
template<class T>
mbl_ar_process<T>::mbl_ar_process()
{
}

//: Destructor
template<class T>
mbl_ar_process<T>::~mbl_ar_process()
{
}

//: Version number for I/O
template<class T>
short mbl_ar_process<T>::version_no() const
{
    return 1;
}

//: Print class to os
template<class T>
void mbl_ar_process<T>::print_summary(vcl_ostream& /*os*/) const
{
#if 0
    os << data_; // example of data output
#endif
    vcl_cerr << "mbl_ar_process<T>::print_summary() is NYI\n";
}

//: Save class to binary file stream
template  <class T>
void mbl_ar_process<T>::b_write(vsl_b_ostream& /*bfs*/) const
{
  vcl_cout<<"mbl_ar_process<T>::b_write - NYI !\n";
}

//: Load class from binary file stream
template  <class T>
void mbl_ar_process<T>::b_read(vsl_b_istream& /*bfs*/)
{
  vcl_cout<<"mbl_ar_process<T>::b_read - NYI !\n";
}

//: Does the name of the class match the argument?
template<class T>
bool mbl_ar_process<T>::is_class(vcl_string const& s) const
{
  return s==mbl_ar_process<T>::is_a();
}

//: Prediction
// of a vcl_vector given the two previous vectors
template<class T>
vnl_vector<T> mbl_ar_process<T>::predict(vnl_vector<T>& Xm1,
                                         vnl_vector<T>& Xm2)
{
  vnl_vector<T> Xm0; // uninitialised
  if (Xm1.size()!=Xm2.size() || Xm1.size()!=Xm.size()) return Xm0;

  vnl_vector<T> wk(Xm.size());

  static mbl_mz_random mz_random;

  for (unsigned int i=0;i<Xm.size();i++)
  {
    wk[i]=(T)mz_random.normal();
  }

  return Xm+A_2*(Xm2-Xm)+A_1*(Xm1-Xm)+B_0*wk;
}

//: Learning using Burg's algorithm
template<class T>
void mbl_ar_process<T>::learn_burg(vcl_vector<vnl_vector<T> >& data)
{
  if (data.size()<2) return;

  Xm=data[0];
  for (unsigned int i=1;i<data.size();i++)
    Xm+=data[i];

  Xm/=(T)data.size();

  for (unsigned int i=0;i<data.size();i++)
    data[i]-=Xm;

  unsigned int dim=data.size();

  vnl_vector<T> Ef(dim);
  vnl_vector<T> Eb(dim);

  A_1.set_size(data[0].size(),data[0].size());
  A_1.fill((T)0.0);
  A_2.set_size(data[0].size(),data[0].size());
  A_2.fill((T)0.0);
  B_0.set_size(data[0].size(),data[0].size());
  B_0.fill((T)0.0);

  for (unsigned int j=0;j<data[0].size();j++)
  {
    T E;

    for (unsigned int i=0;i<dim;i++)
    {
      Ef[i]=data[i][j];
      Eb[i]=data[i][j];
    }

    E=dot_product(Ef,Ef)/((T)dim);

    vnl_vector<T> a(3, T(1),T(0),T(0));
    T km;
    for (unsigned int i=0;i<2;i++)
    {
      vnl_vector<T> Efp(dim-i-1);
      vnl_vector<T> Ebp(dim-i-1);
      for (unsigned int k=0;k<dim-i-1;k++)
        {
        Efp[k]=Ef[k+1];
        Ebp[k]=Eb[k];
        }

      km=(-((T)(2.0))*dot_product(Ebp,Efp)/
        (dot_product(Ebp,Ebp)+dot_product(Efp,Efp)));
      for (unsigned int k=0;k<dim-i-1;k++)
        {
        Ef[k]=Efp[k]+km*Ebp[k];
        Eb[k]=Ebp[k]+km*Efp[k];
        }
      vnl_vector<T> b=a;
      for (unsigned int k=0;k<i+1;k++)
      {
        b[k+1]+=km*a[i-k];
      }
      a=b;
      E=(1-km*km)*E;
    }
    A_1[j][j]=-a[1];
    A_2[j][j]=-a[2];
    B_0[j][j]=E>((T)0.0)?vcl_sqrt(E):((T)0.0);
  }

  for (unsigned int i=0;i<data.size();i++)
    data[i]+=Xm;
}

//: Dynamic learning
template<class T>
void mbl_ar_process<T>::learn(vcl_vector<vnl_vector<T> >& data)
{
  if (data.size()==0) return;
  unsigned int dim=data[0].size();

  vnl_vector<T> R0(dim,T(0)),R1(dim,T(0)),R2(dim,T(0)); // initialise to 0
  vnl_matrix<T> R00(dim,dim,T(0)), R01(dim,dim,T(0)), R02(dim,dim,T(0)),
                R10(dim,dim,T(0)), R11(dim,dim,T(0)), R12(dim,dim,T(0)),
                R20(dim,dim,T(0)), R21(dim,dim,T(0)), R22(dim,dim,T(0));

  Xm=data[0];
  for (unsigned int i=1;i<data.size();i++)
    Xm+=data[i];
  Xm/=(T)data.size();

  for (unsigned int i=0;i<data.size();i++)
    data[i]-=Xm;

  for (unsigned int i=2;i<data.size();i++)
  {
    R0+=data[i];
    R1+=data[i-1];
    R2+=data[i-2];
    R00+=outer_product(data[i],data[i]);
    R01+=outer_product(data[i],data[i-1]);
    R02+=outer_product(data[i],data[i-2]);
    R10+=outer_product(data[i-1],data[i]);
    R11+=outer_product(data[i-1],data[i-1]);
    R12+=outer_product(data[i-1],data[i-2]);
    R20+=outer_product(data[i-2],data[i]);
    R21+=outer_product(data[i-2],data[i-1]);
    R22+=outer_product(data[i-2],data[i-2]);
  }

  T coef=(T)1.0/((T)data.size()-(T)2.0);

  vnl_matrix<T> Rp01=R01-coef*outer_product(R0,R1);
  vnl_matrix<T> Rp02=R02-coef*outer_product(R0,R2);
  vnl_matrix<T> Rp11=R11-coef*outer_product(R1,R1);
  vnl_matrix<T> Rp12=R12-coef*outer_product(R1,R2);
  vnl_matrix<T> Rp21=R21-coef*outer_product(R2,R1);
  vnl_matrix<T> Rp22=R22-coef*outer_product(R2,R2);

  vnl_matrix_inverse<double> ti1A2(Rp11);
  vnl_matrix<T> t1A2=ti1A2*Rp12;
  vnl_matrix_inverse<double> ti2A2(Rp22-Rp21*t1A2);

  A_2=(Rp02-Rp01*t1A2)*ti2A2;
  A_1=(Rp01-A_2*Rp21)*ti1A2;

  vnl_vector<T> D=coef*(R0-A_2*R2-A_1*R1);
  vnl_matrix<T> C=coef*(R00-A_2*R20-A_1*R10-outer_product(D,R0));
  vnl_symmetric_eigensystem<T> srB(C);
  B_0=srB.square_root();

  vnl_qr<double> qr_A_1(A_1);
  vnl_qr<double> qr_A_2(A_2);

  for (unsigned int i=0;i<data.size();i++)
    data[i]+=Xm;
}

//: Write to binary stream
template<class T>
void vsl_b_write(vsl_b_ostream& os, const mbl_ar_process<T>* p)
{
  if (p==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
    //vsl_b_write(os,*p);
  }
}

//: Read data from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& is, mbl_ar_process<T>* & v)
{
  delete v;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    v = new mbl_ar_process<T>();
    v->b_read(is);
    //vsl_b_read(is, *v);
  }
  else
    v = 0;
}

//: Print class to os
template<class T>
void vsl_print_summary(vcl_ostream& os, const mbl_ar_process<T>* p)
{
  p->print_summary(os);
}

#undef MBL_AR_PROCESS_INSTANTIATE
#define MBL_AR_PROCESS_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mbl_ar_process<T >::is_a() const \
{ return vcl_string("mbl_ar_process<" #T ">"); } \
template class mbl_ar_process<T >; \
template void vsl_b_write(vsl_b_ostream& s, const mbl_ar_process<T >* arp); \
template void vsl_b_read(vsl_b_istream& s, mbl_ar_process<T >* & arp); \
template void vsl_print_summary(vcl_ostream& s,const mbl_ar_process<T >* arp)

#endif //mbl_ar_process_txx_
