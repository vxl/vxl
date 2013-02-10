#include "depth_map_scene.h"
//:
// \file
#include "depth_map_region.h"
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_limits.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

depth_map_scene::
depth_map_scene(unsigned ni, unsigned nj,
                vcl_string const& image_path,
                vpgl_perspective_camera<double> const& cam,
                depth_map_region_sptr const& ground_plane,
                depth_map_region_sptr const& sky,
                vcl_vector<depth_map_region_sptr> const& scene_regions)
  : ni_(ni), nj_(nj), image_path_(image_path), cam_(cam)
{
  ground_plane_.push_back(ground_plane);
  sky_.push_back(sky);
  for (vcl_vector<depth_map_region_sptr>::const_iterator rit = scene_regions.begin();
       rit != scene_regions.end(); ++rit)
    scene_regions_[(*rit)->name()]=(*rit);
}

void depth_map_scene::set_ground_plane(vsol_polygon_2d_sptr ground_plane)
{
  vgl_plane_3d<double> gp(0.0, 0.0, 1.0, 0.0);//z axis is the plane normal
  depth_map_region_sptr ground = new depth_map_region(ground_plane, gp,
                                                      vcl_string("ground_plane"),
                                                      depth_map_region::GROUND_PLANE);
  ground_plane_.push_back(ground);
}

void depth_map_scene::add_ground(vsol_polygon_2d_sptr const& ground_plane,
                                 double min_depth,
                                 double max_depth,
                                 unsigned order,
                                 vcl_string name,
                                 unsigned land_id)
{
  vgl_plane_3d<double> gp(0.0, 0.0, 1.0, 0.0); // z axis is the plane normal
  depth_map_region_sptr ground = new depth_map_region(ground_plane, gp, name, depth_map_region::HORIZONTAL, land_id);
  ground->set_order(order);
  ground->set_min_depth(min_depth);
  ground->set_max_depth(max_depth);
  ground_plane_.push_back(ground);
}

void depth_map_scene::set_sky(vsol_polygon_2d_sptr sky)
{
  depth_map_region_sptr sky_pt = new depth_map_region(sky, "sky");
  sky_.push_back(sky_pt);
}

void depth_map_scene::add_sky(vsol_polygon_2d_sptr const& sky,
                              unsigned order,
                              vcl_string name)
{
  depth_map_region_sptr sky_pt = new depth_map_region(sky, name);
  sky_pt->set_order(order);
  sky_.push_back(sky_pt);
}

void depth_map_scene::add_region(vsol_polygon_2d_sptr const& region,
                                 vgl_vector_3d<double> region_normal,
                                 double min_depth,
                                 double max_depth,
                                 vcl_string name,
                                 depth_map_region::orientation orient,
                                 unsigned order,
                                 unsigned land_id)
{
  vgl_plane_3d<double> plane(region_normal.x(), region_normal.y(), region_normal.z(), 0.0);
  depth_map_region_sptr reg = new depth_map_region(region, plane,
                                                   min_depth, max_depth,
                                                   name,
                                                   orient,
                                                   land_id);
  reg->set_order(order);
  scene_regions_[name]=reg;
}


void depth_map_scene::
add_ortho_perp_region(vsol_polygon_2d_sptr const& region,
                      double min_depth, double max_depth,
                      vcl_string name)
{
  vgl_vector_3d<double> normal_dir = depth_map_region::perp_ortho_dir(cam_);
  this->add_region(region, normal_dir, min_depth, max_depth, name,
                   depth_map_region::VERTICAL);
}

vcl_vector<depth_map_region_sptr> depth_map_scene::scene_regions() const
{
  vcl_vector<depth_map_region_sptr> ret;
  vcl_map<vcl_string, depth_map_region_sptr>::const_iterator rit = scene_regions_.begin();
  for (; rit != scene_regions_.end(); ++rit)
    ret.push_back((*rit).second);
  return ret;
}

bool depth_map_scene::set_depth(double depth, vcl_string const& name)
{
  if (name == "sky" || name == "ground_plane")
    return false;
  if (scene_regions_.find(name) == scene_regions_.end())
    return false;
  depth_map_region_sptr r = scene_regions_[name];
  if (!r) return false;
  if (r->orient_type() != depth_map_region::VERTICAL)
    return false;
  r->set_region_3d(depth, cam_);
  return true;
}

