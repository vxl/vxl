// This is brl/bseg/vpro/vpro_line_fit_process.cxx
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <sdet/sdet_fit_lines.h>
#include <vpro/vpro_line_fit_process.h>
#include <vsol/vsol_line_2d.h>

vpro_line_fit_process::vpro_line_fit_process(sdet_fit_lines_params & flp)
  : sdet_fit_lines_params(flp)
{
}

vpro_line_fit_process::~vpro_line_fit_process()
{
}

bool vpro_line_fit_process::execute()
{
  vul_timer t;

  if (!get_N_input_topo_objs())
  {
    vcl_cout << "In vpro_line_fit_process::execute() - no input edges\n";
    return false;
  }
  this->clear_output();
  //initialize the line fitter
  sdet_fit_lines fitter(*((sdet_fit_lines_params*)this));
  vcl_vector<vtol_edge_2d_sptr> edges;
  for (vcl_vector<vtol_topology_object_sptr>::iterator eit = input_topo_objs_.begin();
       eit != input_topo_objs_.end(); eit++)
  {
    vtol_edge_2d_sptr e = (*eit)->cast_to_edge()->cast_to_edge_2d();
    if (e)
      edges.push_back(e);
  }
  fitter.set_edges(edges);
  if (!fitter.fit_lines())
    return false;
  vcl_vector<vsol_line_2d_sptr> & lines = fitter.get_line_segs();
  for (vcl_vector<vsol_line_2d_sptr>::iterator lit = lines.begin();
       lit != lines.end(); lit++)
    output_spat_objs_.push_back((*lit)->cast_to_spatial_object());

  vcl_cout << "process " << lines.size()
           << " line segments in " << t.real() << " msecs.\n";
  return true;
}
