#include <string>
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <bsta/algo/bsta_mean_shift.h>
#include <bsta/algo/bsta_sample_set.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_full.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vnl_random rng;

template<class T>
void test_sample_set(T epsilon, const std::string& type_name)
{
  T meanv1(T(0.5));

  T var1(T(std::pow(T(0.1), 2)));

  bsta_gaussian_sphere<T,1> g1(meanv1, var1);

  T bandwidth = T(0.2);
  //T bandwidth = T(1.0);
  bsta_sample_set<T, 1> ss(bandwidth);
  TEST_NEAR((type_name + ": mean shift sample set constructor").c_str(), bandwidth, ss.bandwidth(), epsilon);

  unsigned n1 = 1000;
  float w0 = 0.0f;
  for (unsigned i = 0; i < n1; i++) {
    T s = g1.sample(rng);
    auto w = float(rng.drand32());
    if (i == 0)
      w0 = w;
    ss.insert_sample(s, w);
  }

  TEST_NEAR((type_name + ": mean shift sample set insert method").c_str(), n1, ss.size(), epsilon);
  TEST_NEAR((type_name + ": mean shift sample set insert method").c_str(), ss.weight(0), w0, epsilon);

  T out;
  TEST("find mean around m1", ss.mean(meanv1, out), true);
  std::cout << "calculated mean:\n " << out << " \nwithin m1:\n " << meanv1 << std::endl;
  T dif = out-meanv1;
  TEST_NEAR((type_name + ": mean shift sample set mean method").c_str(), dif, T(0), epsilon);

  ss.initialize_assignments();
  TEST((type_name + ": mean shift sample set initialize assingment").c_str(), ss.assignments().size(), ss.size());

  for (unsigned i = 0; i < n1; i++) {
    ss.set_assignment(i, 0);
  }

  T out2;
  ss.mode_mean(0, out2);
  std::cout << "calculated mean:\n " << out2 << " \nwithin m1:\n " << meanv1 << std::endl;
  dif = out2-meanv1;
  TEST_NEAR((type_name + ": mean shift sample set mean of the mode method").c_str(), dif, T(0), epsilon);

  // calculate variance
  T var;
  bsta_sample_set_variance<T>(ss, 0, T(MIN_VAR_), var);
  std::cout << "-----------------\n"
           << "calculated variance:\n " << var << '\n'
           << "-----------------\n";
  TEST_NEAR((type_name + ": mean shift sample set variance of the mode method").c_str(), var, var1, epsilon);

  // fit a mixture distribution
  bsta_mixture<bsta_num_obs<bsta_gaussian_sphere<T,1> > > out_dist;
  bool return_value = bsta_sample_set_fit_distribution<T>(ss, out_dist);
  TEST((type_name + ": mean shift fit distribution to sample set method").c_str(), return_value, true);
  TEST((type_name + ": mean shift fit distribution to sample set method").c_str(), out_dist.num_components(), ss.mode_cnt());

  std::cout << "prob density of meanv1: " << out_dist.prob_density(meanv1) << '\n'
           << "prob density of some point from ss: " << out_dist.prob_density(ss.sample(0)) << '\n';

  T likelihood = T(0);
  for (unsigned mi = 0; mi < out_dist.num_components(); mi++) {
    likelihood += bsta_sample_set_log_likelihood(ss, out_dist.distribution(mi), ss.total_weight());
  }
  std::cout << "likelihood for ss: " << likelihood << '\n';
}