void depth_map_scene::
set_ground_plane_max_depth(double max_depth,
                           double proximity_scale_factor)
{
  if (ground_plane_.size()) {
    for (unsigned i = 0 ; i < ground_plane_.size(); i++) {
      ground_plane_[i]->set_ground_plane_max_depth(max_depth, cam_,
                                                   proximity_scale_factor);
    }
  }
}

vil_image_view<float> depth_map_scene::
depth_map(unsigned log2_downsample_ratio)
{
  double ratio = vcl_pow(2.0, static_cast<double>(log2_downsample_ratio));
  double dni = static_cast<double>(ni_)/ratio,
    dnj = static_cast<double>(nj_)/ratio;
  if (dni<2.0) dni = 2.0;
  if (dnj<2.0) dnj = 2.0;
  unsigned sni = static_cast<unsigned>(dni), snj = static_cast<unsigned>(dnj);
  vil_image_view<float> depth(sni, snj);
  depth.fill(-1.0f); // depth is undefined

  // do the sky first so other regions can paint over it if
  // necessary
  bool good = true;
  if (sky_.size()) {
    vcl_vector<depth_map_region_sptr> skys;
    skys = sky_;
    vcl_sort(skys.begin(), skys.end(), compare_order());
    int ns = (int)skys.size();
    for (int i = (ns-1); i>=0; --i)
      good = good && skys[i]->update_depth_image(depth, cam_, ratio);
  }
  assert(good);
  // then do the ground plane
  if (ground_plane_.size()) {
    vcl_vector<depth_map_region_sptr> grounds;
    grounds = ground_plane_;
    vcl_sort(grounds.begin(), grounds.end(), compare_order());
    int ng = (int)grounds.size();
    for (int i = (ng-1); i>=0; i--)
      good = good && grounds[i]->update_depth_image(depth, cam_, ratio);
  }
  assert(good);
  // All the other objects
  vcl_vector<depth_map_region_sptr> regions;
  vcl_map<vcl_string, depth_map_region_sptr>::iterator rit =
    scene_regions_.begin();
  for (; rit != scene_regions_.end(); ++rit)
    if (rit->second->active())
      regions.push_back(rit->second);
  //sort on depth order
  vcl_sort(regions.begin(), regions.end(), compare_order());
  // paint in reverse depth order so closer regions paint over more
  // distant regions
  int nr = (int)regions.size();
  for (int i = (nr-1); i>=0; --i)
    good = good && (regions[i]->update_depth_image(depth, cam_, ratio));
  assert(good);
  return depth;
}

vil_image_view<float> depth_map_scene::
depth_map(vcl_string region_name, unsigned log2_downsample_ratio, double gp_dist_cutoff)
{
  double ratio = vcl_pow(2.0, static_cast<double>(log2_downsample_ratio));
  double dni = static_cast<double>(ni_)/ratio,
    dnj = static_cast<double>(nj_)/ratio;
  if (dni<2.0) dni = 2.0;
  if (dnj<2.0) dnj = 2.0;
  unsigned sni = static_cast<unsigned>(dni), snj = static_cast<unsigned>(dnj);
  vil_image_view<float> depth(sni, snj);
  depth.fill(-1.0f); // depth is undefined

  if (region_name.compare("ground_plane") == 0 && ground_plane_.size()) {
    vcl_vector<depth_map_region_sptr> grounds;
    grounds = ground_plane_;
    vcl_sort(grounds.begin(), grounds.end(), compare_order());
    int ng = (int)grounds.size();
    bool good = true;
    for (int i = (ng-1); i>=0; i--) {
      grounds[i]->set_region_3d(gp_dist_cutoff, this->cam());
      good = good && grounds[i]->update_depth_image(depth, cam_, ratio);
    }
    assert(good);
  }
  else if (region_name.compare("sky") == 0 && sky_.size()) {
    vcl_vector<depth_map_region_sptr> skys;
    skys = sky_;
    vcl_sort(skys.begin(), skys.end(), compare_order());
    int ns = (int)skys.size();
    bool good = true;
    for (int i = (ns-1); i>=0; --i)
      good = good && skys[i]->update_depth_image(depth, cam_, ratio);
    assert(good);
  }
  else {
    vcl_map<vcl_string, depth_map_region_sptr>::iterator rit = scene_regions_.begin();
    for (; rit != scene_regions_.end(); ++rit)
      if (rit->first.compare(region_name) == 0 && rit->second) {
        rit->second->update_depth_image(depth, cam_, ratio);
        break;
      }
  }

  return depth;
}

