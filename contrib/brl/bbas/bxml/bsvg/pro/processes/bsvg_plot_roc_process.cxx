// This is brl/bbas/bxml/bsvg/pro/processes/bsvg_plot_roc_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes for plotting
//
// \author Ozge Can Ozcanli
// \date June 19, 2009
//
// \verbatim
//  Modifications
//   none yet
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bxml/bsvg/bsvg_plot.h>
#include <vul/vul_awk.h>
#include <vul/vul_string.h>
#include <bxml/bxml_write.h>

//: Plot ROC process:
//    input 0: the path to a text file which has the following format:
//  # any number of commented lines which start with character: '#'
//  #line 1: threshold values
//  #line 2: FPR values for thresholds
//  #line 3: TPR values (The number of values in all the lines have to be equal)
//  0  0.0001      0.0002     0.0003     0.0004     . . .
//  0  0.00728813  0.0078134  0.0078134  0.0078134  . . .
//  0  0.416157    0.519584   0.519584   0.519584   . . .
//    input 1: the path to the output SVG plot ( x.svg )
//

//: Constructor
bool bsvg_plot_roc_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");  // file with 3 lines: threshold values, TPRs and FPRs
  input_types.push_back("vcl_string");  // name of the output svg file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}


bool bsvg_plot_roc_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vcl_string roc_path = pro.get_input<vcl_string>(i++);
  vcl_string out_name = pro.get_input<vcl_string>(i++);

  bsvg_plot p(1200, 600);
  p.set_margin(40);
  p.set_font_size(30);
  p.add_axes(0, 1, 0, 1);
  p.add_title("ROC Plot");
  //p.add_x_increments(0.1f);
  p.add_y_increments(0.1f);

  vcl_vector<float> xs, ys;

  vcl_ifstream ifs(roc_path.c_str());
  int line_cnt = 0;
  for (vul_awk awk(ifs); awk; ++awk) {
    if (!awk.NF())
      continue;
    vcl_string field0 = awk[0];
    if (!field0.size())
      continue;
    else if (field0[0] == '#')
      continue;
    line_cnt++;
    if (line_cnt == 2) {  // second line is FPR values, read them from the fields
      for (int j = 0; j < awk.NF(); j++) {
        float fpr = (float)vul_string_atof(awk[j]);
        xs.push_back(fpr);
      }
    } else if (line_cnt == 3) {
      for (int j = 0; j < awk.NF(); j++) {
        float tpr = (float)vul_string_atof(awk[j]);
        ys.push_back(tpr);
      }
      break;
    }
  }

  xs.erase(xs.end()-1); // erase the last element, which is (1,1) pair as a convention
  ys.erase(ys.end()-1);
  p.add_line(xs, ys, "red");
  bxml_write(out_name, p);

  return true;
}