template<class T, unsigned n>
void test_sample_set(T epsilon, const std::string& type_name)
{
  typedef typename bsta_gaussian_indep<T,n>::vector_type vect_t;
  typedef typename bsta_gaussian_full<T,n>::covar_type var_t;

  vnl_vector_fixed<T,n> meanv1(T(0.5));
  for (unsigned i = 1; i < n; i++)
    meanv1[i] = T(rng.drand32());

  vnl_vector_fixed<T,n> sd1(T(std::pow(T(0.1), 2)));
  for (unsigned i = 1; i < n; i++)
    sd1[i] = T(std::pow(rng.drand32()/10,2));

  bsta_gaussian_indep<T,n> g1(meanv1, sd1);

  vnl_vector_fixed<T,n> meanv2(T(0.1));
  for (unsigned i = 1; i < n; i++)
    meanv2[i] = T(rng.drand32());
  bsta_gaussian_indep<T,n> g2(meanv2, sd1);

  T bandwidth = T(0.2);
  //T bandwidth = T(1.0);
  bsta_sample_set<T, n> ss(bandwidth);
  TEST_NEAR((type_name + ": mean shift sample set constructor").c_str(), bandwidth, ss.bandwidth(), epsilon);

  int n1 = 1000;
  T w0 = T(0.0);
  T total_weight = T(0.0);
  for (int i = 0; i < n1; i++) {
    vect_t s = g1.sample(rng);
    T w = T(rng.drand32());
    if (i == 0)
      w0 = w;
    ss.insert_sample(s, w);
    total_weight += w;
  }

  int n2 = 1000;
  T weight1 = total_weight;
  for (int i = 0; i < n2; i++) {
    vect_t s = g2.sample(rng);
    T w = T(rng.drand32());
    ss.insert_sample(s, w);
    total_weight += w;
  }

  TEST_NEAR((type_name + ": mean shift sample set insert method").c_str(), n1+n2, ss.size(), epsilon);
  TEST_NEAR((type_name + ": mean shift sample set insert method").c_str(), ss.weight(0), w0, epsilon);

  vect_t out;
  TEST("find mean around m1", ss.mean(meanv1, out), true);
  std::cout << "calculated mean:\n " << out << " \nwithin m1:\n " << meanv1 << std::endl;
  vect_t dif = out-meanv1;
  vnl_vector_fixed<T,n> dummy(dif);
  TEST_NEAR((type_name + ": mean shift sample set mean method").c_str(), dummy.magnitude(), T(0), epsilon);

  ss.initialize_assignments();
  TEST((type_name + ": mean shift sample set initialize assingment").c_str(), ss.assignments().size(), ss.size());
  for (int i = 0; i < n1; i++) {
    ss.set_assignment(i, 0);
  }
  for (int i = n1; i < n1+n2; i++) {
    ss.set_assignment(i, 1);
  }

  TEST((type_name + ": mean shift sample set assingment cnt").c_str(), n1, ss.mode_size(0));
  TEST((type_name + ": mean shift sample set assingment cnt").c_str(), n2, ss.mode_size(1));
  TEST((type_name + ": mean shift sample set number of modes").c_str(), 2, ss.mode_cnt());
  TEST_NEAR((type_name + ": mean shift sample set total_weight method").c_str(), ss.total_weight(), total_weight, epsilon);
  TEST_NEAR((type_name + ": mean shift sample set mode weight method").c_str(), ss.mode_weight(0), weight1, epsilon);

  vect_t out2;
  ss.mode_mean(0, out2);
  std::cout << "calculated mean:\n " << out2 << " \nwithin m1:\n " << meanv1 << std::endl;
  dif = out2-meanv1;
  vnl_vector_fixed<T,n> dummy2(dif);
  TEST_NEAR((type_name + ": mean shift sample set mean of the mode method").c_str(), dummy2.magnitude(), T(0), epsilon);

  // calculate variance
  var_t var;
  bsta_sample_set_variance<T,n>(ss, 0, var);
  std::cout << "-----------------\n"
           << "calculated variance:\n " << var << '\n'
           << "-----------------\n";

  vect_t out3;
  ss.mode_mean(1, out3);
  std::cout << "calculated mean:\n " << out3 << " \nwithin m1:\n " << meanv2 << std::endl;
  dif = out3-meanv2;
  vnl_vector_fixed<T,n> dummy3(dif);
  TEST_NEAR((type_name + ": mean shift sample set mean of the mode method").c_str(), dummy3.magnitude(), T(0), epsilon);

  // calculate variance
  bsta_sample_set_variance<T,n>(ss, 1, var);
  std::cout << "-----------------\n"
           << "calculated variance:\n " << var << '\n'
           << "-----------------\n";

  // fit a mixture distribution
  bsta_mixture<bsta_num_obs<bsta_gaussian_full<T,n> > > out_dist;
  bool return_value = bsta_sample_set_fit_distribution<T,n>(ss, out_dist);
  TEST((type_name + ": mean shift fit distribution to sample set method").c_str(), return_value, true);
  TEST((type_name + ": mean shift fit distribution to sample set method").c_str(), out_dist.num_components(), ss.mode_cnt());

  std::cout << "prob density of meanv1: " << out_dist.prob_density(meanv1) << '\n'
           << "prob density of meanv2: " << out_dist.prob_density(meanv2) << '\n'
           << "prob density of some point from ss: " << out_dist.prob_density(ss.sample(0)) << '\n';

  // compare the data likelihood of out_dist with a random gaussian mixture distribution with the same number of modes
  bsta_mixture<bsta_num_obs<bsta_gaussian_full<T,n> > > rand_dist;
  for (unsigned mi = 0; mi < ss.mode_cnt(); mi++) {
    vnl_vector_fixed<T,n> mv(T(0));
    for (unsigned i = 1; i < n; i++)
      mv[i] = T(rng.drand32());

    vnl_matrix_fixed<T,n,n> covar(T(0.01));
    for (unsigned r = 0; r < n; r++)
      covar[r][r] = T(0.1);

    bsta_gaussian_full<T,n> gauss_d(mv,covar);
    bsta_num_obs<bsta_gaussian_full<T,n> > gauss_d_n(gauss_d, ss.total_weight()/ss.mode_cnt());
    //bsta_gaussian_full<T,n> gauss_d(meanv1,covar);
    TEST((type_name + ": mean shift mode addition to random distribution").c_str(), rand_dist.insert(gauss_d_n, 1.0f/ss.mode_cnt()), true);
  }

  out_dist.normalize_weights();
  rand_dist.normalize_weights();

  T likelihood_sum1 = T(0); T likelihood_sum2 = T(0);
  for (unsigned i = 0; i < ss.size(); i++) {
    likelihood_sum1 += std::log(out_dist.prob_density(ss.sample(i)));
    likelihood_sum2 += std::log(rand_dist.prob_density(ss.sample(i)));
  }
  std::cout << "data likelihood of ss fitted dist: " << likelihood_sum1 << '\n'
           << "data likelihood of random dist: " << likelihood_sum2 << '\n';


  for (unsigned mi = 0; mi < out_dist.num_components(); mi++) {
    T likelihood = bsta_sample_set_log_likelihood(ss, out_dist.distribution(mi), ss.total_weight());
    std::cout << "data likelihood by component: " << mi << ": " << likelihood << '\n';
  }

  for (unsigned mi = 0; mi < rand_dist.num_components(); mi++) {
    T likelihood = bsta_sample_set_log_likelihood(ss, rand_dist.distribution(mi), ss.total_weight());
    std::cout << "data likelihood of random dist by component: " << mi << ": " << likelihood << '\n';
  }
}

