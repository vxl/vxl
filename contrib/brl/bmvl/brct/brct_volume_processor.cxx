#include "brct_volume_processor.h"
#include <vcl_fstream.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <bsol/bsol_algs.h>
#include <brct/brct_algos.h>

brct_volume_processor::brct_volume_processor(brct_volume_processor_params const& vp) // FIXME: vp unused
{
  vsol_box_3d_sptr b = new vsol_box_3d;
  b->add_point(xmin_, ymin_, zmin_);
  b->add_point(xmax_, ymax_, zmax_);
  double w = b->width(), h = b->height(), d = b->depth();
  double r = 1;
  if (cube_edge_length_&&cube_edge_length_>0)
    r = 1.0/cube_edge_length_;
  ncols_ = int(w*r), nrows_ = int(h*r), nslabs_ = int(d*r);
  index_ = new bsol_point_index_3d(ncols_, nrows_, nslabs_, b);
  change_index_ = new bsol_point_index_3d(ncols_, nrows_, nslabs_, b);
}

brct_volume_processor::~brct_volume_processor()
{
  delete index_;
  delete change_index_;
}

bool brct_volume_processor::read_points_3d_vrml(vcl_string const& filename)
{
  vcl_ifstream is(filename.c_str());
  if (!is)
  {
    vcl_cout << "In brct_volume_processor::read points vrml -"
             << " could not open file " << filename << '\n';
    return false;
  }
  vcl_vector<vsol_point_3d_sptr> pts3d;
  brct_algos::read_vrml_points(is, pts3d);
  int npts = pts3d.size(),nin = 0;
  for (int i = 0; i<npts; i++)
    if ((*index_).add_point(pts3d[i]))
      nin++;
  vcl_cout << "Added " << nin << "out of " << npts << " points\n"
           << "Point Bounds\n";
  bsol_algs::print((*index_).point_bounds());
  return true;
}

bool brct_volume_processor::write_prob_volumes_vrml(vcl_string const&  filename)
{
  vcl_ofstream os(filename.c_str());
  if (!os)
  {
    vcl_cout << "In brct_volume_processor::write_prob_volumes vrml -"
             << " could not open file " << filename << '\n';
    return false;
  }
  brct_algos::write_vrml_header(os);
  vcl_vector<vsol_point_3d_sptr> points;
#if 0 // "scal" is not used !?!
  float scal = 100.f;
  if ((*index_).n_points() != 0)
    scal /= (*index_).n_points();
#endif
  for (int r = 0; r<nrows_; r++)
    for (int c = 0; c<ncols_; c++)
      for (int s = 0; s<nslabs_; s++)
      {
        int n_points = (*index_).n_points(r, c, s);
        vsol_box_3d_sptr box = (*index_).index_cell(r, c, s);
        float f = 1;
        if (n_points>0)
          f = 0.5;
        brct_algos::write_vrml_box(os, box, 1.0f, 1.0f, 1.0f, f);
      }
  //brct_algos::write_vrml_points(os, points);
  brct_algos::write_vrml_trailer(os);
  return true;
}

bool brct_volume_processor::read_change_data_vrml(vcl_string const&  filename)
{
  vcl_ifstream is(filename.c_str());
  if (!is)
  {
    vcl_cout << "In brct_volume_processor::read change data vrml -"
             << " could not open file " << filename << '\n';
    return false;
  }
  (*change_index_).clear();
  vcl_vector<vsol_point_3d_sptr> pts3d;
  brct_algos::read_vrml_points(is, pts3d);
  int npts = pts3d.size(),nin = 0;
  for (int i = 0; i<npts; i++)
    if ((*change_index_).add_point(pts3d[i]))
      nin++;
  if (!npts||!nin)
  {
    vcl_cout << "In brct_volume_processor::read_change_data_vrml -"
             << " no data or can't index data\n";
    return false;
  }
  return true;
}

bool brct_volume_processor::compute_change()
{
  change_volumes_.clear();
  for (int r = 0; r<nrows_; r++)
    for (int c = 0; c<ncols_; c++)
      for (int s = 0; s<nslabs_; s++)
      {
        int ni = (*index_).n_points(r, c, s);
        int nc = (*change_index_).n_points(r, c, s);
        if (nc>cell_thresh_&&ni<cell_thresh_)
          change_volumes_.push_back((*index_).index_cell(r, c, s));
      }
  vcl_cout << "Found " << change_volumes_.size() << " change cells\n";
  return true;
}

bool brct_volume_processor::
write_changed_volumes_vrml(vcl_string const&  filename)
{
  int nv = change_volumes_.size();
  if (!nv)
  {
    vcl_cout << "In bool brct_volume_processor::write_changed_volumes_vrml -"
             << " no change volumes\n";
    return false;
  }
  vcl_ofstream os(filename.c_str());
  if (!os)
  {
    vcl_cout << "In brct_volume_processor::write_changed_volumes_vrml -"
             << " could not open file " << filename << '\n';
    return false;
  }

  brct_algos::write_vrml_header(os);
  for (int i = 0; i<nv; i++)
    brct_algos::write_vrml_box(os, change_volumes_[i], 1.0, 0.0);
  brct_algos::write_vrml_trailer(os);
  return true;
}
