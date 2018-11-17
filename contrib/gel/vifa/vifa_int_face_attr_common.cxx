// This is gel/vifa/vifa_int_face_attr_common.cxx
#include <iostream>
#include <cmath>
#include "vifa_int_face_attr_common.h"
//:
// \file

#include <vdgl/vdgl_fit_lines.h>
#include <vtol/vtol_intensity_face.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vifa/vifa_group_pgram.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vifa_int_face_attr_common::
vifa_int_face_attr_common(vdgl_fit_lines_params*    fitter_params,
                          vifa_group_pgram_params*  gpp_s,
                          vifa_group_pgram_params*  gpp_w,
                          vifa_coll_lines_params*   cpp,
                          vifa_norm_params*         np) :
  vifa_int_face_attr_common_params(fitter_params, gpp_s, gpp_w, cpp, np)
{
  this->init();
}

vifa_int_face_attr_common::~vifa_int_face_attr_common() = default;


// ------------------------------------------------------------
// Data access & computation for non-attributes
//


edge_2d_list& vifa_int_face_attr_common::
GetFittedEdges()
{
  if (!fitted_edges_.size())
    this->fit_lines();

  return fitted_edges_;
}

coll_list& vifa_int_face_attr_common::
get_collinear_lines()
{
  if (!cpp_)
    cpp_ = new vifa_coll_lines_params;

  if (!collinear_lines_.size())
    this->find_collinear_lines();

  return collinear_lines_;
}

double vifa_int_face_attr_common::
col_collapse()
{
  double  collapsed_count = 0;
  double  total_count = 0;

  for (auto & collinear_line : collinear_lines_)
  {
    total_count++;

    if (collinear_line->get_discard_flag())
      collapsed_count++;
  }

  if (total_count == 0)
    return 0;

  return collapsed_count / total_count;
}

double vifa_int_face_attr_common::
get_contrast_across_edge(const vtol_edge_sptr&  e, double dflt_cont)
{
  double    cont = dflt_cont;
  face_list faces; e->faces(faces);

  // Expect only one or two intensity faces for 2-D case
  if (faces.size() == 2)
  {
    vtol_intensity_face*  f1 = faces[0]->cast_to_intensity_face();
    vtol_intensity_face*  f2 = faces[1]->cast_to_intensity_face();

    if (f1 && f2)
      cont = std::fabs(f1->Io() - f2->Io());
  }
  return cont;
}

vifa_coll_lines_sptr vifa_int_face_attr_common::
get_line_along_edge(vtol_edge* edge)
{
  vifa_coll_lines_sptr  ret(nullptr);
  vtol_vertex_sptr    ev1 = edge->v1();
  vtol_vertex_sptr    ev2 = edge->v2();

  for (auto & collinear_line : collinear_lines_)
  {
    edge_2d_list&  c_edges = collinear_line->get_contributors();

    for (auto & c_edge : c_edges)
    {
      vtol_vertex_sptr  v1 = c_edge->v1();
      vtol_vertex_sptr  v2 = c_edge->v2();

      if (((*ev1 == *v1) && (*ev2 == *v2)) ||
          ((*ev1 == *v2) && (*ev2 == *v1)))
        ret = collinear_line;
    }
  }

  return ret;
}


// ------------------------------------------------------------
// Individual attribute computation
//


// Compute measure of projective parallelism, ratio of edges that have some
// projective overlap to total edge length.  Computes fitted_edges_ if needed.
float vifa_int_face_attr_common::
StrongParallelSal()
{
  if (para_sal_strong_ < 0)
  {
    if (!gpp_s_.ptr())
    {
      const float  angle_increment = 5.f;
      gpp_s_ = new vifa_group_pgram_params(angle_increment);
    }

    para_sal_strong_ = this->compute_parallel_sal(gpp_s_);
  }

  return para_sal_strong_;
}

