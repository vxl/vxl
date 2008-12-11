// This is brl/bseg/bvxm/rec/pro/bvxm_prob_map_roc_compute_process.cxx
#include "bvxm_prob_map_roc_compute_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <core/vidl2_pro/vidl2_pro_utils.h>

//: Constructor
bvxm_prob_map_roc_compute_process::bvxm_prob_map_roc_compute_process()
{
  //input
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask
  input_types_[2]= "vil_image_view_base_sptr"; //foreground mask
  input_types_[3]= "vcl_string"; //output file name

  //output - no outputs as yet
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0] = "float";  // return the threshold at the best operating point
}

//: Execute the process
bool
bvxm_prob_map_roc_compute_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In bvxm_prob_map_roc_compute_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr temp = input0->value();
  vil_image_view<float> frame = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = frame.ni(), nj = frame.nj();

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  temp = input1->value();
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  brdb_value_t<vil_image_view_base_sptr>* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  temp = input2->value();
  vil_image_view<bool> fore_mask = *vil_convert_cast(bool(), temp);

  brdb_value_t<vcl_string>* input3 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[3].ptr());
  vcl_string out_file = input3->value();

  vcl_vector<float> forepix, backpix;
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
  vcl_cout << "false_pos_fore true_pos_fore\n"
           << "----------------------------------------------------------\n";
  double inc = 0.05;
  unsigned N = 47;
  double pa[47]={0.0, 0.0005,  0.001, 0.005, 0.01, 0.05,  0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.82, 0.85, 0.87,0.88, 0.89, 0.9, 0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99, 0.992, 0.995, 0.997, 0.9975, 0.998,0.9982, 0.9985, 0.9987, 0.999, 0.9992, 0.9995, 0.9996, 0.99965, 0.9997, 0.99975, 0.99977, 0.9998, 1.0};
#endif // 0
  unsigned N = 63;
  double pa[63]={0.0, 0.0001, 0.0002, 0.0003, 0.0004, 0.0005,  0.0006, 0.0007, 0.0008, 0.0009, 0.001, 0.005, 0.01, 0.05,  0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.82, 0.85, 0.87,0.88, 0.89, 0.9, 0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99, 0.992, 0.995, 0.997, 0.9975, 0.998,0.9982, 0.9985, 0.9987, 0.999, 0.9992, 0.9995, 0.9996, 0.99965, 0.9997, 0.99975, 0.99977, 0.9998, 0.99991, 0.99995, 0.999991, 0.999995, 0.9999991, 0.9999995, 0.99999991, 0.99999995, 1.0};

  vcl_vector<double> fpr, tpr;
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
    //vcl_cout << fore_false_pos_frac << ' ' << fore_true_pos_frac << vcl_endl;
  }

  double best_dist = 10000000000.0;
  unsigned best_id = 0;
  for (unsigned ip = 0; ip < N; ip++) {
    double dist = fpr[ip]*fpr[ip]+(1-tpr[ip])*(1-tpr[ip]);
    if (dist < best_dist) {
      best_id = ip;
      best_dist = dist;
    }
  }

  brdb_value_sptr output0 = new brdb_value_t<float>(pa[best_id]);
  output_data_[0] = output0;

  vcl_ofstream of(out_file.c_str());
  of << "# bvxm_prob_map_roc_compute_process\n#line 1: threshold values\n#line 2: FPR values for thresholds\n#line 3: TPR values\n";
  for (unsigned ip = 0; ip<N; ++ip)
    of << pa[ip] << '\t';
  of << vcl_endl;
  for (unsigned ip = 0; ip<N; ++ip)
    of << fpr[ip] << '\t';
  of << vcl_endl;
  for (unsigned ip = 0; ip<N; ++ip)
    of << tpr[ip] << '\t';
  of << vcl_endl;
  of.close();

  return true;
}

