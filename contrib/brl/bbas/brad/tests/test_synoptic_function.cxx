#include <iostream>
#include <cstdlib>
#include <fstream>
#include <testlib/testlib_test.h>
#include <brad/brad_synoptic_function_1d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_synoptic_function()
{
  START("synoptic function test");
  brad_synoptic_function_1d sf;
  if(!sf.load_samples("E:/mundy/CVGroup/Nibbler/Notes/Experiments/intensities_and_viewing_direction_e2.txt"))
    return;
#if 1
  unsigned n = sf.size();
  for (unsigned i = 0; i<n; ++i){
    double s = sf.arc_length(i);

    std::cout << s << ' ' << sf.intensity(i)
             << ' ' << sf.cubic_interp_inten(s)<< ' ' << sf.vis(i) <<  '\n';
  }
  std::cout << "Fit Sigma " << sf.cubic_fit_sigma() <<'\n';
  std::cout << "Effective N obs " << sf.effective_n_obs() << '\n';
  std::cout << "Prob density " << sf.cubic_fit_prob_density() << '\n';
#endif

  sf.compute_auto_correlation();
  std::vector<double> corr = sf.auto_correlation();
  unsigned nc = corr.size();
  std::cout << "Autocorrelation function \n";
  for (unsigned i = 0; i<nc; ++i){
    double s = sf.arc_length(i);

    std::cout << s << ' ' << corr[i] <<  '\n';
  }
#if 0
  sf.fit_linear_const();
  std::cout << "tau_s " << sf.tau_s() << '\n';
  std::cout << "alpha " << sf.alpha() << '\n';
  std::cout << "mu " << sf.mu() << '\n';
  std::cout << "lin const sigma " << sf.lin_const_fit_sigma() << '\n';
  sf.set_inherent_data_sigma(0.348);
  std::cout << "lin const prob density " << sf.lin_const_fit_prob_density() << '\n';
#endif

#if 1
  std::cout << " Frequency Amplitudes\n";
  std::vector<double> freq_amps;
  sf.auto_corr_freq_amplitudes(freq_amps);
  for(unsigned i = 0; i<freq_amps.size(); ++i)
    std::cout << i << ' ' << freq_amps[i] << '\n';
  std::cout << "Max Frequency Prob. Density\n" << sf.max_frequency_prob_density() << '\n';
#endif
}
TESTMAIN( test_synoptic_function );
