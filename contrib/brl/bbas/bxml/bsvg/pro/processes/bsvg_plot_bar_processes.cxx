// This is brl/bbas/bxml/bsvg/pro/processes/bsvg_plot_bar_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes for plotting bar graphs
//
// \author Ozge Can Ozcanli
// \date July 15, 2009
//
// \verbatim
//  Modifications
//   none yet
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bxml/bsvg/bsvg_plot.h>
#include <bxml/bxml_write.h>

//: Constructor
//  initialize a bar plot with no bars, new bars will be added by the add_bar process
bool bsvg_bar_plot_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("float");  // width of the drawing region in the svg file
  input_types.emplace_back("float");  // height
  input_types.emplace_back("vcl_string");  // title of the plot
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("bxml_document_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool bsvg_bar_plot_initialize_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  auto w = pro.get_input<float>(i++);
  auto h = pro.get_input<float>(i++);
  std::string title = pro.get_input<std::string>(i++);

  auto* p = new bsvg_plot(w, h);
  bxml_document_sptr pd = p;
  p->set_margin(40);
  p->set_font_size(15);
  p->add_axes(0, 1, 0, 1);
  p->add_title(title);
  //p->add_x_increments(0.1f);
  p->add_y_increments(0.1f);

  pro.set_output_val<bxml_document_sptr>(0, pd);
  return true;
}

//: Constructor
//  Add a bar to the plot
bool bsvg_bar_plot_add_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bxml_document_sptr");
  input_types.emplace_back("float");  // height
  input_types.emplace_back("vcl_string");  // label
  input_types.emplace_back("vcl_string");  // color
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool bsvg_bar_plot_add_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  bxml_document_sptr doc = pro.get_input<bxml_document_sptr>(i++);
  auto h = pro.get_input<float>(i++);
  std::string label = pro.get_input<std::string>(i++);
  std::string color = pro.get_input<std::string>(i++);

  auto* p = dynamic_cast<bsvg_plot*>(doc.ptr());
  p->add_bar(h, label, true, color);
  return true;
}

//: Constructor
bool bsvg_plot_write_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bxml_document_sptr");
  input_types.emplace_back("vcl_string");  // out file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool bsvg_plot_write_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    std::cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  bxml_document_sptr doc = pro.get_input<bxml_document_sptr>(i++);
  std::string out_file = pro.get_input<std::string>(i++);

  bxml_write(out_file, *doc);
  return true;
}