//: the iterator at the start of depth search
scene_depth_iterator depth_map_scene::begin()
{
  this->init_depths();
  return scene_depth_iterator(this);
}

//: the iterator at the end of depth search
scene_depth_iterator depth_map_scene::end()
{
  scene_depth_iterator temp;
  temp.set_end();
  return temp;
}

//: move vert regions to next depth configuration. returns false if done
bool depth_map_scene::next_depth()
{
  bool increment_done = false;
  unsigned nst = depth_states_.size();
  for (unsigned i = 0; (i<nst)&&!increment_done; ++i) {
    if (!increment_done) {
      double depth = depth_states_[i]->depth();
      depth_map_region_sptr& dmr = depth_states_[i];
      double min_depth = dmr->min_depth();
      double max_depth = dmr->max_depth();
      double depth_inc = dmr->depth_inc();
      depth += depth_inc;
      bool depth_order = true;
      if (i<(nst-1))
        depth_order = depth <= depth_states_[i+1]->depth();
      if (depth_order&& depth <= max_depth) {
        this->set_depth(depth, dmr->name());
        increment_done = true;
      }
      else
        this->set_depth(min_depth, dmr->name());
    }
  }
  return increment_done;
}


//: initialize the movable depth configuration
void depth_map_scene::init_depths()
{
  depth_states_.clear();
  vcl_map<vcl_string, depth_map_region_sptr>::iterator rit =
    scene_regions_.begin();
  for (; rit !=     scene_regions_.end(); ++rit)
    if (rit->second->active())
      depth_states_.push_back(rit->second);
  //sort on depth order
  vcl_sort(depth_states_.begin(), depth_states_.end(), compare_order());
  //set depths to min depth.
  unsigned ns = depth_states_.size();
  for (unsigned i = 0; i<ns; ++i)
    this->set_depth(depth_states_[i]->min_depth(), depth_states_[i]->name());
  //assert on inconsistent depths, i.e. min_depths must respect depth order
  for (unsigned i = 1; i<ns; ++i)
    assert(depth_states_[i]->depth()>=depth_states_[i-1]->depth());
}

void depth_map_scene::print_depth_states()
{
  unsigned ns = depth_states_.size();
  for (unsigned i = 0; i<ns; ++i) {
    depth_map_region_sptr dmr = depth_states_[i];
    vcl_cout << dmr->name() << ' ' << dmr->depth() << ' ';
  }
  vcl_cout << '\n';
}

//: binary IO write
void depth_map_scene::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, ni_);
  vsl_b_write(os, nj_);
  vsl_b_write(os, image_path_);
  vsl_b_write(os, scene_regions_);
  vsl_b_write(os, sky_);
  vsl_b_write(os, ground_plane_);
  vsl_b_write(os, cam_);
}

//: binary IO read
void depth_map_scene::b_read(vsl_b_istream& is)
{
  unsigned ver = 1;
  vsl_b_read(is, ver);
  if (ver == 1) {
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    vsl_b_read(is, image_path_);
    vsl_b_read(is, scene_regions_);
    depth_map_region_sptr gp;
    vsl_b_read(is, gp);
    ground_plane_.push_back(gp);
    depth_map_region_sptr sky;
    vsl_b_read(is, sky);
    sky_.push_back(sky);
    vsl_b_read(is, cam_);
  }
  else if (ver == 2) {
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    vsl_b_read(is, image_path_);
    vsl_b_read(is, scene_regions_);
    unsigned cnt;
    vsl_b_read(is, cnt);
    for (unsigned i = 0; i < cnt; i++) {
      depth_map_region_sptr gp;
      vsl_b_read(is, gp);
      ground_plane_.push_back(gp);
    }
    vsl_b_read(is, cnt);
    for (unsigned i = 0; i < cnt; i++) {
      depth_map_region_sptr sky;
      vsl_b_read(is, sky);
      sky_.push_back(sky);
    }
    vsl_b_read(is, cam_);
  }
  else if (ver == 3) {
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    vsl_b_read(is, image_path_);
    vsl_b_read(is, scene_regions_);
    vsl_b_read(is, sky_);
    vsl_b_read(is, ground_plane_);
    vsl_b_read(is, cam_);
  }
  else {
    vcl_cout << " in depth_map_scene::b_read - unknown version\n";
    return;
  }
}

