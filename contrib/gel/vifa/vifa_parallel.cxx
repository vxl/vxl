// This is gel/vifa/vifa_parallel.cxx
#include <vnl/vnl_math.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vsol/vsol_curve_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vifa/vifa_gaussian.h>
#include <vifa/vifa_parallel.h>

#ifdef DUMP
#include <vul/vul_sprintf.h>
#include <vcl_fstream.h>

static int pass = 0;
#endif

const float n_sigma = 2.0;  // on either side of center


vifa_parallel::
vifa_parallel(iface_list&      faces,
              bool          contrast_weighted,
              vifa_parallel_params*  params) :
  vifa_parallel_params(params)
{
  raw_h_ = new vifa_histogram(nbuckets, min_angle, max_angle);
  float  range = max_angle - min_angle;

  for (iface_iterator ifi = faces.begin(); ifi != faces.end(); ++ifi)
  {
    edge_list*  edges = (*ifi)->edges();

    if (edges)
    {
      for (edge_iterator ei = edges->begin(); ei != edges->end(); ei++)
      {
        vtol_edge_2d* e = (*ei)->cast_to_edge_2d();

        if (e)
        {
#ifdef OLD_LINE_APPROX
          const vtol_vertex_2d*  v1 = e->v1()->cast_to_vertex_2d();
          const vtol_vertex_2d*  v2 = e->v2()->cast_to_vertex_2d();
          float  dy = v1->y() - v2->y();
          float  dx = v1->x() - v2->x();
          float  length = vcl_sqrt((dx * dx) + (dy * dy));

          if (contrast_weighted)
          {
            vtol_intensity_face*  other_f =
              get_adjacent_iface((*ifi).ptr(), e);

            if (other_f &&
                other_f->topology_type() == vtol_topology_object::INTENSITYFACE)
            {
              vtol_intensity_face*  other_int_f =
                        (vtol_intensity_face*)other_f;
              length *= vcl_fabs((*ifi)->Io() - other_int_f->Io());
            }
            else
            {
              length = 0;  // handles edge-on-ROI problem!
            }
          }

          float  orientation = vcl_atan2(dy, dx);
          if (orientation < 0)
          {
            orientation += vnl_math::pi;
          }

          float  theta = orientation * 180.0 / vnl_math::pi;
          theta = map_x(theta);
          raw_h_->SetCount(theta, raw_h_->GetCount(theta) + length);
#else
          vsol_curve_2d_sptr  c = e->curve();

          if (c)
          {
            vdgl_digital_curve*  dc = c->cast_to_digital_curve();

            if (dc)
            {
              float l = dc->length();

              for (int i = 0; i < l; i++)
              {
                // Use parametric index representation (0 -- 1)
                float theta = dc->get_theta(i / l);

//                vcl_cout << "raw theta: " << theta;

                while (theta < min_angle)
                {
                  theta += range;
                }

                while (theta > max_angle)
                {
                  theta -= range;
                }

//                vcl_cout << " to " << theta << vcl_endl;

                raw_h_->UpCount(theta);
              }
            }
          }
#endif  // OLD_LINE_APPROX
        }
      }

      delete edges;
    }
  }

  norm_h_ = vifa_parallel::normalize_histogram(raw_h_);
}


vifa_parallel::
vifa_parallel(vcl_vector<float>&  pixel_orientations,
              vifa_parallel_params*  params) :
  vifa_parallel_params(params)
{
  raw_h_ = new vifa_histogram(nbuckets, min_angle, max_angle);
  float  range = max_angle - min_angle;

  for (vcl_vector<float>::iterator p = pixel_orientations.begin();
      p != pixel_orientations.end();
      ++p)
  {
    float  theta = (*p);

    while (theta < min_angle)
    {
      theta += range;
    }
    while (theta > max_angle)
    {
      theta -= range;
    }

    raw_h_->UpCount(theta);
  }

  norm_h_ = vifa_parallel::normalize_histogram(raw_h_);
}

