#include "volm_desc_ex_2d_matcher.h"
#include <vcl_algorithm.h>
//
// \file

float volm_desc_ex_2d_matcher::score(volm_desc_sptr const& a, volm_desc_sptr const& b)
{
  unsigned nbins = a->nbins();

  if( b->nbins() != nbins )
    {
    return 0.0f;
    }
  float score = 0.0f;
  for( unsigned idx = 0; idx < nbins; idx++ )
    {
    float intersec = (float)vcl_min(a->count(idx), b->count(idx) );
    score += intersec * weights_hist_[idx];
    }
  return score;
  /*return a->similarity(b);*/
}

// find the object weight value from object name (return 0 if not found)
float volm_desc_ex_2d_matcher::find_wgt_value(vcl_string const& name)
{
  for( vcl_vector<volm_weight>::iterator wit = weights_.begin(); wit != weights_.end(); ++wit )
    {
    if( (*wit).w_name_.compare(name) == 0 )
      {
      return (*wit).w_obj_;
      }
    }
  return 0.0f;
}

volm_desc_sptr volm_desc_ex_2d_matcher::create_query_desc()
{
  if( radius_.empty() )
    {
    radius_.push_back(1.0);
    }
  // sort the radius to ensure the bin order
  vcl_sort(radius_.begin(), radius_.end() );
  unsigned ndists = (unsigned)radius_.size() + 1;

  double largest_rad = radius_[radius_.size() - 1];  // this is in meters

  // create the ex_land_only descriptor
  volm_desc_ex_land_only* desc = new volm_desc_ex_land_only(ndists, nlands_, radius_);

  // re-define the weight parameter to satisfy current radius setting (note this destroy previous weight ratio...)
  float    extra_wgt = 0.0f;
  unsigned excluded_obj = 0;
  if( !dms_->sky().empty() )
    {
    extra_wgt += this->find_wgt_value("sky");
    }
  if( !dms_->scene_regions().empty() )
    {
    vcl_vector<depth_map_region_sptr> objs = dms_->scene_regions();
    for( unsigned i = 0; i < objs.size(); i++ )
      {
      if( objs[i]->min_depth() > largest_rad || !objs[i]->active() )
        {
        extra_wgt += this->find_wgt_value(objs[i]->name() );
        excluded_obj++;
        }
      }
    }
  unsigned n_label = 0;
  if( !dms_->ground_plane().empty() )
    {
    n_label++;
    }
  n_label += dms_->scene_regions().size() - excluded_obj;

  float wgt_inc = extra_wgt / n_label;
  if( !dms_->ground_plane().empty() )
    {
    for( vcl_vector<volm_weight>::iterator wit = weights_.begin(); wit != weights_.end(); ++wit )
      {
      if( (*wit).w_typ_.compare("ground_plane") == 0 )
        {
        (*wit).w_obj_ += wgt_inc;
        break;
        }
      }
    }
  if( !dms_->scene_regions().empty() )
    {
    vcl_vector<depth_map_region_sptr> objs = dms_->scene_regions();
    for( unsigned i = 0; i < objs.size(); i++ )
      {
      if( objs[i]->min_depth() > largest_rad )
        {
        continue;
        }
      for( vcl_vector<volm_weight>::iterator wit = weights_.begin(); wit != weights_.end(); ++wit )
        {
        if( (*wit).w_name_.compare(objs[i]->name() ) == 0 )
          {
          (*wit).w_obj_ += wgt_inc;
          break;
          }
        }
      }
    }

  // loop over the depth map scene to create query histogram, along with re-defined weight
  vcl_vector<double> radius = desc->radius();
  unsigned           nbins = desc->nbins();
  weights_hist_.resize(nbins);

  // ground
  if( !dms_->ground_plane().empty() )
    {
    vcl_vector<unsigned>              grd_bin_id;
    vcl_vector<depth_map_region_sptr> grd = dms_->ground_plane();
    for( unsigned g_idx = 0; g_idx < grd.size(); g_idx++ )
      {
      if( !grd[g_idx]->active() || grd[g_idx]->min_depth() > largest_rad )
        {
        continue;
        }
      double        min_depth = grd[g_idx]->min_depth();
      double        max_depth = grd[g_idx]->max_depth();
      unsigned char land_id = grd[g_idx]->land_id();
      double        dist = min_depth;
      if( dist > max_depth )
        {
        continue;
        }
      grd_bin_id.push_back(desc->bin_index(dist, land_id) );
      desc->set_count(dist, land_id, (unsigned char)1);
      for( unsigned i = 0; i < radius.size(); i++ )
        {
        // dist = min_depth + radius[i]+1;
        dist = radius[i];
        if( dist < min_depth )
          {
          continue;
          }
        if( dist < max_depth && dist < largest_rad )
          {
          grd_bin_id.push_back(desc->bin_index(dist, land_id) );
          desc->set_count(dist, land_id, (unsigned char)1);
          }
        }
      }
    unsigned n_grd_bin = grd_bin_id.size();
    float    w_grd_total, w_grd;
    w_grd_total = this->find_wgt_value("ground_plane");
    w_grd = w_grd_total / n_grd_bin;
    for( vcl_vector<unsigned>::iterator vit = grd_bin_id.begin(); vit != grd_bin_id.end(); ++vit )
      {
      weights_hist_[*vit] += w_grd;
      }
    }

  // objects
  if( !dms_->scene_regions().empty() )
    {
    vcl_vector<depth_map_region_sptr> objs = dms_->scene_regions();
    for( unsigned o_idx = 0; o_idx < objs.size(); o_idx++ )
      {
      if( !objs[o_idx]->active() || objs[o_idx]->min_depth() > largest_rad )
        {
        continue;
        }
      vcl_vector<unsigned> obj_bin_id;
      double               min_d = objs[o_idx]->min_depth();
      double               max_d = objs[o_idx]->max_depth();
      unsigned char        land_id = objs[o_idx]->land_id();
      float                wgt_obj = this->find_wgt_value(objs[o_idx]->name() );
      double               dist = min_d;
      if( dist > max_d )
        {
        continue;
        }
      obj_bin_id.push_back(desc->bin_index(dist, land_id) );
      desc->set_count(dist, land_id, (unsigned char)1);
      for( unsigned i = 0; i < radius.size(); i++ )
        {
        // dist = min_d + radius[i] + 1;
        dist = radius[i];
        if( dist <= min_d )
          {
          continue;
          }
        if( dist < max_d && dist < largest_rad )
          {
          obj_bin_id.push_back(desc->bin_index(dist, land_id) );
          desc->set_count(dist, land_id, (unsigned char)1);
          }
        }
      unsigned n_obj_bin = obj_bin_id.size();
      float    w_obj = wgt_obj / n_obj_bin;
      for( vcl_vector<unsigned>::iterator vit = obj_bin_id.begin(); vit != obj_bin_id.end(); ++vit )
        {
        weights_hist_[*vit] += w_obj;
        }
      }
    }
  volm_desc_sptr query(desc);
#if 0
  vcl_cout << "generated weight histogram:" << vcl_endl;
  for( vcl_vector<float>::iterator vit = weights_hist_.begin();  vit != weights_hist_.end(); ++vit )
    {
    vcl_cout << *vit << ' ';
    }
  vcl_cout << vcl_endl;
#endif

  return query;
}

bool volm_desc_ex_2d_matcher::check_threshold(volm_desc_sptr const& query, float& thres_value)
{
  unsigned num_valid_bin = query->get_area();

  if( thres_value < 1.0f / num_valid_bin )
    {
    thres_value = 1.0f / num_valid_bin;
    }
  return true;
}