void vsl_b_write(vsl_b_ostream& os, const depth_map_scene* ds_ptr)
{
  if (ds_ptr ==0)
    vsl_b_write(os, false);
  else
    vsl_b_write(os, true);
  depth_map_scene* ds_non_const = const_cast<depth_map_scene*>(ds_ptr);
  ds_non_const->b_write(os);
}

void vsl_b_read(vsl_b_istream &is, depth_map_scene*& ds_ptr)
{
  bool valid_ptr = false;
  vsl_b_read(is, valid_ptr);
  if (valid_ptr) {
    ds_ptr = new depth_map_scene();
    ds_ptr->b_read(is);
    return;
  }
  ds_ptr = 0;
}

void vsl_b_write(vsl_b_ostream& os, const depth_map_scene_sptr& ds_ptr)
{
  depth_map_scene* ds=ds_ptr.ptr();
  vsl_b_write(os, ds);
}

void vsl_b_read(vsl_b_istream &is, depth_map_scene_sptr& ds_ptr)
{
  depth_map_scene* ds=0;
  vsl_b_read(is, ds);
  ds_ptr = ds;
}

bool pair_sort(vcl_pair<float, int> const& lhs, vcl_pair<float, int> const& rhs)
{
  return lhs.first < rhs.first;
}

inline float denominator(float std_dev)
{
  return (float)(1.0f/(std_dev*vcl_sqrt(2*vnl_math::pi)));
}

inline float normal_pdf(float d, float mean, float std_dev, float den)
{
  float expo = (d-mean)/std_dev;
  return (float)den*vcl_exp(-0.5*expo*expo);
}