float vifa_int_face_attr_common::
WeakParallelSal()
{
  if (para_sal_weak_ < 0)
  {
    if (!gpp_w_.ptr())
    {
      const float  angle_increment = 20.f;
      gpp_w_ = new vifa_group_pgram_params(angle_increment);
    }

    para_sal_weak_ = this->compute_parallel_sal(gpp_w_);
  }

  return para_sal_weak_;
}


// ------------------------------------------------------------
// Protected methods
//


void vifa_int_face_attr_common::
init()
{
  attributes_valid_    = false;
  complexity_        = -1;
  weighted_complexity_  = -1;
  aspect_ratio_      = -1;
  peri_length_      = -1;
  weighted_peri_length_  = -1;
  para_sal_weak_      = -1;
  para_sal_strong_    = -1;
}

// Fit edges of face_ with straight lines, but only if cached versions
// are not available.  Sets fitter_params_ with defaults if empty.
// Results are added to fitted_edges_.
void vifa_int_face_attr_common::
fit_lines()
{
  edge_2d_list  edges_in_vect = this->GetEdges();

#ifdef DEBUG
    std::cout << "ifac::fit_lines(): " << edges_in_vect.size()
             << " edges available\n";
#endif

  if (!edges_in_vect.size())
  {
    std::cerr << "vifa_int_face_attr_common::fit_lines: face_ is not set\n";
    return;
  }

  std::vector<vdgl_digital_curve_sptr>  curves_in;
  for (auto & ei : edges_in_vect)
  {
    vsol_curve_2d_sptr c = ei->curve();
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;
    curves_in.push_back(dc);
  }

  if (!fitter_params_.ptr())
  {
    constexpr int fit_length = 6;
    fitter_params_ = new vdgl_fit_lines_params(fit_length);
  }

  // Call the line fitting routine (thanks Joe!)
  vdgl_fit_lines  fitter(*(fitter_params_.ptr()));
  fitter.set_curves(curves_in);
  std::vector<vsol_line_2d_sptr>&  segs = fitter.get_line_segs();

#ifdef DEBUG
    std::cout << "ifac::fit_lines(): " << segs.size() << " segments from fitter\n";
#endif

  // Convert fitter output to edges & update statistics
  auto  segi = segs.begin();
  for (; segi != segs.end(); segi++)
  {
    vsol_line_2d_sptr  seg = *segi;
    vtol_vertex_2d_sptr  v1 = new vtol_vertex_2d(*(seg->p0()));
    vtol_vertex_2d_sptr  v2 = new vtol_vertex_2d(*(seg->p1()));
    vtol_edge_2d_sptr  e = new vtol_edge_2d(v1, v2);
    fitted_edges_.push_back(e);

    // Update statistics
    fitted_edges_stats_.add_sample(seg->length());
  }
}