vifa_parallel::
vifa_parallel(float  center_angle,
              float  std_dev)
{
  raw_h_ = new vifa_histogram(nbuckets, min_angle, max_angle);

//  vcl_cout << "vifa_parallel(): 0.5 is " << raw_h_->GetValIndex(0.5) <<
//    vcl_endl;

  vifa_gaussian  g(center_angle, std_dev);
  for (float i = min_angle; i < 2 * max_angle; i++)
  {
    float  v = g.pdf(i);
    float  vx = map_x(i);

    raw_h_->SetCount(vx, raw_h_->GetCount(vx) + v);
  }

  norm_h_ = vifa_parallel::normalize_histogram(raw_h_);
}

vifa_parallel::
~vifa_parallel()
{
  delete raw_h_;
  delete norm_h_;
}

void vifa_parallel::
reset(void)
{
  delete norm_h_;
  norm_h_ = normalize_histogram(raw_h_);
}

vifa_histogram* vifa_parallel::
get_raw_hist(void)
{
  return raw_h_;
}

vifa_histogram* vifa_parallel::
get_norm_hist(void)
{
  return norm_h_;
}

void vifa_parallel::
map_gaussian(float&  max_angle,
             float&  std_dev,
             float&  scale)
{
  bool    set_min_res_flag = true;

  float    local_std_dev = 0.0;  // degrees
  const float  incr = 3.0;  // put me in the params!
  float    max_value;
  float    local_max_angle = find_peak(max_value);
  max_angle = 0.0;
  std_dev = 0.0;
  scale = 0.0;

  // Skip histograms that are empty
  if (local_max_angle != -1.0)
  {
    float  min_residual = 0.f; // dummy initialisation
//  int    min_index = -1; // dummy initialisation

    for (float j=-(n_sigma+1); j<=(n_sigma+1); j++)
    {
      float  new_center = map_x(local_max_angle + (j * incr));
      local_std_dev = 0.0;

      for (int i = 0; i < 5; i++)
      {
        local_std_dev += incr;
        vifa_gaussian  g(new_center, local_std_dev);
        float      g_max = g.pdf(new_center);
        float      local_scale = norm_h_->GetCount(new_center) /
                        g_max;
        // NOTE: local_scale could be zero if the histogram is 0 here...

#ifdef DUMP
        char      buf[25];
        vul_sprintf(buf, "gauss-%d-%d-%d.dat", pass, (int)j, i);
        vcl_ofstream  gdump(buf);
#endif
        int    sample_count = 0;
        float  sample_sum = 0.0;

        for (float  dx = (-n_sigma * local_std_dev);
            dx <= (n_sigma * local_std_dev);
            dx += norm_h_->GetBucketSize())
        {
          float  vx = new_center + dx;
          float  e = g.pdf(vx) * local_scale;
          // e could be zero because of local_scale, see above
          float  x = map_x(vx);
          float  f = norm_h_->GetCount(x);
          if (f < 0)
          {
            f = 0;
          }

          float  diff = vcl_fabs(f - e);

#ifdef DUMP
          gdump << x << ' ' << e << ' ' << diff << ' ' << vx << ' ' << f << vcl_endl;
#endif
          if (e != 0.0)
          {
            sample_sum += ((diff * diff) / e);
            sample_count++;
          }
        }

        // Set min_residual the first time thru
        if ((set_min_res_flag || sample_sum < min_residual) && sample_sum != 0)
        {
          set_min_res_flag = false;
          min_residual = sample_sum;
          std_dev = local_std_dev;
          max_angle = new_center;
          scale = local_scale;
//        min_index = i;

          // vcl_cout << "*** ";
        }

        // vcl_cout << j << " gaussian " << i << " residual " <<
        //  sample_sum << " sample count " << sample_count << vcl_endl;
      }
    }

    //  vcl_cout << "best is at " << max_angle << " sd of " << std_dev <<
    //    " scale " << scale << " (index " << min_index << " )" <<
    //    vcl_endl;

#ifdef DUMP
    pass++;
#endif
  }
}