template <class T, unsigned n>
void test_mean_shift_algo(T epsilon, const std::string& type_name)
{
  typedef typename bsta_gaussian_indep<T,n>::vector_type vect_t;

  // initialize 3 gaussian distributions
  T mean1 = T(0.504); T std_dev1 = T(0.05*epsilon);
  T mean2 = T(0.004); T std_dev2 = T(0.05*epsilon);
  T mean3 = T(1.004); T std_dev3 = T(0.05*epsilon);

  vect_t m1(mean1);
  vect_t m2(mean2);
  vect_t m3(mean3);

  //bsta_gaussian_sphere<T,n> g1(m1, std_dev1*std_dev1);
  //bsta_gaussian_sphere<T,n> g2(m2, std_dev2*std_dev2);
  //bsta_gaussian_sphere<T,n> g3(m3, std_dev3*std_dev3);
  vnl_vector_fixed<T,n> meanv1(m1), meanv2(m2), meanv3(m3);
  for (unsigned i = 1; i < n; i++) {
    meanv1[i] = T(rng.drand32()); meanv2[i] = T(rng.drand32()); meanv3[i] = T(rng.drand32()); }

  vnl_vector_fixed<T,n> sd1(T(std::pow(std_dev1, 2))), sd2(T(std::pow(std_dev2,2))), sd3(T(std::pow(std_dev3,2)));
  for (unsigned i = 1; i < n; i++) {
    sd1[i] = T(std::pow(rng.drand32()/1000,2)); sd2[i] = T(std::pow(rng.drand32()/1000,2)); sd3[i] = T(std::pow(rng.drand32()/1000,2)); }

  bsta_gaussian_indep<T,n> g1(meanv1, sd1);
  bsta_gaussian_indep<T,n> g2(meanv2, sd2);
  bsta_gaussian_indep<T,n> g3(meanv3, sd3);

  auto n1 = (unsigned)(rng.drand32()*100);
  auto n2 = (unsigned)(rng.drand32()*100);
  auto n3 = (unsigned)(rng.drand32()*100);

  T bandwidth = T(0.2);
  //T bandwidth = T(1.0);
  bsta_sample_set<T, n> ss(bandwidth);

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

  bsta_mean_shift<T,n> ms;
  ms.find_modes(ss, rng, 50.0f, T(1e-2));
  //ms.find_modes(ss, T(1e-2));
  std::cout << "modes size: " << ms.size() << std::endl;

  ms.trim_modes(ss, T(1e-1));
  std::cout << "modes size after trimming: " << ms.size() << std::endl;
  TEST((type_name + ": mean shift find modes and trim modes methods").c_str(), ms.size(), 3);

  // print m files to visualize the sample set and the fitted modes
  std::ofstream of((type_name + "_out.m").c_str(), std::ios::out);
  switch (n)
  {
    case 1:
      std::cout << "case n==1\n";
      of << "x = [" << ss.sample(0);
      for (unsigned i = 0; i < ss.size(); i++) {
        of << ", " << ss.sample(i);
      }
      of << "];\ny = zeros(size(x));\nplot(x,y,'or');\n";
      break;

    case 2:
      std::cout << "case n==2\n";
      of << "cmap = colormap(lines(" << ms.modes().size() << "));\n";
      // plot the assignments
      for (unsigned m = 0; m < ms.modes().size(); m++) {
        std::vector<std::pair<T,T> > points;
        for (unsigned i = 0; i < ss.assignments().size(); i++) {
          if (ss.assignment(i) == (int)m)
            points.push_back(std::pair<T,T>(ss.sample(i)[0], ss.sample(i)[1]));
        }
        if (points.size() > 0) {
          of << "x = [" << points[0].first;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].first;
          }
          of << "];\ny = [" << points[0].second;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].second;
          }
          of << "];\n"
             << "h = plot(x,y,'or');\nset(h, 'Color', cmap(" << m+1 << ",:));\n"
             << "hold on\n";
        }
        of << "xx = [" << (ms.modes()[m])[0] << "];\n"
           << "yy = [" << (ms.modes()[m])[1] << "];\n"
           << "h = plot(xx,yy,'+r');\nset(h, 'Color', cmap(" << m+1 << ",:));\n"
           << "hold on\n";
      }
