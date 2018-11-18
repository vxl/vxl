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

#include <bbas_pro/bbas_1d_array_float.h>

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
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");  // file with 3 lines: threshold values, TPRs and FPRs
  input_types.emplace_back("vcl_string");  // name of the output svg file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}


bool bsvg_plot_roc_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << "bsvg_plot_roc_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  std::string roc_path = pro.get_input<std::string>(i++);
  std::string out_name = pro.get_input<std::string>(i++);

  bsvg_plot p(1200, 600);
  p.set_margin(40);
  p.set_font_size(30);
  p.add_axes(0, 1, 0, 1);
  p.add_title("ROC Plot");
  //p.add_x_increments(0.1f);
  p.add_y_increments(0.1f);

  std::vector<float> xs, ys;

  std::ifstream ifs(roc_path.c_str());
  int line_cnt = 0;
  for (vul_awk awk(ifs); awk; ++awk) {
    if (!awk.NF())
      continue;
    std::string field0 = awk[0];
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
    }
    else if (line_cnt == 3) {
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


//: Constructor
bool bsvg_plot_roc_process2_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bbas_1d_array_float_sptr");  // vector of TPR values
  input_types.emplace_back("bbas_1d_array_float_sptr");  // vector of FPR values
  input_types.emplace_back("vcl_string");  // name of the output svg file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}


bool bsvg_plot_roc_process2(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cerr << "bsvg_plot_roc_process2 - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  bbas_1d_array_float_sptr tpr_vals = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bbas_1d_array_float_sptr fpr_vals = pro.get_input<bbas_1d_array_float_sptr>(i++);
  std::string out_name = pro.get_input<std::string>(i++);

  bsvg_plot p(1200, 600);
  p.set_margin(40);
  p.set_font_size(30);
  p.add_axes(0, 1, 0, 1);
  p.add_title("ROC Plot");
  //p.add_x_increments(0.1f);
  p.add_y_increments(0.1f);

  if (tpr_vals->data_array.size() != fpr_vals->data_array.size()) {
    std::cout << "In bsvg_plot_roc_process2_cons : inconsistent tpr and fpr array sizes!\n";
    return false;
  }

  std::vector<float> xs, ys;
  for (vbl_array_1d<float>::iterator iter = tpr_vals->data_array.begin(), iter2 = fpr_vals->data_array.begin();
       iter != tpr_vals->data_array.end(); iter++, iter2++) {
    ys.push_back(*iter);
    xs.push_back(*iter2);
    std::cout << "tp: " << *iter << " fp: " << *iter2 << std::endl;
  }
  std::cout << std::endl;

  //xs.erase(xs.end()-1); // erase the last element, which is (1,1) pair as a convention
  //ys.erase(ys.end()-1);
  p.add_line(xs, ys, "red");
  bxml_write(out_name, p);

  return true;
}


//: Constructor
//  initialize a bar plot with no bars, new bars will be added by the add_bar process
bool bsvg_plot_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");  // title of the plot
  input_types.emplace_back("int");  // width
  input_types.emplace_back("int");  // height
  input_types.emplace_back("int");  // margin
  input_types.emplace_back("int");  // fs
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  brdb_value_sptr idw = new brdb_value_t<int>(1200);
  pro.set_input(1, idw);
  brdb_value_sptr idh = new brdb_value_t<int>(600);
  pro.set_input(2, idh);
  brdb_value_sptr idm = new brdb_value_t<int>(40);
  pro.set_input(3, idm);
  brdb_value_sptr idfs = new brdb_value_t<int>(30);
  pro.set_input(4, idfs);

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("bxml_document_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool bsvg_plot_initialize_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    std::cerr << "bsvg_roc_plot_initialize_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  std::string title = pro.get_input<std::string>(i++);
  int w = pro.get_input<int>(i++);
  int h = pro.get_input<int>(i++);
  int m = pro.get_input<int>(i++);
  int fs = pro.get_input<int>(i++);

  auto* p = new bsvg_plot((float)w, (float)h);
  p->set_margin((float)m);
  p->set_font_size(fs);
  p->add_axes(0, 1, 0, 1);
  p->add_y_increments(0.1f);
  p->add_title(title);
  bxml_document_sptr pd = p;
  pro.set_output_val<bxml_document_sptr>(0, pd);
  return true;
}

//: Constructor
bool bsvg_roc_plot_add_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bxml_document_sptr");
  input_types.emplace_back("bbas_1d_array_float_sptr");  // vector of TPR values
  input_types.emplace_back("bbas_1d_array_float_sptr");  // vector of FPR values
  input_types.emplace_back("vcl_string");  // color of the line
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool bsvg_roc_plot_add_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << "bsvg_plot_roc_process2 - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  bxml_document_sptr doc = pro.get_input<bxml_document_sptr>(i++);
  bbas_1d_array_float_sptr tpr_vals = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bbas_1d_array_float_sptr fpr_vals = pro.get_input<bbas_1d_array_float_sptr>(i++);
  std::string color = pro.get_input<std::string>(i++);

  if (tpr_vals->data_array.size() != fpr_vals->data_array.size()) {
    std::cout << "In bsvg_plot_roc_process2_cons : inconsistent tpr and fpr array sizes!\n";
    return false;
  }
  std::vector<float> xs, ys;
  for (vbl_array_1d<float>::iterator iter = tpr_vals->data_array.begin(), iter2 = fpr_vals->data_array.begin();
       iter != tpr_vals->data_array.end(); iter++, iter2++) {
    xs.push_back(*iter2);
    ys.push_back(*iter);
  }
  auto* p = dynamic_cast<bsvg_plot*>(doc.ptr());
  p->add_line(xs, ys, color);
  return true;
}