void vifa_parallel::
remove_gaussian(float  max_angle,
                float  std_dev,
                float  scale)
{
  // Skip if histogram is empty
  if (norm_h_->GetMaxCount() != 0.0)
  {
    vifa_gaussian  g(max_angle, std_dev);
    for (float  dx = (-n_sigma * std_dev);
        dx <= (n_sigma * std_dev);
        dx += norm_h_->GetBucketSize())
    {
      float  vx = max_angle + dx;
      float  e = g.pdf(vx) * scale;
      float  x = map_x(vx);
      float  f = norm_h_->GetCount(x);

      if (f >= 0)
      {
        float  new_val = ((f - e) < 0) ? 0 : (f - e);

//        vcl_cout << "  --- " << x << ": " << f <<" to " << new_val
//          << vcl_endl;

        norm_h_->SetCount(x, new_val);
      }
      else
      {
        vcl_cerr << "  --- " << x << ": bad " <<
          norm_h_->GetValIndex(x) << vcl_endl;
      }
    }
  }
}

void vifa_parallel::
snapshot(char* fname)
{
  norm_h_->WritePlot(fname);
}

float vifa_parallel::
area(void)
{
  if (norm_h_->GetMaxCount() == 0.0)
  {
    // Return 0 area for empty histograms
    return 0.0;
  }
  else
  {
    return norm_h_->ComputeArea();
  }
}

float vifa_parallel::
bin_variance(void)
{
  float* counts = norm_h_->GetCounts();
  int    res = norm_h_->GetRes();
  float  sum = 0;
  float  sum2 = 0;
  float  mean = 0;
  float  var = 0;

  for (int i = 0; i < res; i++)
  {
    sum += counts[i];
    sum2 += (counts[i] * counts[i]);
  }

  mean = sum / res;
  var = (sum2 / res) - (mean * mean);

  return var;
}

float vifa_parallel::
map_x(float  raw_x)
{
  float  range = max_angle - min_angle + 1;

  while (raw_x < min_angle)
  {
    raw_x += range;
  }
  while (raw_x > max_angle)
  {
    raw_x -= range;
  }

  return raw_x;
}

vifa_histogram* vifa_parallel::
normalize_histogram(vifa_histogram* h)
{
  vifa_histogram*  norm = new vifa_histogram(nbuckets, min_angle, max_angle);
  int        nbuckets = h->GetRes();
  float      area = h->ComputeArea();
  float*      x_vals = h->GetVals();
  float*      y_vals = h->GetCounts();

  for (int i = 0; i < nbuckets; i++)
  {
    float  new_area = y_vals[i] / area;
    norm->SetCount(x_vals[i], new_area);
  }

  return norm;
}

float vifa_parallel::
find_peak(float&  max_value)
{
  int    nbuckets = norm_h_->GetRes();
  float*  x_vals = norm_h_->GetVals();
  float*  y_vals = norm_h_->GetCounts();
  int    max_index = -1;
  max_value = -1;

  for (int i = 0; i < nbuckets; i++)
  {
    if (y_vals[i] > max_value)
    {
      max_index = i;
      max_value = y_vals[i];
    }
  }

  if (max_index == -1)
  {
    return -1;
  }

  max_value = y_vals[max_index];
  return x_vals[max_index];
}

vtol_intensity_face* vifa_parallel::
get_adjacent_iface(vtol_intensity_face*  known_face,
                   vtol_edge_2d*         e)
{
  vtol_intensity_face*  adj_face = 0;
  face_list*        faces = e->faces();

  // Expect only one or two intensity faces for 2-D case
  if (faces)
  {
    if (faces->size() == 2)
    {
      vtol_intensity_face*    f1 = (vtol_intensity_face*)
                            ((*faces)[0].ptr());
      vtol_intensity_face*    f2 = (vtol_intensity_face*)
                            ((*faces)[1].ptr());

      if (f1 && f2 &&
          f1->topology_type() == vtol_topology_object::INTENSITYFACE &&
          f2->topology_type() == vtol_topology_object::INTENSITYFACE)
      {
        if (*known_face == *f1)
        {
          adj_face = f2;
        }
        else if (*known_face == *f2)
        {
          adj_face = f1;
        }
        else
        {
          // Known face does not contain the
          // given edge -- leave result NULL
        }
      }
    }

    delete faces;
  }

  return adj_face;
}