//: match to a given continuous depth image
bool depth_map_scene::match(vil_image_view<float> const& depth_img, vil_image_view<float> const& vis_img, unsigned level, float& score)
{
  //vcl_cout << "Scene depth map file, # of regions: " << scene.scene_regions().size() << '\n';
  this->begin();
  // first count number of overlapping sky pixels
  vil_image_view<float> sky_img = this->depth_map("sky", level);

  unsigned ni = sky_img.ni(); unsigned nj = sky_img.nj();
  assert(ni == depth_img.ni() && nj == depth_img.nj() && ni == vis_img.ni() && nj == vis_img.nj());

  vcl_vector<vil_image_view<float> > region_imgs;

  for (unsigned r = 0; r < this->scene_regions().size(); r++) {
    vil_image_view<float> reg_img = this->depth_map(this->scene_regions()[r]->name(), level);
    region_imgs.push_back(reg_img);
    //vil_save(reg_img, "C:\\projects\\FINDER\\query_matching\\scene_depth_exp\\reg.tif");
  }
  vcl_vector<float> region_means(this->scene_regions().size(), 0.0f);
  vcl_vector<float> region_std_dev(this->scene_regions().size(), 0.0f);

  vil_image_view<bool> processed_pixels(ni, nj);
  processed_pixels.fill(false);

  score = 0.0f;
  float pixel_cnt = 0.0f;
  float vis_threshold = 0.2f;

  // first measure the correspondence of sky pixels
  for (unsigned j = 0; j < nj; j++)
  {
    for (unsigned i = 0; i < ni; i++)
    {
      if (processed_pixels(i,j))
        continue;
      if (sky_img(i,j) > 0) {
        pixel_cnt++;
        processed_pixels(i,j) = true;
        if (vis_img(i,j) >= vis_threshold)
          score++;
      }
      else
      {
#if 0
        // if not sky then first make sure this is an occupied portion of the image
        if (vis_img(i,j) >= 1.0f) { // TODO: TRY THIS CONDITION AFTER TRYING REGIONS AND IT CORRESPONDS TO GROUND PLANE, MAYBE DO STH..
          processed_pixels(i,j) = true;
          pixel_cnt++;
          continue;
        }
#endif
        for (unsigned r = 0; r < region_imgs.size(); r++)
        {
          if ((region_imgs[r])(i,j) > 0) {
            // collect depth variation in the region
            float mean = 0.0;
            int cnt = 0;
            for (unsigned v = 0; v < nj; v++)
              for (unsigned u = 0; u < ni; u++) {
                if (region_imgs[r](u,v) > 0 && vis_img(u,v) < vis_threshold) { // if labeled region and on a visible surface == use this as mask
                  mean += depth_img(u,v);
                  cnt++;
                }
              }
            float std_dev = 0.0;
            mean /= cnt;
            region_means[r] = mean;
            for (unsigned v = 0; v < nj; v++)
              for (unsigned u = 0; u < ni; u++)
                if (region_imgs[r](u,v) > 0 && vis_img(u,v) < vis_threshold)  // use this as mask
                  std_dev += (depth_img(u,v)-mean)*(depth_img(u,v)-mean);
            std_dev /= cnt;
            std_dev = (float)vcl_sqrt(std_dev);
            region_std_dev[r] = std_dev;
            // will normalize probs by assuming that non-object depths are given by a uniform distribution of 4*std_dev
            float norm_val = 1.0/(4*std_dev);
#ifdef DEBUG
            vcl_cout << " region: " << scene.scene_regions()[r]->name() << " depth mean: " << mean << " std dev: " << std_dev << " score prev: " << score << vcl_endl;
#endif
            float den = denominator(std_dev); // = 1.0f/float(std_dev*vnl_math::sqrt2pi);
            // now find the likelihood of each pixel in this region to be a part of the same object with this depth profile
            for (unsigned v = 0; v < nj; v++)
              for (unsigned u = 0; u < ni; u++)
                if (region_imgs[r](u,v) > 0) { // use this as mask
                  if (vis_img(u,v) < vis_threshold) {
#if 0
                    float expo = (depth_img(u,v)-mean)/std_dev;
                    float prob = 1.0f/den*(float)vcl_exp(-0.5*expo*expo);
#else
                    float prob = normal_pdf(depth_img(u,v), mean, std_dev, den);
#endif
                    prob /= prob+norm_val;
                    score += prob;
                    pixel_cnt++;
                  }
                  processed_pixels(u,v) = true;
                }
            break;
          }
        }
      }
    }
  }
  // check the min depth and ordering of the images
  vcl_vector<vcl_pair<float, int> > region_order;
  for (unsigned r = 0; r < this->scene_regions().size(); r++) {
    if (this->scene_regions()[r]->min_depth() > region_means[r]) {
      //vcl_cout << " region: " << scene.scene_regions()[r]->name() << " has min depth: " << scene.scene_regions()[r]->min_depth() <<" but mean is: " << region_means[r] << vcl_endl;
      score = 0.0f;
      region_imgs.clear();
      return true;
    }
    region_order.push_back(vcl_pair<float, int>(region_means[r], this->scene_regions()[r]->order()));
  }
  vcl_sort(region_order.begin(), region_order.end(), &pair_sort);

#ifdef DEBUG
  for (unsigned r = 0; r < scene.scene_regions().size(); r++)
    vcl_cout << "region: " << scene.scene_regions()[r]->name() << ' ' << region_order[r].first << ' ' << region_order[r].second << '\n';
#endif
  for (unsigned r = 0; r < this->scene_regions().size(); ++r)
    if (region_order[r].second != (int)r+1) {
      score = 0.0f;
      region_imgs.clear();
      return true;
    }

  if (pixel_cnt > 0)
    score /= pixel_cnt;
  else
    score = 0.0f;

  region_imgs.clear();
  return true;
}

