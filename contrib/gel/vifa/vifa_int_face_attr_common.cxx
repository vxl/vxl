// This is gel/vifa/vifa_int_face_attr_common.cxx
#include <sdet/sdet_fit_lines.h>
#include <vdgl/vdgl_intensity_face.h>
#include <vdgl/vdgl_intensity_face_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vifa/vifa_int_face_attr_common.h>
#include <vifa/vifa_group_pgram.h>
#include <vcl_cmath.h> // fabs()

vifa_int_face_attr_common::
vifa_int_face_attr_common(sdet_fit_lines_params*    fitter_params,
                          vifa_group_pgram_params*  gpp_s,
                          vifa_group_pgram_params*  gpp_w,
                          vifa_coll_lines_params*   cpp,
                          vifa_norm_params*         np) :
  vifa_int_face_attr_common_params(fitter_params, gpp_s, gpp_w, cpp, np)
{
  this->init();
}

vifa_int_face_attr_common::
~vifa_int_face_attr_common()
{
}


// ------------------------------------------------------------
// Data access & computation for non-attributes
//


edge_2d_list& vifa_int_face_attr_common::
GetFittedEdges()
{
  if (!_fitted_edges.size())
  {
    this->fit_lines();
  }

  return _fitted_edges;
}

coll_list& vifa_int_face_attr_common::
get_collinear_lines()
{
  if (!_cpp)
  {
    _cpp = new vifa_coll_lines_params;
  }

  if (!_collinear_lines.size())
  {
    this->find_collinear_lines();
  }

  return _collinear_lines;
}

double vifa_int_face_attr_common::
col_collapse()
{
  double  collapsed_count = 0;
  double  total_count = 0;

  for (coll_iterator c = _collinear_lines.begin();
      c != _collinear_lines.end(); ++c)
  {
    total_count++;

    if ((*c)->get_discard_flag())
    {
      collapsed_count++;
    }
  }

  if (total_count == 0)
  {
    return 0;
  }

  return collapsed_count / total_count;
}

double vifa_int_face_attr_common::
get_contrast_across_edge(vtol_edge_sptr  e, double dflt_cont)
{
  double    cont = dflt_cont;
  face_list*  faces = e->faces();

  // Expect only one or two intensity faces for 2-D case
  if (faces && (faces->size() == 2))
  {
    vdgl_intensity_face*  f1 = (vdgl_intensity_face*)((*faces)[0].ptr());
    vdgl_intensity_face*  f2 = (vdgl_intensity_face*)((*faces)[1].ptr());

    if (f1 && f2 &&
        f1->topology_type() == vtol_topology_object::INTENSITYFACE &&
        f2->topology_type() == vtol_topology_object::INTENSITYFACE)
    {
      cont = vcl_fabs(f1->Io() - f2->Io());
    }

    delete faces;
  }

  return cont;
}

vifa_coll_lines_sptr vifa_int_face_attr_common::
get_line_along_edge(vtol_edge* edge)
{
  vifa_coll_lines_sptr  ret(0);
  vtol_vertex_sptr    ev1 = edge->v1();
  vtol_vertex_sptr    ev2 = edge->v2();

  for (coll_iterator c = _collinear_lines.begin();
      c != _collinear_lines.end(); ++c)
  {
    edge_2d_list&  c_edges = (*c)->get_contributors();

    for (edge_2d_iterator e = c_edges.begin(); e != c_edges.end(); ++e)
    {
      vtol_vertex_sptr  v1 = (*e)->v1();
      vtol_vertex_sptr  v2 = (*e)->v2();

      if (((*ev1 == *v1) && (*ev2 == *v2)) ||
        ((*ev1 == *v2) && (*ev2 == *v1)))
      {
        ret = *c;
      }
    }
  }

  return ret;
}


// ------------------------------------------------------------
// Individual attribute computation
//


