#include <testlib/testlib_test.h>
#include <brad/brad_synoptic_function_1d.h>
#include <vcl_cstdlib.h> // for rand()
#include <vcl_fstream.h>

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

    vcl_cout << s << ' ' << sf.intensity(i) 
             << ' ' << sf.cubic_interp_inten(s)<< ' ' << sf.vis(i) <<  '\n';
  }
  vcl_cout << "Fit Sigma " << sf.cubic_fit_sigma() <<'\n';
  vcl_cout << "Effective N obs " << sf.effective_n_obs() << '\n';
  vcl_cout << "Prob density " << sf.cubic_fit_prob_density() << '\n';
#endif

  sf.compute_auto_correlation();
  vcl_vector<double> corr = sf.auto_correlation();
  unsigned nc = corr.size();
  vcl_cout << "Autocorrelation function \n";
  for (unsigned i = 0; i<nc; ++i){
    double s = sf.arc_length(i);

    vcl_cout << s << ' ' << corr[i] <<  '\n';
  }
#if 0
  sf.fit_linear_const();
  vcl_cout << "tau_s " << sf.tau_s() << '\n';
  vcl_cout << "alpha " << sf.alpha() << '\n';
  vcl_cout << "mu " << sf.mu() << '\n';
  vcl_cout << "lin const sigma " << sf.lin_const_fit_sigma() << '\n';
  sf.set_inherent_data_sigma(0.348);
  vcl_cout << "lin const prob density " << sf.lin_const_fit_prob_density() << '\n';
#endif

#if 1
  vcl_cout << " Frequency Amplitudes\n";
  vcl_vector<double> freq_amps;
  sf.auto_corr_freq_amplitudes(freq_amps);
  for(unsigned i = 0; i<freq_amps.size(); ++i)
    vcl_cout << i << ' ' << freq_amps[i] << '\n';
  vcl_cout << "Max Frequency Prob. Density\n" << sf.max_frequency_prob_density() << '\n';
#endif
}
TESTMAIN( test_synoptic_function );

