#include <testlib/testlib_test.h>
#include <bsta/algo/bsta_mean_shift.h>
#include <vcl_string.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gaussian_indep.h>
#include <vcl_fstream.h>

vnl_random rng;

template <class T, unsigned n>
void test_mean_shift_algo(T epsilon, const vcl_string& type_name)
{
  typedef typename bsta_gaussian_indep<T,n>::vector_type vect_t;

  //: initialize 3 gaussian distributions
  T mean1 = T(0.504); T std_dev1 = T(0.005);
  T mean2 = T(0.004); T std_dev2 = T(0.005);
  T mean3 = T(1.004); T std_dev3 = T(0.005);
  
  vect_t m1(mean1); 
  vect_t m2(mean2); 
  vect_t m3(mean3); 

  //bsta_gaussian_sphere<T,n> g1(m1, std_dev1*std_dev1);
  //bsta_gaussian_sphere<T,n> g2(m2, std_dev2*std_dev2);
  //bsta_gaussian_sphere<T,n> g3(m3, std_dev3*std_dev3);
  vnl_vector_fixed<T,n> meanv1(m1), meanv2(m2), meanv3(m3);
  for (unsigned i = 1; i < n; i++) {
    meanv1[i] = T(rng.drand32()); meanv2[i] = T(rng.drand32()); meanv3[i] = T(rng.drand32()); }
  
  vnl_vector_fixed<T,n> sd1(T(vcl_pow(std_dev1, 2))), sd2(T(vcl_pow(std_dev2,2))), sd3(T(vcl_pow(std_dev3,2)));
  for (unsigned i = 1; i < n; i++) {
    sd1[i] = T(vcl_pow(rng.drand32()/1000,2)); sd2[i] = T(vcl_pow(rng.drand32()/1000,2)); sd3[i] = T(vcl_pow(rng.drand32()/1000,2)); }

  bsta_gaussian_indep<T,n> g1(meanv1, sd1);
  bsta_gaussian_indep<T,n> g2(meanv2, sd2);
  bsta_gaussian_indep<T,n> g3(meanv3, sd3);

  unsigned n1 = (unsigned)(rng.drand32()*100);
  unsigned n2 = (unsigned)(rng.drand32()*100);
  unsigned n3 = (unsigned)(rng.drand32()*100);

  T bandwidth = T(0.2);
  //T bandwidth = T(1.0);
  bsta_mean_shift_sample_set<T, n> ss(bandwidth);
  TEST_NEAR((type_name + ": mean shift sample set constructor").c_str(), bandwidth, ss.bandwidth(), epsilon);
  
  for (unsigned i = 0; i < n1; i++) {
    vect_t s = g1.sample(rng);
    //float w1 = 0.99f;
    ss.insert_sample(s);
    //ss.insert_w_sample(s, w1);
  }
  for (unsigned i = 0; i < n2; i++) {
    //float w2 = 0.01f;
    ss.insert_sample(g2.sample(rng));
    //ss.insert_w_sample(g2.sample(rng), w2);
  }
  for (unsigned i = 0; i < n3; i++) {
    //float w3 = 0.01f;
    ss.insert_sample(g3.sample(rng));
    //ss.insert_w_sample(g3.sample(rng), w3);
  }
  TEST_NEAR((type_name + ": mean shift sample set insert method").c_str(), n1+n2+n3, ss.size(), epsilon);
  
  vect_t out;
  TEST("find mean around m1", ss.mean(meanv1, out), true);
  vcl_cout << "calculated mean: " << out << " within m1: " << meanv1 << vcl_endl;
  vect_t dif = out-meanv1;
  vnl_vector_fixed<T,n> dummy(dif);
  TEST_NEAR((type_name + ": mean shift sample set mean method").c_str(), dummy.magnitude(), T(0), epsilon);
  

  bsta_mean_shift<T,n> ms;
  ms.find_modes(ss, rng, 50.0f, T(1e-2));
  //ms.find_modes(ss, T(1e-2));
  vcl_cout << "modes size: " << ms.size() << vcl_endl;

  ms.trim_modes(T(1e-1));
  vcl_cout << "modes size after trimming: " << ms.size() << vcl_endl;
  TEST((type_name + ": mean shift find modes and trim modes methods").c_str(), ms.size(), 3);

  //: print m files to visualize the sample set and the fitted modes
  vcl_ofstream of((type_name + "_out.m").c_str(), vcl_ios::out);
  switch(n) {
    case 1: {
      vcl_cout << "case 1\n"; 
      of << "x = [" << ss.sample(0);
      for (unsigned i = 0; i < ss.size(); i++) {
        of << ", " << ss.sample(i);
      }
      of << "];\n";
      of << "y = zeros(size(x));\n";
      of << "plot(x,y,'or');\n";
      break;
            }
    case 2: {
      vcl_cout << "case 2\n";
      of << "cmap = colormap(lines(" << ms.modes().size() << "));\n";
      //: plot the assignments
      for (unsigned m = 0; m < ms.modes().size(); m++) {
        vcl_vector<vcl_pair<T,T> > points;
        for (unsigned i = 0; i < ms.assignments().size(); i++) {
          if (ms.assignments()[i].first)
            if (ms.assignments()[i].second == m)
              points.push_back(vcl_pair<T,T>(ss.sample(i)[0], ss.sample(i)[1]));
        }
        if (points.size() > 0) {
          of << "x = [" << points[0].first;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].first;
          }
          of << "];\n";
          of << "y = [" << points[0].second;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].second;
          }
          of << "];\n";
          of << "h = plot(x,y,'or');\nset(h, 'Color', cmap(" << m+1 << ",:));\n";
          of << "hold on\n";
        }
        of << "xx = [" << (ms.modes()[m])[0] << "];\n";
        of << "yy = [" << (ms.modes()[m])[1] << "];\n";
        of << "h = plot(xx,yy,'+r');\nset(h, 'Color', cmap(" << m+1 << ",:));\n";  
        of << "hold on\n";
      }

      /*
      vnl_vector_fixed<T,2> s0(ss.sample(0));
      of << "x = [" << s0[0];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,2> si(ss.sample(i));
        of << ", " << si[0];
      }
      of << "];\n";
      of << "y = [" << s0[1];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,2> si(ss.sample(i));
        of << ", " << si[1];
      }
      of << "];\n";
      of << "plot(x,y,'or');\n";
      */
      /*
      vnl_vector_fixed<T,2> m0(ms.modes()[0]);
      of << "xm = [" << m0[0];
      for (unsigned i = 0; i < ms.size(); i++) {
        vnl_vector_fixed<T,2> mi(ms.modes()[i]);
        of << ", " << mi[0];
      }
      of << "];\n";
      of << "ym = [" << m0[1];
      for (unsigned i = 0; i < ms.size(); i++) {
        vnl_vector_fixed<T,2> mi(ms.modes()[i]);
        of << ", " << mi[1];
      }
      of << "];\nhold on\n";
      of << "plot(xm, ym, 'ob');\n";
     */

      break;
            }
     
    case 3: {
      vcl_cout << "case 3\n";
      vnl_vector_fixed<T,3> s0(ss.sample(0));
      of << "x = [" << s0[0];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[0];
      }
      of << "];\n";
      of << "y = [" << s0[1];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[1];
      }
      of << "];\n";
      of << "z = [" << s0[2];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[2];
      }
      of << "];\n";
      of << "plot3(x,y,z,'or');\n";
      break;
            }
    default: vcl_cout << " default\n"; 
  }
  of.close();

  TEST("recompute modes ", ms.recompute_modes(ss), true);

  //: print m files to visualize the sample set and the fitted modes
  of.open((type_name + "_out_recomputed.m").c_str(), vcl_ios::out);
 
  switch(n) {
    case 1: {
      vcl_cout << "case 1\n"; 
      of << "x = [" << ss.sample(0);
      for (unsigned i = 0; i < ss.size(); i++) {
        of << ", " << ss.sample(i);
      }
      of << "];\n";
      of << "y = zeros(size(x));\n";
      of << "plot(x,y,'or');\n";
      break;
            }
    case 2: {
      vcl_cout << "case 2\n";
      of << "cmap = colormap(lines(" << ms.modes().size() << "));\n";
      //: plot the assignments
      for (unsigned m = 0; m < ms.modes().size(); m++) {
        vcl_vector<vcl_pair<T,T> > points;
        for (unsigned i = 0; i < ms.assignments().size(); i++) {
          if (ms.assignments()[i].first)
            if (ms.assignments()[i].second == m)
              points.push_back(vcl_pair<T,T>(ss.sample(i)[0], ss.sample(i)[1]));
        }
        if (points.size() > 0) {
          of << "x = [" << points[0].first;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].first;
          }
          of << "];\n";
          of << "y = [" << points[0].second;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].second;
          }
          of << "];\n";
          of << "h = plot(x,y,'or');\nset(h, 'Color', cmap(" << m+1 << ",:));\n";
          of << "hold on\n";
        }
        of << "xx = [" << (ms.modes()[m])[0] << "];\n";
        of << "yy = [" << (ms.modes()[m])[1] << "];\n";
        of << "h = plot(xx,yy,'*r');\nset(h, 'Color', cmap(" << m+1 << ",:));\n";  
        of << "hold on\n";
      }

      break;
            }
     
    case 3: {
      vcl_cout << "case 3\n";
      vnl_vector_fixed<T,3> s0(ss.sample(0));
      of << "x = [" << s0[0];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[0];
      }
      of << "];\n";
      of << "y = [" << s0[1];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[1];
      }
      of << "];\n";
      of << "z = [" << s0[2];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[2];
      }
      of << "];\n";
      of << "plot3(x,y,z,'or');\n";
      break;
            }
    default: vcl_cout << " default\n"; 
  }
  of.close();
}


MAIN( test_mean_shift )
{
  START ("mean_shift");
  
  test_mean_shift_algo<float,2>(float(1e-1),"float_2");
  test_mean_shift_algo<float,3>(float(1e-1),"float_3");
  SUMMARY();
}