//: match to a given continuoue depth image, use the ground_plane constraint as well
bool depth_map_scene::match_with_ground(vil_image_view<float> const& depth_img, vil_image_view<float> const& vis_img, unsigned level, float ground_depth_std_dev, float& score)
{
  unsigned ni = depth_img.ni(); unsigned nj = depth_img.nj();
  assert(ni == vis_img.ni() && nj == vis_img.nj());

  //vil_image_view<float> mag(ni, nj);
  //brip_vil_float_ops::gradient_mag_3x3(depth_img, mag);
  //vil_save(mag, "C:\\projects\\FINDER\\gradient.tif");

  this->begin();
  // first count number of overlapping sky pixels
  // ensure all sky regions are active ?
  bool sky_active = false;
  if (sky_.size()) {
    sky_active = true;
    unsigned ns = (unsigned)sky_.size();
    for (unsigned i = 0 ; i < ns; i++)
      sky_active = sky_active && sky_[i]->active();
  }
  vil_image_view<float> sky_img = this->depth_map("sky",level);
  assert(ni == sky_img.ni() && ni == sky_img.nj());

#if 0
  bool sky_active = this->sky() ? this->sky()->active() : false;
  vil_image_view<float> sky_img = this->depth_map("sky", level);
  assert(ni == sky_img.ni() && nj == sky_img.nj());
#endif
  bool ground_active = false;
  if (ground_plane_.size()) {
    ground_active = true;
    unsigned ng = (unsigned)ground_plane_.size();
    for (unsigned i = 0; i < ng; i++)
      ground_active = ground_active && ground_plane_[i]->active();
  }
  if (ground_active) {
    // ensure the region_3D exists for all grounds
    for (unsigned i = 0; i < ground_plane_.size(); i++)
      ground_plane_[i]->set_region_3d(ground_plane_[i]->max_depth(), this->cam());
  }
  vil_image_view<float> ground_img = this->depth_map("ground_plane", level);
  vcl_vector<vil_image_view<float> > region_imgs;
  vcl_vector<vcl_string> region_names;
  vcl_vector<vcl_pair<float, int> > region_min_depths;
  vcl_vector<float> region_means;
  //vcl_vector<float> region_std_dev(this->scene_regions().size(), 0.0f);
  vcl_vector<float> region_scores;


#if 0
  vcl_map<vcl_string, vil_image_view<float> > debug_imgs;
  vil_image_view<float> temp(ni, nj); temp.fill(-1);
  debug_imgs["sky"] = temp;
  vil_image_view<float> temp1(ni, nj); temp1.fill(-1);
  debug_imgs["ground"] = temp1;
#endif
  for (unsigned r = 0; r < this->scene_regions().size(); r++) {
    if (this->scene_regions()[r]->active()) {
      vil_image_view<float> reg_img = this->depth_map(this->scene_regions()[r]->name(), level);
      region_imgs.push_back(reg_img);
      region_names.push_back(this->scene_regions()[r]->name());
      region_scores.push_back(0.0f);
      region_means.push_back(0.0f);
      region_min_depths.push_back(vcl_pair<float, int>(this->scene_regions()[r]->min_depth(), this->scene_regions()[r]->order()));
#if 0
      vil_image_view<float> temp2(ni, nj); temp2.fill(-1);
      debug_imgs[this->scene_regions()[r]->name()] = temp2;
#endif
    }
  }

#if 0
  vil_image_view<bool> processed_pixels(ni, nj);
  processed_pixels.fill(false);
#endif

  score = 0.0f;
  int score_cnt = 0;
  float vis_threshold = 0.2f;

  float gd_den = denominator(ground_depth_std_dev);
  float gd_norm_val = 1.0/(4*ground_depth_std_dev);
#if 0
  float pixel_cnt = 0.0f;
  // set sky prob to norm(mu+-2*ground_depth_std_dev), i.e. whatever ground would be at mean+-std_dev
  float dummy_mean = 0.0;
  float sky_prob = normal_pdf(dummy_mean-4*ground_depth_std_dev, dummy_mean, ground_depth_std_dev, gd_den);
  vcl_cout << "sky prob: " << sky_prob << '\n';

  // weights for beachgrass query
  float ground_weight = 0.899999999f;
  float sky_weight =    0.000000001f;
  float region_weight = (1.0f-(ground_weight+sky_weight))/region_names.size();
#endif

  // weights for res01 : clouds
  float ground_weight = 0.0;
  float sky_weight = 1.0f/(region_names.size()+1);
  float region_weight = sky_weight;

#if 0
  // first measure the correspondence of sky pixels
  for (unsigned j = 0; j < nj; j++)
  {
    for (unsigned i = 0; i < ni; i++)
    {
      if (processed_pixels(i,j))
        continue;
      if (sky_img(i,j) > 0) {
        processed_pixels(i,j) = true;
        if (vis_img(i,j) >= vis_threshold) {
          debug_imgs["sky"](i,j) = 1.0f;
          //score++;
          scores["sky"].first = scores["sky"].first + 1;
        }
        scores["sky"].second = scores["sky"].second + 1;
      }
      else if (ground_img(i,j) > 0) {
        // use depth image value as mean and a fixed std_dev
        float prob = normal_pdf(ground_img(i,j), depth_img(i,j), ground_depth_std_dev, gd_den);
        prob = prob/(prob+gd_norm_val);
        scores["ground"].first = scores["ground"].first + prob;
        scores["ground"].second = scores["ground"].second + 1;
        processed_pixels(i,j) = true;
      }
      else
      {
        for (unsigned r = 0; r < region_imgs.size(); r++) {
          if ((region_imgs[r])(i,j) > 0) {
            // collect depth variation in the region
            float mean = 0.0;
            int cnt = 0;
            for (unsigned v = 0; v < nj; v++)
              for (unsigned u = 0; u < ni; u++) {
                if (region_imgs[r](u,v) > 0 && vis_img(u,v) < vis_threshold) { // if labeled region and on a visible surface == use this as mask
                  mean += depth_img(u,v);
                  cnt++;
                  debug_imgs[region_names[r]](u,v) = depth_img(u,v);
                }
              }
            float std_dev = 0.0;
            mean /= cnt;
            region_means[r] = mean;
            for (unsigned v = 0; v < nj; v++)
              for (unsigned u = 0; u < ni; u++)
                if (region_imgs[r](u,v) > 0 && vis_img(u,v) < vis_threshold)  // use this as mask
                  std_dev += (depth_img(u,v)-mean)*(depth_img(u,v)-mean);
            std_dev /= cnt;
            std_dev = (float)vcl_sqrt(std_dev);
            region_std_dev[r] = std_dev;
            // will normalize probs by assuming that non-object depths are given by a uniform distribution of 4*std_dev
            float norm_val = 1.0/(4*std_dev);
            //vcl_cout << " region: " << scene.scene_regions()[r]->name() << " depth mean: " << mean << " std dev: " << std_dev << " score prev: " << score << vcl_endl;
            float den = denominator(std_dev);
            // now find the likelihood of each pixel in this region to be a part of the same object with this depth profile
            for (unsigned v = 0; v < nj; v++)
              for (unsigned u = 0; u < ni; u++)
                if (region_imgs[r](u,v) > 0)  {  // use this as mask
                  if (vis_img(u,v) < vis_threshold) {
                    float d = depth_img(u,v);
                    float prob = normal_pdf(d, mean, std_dev, den);
                    prob = prob/(prob+norm_val);
                    scores[region_names[r]].first = scores[region_names[r]].first + prob;
                    scores[region_names[r]].second = scores[region_names[r]].second + 1;
                  }
                  processed_pixels(u,v) = true;
                }
            break;
          }
        }
      }
    }
  }
#endif
  vcl_cout << "score: " << score << '\n';
  if (sky_active) {
    float sky_score = 0.0f;
    int sky_cnt = 0;
    for (unsigned j = 0; j < nj; j++) {
      for (unsigned i = 0; i < ni; i++) {
        if (sky_img(i,j) > 0) {
          if (vis_img(i,j) >= vis_threshold)
            sky_score += 1.0f;
          sky_cnt++;
        }
      }
    }
    if (sky_cnt > 0) sky_score /= sky_cnt;
    vcl_cout << "sky score: " << sky_score << '\n';
    score += sky_weight*sky_score;
    score_cnt++;
  }
  vcl_cout << "score: " << score << '\n';
  if (ground_active) {
    float g_score = 0;
    int g_cnt = 0;
    for (unsigned j = 0; j < nj; j++) {
      for (unsigned i = 0; i < ni; i++) {
        if (ground_img(i,j) > 0) {
          // use depth image value as mean and a fixed std_dev
          float prob = normal_pdf(ground_img(i,j), depth_img(i,j), ground_depth_std_dev, gd_den);
          prob = prob/(prob+gd_norm_val);
          g_score += prob;
          g_cnt++;
        }
      }
    }
    if (g_cnt > 0) g_score /= g_cnt;
    vcl_cout << "ground score: " << g_score << '\n';
    score += ground_weight*g_score;
    score_cnt++;
  }
  vcl_cout << "score: " << score << '\n';
  for (unsigned r = 0; r < region_imgs.size(); r++) {
    float region_score = 0.0f;
    float mean = 0.0f;
    int cnt = 0;
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        if (region_imgs[r](i,j) > 0 && vis_img(i,j) < vis_threshold) { // if labeled region and on a visible surface == use this as mask
          mean += depth_img(i,j);
          cnt++;
        }
    if (!cnt) {
      region_scores[r] = region_score;
      vcl_cout << "region " << region_names[r] << " score: " << region_score << '\n';
      continue;
    }
    float std_dev = 0.0f;
    mean /= cnt;
    region_means[r] = mean;
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        if (region_imgs[r](i,j) > 0 && vis_img(i,j) < vis_threshold)  // use this as mask
          std_dev += (depth_img(i,j)-mean)*(depth_img(i,j)-mean);
    std_dev /= cnt;
    std_dev = (float)vcl_sqrt(std_dev);
    // normalize probs by assuming that non-object depths are given by a uniform distribution of 4*std_dev
    float norm_val = 1.0/(4*std_dev);
#if 0
    vcl_cout << " region: " << scene.scene_regions()[r]->name() << " depth mean: " << mean << " std dev: " << std_dev << " score prev: " << score << vcl_endl;
#endif
    float den = denominator(std_dev);
    // find the likelihood of each pixel in this region to be a part of the same object with this depth profile
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        if (region_imgs[r](i,j) > 0 && vis_img(i,j) < vis_threshold)  {  // use this as mask
          float d = depth_img(i,j);
          float prob = normal_pdf(d, mean, std_dev, den);
          prob = prob/(prob+norm_val);
          region_score += prob;
        }
    region_score /= cnt;
    vcl_cout << "region " << region_names[r] << " score: " << region_score << '\n';
    region_scores[r] = region_score;
  }

  // check the min depth and ordering of the images
  vcl_vector<vcl_pair<float, int> > region_order;
  for (unsigned r = 0; r < region_min_depths.size(); r++) {
    if (region_min_depths[r].first > region_means[r]) {
      //vcl_cout << " region: " << scene.scene_regions()[r]->name() << " has min depth: " << scene.scene_regions()[r]->min_depth() <<" but mean is: " << region_means[r] << vcl_endl;
      score = 0.0f;
      region_imgs.clear();
      return true;
    }
    region_order.push_back(vcl_pair<float, int>(region_means[r], region_min_depths[r].second));
  }
  vcl_sort(region_order.begin(), region_order.end(), &pair_sort);

  for (unsigned r = 0; r < region_names.size(); r++)
    vcl_cout << "region: " << region_names[r] << ' ' << region_order[r].first << ' ' << region_order[r].second << '\n';
  for (unsigned r = 1; r < region_order.size(); r++)
    if (region_order[r].second < region_order[r-1].second) {
      score = 0.0f;
      region_imgs.clear();
      return true;
    }
  for (unsigned r = 0; r < region_names.size(); r++) {
    score += region_weight*region_scores[r];
    score_cnt++;
  }
  if (score_cnt > 0) score /= score_cnt;
  vcl_cout << "score: " << score << vcl_endl;

#if 0
  if (pixel_cnt > 0)
    score /= pixel_cnt;
  else
    score = 0.0f;

  for (unsigned v = 0; v < nj; v++)
    for (unsigned u = 0; u < ni; u++) {
      if (ground_img(u,v) > 0)  // if labeled region and on a visible surface == use this as mask
        debug_imgs["ground"](u,v) = depth_img(u,v);
     }

  for (vcl_map<vcl_string, vil_image_view<float> >::iterator iter = debug_imgs.begin(); iter != debug_imgs.end(); iter++) {
    vcl_string name = "C:\\projects\\FINDER\\query_matching\\scene_depth_exp\\reg_debug_"+iter->first+".tif";
    vil_save(iter->second, name.c_str());
    name = "C:\\projects\\query_matching\\scene_depth_exp\\reg_debug_"+iter->first+".png";
    vil_image_view<vxl_byte> dest(ni, nj);
    vil_convert_stretch_range(iter->second, dest);
    vil_save(dest, name.c_str());
  }

  vcl_string name = "C:\\projects\\query_matching\\scene_depth_exp\\reg_sky.tif";
  vil_save(sky_img, name.c_str());
  name = "C:\\projects\\query_matching\\scene_depth_exp\\reg_sky.png";
  vil_image_view<vxl_byte> dest(ni, nj);
  vil_convert_stretch_range(sky_img, dest);
  vil_save(dest, name.c_str());

  name = "C:\\projects\\query_matching\\scene_depth_exp\\reg_ground.tif";
  vil_save(ground_img, name.c_str());
  name = "C:\\projects\\query_matching\\scene_depth_exp\\reg_ground.png";
  vil_convert_stretch_range(ground_img, dest);
  vil_save(dest, name.c_str());

  for (unsigned r = 0, rr = 0; r < this->scene_regions().size(); r++) {
    if (this->scene_regions()[r]->active()) {
      vcl_stringstream ss; ss << this->scene_regions()[r]->name();
      vcl_string name = "C:\\projects\\query_matching\\scene_depth_exp\\reg_"+ss.str()+".png";
      vil_convert_stretch_range(region_imgs[rr], dest);
      vil_save(dest, name.c_str());
      rr++;
    }
  }
#endif
  region_imgs.clear();
  return true;
}