void vifa_int_face_attr_common::
find_collinear_lines()
{
  // sort fitted edges by length into f_edges
  edge_2d_list  unsorted_edges = this->GetFittedEdges();
  edge_2d_list  f_edges;
  for (auto & unsorted_edge : unsorted_edges)
  {
    double        len = unsorted_edge->curve()->length();
    auto  slot = f_edges.begin();
    for (; slot != f_edges.end(); ++slot)
      if ((*slot)->curve()->length() < len)
      {
        f_edges.insert(slot, unsorted_edge);
        break;
      }
  }

  // build up a list of collinear line buckets
  coll_list    unfiltered_lines;
  coll_iterator  match;

#ifdef DEBUG
  std::cout << "Collineating: ";
#endif
  for (auto & f_edge : f_edges)
  {
#ifdef DEBUG
    std::cout << '.';
#endif

    if (f_edge->curve()->length() == 0)
      continue;

    bool  match_flag = find_collinear_match(f_edge,
                                            unfiltered_lines,
                                            cpp_->midpt_distance(),
                                            match);

    if (!match_flag)
    {
      vifa_coll_lines_sptr  cl(new vifa_coll_lines(f_edge, cpp_->angle_tolerance()));

      unfiltered_lines.push_back(cl);
    }
    else
      (*match)->add_and_update(f_edge);
  }

#ifdef DEBUG
  std::cout << std::endl;
#endif

  // remove lines whose support is too low
  for (auto & unfiltered_line : unfiltered_lines)
  {
    double  span = unfiltered_line->spanning_length();
    double  support = unfiltered_line->support_length();

    if ((support/span) >= cpp_->discard_threshold())
    {
      collinear_lines_.push_back(unfiltered_line);
      col_span_.add_sample(span);
      col_support_.add_sample(support);
      col_contrib_.add_sample(unfiltered_line->get_contributors().size());
    }
    else
    {
      // Unwind the unsupported line
      edge_2d_list&  contrib = unfiltered_line->get_contributors();
      for (auto & e : contrib)
      {
        vifa_coll_lines_sptr  cl(new vifa_coll_lines(e,
                                                     cpp_->angle_tolerance(),
                                                     cpp_->endpt_distance(),
                                                     true));

        collinear_lines_.push_back(cl);

        double  span = cl->spanning_length();
        double  support = cl->support_length();

        col_span_.add_sample(span);
        col_support_.add_sample(support);
        col_contrib_.add_sample(cl->get_contributors().size());
      }
    }
  }

#ifdef DEBUG
  std::cout << unfiltered_lines.size() << " raw collinear lines; "
           << collinear_lines_.size() << " lines above discard threshold "
           << cpp_->discard_threshold_ << std::endl;
#endif
}

bool vifa_int_face_attr_common::
find_collinear_match(const vtol_edge_2d_sptr& edge,
                     coll_list&        lines,
                     double            dist_threshold,
                     coll_iterator&    result)
{
  double      min_dist = dist_threshold;
  auto  match = lines.end();
  for (auto c = lines.begin(); c != lines.end(); ++c)
  {
    double  dist = (*c)->get_measure(edge);
    if ((dist < dist_threshold) && (dist < min_dist))
    {
      min_dist = dist;
      match = c;
    }
  }

  if (match == lines.end())
  {
    return false;
  }

  result = match;
  return true;
}

float vifa_int_face_attr_common::
compute_parallel_sal(const vifa_group_pgram_params_sptr&  gpp)
{
  float      sal = 0.f;
  edge_2d_list  fedges = this->GetFittedEdges();

#ifdef DEBUG
  std::cout << "ifac::compute_parallel_sal(): " << fedges.size()
           << " edges found\n";
#endif

  if (fedges.size())
  {
#ifdef DEBUG
    std::cout << (*fitter_params_);
#endif

    float    total_len = 0.f;
    int      nlines = 0;
    imp_line_list    lg_filtered;
    for (auto & fedge : fedges)
    {
      vsol_curve_2d_sptr  cur = fedge->curve();
      auto        len = float(cur->length());
      total_len += len;

      if (len >= fitter_params_->min_fit_length_)
      {
        const vgl_point_2d<double>&  p1 = cur->p0()->get_p();
        const vgl_point_2d<double>&  p2 = cur->p1()->get_p();
        imp_line_sptr        filt_line  = new imp_line(p1, p2);

        lg_filtered.push_back(filt_line);
        nlines++;
      }
    }

#ifdef DEBUG
    std::cout << "ifac::compute_parallel_sal(): " << nlines
             << " lines after filtering, total_len = " << total_len << std::endl;
#endif

    // compute the score for the set of fitted lines
    if (nlines > 2)
    {
      // Insert the lines into the parallellogram orientation index
      vifa_group_pgram  gp(lg_filtered, *gpp);
      gp.SetTemp1(total_len);

      // Get the resultant value
      sal = float(gp.norm_parallel_line_length());
    }
  }

  return sal;
}