#if 0
      vnl_vector_fixed<T,2> s0(ss.sample(0));
      of << "x = [" << s0[0];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,2> si(ss.sample(i));
        of << ", " << si[0];
      }
      of << "];\ny = [" << s0[1];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,2> si(ss.sample(i));
        of << ", " << si[1];
      }
      of << "];\nplot(x,y,'or');\n";

      vnl_vector_fixed<T,2> m0(ms.modes()[0]);
      of << "xm = [" << m0[0];
      for (unsigned i = 0; i < ms.size(); i++) {
        vnl_vector_fixed<T,2> mi(ms.modes()[i]);
        of << ", " << mi[0];
      }
      of << "];\nym = [" << m0[1];
      for (unsigned i = 0; i < ms.size(); i++) {
        vnl_vector_fixed<T,2> mi(ms.modes()[i]);
        of << ", " << mi[1];
      }
      of << "];\nhold on\nplot(xm, ym, 'ob');\n";
#endif
      break;

    case 3:
    {
      std::cout << "case n==3\n";
      vnl_vector_fixed<T,n> s0(ss.sample(0));
      of << "x = [" << s0[0];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,n> si(ss.sample(i));
        of << ", " << si[0];
      }
      of << "];\ny = [" << s0[1];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,n> si(ss.sample(i));
        of << ", " << si[1];
      }
      of << "];\nz = [" << s0[2];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,n> si(ss.sample(i));
        of << ", " << si[2];
      }
      of << "];\nplot3(x,y,z,'or');\n";
      break;
    }
    default:
      std::cout << " default\n";
  }
  of.close();