// Compute measure of projective parallelism, ratio of edges that have some
// projective overlap to total edge length.  Computes _fitted_edges if needed.
float vifa_int_face_attr_common::
StrongParallelSal()
{
  if (_para_sal_strong < 0)
  {
    if (!_gpp_s.ptr())
    {
      const float  angle_increment = 5.0;
      _gpp_s = new vifa_group_pgram_params(angle_increment);
    }

    _para_sal_strong = this->compute_parallel_sal(_gpp_s);
  }

  return _para_sal_strong;
}

float vifa_int_face_attr_common::
WeakParallelSal()
{
  if (_para_sal_weak < 0)
  {
    if (!_gpp_w.ptr())
    {
      const float  angle_increment = 20.0;
      _gpp_w = new vifa_group_pgram_params(angle_increment);
    }

    _para_sal_weak = this->compute_parallel_sal(_gpp_w);
  }

  return _para_sal_weak;
}


// ------------------------------------------------------------
// Protected methods
//


void vifa_int_face_attr_common::
init()
{
  _attributes_valid    = false;
  _complexity        = -1;
  _weighted_complexity  = -1;
  _aspect_ratio      = -1;
  _peri_length      = -1;
  _weighted_peri_length  = -1;
  _para_sal_weak      = -1;
  _para_sal_strong    = -1;
}

// Fit edges of _face with straight lines, but only if cached versions
// are not available.  Sets _fitter_params with defaults if empty.
// Results are added to _fitted_edges.
void vifa_int_face_attr_common::
fit_lines()
{
  edge_2d_list  edges_in_vect = this->GetEdges();

//  vcl_cout << "ifac::fit_lines(): " << edges_in_vect.size()
//           << " edges available\n";

  if (!edges_in_vect.size())
  {
    vcl_cerr << "vifa_int_face_attr_common::fit_lines: _face is not set\n";
    return;
  }

  edge_2d_list  edges_in;
  for (edge_2d_iterator ei = edges_in_vect.begin();
      ei != edges_in_vect.end(); ei++)
  {
    edges_in.push_back(*ei);
  }

  if (!_fitter_params.ptr())
  {
    const int  fit_length = 6;
    _fitter_params = new sdet_fit_lines_params(fit_length);
  }

  // Call the line fitting routine (thanks Joe!)
  sdet_fit_lines  fitter(*(_fitter_params.ptr()));
  fitter.set_edges(edges_in);
  fitter.fit_lines();
  vcl_vector<vsol_line_2d_sptr>&  segs = fitter.get_line_segs();

//  vcl_cout << "ifac::fit_lines(): " << segs.size() << " segments from fitter\n";

  // Convert fitter output to edges & update statistics
  vcl_vector<vsol_line_2d_sptr>::iterator  segi = segs.begin();
  for (; segi != segs.end(); segi++)
  {
    vsol_line_2d_sptr  seg = *segi;
    vtol_vertex_2d_sptr  v1 = new vtol_vertex_2d(*(seg->p0()));
    vtol_vertex_2d_sptr  v2 = new vtol_vertex_2d(*(seg->p1()));
    vtol_edge_2d_sptr  e = new vtol_edge_2d(v1, v2);
    _fitted_edges.push_back(e);

    // Update statistics
    _fitted_edges_stats.add_sample(seg->length());
  }
}

