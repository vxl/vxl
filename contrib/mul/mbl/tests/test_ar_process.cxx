// This is mul/mbl/tests/test_ar_process.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mbl/mbl_ar_process.h>
#include <mbl/mbl_mz_random.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

vnl_vector<double> arp_mean(const vcl_vector<vnl_vector<double> >& v)
{
unsigned int dim=v[0].size();
vnl_vector<double> m(dim);
for (unsigned int i=0;i<v.size();i++)
  {
  m+=v[i];
  }
return m/((double)v.size());
}

vnl_matrix<double> arp_covar(const vcl_vector<vnl_vector<double> >& v)
{
unsigned int dim=v[0].size();
vnl_matrix<double> C(dim,dim);
vnl_vector<double> m(dim);
m=arp_mean(v);
double n=((double)v.size());

for (unsigned int i=0;i<v.size();i++)
  {
  C+=outer_product(v[i]-m,v[i]-m);
  }
C/=n;

return C;
}

void test_ar_process()
{
  vcl_cout << "*****************************" << vcl_endl;
  vcl_cout << " Testing mbl_ar_process " << vcl_endl;
  vcl_cout << "*****************************" << vcl_endl;

  vnl_matrix<double> A(2,2),B(2,2),C(2,2),dC(2,2);
  vnl_vector<double> X(2),W(2),dm(2);
  vcl_vector<vnl_vector<double> > vlist,glist;
  mbl_mz_random mz_random;
  mbl_ar_process<double> arp;

  mz_random.reseed(1000);

  A(0,0)=mz_random.normal()/3.0; 
  A(1,1)=mz_random.normal()/3.0;  
  B(0,0)=mz_random.normal()/3.0;  
  B(1,1)=mz_random.normal()/3.0;  
  C(0,0)=mz_random.normal()/100.0;  
  C(1,1)=mz_random.normal()/100.0; 

  X(0)=mz_random.normal(); 
  X(1)=mz_random.normal(); 
  vlist.push_back(X);
  X(0)=mz_random.normal(); 
  X(1)=mz_random.normal(); 
  vlist.push_back(X);

  for (unsigned int i=0;i<100;i++)
    {
    W(0)=mz_random.normal(); 
    W(1)=mz_random.normal(); 
    X=A*vlist[i]+B*vlist[i+1]+C*W;
    vlist.push_back(X);
    }

  arp.learn_burg(vlist);

  glist.push_back(vlist[0]);
  glist.push_back(vlist[1]);

  for (unsigned int i=0;i<100;i++)
    {
    glist.push_back(arp.predict(glist[i],glist[i+1]));
    }

  dm=arp_mean(vlist)-arp_mean(glist);
  dC=arp_covar(vlist)-arp_covar(glist);

  TEST("Similar mean for burg algorithm",dm.inf_norm()<0.5,true);
  TEST("Similar covariance matrix for burg algorithm",
    dC.array_inf_norm()<0.5,true);

  arp.learn(vlist);

  glist.clear();

  glist.push_back(vlist[0]);
  glist.push_back(vlist[1]);

  for (unsigned int i=0;i<100;i++)
    {
    glist.push_back(arp.predict(glist[i],glist[i+1]));
    }

  dm=arp_mean(vlist)-arp_mean(glist);
  dC=arp_covar(vlist)-arp_covar(glist);

  TEST("Similar mean",dm.inf_norm()<0.5,true);
  TEST("Similar covariance matrix",dC.array_inf_norm()<0.5,true);
}

TESTLIB_DEFINE_MAIN(test_ar_process);
