// This is mul/mbl/tests/test_ar_process.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_ar_process.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_matrix.h>

vnl_vector<double> arp_mean(const vcl_vector<vnl_vector<double> >& v)
{
  vnl_vector<double> m = v[0];
  for (unsigned int i=1;i<v.size();i++)
    m+=v[i];
  return m/=((double)v.size());
}

vnl_matrix<double> arp_covar(const vcl_vector<vnl_vector<double> >& v)
{
  vnl_vector<double> m=arp_mean(v);
  vnl_matrix<double> C=outer_product(v[0]-m,v[0]-m);

  for (unsigned int i=1;i<v.size();i++)
    C+=outer_product(v[i]-m,v[i]-m);

  return C/=((double)v.size());
}

vnl_vector<double> vector_of_squares(const vnl_vector<double>& v)
{
  vnl_vector<double> s=v;

  for (unsigned int i=0;i<s.size();i++)
    s[i]*=v[i];

  return s;
}

vnl_vector<double> arp_vars(const vcl_vector<vnl_vector<double> >& v)
{
  vnl_vector<double> m=arp_mean(v);
  vnl_vector<double> s=vector_of_squares(v[0]-m);

  for (unsigned int i=1;i<v.size();i++)
    s+=vector_of_squares(v[i]-m);

  return s/=((double)v.size());
}

void test_ar_process()
{
  const unsigned int SIZE = 100;
  vcl_cout << "************************\n"
           << " Testing mbl_ar_process\n"
           << "************************\n";

  vnl_random mz_random;
  mz_random.reseed(1000);

  // A, B and C are random diagonal matrices:
  vnl_matrix<double> A(2,2,0.0),B(2,2,0.0),C(2,2,0.0); // initialise to 0
  A(0,0)=mz_random.normal()/2.0;
  A(1,1)=mz_random.normal()/2.0;
  B(0,0)=mz_random.normal()/2.0;
  B(1,0)=mz_random.normal()/2.0;
  B(0,1)=mz_random.normal()/2.0;
  B(1,1)=mz_random.normal()/2.0;
#if 0 // was:
  A(0,0)=mz_random.normal()/3.0;
  A(1,0)=mz_random.normal()/3.0;
  A(0,1)=mz_random.normal()/3.0;
  A(1,1)=mz_random.normal()/3.0;
  B(0,0)=mz_random.normal()/3.0;
  B(1,0)=mz_random.normal()/3.0;
  B(0,1)=mz_random.normal()/3.0;
  B(1,1)=mz_random.normal()/3.0;
#endif
  C(0,0)=mz_random.normal()/20.0;
  C(1,0)=mz_random.normal()/20.0;
  C(0,1)=mz_random.normal()/20.0;
  C(1,1)=mz_random.normal()/20.0;

  // two start values of autoregressive process are random:
  vcl_vector<vnl_vector<double> > vlist;
  vlist.push_back(vnl_double_2(mz_random.normal(), mz_random.normal()).as_vector());
  vlist.push_back(vnl_double_2(mz_random.normal(), mz_random.normal()).as_vector());

  for (unsigned int i=0;i<SIZE;i++)
  {
    vnl_double_2 W(mz_random.normal(), mz_random.normal());
    vlist.push_back(A*vlist[i]+B*vlist[i+1]+C*W.as_vector());
  }

  mbl_ar_process<double> arp;
  arp.learn_burg(vlist);

  // glist has same 2 start values as vlist:
  vcl_vector<vnl_vector<double> > glist;
  glist.push_back(vlist[0]);
  glist.push_back(vlist[1]);

  for (unsigned int i=0;i<SIZE;i++)
    glist.push_back(arp.predict(glist[i],glist[i+1]));

  vnl_vector<double> dm=arp_mean(vlist)-arp_mean(glist);
  vnl_vector<double> ds=arp_vars(vlist)-arp_vars(glist);
#ifdef DEBUG
  vcl_cout << "\nvlist:"; for (unsigned int i=0;i<SIZE+2;++i) vcl_cout<<' '<<vlist[i];
  vcl_cout << "\nglist:"; for (unsigned int i=0;i<SIZE+2;++i) vcl_cout<<' '<<glist[i];
  vcl_cout << "\nv_mean: "<<arp_mean(vlist)<<"\tv_vars: "<<arp_vars(vlist)
           << "\ng_mean: "<<arp_mean(glist)<<"\tg_vars: "<<arp_vars(glist)
           << "\ndm="<<dm<<"\nds="<<ds<<'\n'
           << "\n||dm||="<<dm.inf_norm()<<"\n||ds||="<<ds.inf_norm()<<'\n';
#endif
  TEST("Similar mean for burg algorithm",dm.inf_norm()<0.1,true);
  TEST("Similar variances on each axis for burg algorithm",
       ds.inf_norm()<0.4,true);

  arp.learn(vlist);

  glist.clear();

  glist.push_back(vlist[0]);
  glist.push_back(vlist[1]);

  for (unsigned int i=0;i<SIZE;i++)
    glist.push_back(arp.predict(glist[i],glist[i+1]));

  dm=arp_mean(vlist)-arp_mean(glist);
  vnl_matrix<double> dC=arp_covar(vlist)-arp_covar(glist);
#ifdef DEBUG
  vcl_cout << "\nvlist:"; for (unsigned int i=0;i<SIZE+2;++i) vcl_cout<<' '<<vlist[i];
  vcl_cout << "\nglist:"; for (unsigned int i=0;i<SIZE+2;++i) vcl_cout<<' '<<glist[i];
  vcl_cout << "\nv_mean: "<<arp_mean(vlist)<<"\tv_covar: "<<arp_covar(vlist)
           << "\ng_mean: "<<arp_mean(glist)<<"\tg_covar: "<<arp_covar(glist)
           << "\ndm="<<dm<<"\ndC="<<dC<<'\n'
           << "\n||dm||="<<dm.inf_norm()<<"\n||dC||="<<dC.array_inf_norm()<<'\n';
#endif
  TEST("Similar mean",dm.inf_norm()<0.1,true);
  TEST("Similar covariance matrix",dC.array_inf_norm()<0.1,true);
}

TESTLIB_DEFINE_MAIN(test_ar_process);