void vifa_int_face_attr_common::
find_collinear_lines()
{
  // sort fitted edges by length into f_edges
  edge_2d_list  unsorted_edges = this->GetFittedEdges();
  edge_2d_list  f_edges;
  for (edge_2d_iterator e = unsorted_edges.begin();
      e != unsorted_edges.end(); ++e)
  {
    double        len = (*e)->curve()->length();
    edge_2d_iterator  slot = f_edges.begin();
    while (slot != f_edges.end())
    {
      if ((*slot)->curve()->length() < len)
      {
        break;
      }

      ++slot;
    }

    f_edges.insert(slot, *e);
  }

  // build up a list of collinear line buckets
  coll_list    unfiltered_lines;
  coll_iterator  match;

//  vcl_cout << "Collineating: ";
  for (edge_2d_iterator e = f_edges.begin(); e != f_edges.end(); ++e)
  {
//    vcl_cout << '.';

    if ((*e)->curve()->length() == 0)
      continue;

    bool  match_flag = find_collinear_match(*e,
                                            unfiltered_lines,
                                            _cpp->_midpt_distance,
                                            match);

    if (!match_flag)
    {
      vifa_coll_lines_sptr  cl(new vifa_coll_lines(*e, _cpp->_angle_tolerance));

      unfiltered_lines.push_back(cl);
    }
    else
    {
      (*match)->add_and_update(*e);
    }
  }

//  vcl_cout << vcl_endl;

  // remove lines whose support is too low
  int  unwind_count = 0;
  for (coll_iterator c = unfiltered_lines.begin();
      c != unfiltered_lines.end(); ++c)
  {
    double  span = (*c)->spanning_length();
    double  support = (*c)->support_length();

    if ((support/span) >= _cpp->_discard_threshold)
    {
      _collinear_lines.push_back(*c);
      _col_span.add_sample(span);
      _col_support.add_sample(support);
      _col_contrib.add_sample((*c)->get_contributors().size());
    }
    else
    {
      // Unwind the unsupported line
      edge_2d_list&  contrib = (*c)->get_contributors();
      for (edge_2d_iterator e = contrib.begin(); e != contrib.end(); ++e)
      {
        vifa_coll_lines_sptr  cl(new vifa_coll_lines(*e,
                                                     _cpp->_angle_tolerance,
                                                     _cpp->_endpt_distance,
                                                     true)
                                );

        _collinear_lines.push_back(cl);

        double  span = cl->spanning_length();
        double  support = cl->support_length();

        _col_span.add_sample(span);
        _col_support.add_sample(support);
        _col_contrib.add_sample(cl->get_contributors().size());
        unwind_count++;
      }
    }
  }

//  vcl_cout << unfiltered_lines.size() << " raw collinear lines; "
//           << unwind_count << " lines from unsupported lines; "
//           << _collinear_lines.size() << " lines above discard threshold "
//           << _cpp->_discard_threshold << vcl_endl;
}

bool vifa_int_face_attr_common::
find_collinear_match(vtol_edge_2d_sptr edge,
                     coll_list&        lines,
                     double            dist_threshold,
                     coll_iterator&    result)
{
  double      min_dist = dist_threshold;
  coll_iterator  match = lines.end();
  for (coll_iterator c = lines.begin(); c != lines.end(); ++c)
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
compute_parallel_sal(vifa_group_pgram_params_sptr  gpp)
{
  float      sal = 0.0;
  edge_2d_list  fedges = this->GetFittedEdges();

//  vcl_cout << "ifac::compute_parallel_sal(): " << fedges.size()
//           << " edges found\n";

  if (fedges.size())
  {
//    vcl_cout << (*_fitter_params);

    float        total_len = 0.0;
    int          nlines = 0;
    imp_line_list    lg_filtered;
    edge_2d_iterator  ei = fedges.begin();
    int          i = 0;
    for (; ei != fedges.end(); ei++, i++)
    {
      vsol_curve_2d_sptr  cur = (*ei)->curve();
      float        len = cur->length();
      total_len += len;

//      vcl_cout << "  edge #" << i << " curve length: " << len << vcl_endl;

      if (len >= _fitter_params->min_fit_length_)
      {
        const vgl_point_2d<double>&  p1 = cur->p0()->get_p();
        const vgl_point_2d<double>&  p2 = cur->p1()->get_p();
        imp_line_sptr        filt_line  = new imp_line(p1, p2);

        lg_filtered.push_back(filt_line);
        nlines++;
      }
    }

//  vcl_cout << "ifac::compute_parallel_sal(): " << nlines
//           << " lines after filtering, total_len = " << total_len << vcl_endl;

    // compute the score for the set of fitted lines
    if (nlines > 2)
    {
      // Insert the lines into the parallelogram orientation index
      vifa_group_pgram  gp(lg_filtered, *gpp);
      gp.SetTemp1(total_len);

      // Get the resultant value
      sal = gp.norm_parallel_line_length();
    }
  }

  return sal;
}
