// This is brl/bseg/brec/pro/processes/brec_prob_map_roc_compute_process.cxx

//:
// \file
// \brief A process to generate TPR and FPR for a given prob map and
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli adapted from Joseph Mundy's bbgm_roc_compute_process
// \date September 10, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//: Constructor
bool brec_prob_map_roc_compute_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame's mask
  input_types.emplace_back("vil_image_view_base_sptr"); //foreground mask
  input_types.emplace_back("vcl_string"); //output file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("float");  // return the threshold at the best operating point
  output_types.emplace_back("float");  // return a threshold
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool brec_prob_map_roc_compute_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << " brec_prob_map_roc_compute_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> frame = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = frame.ni(), nj = frame.nj();

  temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<bool> fore_mask = *vil_convert_cast(bool(), temp);

  std::string out_file = pro.get_input<std::string>(i++);

  std::vector<float> forepix, backpix;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        if (fore_mask(i,j))
          forepix.push_back(frame(i,j));
        else
          backpix.push_back(frame(i,j));
      }
    }
  unsigned nf = forepix.size(), nb = backpix.size();
  if (!nf||!nb)
    return false;
  double nfd = nf, nbd = nb;
#if 0
  std::cout << "false_pos_fore true_pos_fore\n"
           << "----------------------------------------------------------\n";
  double inc = 0.05;
  unsigned N = 47;
  double pa[47]={0.0, 0.0005,  0.001, 0.005, 0.01, 0.05,  0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.82, 0.85, 0.87,0.88, 0.89, 0.9, 0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99, 0.992, 0.995, 0.997, 0.9975, 0.998,0.9982, 0.9985, 0.9987, 0.999, 0.9992, 0.9995, 0.9996, 0.99965, 0.9997, 0.99975, 0.99977, 0.9998, 1.0};
#endif // 0
  unsigned N = 63;
  double pa[63]={0.0, 0.0001, 0.0002, 0.0003, 0.0004, 0.0005,  0.0006, 0.0007, 0.0008, 0.0009, 0.001, 0.005, 0.01, 0.05,  0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.82, 0.85, 0.87,0.88, 0.89, 0.9, 0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99, 0.992, 0.995, 0.997, 0.9975, 0.998,0.9982, 0.9985, 0.9987, 0.999, 0.9992, 0.9995, 0.9996, 0.99965, 0.9997, 0.99975, 0.99977, 0.9998, 0.99991, 0.99995, 0.999991, 0.999995, 0.9999991, 0.9999995, 0.99999991, 0.99999995, 1.0};
  //unsigned N = 58;
  //double pa[58]={0.0, 0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009, 0.01, 0.015, 0.02, 0.025, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.13, 0.15, 0.17, 0.2, 0.23, 0.25, 0.27, 0.3, 0.33, 0.35, 0.37, 0.4, 0.43, 0.45, 0.47, 0.5, 0.53, 0.55, 0.57, 0.6, 0.63, 0.65, 0.67, 0.7, 0.73, 0.75, 0.77, 0.8, 0.83, 0.85, 0.87, 0.9, 0.93, 0.95, 0.97, 1.0};
#if 0
  unsigned N = 0;
  std::vector<double> pa;
  for (double th = 0.0; th <= 1.0; th+=0.002) {
    pa.push_back(th);
    N++;
  }
#endif // 0

  std::vector<double> fpr, tpr;
  for (unsigned ip = 0; ip<N; ++ip)
  {
    double p = pa[ip];
    unsigned nfore = 0, nback = 0;
    for (unsigned kf =0; kf<nf; ++kf)
      if (forepix[kf]<=p) nfore++;
    for (unsigned kb =0; kb<nb; ++kb)
      if (backpix[kb]>p) nback++;

    double nfored = nfore, nbackd = nback;
    double fore_true_pos_frac = nfored/nfd;
    double fore_false_pos_frac = (nbd-nbackd)/nbd;
    fpr.push_back(fore_false_pos_frac);
    tpr.push_back(fore_true_pos_frac);
    //std::cout << fore_false_pos_frac << ' ' << fore_true_pos_frac << std::endl;
  }

  double best_dist = 10000000000.0;
  unsigned best_id = 0;
  unsigned fpr_id = 0;
  bool fpr_set = false;
  for (unsigned ip = 0; ip < N; ip++) {
    double dist = fpr[ip]*fpr[ip]+(1-tpr[ip])*(1-tpr[ip]);
    if (dist < best_dist) {
      best_id = ip;
      best_dist = dist;
    }
    if (fpr[ip] >= 0.1 && !fpr_set) {
      fpr_id = ip;
      fpr_set = true;
    }
  }


  std::cout << " threshold at fpr >= 0.1 is " << pa[fpr_id] << " and actual fpr is: " << fpr[fpr_id] << std::endl;

  pro.set_output_val<float>(0, (float)pa[best_id]);
  pro.set_output_val<float>(1, (float)pa[fpr_id]);

  std::ofstream of(out_file.c_str());
  of << "# brec_prob_map_roc_compute_process\n#line 1: threshold values\n#line 2: FPR values for thresholds\n#line 3: TPR values\n";
  for (unsigned ip = 0; ip<N; ++ip)
    of << pa[ip] << '\t';
  of << std::endl;
  for (unsigned ip = 0; ip<N; ++ip)
    of << fpr[ip] << '\t';
  of << std::endl;
  for (unsigned ip = 0; ip<N; ++ip)
    of << tpr[ip] << '\t';
  of << std::endl;
  of.close();

  return true;
}

//: Process to construct ROC cumulatively over many images, computes FP and TP for a given image and outputs these values to be summed by an external process, e.g. through python
//  Constructor
bool brec_prob_map_roc_compute2_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame's mask
  input_types.emplace_back("vil_image_view_base_sptr"); //foreground mask
  input_types.emplace_back("double");  // threshold value
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("float");  // return TP
  output_types.emplace_back("float");  // return FP
  output_types.emplace_back("float");  // return number of foreground pixels, nfd, TPR is TP/nfd
  output_types.emplace_back("float");  // return number of background pixels, nbd, FPR is FP/nbd
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool brec_prob_map_roc_compute2_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << " brec_prob_map_roc_compute2_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> frame = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = frame.ni(), nj = frame.nj();

  temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<bool> fore_mask = *vil_convert_cast(bool(), temp);

  auto threshold = pro.get_input<double>(i++);

  std::vector<float> forepix, backpix;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        if (fore_mask(i,j))
          forepix.push_back(frame(i,j));
        else
          backpix.push_back(frame(i,j));
      }
    }
  unsigned nf = forepix.size(), nb = backpix.size();
  if (!nf||!nb)
    return false;
  auto nfd = (float)nf, nbd = (float)nb;

  double p = threshold;
  unsigned nfore = 0, nback = 0;
  for (unsigned kf =0; kf<nf; ++kf)
    if (forepix[kf]<=p) nfore++;
  for (unsigned kb =0; kb<nb; ++kb)
    if (backpix[kb]>p) nback++;

  auto nfored = (float)nfore, nbackd = (float)nback;
  //double fore_true_pos_frac = nfored/nfd;
  //double fore_false_pos_frac = (nbd-nbackd)/nbd;

  pro.set_output_val<float>(0, nfored);
  pro.set_output_val<float>(1, nbd-nbackd);
  pro.set_output_val<float>(2, nfd);
  pro.set_output_val<float>(3, nbd);
  return true;
}