#if 0
  TEST("recompute modes ", ms.recompute_modes(ss), true);

  // print m files to visualize the sample set and the fitted modes
  of.open((type_name + "_out_recomputed.m").c_str(), std::ios::out);

  switch (n)
  {
    case 1:
      std::cout << "case 1\n";
      of << "x = [" << ss.sample(0);
      for (unsigned i = 0; i < ss.size(); i++) {
        of << ", " << ss.sample(i);
      }
      of << "];\ny = zeros(size(x));\nplot(x,y,'or');\n";
      break;

    case 2:
      std::cout << "case 2\n";
      of << "cmap = colormap(lines(" << ms.modes().size() << "));\n";
      // plot the assignments
      for (unsigned m = 0; m < ms.modes().size(); m++) {
        std::vector<std::pair<T,T> > points;
        for (unsigned i = 0; i < ms.assignments().size(); i++) {
          if (ms.assignments()[i].first)
            if (ms.assignments()[i].second == m)
              points.push_back(std::pair<T,T>(ss.sample(i)[0], ss.sample(i)[1]));
        }
        if (points.size() > 0) {
          of << "x = [" << points[0].first;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].first;
          }
          of << "];\ny = [" << points[0].second;
          for (unsigned i = 1; i < points.size(); i++) {
            of << ", " << points[i].second;
          }
          of << "];\n"
             << "h = plot(x,y,'or');\nset(h, 'Color', cmap(" << m+1 << ",:));\n"
             << "hold on\n";
        }
        of << "xx = [" << (ms.modes()[m])[0] << "];\n"
           << "yy = [" << (ms.modes()[m])[1] << "];\n"
           << "h = plot(xx,yy,'*r');\nset(h, 'Color', cmap(" << m+1 << ",:));\n"
           << "hold on\n";
      }
      break;

    case 3:
    {
      std::cout << "case 3\n";
      vnl_vector_fixed<T,3> s0(ss.sample(0));
      of << "x = [" << s0[0];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[0];
      }
      of << "];\ny = [" << s0[1];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[1];
      }
      of << "];\nz = [" << s0[2];
      for (unsigned i = 0; i < ss.size(); i++) {
        vnl_vector_fixed<T,3> si(ss.sample(i));
        of << ", " << si[2];
      }
      of << "];\nplot3(x,y,z,'or');\n";
      break;
    }
    default:
      std::cout << " default\n";
  }
  of.close();
#endif // 0
}


static void test_mean_shift()
{
  test_sample_set<float,2>(0.1f, "float_2");
  test_sample_set<float>(0.1f, "float_1");

  test_mean_shift_algo<float,2>(0.1f,"float_2");
  test_mean_shift_algo<float,3>(0.1f,"float_3");
}

TESTMAIN(test_mean_shift);
