#include "sdet_texture_classifier.h"
//
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_filter_bank.h>
#include <bsta/bsta_k_means.h>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_numeric_traits.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vcl_cstdlib.h> // for std::rand()
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vul/vul_file_iterator.h>

vcl_vector<vgl_polygon<double> > sdet_texture_classifier::load_polys(vcl_string const& poly_path)
{
  vcl_vector<vsol_spatial_object_2d_sptr> sos;
  vsl_b_ifstream istr(poly_path);
  if (!istr) {
    vcl_cout << "Failed to open input stream "
             << poly_path << vcl_endl;
    return vcl_vector<vgl_polygon<double> >();
  }
  vsl_b_read(istr, sos);
  if (!sos.size()) {
    vcl_cout << "no polys "
             << poly_path << vcl_endl;
    return vcl_vector<vgl_polygon<double> >();
  }
  vcl_vector<vgl_polygon<double> > vpolys;
  for (unsigned i = 0; i<sos.size(); ++i) {
    vsol_polygon_2d* poly = static_cast<vsol_polygon_2d*>(sos[i].ptr());
    vgl_polygon<double> vpoly; vpoly.new_sheet();
    unsigned nverts = poly->size();
    for (unsigned i = 0; i<nverts; ++i) {
      vsol_point_2d_sptr v = poly->vertex(i);
      vpoly.push_back(v->x(), v->y());
    }
    vpolys.push_back(vpoly);
  }
  return vpolys;
}

vil_image_view<float> sdet_texture_classifier::
scale_image(vil_image_resource_sptr const& resc)
{
  vil_pixel_format fmt = resc->pixel_format();
  vil_image_view<float> img = brip_vil_float_ops::convert_to_float(resc);
  if (fmt == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);
  if (fmt == VIL_PIXEL_FORMAT_UINT_16)
    vil_math_scale_values(img,1.0/2048.0);
  return img;
}

static unsigned gauss_radius(float sigma, float cutoff_ratio)
{
  double sigma_sq_inv = 1/(sigma*sigma);
  int r = static_cast<unsigned>(vcl_sqrt((-2.0*vcl_log(cutoff_ratio))/sigma_sq_inv)+0.5);
  return r;
}

// define a color map for texture categories. Defined for up to eight classes
void sdet_texture_classifier::init_color_map()
{
  vcl_vector<vnl_vector_fixed<float, 3> > colors(8);
  colors[0][0]=0.0f; colors[0][1]=0.0f; colors[0][2]=1.0f;
  colors[1][0]=0.0f; colors[1][1]=1.0f; colors[1][2]=0.0f;
  colors[2][0]=0.0f; colors[2][1]=0.5f; colors[2][2]=0.5f;
  colors[3][0]=1.0f; colors[3][1]=0.0f; colors[3][2]=0.0f;
  colors[4][0]=0.5f; colors[4][1]=0.0f; colors[4][2]=0.5f;
  colors[5][0]=0.5f; colors[5][1]=0.5f; colors[5][2]=0.0f;
  colors[6][0]=0.0f; colors[6][1]=0.25f; colors[6][2]=0.75f;
  colors[7][0]=0.25f; colors[7][1]=0.25f; colors[7][2]=0.5f;
  unsigned i = 0;
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::const_iterator it= texton_dictionary_.begin();
  for (; it!= texton_dictionary_.end(); ++it,++i) {
    color_map_[(*it).first] = colors[i];
    vcl_cout << (*it).first <<":(" << colors[i][0] << ' '
             << colors[i][1] << ' ' << colors[i][2] << ")\n" << vcl_flush;
  }
  color_map_valid_ = true;
}

void sdet_texture_classifier::print_color_map() const
{
  if (!color_map_valid_) return;
  vcl_cout << "Category Color Map\n";
  vcl_map< vcl_string, vnl_vector_fixed<float, 3> >::const_iterator it= color_map_.begin();
  for (; it!= color_map_.end(); ++it) {
    vnl_vector_fixed<float,3> const & colors = (*it).second;
    vcl_cout << (*it).first <<":(" << colors[0] << ' '
             << colors[1] << ' ' << colors[2] << ")\n" << vcl_flush;
  }
}

sdet_texture_classifier::
sdet_texture_classifier(sdet_texture_classifier_params const& params)
: sdet_texture_classifier_params(params),
  filter_responses_(brip_filter_bank(params.n_scales_,
                                     params.scale_interval_,
                                     params.lambda0_,
                                     params.lambda1_,
                                     params.angle_interval_,
                                     params.cutoff_per_)),
  distances_valid_(false), inter_prob_valid_(false), color_map_valid_(false),
  texton_index_valid_(false), texton_weights_valid_(false)
{ 
  maxr_ = this->max_filter_radius(); 
  vcl_cout << " the max image border eaten by classifier: " << maxr_ << '\n'; 
}


//: helper function to compute gauss response of an image using params of the instance, assumes the output image is properly initialized
void sdet_texture_classifier::compute_gauss_response(vil_image_view<float> const& img, vil_image_view<float>& out_gauss)
{
  out_gauss = brip_vil_float_ops::gaussian(img, gauss_radius_);
  for (unsigned j = 0; j<out_gauss.nj(); ++j)
    for (unsigned i = 0; i<out_gauss.ni(); ++i)
      out_gauss(i,j) =out_gauss(i,j)*0.03f;//HACK!! Need principled way to scale
}


bool sdet_texture_classifier::
compute_filter_bank(vil_image_view<float> const& img)
{
  vul_timer t;
  filter_responses_.set_image(img);
  bool bright = false;
  bool scale_invariant = true;

  vcl_cout<< "s = ("<< laplace_radius_<< ' ' << laplace_radius_ << ")\n"
          << vcl_flush;

  laplace_ = brip_vil_float_ops::fast_extrema(img, laplace_radius_,
                                              laplace_radius_, bright, mag_,
                                              false,false, signed_response_,
                                              scale_invariant, false,
                                              cutoff_per_);

  vcl_cout<< "s = ("<< gauss_radius_<< ' ' << gauss_radius_ << ")\n"
          << vcl_flush;
  
  this->compute_gauss_response(img, gauss_);

  //filter responses
  vcl_cout << "Computed filter bank in " << t.real()/1000.0 << " secs.\n";
#if 0
  //=============== temporary debug ====================
  vil_save(filter_responses_.response(0), "e:/images/TextureTraining/s0.tiff");
  vil_save(filter_responses_.response(1), "e:/images/TextureTraining/s1.tiff");
  vil_save(filter_responses_.response(2), "e:/images/TextureTraining/s2.tiff");
  vil_save(laplace_, "e:/images/TextureTraining/laplace.tiff");
  vil_save(gauss_, "e:/images/TextureTraining/gauss.tiff");
  //===========================================================
#endif
  return true;
}

#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vul/vul_file.h>

//: check the folder if already computed using the image name, otherwise compute and save
bool sdet_texture_classifier::compute_filter_bank_color_img(vcl_string const& filter_folder, vcl_string const& img_name)
{
  vcl_cout << "computing filter bank on: " << img_name << vcl_endl;
  vcl_string name = vul_file::strip_directory(img_name);
  name = vul_file::strip_extension(name);

  vcl_string filterbank_dir = filter_folder + this->filter_dir_name();
  if (!vul_file::exists(filterbank_dir))
    vul_file::make_directory(filterbank_dir);

  // first check filter folder if already computed
  vcl_string img_out_dir = filterbank_dir + "/" + name + "/";
  bool exists = vul_file::exists(img_out_dir);
  if (!exists || !this->load_filter_responses(img_out_dir))
  {
    vil_image_resource_sptr img = vil_load_image_resource(img_name.c_str());
    vil_image_view<unsigned char> out_img(img->ni(),img->nj());
    vil_image_view<float> out_imgf(img->ni(),img->nj());
    vil_convert_planes_to_grey<unsigned char,unsigned char>(*((img->get_view()).as_pointer()),out_img);
    vil_convert_stretch_range(out_img, out_imgf, 0.0f, 1.0f);
    //vil_save(out_imgf, (in_poly() + imgs[ii].first + "_stretched.tif").c_str());

    if (!this->compute_filter_bank(out_imgf))
      return false;
    if (!exists) 
      if (!vul_file::make_directory(img_out_dir)) {
        vcl_cerr << " in sdet_texture_classifier::compute_filter_bank_color_img - cannot create directory: " << img_out_dir << vcl_endl;
        return false;
      }
    this->save_filter_responses(img_out_dir);
  }
  return true;
}

//: check the folder if already computed using the image name, otherwise compute and save
bool sdet_texture_classifier::compute_filter_bank_float_img(vcl_string const& filter_folder, vcl_string const& img_name, float max_val)
{
  unsigned size = this->max_filter_radius();
  vcl_cout << "computing filter bank on: " << img_name << " max filter radius: " << size << vcl_endl;
  vcl_string name = vul_file::strip_directory(img_name);
  name = vul_file::strip_extension(name);
  
  vcl_string filterbank_dir = filter_folder + this->filter_dir_name();
  if (!vul_file::exists(filterbank_dir))
    vul_file::make_directory(filterbank_dir);

  // first check filter folder if already computed
  vcl_string img_out_dir = filterbank_dir + "/" + name + "/";
  bool exists = vul_file::exists(img_out_dir);
  if (!exists || !this->load_filter_responses(img_out_dir))
  {
    vil_image_view<float> img_orig = vil_load(img_name.c_str());

    if (img_orig.ni() <= size || img_orig.nj() <= size) {
      vcl_cerr << " the image does not have sufficient size to extract this filter bank, skipping!\n";
      return false;
    }

    vil_image_view<float> img(img_orig.ni(),img_orig.nj());
    
    if (max_val > 0) { // stretch if max_val is passed
      vil_math_truncate_range(img_orig, 0.0f, max_val);
      // stretch as classification requires an image in [0,1]
      vil_convert_stretch_range_limited(img_orig, img, 0.0f, max_val, 0.0f, 1.0f);
    } else
      img.deep_copy(img_orig);

    if (!this->compute_filter_bank(img))
      return false;
    if (!exists)
      if (!vul_file::make_directory(img_out_dir)) {
        vcl_cerr << " in sdet_texture_classifier::compute_filter_bank_float_img() - cannot create directory: " << img_out_dir << vcl_endl;
        return false;
      }
    this->save_filter_responses(img_out_dir);
  }
  return true;
}

//: append to the vector of other_responses_
//  it may be necessary to increase the dimensionality using another source of info than the original image
// this method checks whether an other response with this name is already computed and saved
void sdet_texture_classifier::add_gauss_response(vil_image_view<float>& img_f, vcl_string const& filter_folder,
                                                 vcl_string const& img_name, vcl_string const& response_name,
                                                 bool const& is_smooth)
{
  // check whether it already exists
  bool found_it = false;
  for (unsigned i = 0; i < other_responses_names_.size(); i++) {
    if (other_responses_names_[i].compare(response_name) == 0) {
      found_it = true; break;
    }
  }
  vil_image_view<float> out_gauss(img_f.ni(), img_f.nj());
  if (is_smooth)
    this->compute_gauss_response(img_f, out_gauss); 
  else
    out_gauss.deep_copy(img_f);
  other_responses_.push_back(out_gauss);
  other_responses_names_.push_back(response_name);

  vcl_string filterbank_dir = filter_folder + this->filter_dir_name();
  if (!vul_file::exists(filterbank_dir))
    vul_file::make_directory(filterbank_dir);

  vcl_string name = vul_file::strip_directory(img_name);
  name = vul_file::strip_extension(name);
  
  // first check filter folder if already computed
  vcl_string img_out_dir = filterbank_dir + "/" + name + "/";
  if (!vul_file::exists(img_out_dir))
    vul_file::make_directory(img_out_dir);
  
  this->save_other_filter_responses(img_out_dir);
}



// Used to define the initial k means cluster centers
// by random selection from the training data
vcl_vector<vnl_vector<double> > sdet_texture_classifier::
random_centers(vcl_vector<vnl_vector<double> > const& training_data,
               unsigned k) const{
  double n = training_data.size();
  vcl_vector<vnl_vector<double> > rand_centers(k);
  for (unsigned i = 0; i<k; ++i) {
    unsigned index = static_cast<unsigned>((n-1)*(vcl_rand()/(RAND_MAX+1.0)));
    rand_centers[i] = training_data[index];
  }
  return rand_centers;
}

// compute a vector of filter responses, which are sampled from the
// response pixels for the input image
bool sdet_texture_classifier::compute_training_data(vcl_string const& category)
{
  // dimension of filter bank
  unsigned dim = filter_responses_.n_levels();
  if (!dim) {
    vcl_cout << "zero dimensional filter bank\n" << vcl_flush;
    return false;
  }
  vul_timer t;
  // collect set of points
  unsigned maxr = this->max_filter_radius();
  vcl_vector<vnl_vector<double> > training_data, sampled_data;
  // assume all filter response images are the same size;
  int ni = filter_responses_.ni()-maxr;
  int nj = filter_responses_.nj()-maxr;
  if (ni<=0||nj<=0) {
    vcl_cout << "training image too small ni or nj <= " << maxr << '\n';
    return false;
  }
  //vcl_cout << " texton dimension: " << dim + 2 << '\n' << vcl_flush;
  unsigned dim_total = dim + 2 + other_responses_.size(); 
  vcl_cout << " texton dimension: " << dim_total << '\n' << vcl_flush;
  for (int j = maxr; j<nj; ++j)
    for (int i = maxr; i<ni; ++i) {
      //vnl_vector<double> tx(dim+2);
      vnl_vector<double> tx(dim_total);
      for (unsigned f = 0; f<dim; ++f)
        tx[f]=filter_responses_.response(f)(i,j);
      tx[dim]=laplace_(i,j); tx[dim+1]=gauss_(i,j);
      for (unsigned f = 0; f<other_responses_.size(); ++f)
        tx[dim+2+f]=(other_responses_[f])(i,j);
      training_data.push_back(tx);
    }
  // reduce the number of samples to specified size
  unsigned ns = training_data.size();
  if (ns>n_samples_) {
    for (unsigned i = 0; i<n_samples_; ++i) {
      unsigned s = static_cast<unsigned>((n_samples_-1)*(vcl_rand()/(RAND_MAX+1.0)));
      sampled_data.push_back(training_data[s]);
    }
    training_data.clear();
    training_data = sampled_data;
  }
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator dit;
  dit = training_data_.find(category);
  if (dit == training_data_.end()) {
    training_data_[category]=training_data;
  }
  else {
    training_data_[category].insert(training_data_[category].end(),
                                    training_data.begin(),
                                    training_data.end());
  }
  vcl_cout << "Collect texture samples in texture box region" << t.real()/1000.0 << " secs.\n";
  return true;
}
#include <vul/vul_file.h>
//: save filter responses
bool sdet_texture_classifier::save_filter_responses(vcl_string const& dir) 
{
  if (!filter_responses_.save_filter_responses(dir))
    return false;
  vcl_string path = dir + "/filter_response_gauss.tif";
  vil_save(gauss_, path.c_str());
  vcl_string path2 = dir + "/filter_response_laplace.tif";
  vil_save(laplace_, path2.c_str());
  return true;
}
bool sdet_texture_classifier::load_filter_responses(vcl_string const& dir) 
{
  if (!filter_responses_.load_filter_responses(dir, this->n_scales_))
    return false;
  vcl_string path = dir + "/filter_response_gauss.tif";
  gauss_ = vil_load(path.c_str());
  vcl_string path2 = dir + "/filter_response_laplace.tif";
  laplace_ = vil_load(path2.c_str());
  return true;
}

//: save filter responses
bool sdet_texture_classifier::save_other_filter_responses(vcl_string const& dir) 
{
  for (unsigned i = 0; i < other_responses_names_.size(); i++) {
    vcl_stringstream path; path << dir << "/other_response_" << other_responses_names_[i] << ".tif";
    vil_save(other_responses_[i], path.str().c_str());
  }
  return true;
}
bool sdet_texture_classifier::load_other_filter_responses(vcl_string const& dir) 
{
  vcl_string glob = dir + "/other_response_*.tif";
  for (vul_file_iterator fit = glob; fit; ++fit) {
    vcl_string name = fit();
    vil_image_view<float> img = vil_load(name.c_str());
    other_responses_.push_back(img);
    name = vul_file::strip_directory(name);
    name = vul_file::strip_extension(name);
    name = name.substr(15, name.size());
    other_responses_names_.push_back(name);
  }
  return true;
}

// compute a vector of filter responses, which are sampled from the
// response pixels for the input image. Only responses within the
// polygon are considered
bool sdet_texture_classifier::compute_training_data(vcl_string const& category,
                                                    vgl_polygon<double> const& texture_region)
{
  // dimension of filter bank
  unsigned dim = filter_responses_.n_levels();
  if (!dim) {
    vcl_cout << "zero dimensional filter bank\n" << vcl_flush;
    return false;
  }
  vul_timer t;
  // collect set of points
  unsigned maxr = this->max_filter_radius();
  vcl_vector<vnl_vector<double> > training_data, sampled_data;
  // assume all filter response images are the same size;
  int ni = filter_responses_.ni()-maxr;
  int nj = filter_responses_.nj()-maxr;
  if (ni<=0||nj<=0) {
    vcl_cout << "training image too small ni or nj <= " << maxr << '\n';
    return false;
  }
  //vcl_cout << " texton dimension: " << dim + 2  << '\n' << vcl_flush;
  unsigned dim_total = other_responses_.size() + dim + 2;
  vcl_cout << " texton dimension: " << dim_total  << '\n' << vcl_flush;
  vgl_polygon_scan_iterator<double> psi(texture_region);
  for (psi.reset(); psi.next(); ) {
    int j = psi.scany();
    for (int i  = psi.startx(); i <= psi.endx(); ++i) {
      //vnl_vector<double> tx(dim+2);
      vnl_vector<double> tx(dim_total);
      for (unsigned f = 0; f<dim; ++f)
        tx[f]=filter_responses_.response(f)(i,j);
      tx[dim]=laplace_(i,j); tx[dim+1]=gauss_(i,j);
      for (unsigned f = 0; f<other_responses_.size(); ++f)
        tx[dim+2+f]=(other_responses_[f])(i,j);
      training_data.push_back(tx);
    }
  }
  // reduce the number of samples to specified size
  unsigned ns = training_data.size();
  if (ns>n_samples_) {
    for (unsigned i = 0; i<n_samples_; ++i) {
      unsigned s = static_cast<unsigned>((n_samples_-1)*(vcl_rand()/(RAND_MAX+1.0)));
      sampled_data.push_back(training_data[s]);
    }
    training_data.clear();
    training_data = sampled_data;
  }
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator dit;
  dit = training_data_.find(category);
  if (dit == training_data_.end()) {
    training_data_[category]=training_data;
  }
  else {
    training_data_[category].insert(training_data_[category].end(),
                                    training_data.begin(),
                                    training_data.end());
  }
  vcl_cout << "Collect texture samples in texture polygon in " << t.real()/1000.0 << " secs.\n";
  return true;
}

// compute a vector of filter responses, which are sampled from the
// response pixels for the input image. Only responses within the
// the set of polygon regions are considered
bool   sdet_texture_classifier::
compute_training_data(vcl_string const& category,
                      vcl_vector<vgl_polygon<double> > const& texture_regions) {
  // dimension of filter bank
  unsigned dim = filter_responses_.n_levels();
  if (!dim) {
    vcl_cout << "zero dimensional filter bank\n" << vcl_flush;
    return false;
  }
  vul_timer t;
  //vcl_cout << " texton dimension: " << dim + 2 << '\n' << vcl_flush;

  // collect set of points
  unsigned maxr = this->max_filter_radius();
  vcl_vector<vnl_vector<double> > training_data, sampled_data;
  // assume all filter response images are the same size;
  int ni = filter_responses_.ni()-maxr;
  int nj = filter_responses_.nj()-maxr;
  if (ni<=0||nj<=0) {
    vcl_cout << "training image too small ni or nj <= " << maxr << '\n';
    return false;
  }
  unsigned dim_total = dim + 2 + other_responses_.size();
  vcl_cout << " texton dimension: " << dim_total << '\n' << vcl_flush;
  vcl_vector<vgl_polygon<double> >::const_iterator pit = texture_regions.begin();
  for (; pit != texture_regions.end(); ++pit) {
    vgl_polygon_scan_iterator<double> psi(*pit, false);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i  = psi.startx(); i <= psi.endx(); ++i) {
        if (i >= ni || j >= nj || i < maxr || j < maxr)
          continue;
        //vnl_vector<double> tx(dim+2);
        vnl_vector<double> tx(dim_total);
        for (unsigned f = 0; f<dim; ++f)
          tx[f]=filter_responses_.response(f)(i,j);
        double g = gauss_(i,j);
        tx[dim]=laplace_(i,j); tx[dim+1]=g;
        for (unsigned f = 0; f<other_responses_.size(); ++f)
          tx[dim+2+f]=(other_responses_[f])(i,j);
        training_data.push_back(tx);
      }
    }
  }
  // reduce the number of samples to specified size
  unsigned ns = training_data.size();
  if (ns>n_samples_) {
    for (unsigned i = 0; i<n_samples_; ++i) {
      unsigned s = static_cast<unsigned>((n_samples_-1)*(vcl_rand()/(RAND_MAX+1.0)));
      sampled_data.push_back(training_data[s]);
    }
    training_data.clear();
    training_data = sampled_data;
  }
  vcl_cout<< "Is category " << category << " in dictionary?\n" << vcl_flush;
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator dit;
  dit = training_data_.find(category);
  if (dit == training_data_.end()) {
    vcl_cout << " No, so adding new map entry with " << training_data.size()
             << " samples\n" << vcl_flush;
    training_data_[category]=training_data;
  }
  else {
    vcl_cout << "Yes, starting with " << training_data_[category].size()
             << " samples\n" << vcl_flush;
    training_data_[category].insert(training_data_[category].end(),
                                    training_data.begin(),
                                    training_data.end());
    vcl_cout << "after addition, training size is "
             << training_data_[category].size() << '\n' << vcl_flush;
  }
  vcl_cout << "Collect texture samples in texture polygon in " << t.real()/1000.0 << " secs.\n";
  return true;
}

//: extract trainig data for the pixels in the array
bool sdet_texture_classifier::compute_training_data(vcl_string const& category, vcl_vector<vcl_pair<int, int> >const& pixels)
{
  // dimension of filter bank
  unsigned dim = filter_responses_.n_levels();
  if (!dim) {
    vcl_cout << "zero dimensional filter bank\n" << vcl_flush;
    return false;
  }
  vul_timer t;
  //vcl_cout << " texton dimension: " << dim + 2 << '\n' << vcl_flush;
  unsigned dim_total = dim + 2 + other_responses_.size();
  vcl_cout << " texton dimension: " << dim_total << '\n' << vcl_flush;

  // collect set of points
  //unsigned maxr = this->max_filter_radius();
  vcl_vector<vnl_vector<double> > training_data, sampled_data;
  // assume all filter response images are the same size;
  int ni = filter_responses_.ni()-maxr_;
  int nj = filter_responses_.nj()-maxr_;
  if (ni<=0||nj<=0) {
    vcl_cout << "training image too small ni or nj <= " << maxr_ << '\n';
    return false;
  }
  for (unsigned kk = 0; kk < pixels.size(); kk++) {
    int ii = pixels[kk].first;
    int jj = pixels[kk].second;
    if (ii < 0 || jj < 0 || ii >= ni || jj >= nj) continue;  // invalid pixels are marked -1, -1 in the input
    //vnl_vector<double> tx(dim+2);
    vnl_vector<double> tx(dim_total);
    for (unsigned f = 0; f<dim; ++f)
      tx[f]=filter_responses_.response(f)(ii,jj);
    double g = gauss_(ii,jj);
    tx[dim]=laplace_(ii,jj); tx[dim+1]=g;
    for (unsigned f = 0; f<other_responses_.size(); ++f)
      tx[dim+2+f]=(other_responses_[f])(ii,jj);
    
    training_data.push_back(tx);
  }
  // reduce the number of samples to specified size
  unsigned ns = training_data.size();
  if (ns>n_samples_) {
    for (unsigned i = 0; i<n_samples_; ++i) {
      unsigned s = static_cast<unsigned>((n_samples_-1)*(vcl_rand()/(RAND_MAX+1.0)));
      sampled_data.push_back(training_data[s]);
    }
    training_data.clear();
    training_data = sampled_data;
  }

  this->add_training_data(category, training_data);
  return true;
}
//: extract filter outputs for the specified pixels 
bool sdet_texture_classifier::compute_data(vcl_vector<vcl_pair<int, int> >const& pixels, vcl_vector<vnl_vector<double> >& data)
{
  // dimension of filter bank
  unsigned dim = filter_responses_.n_levels();
  assert(dim != 0);

  // assume all filter response images are the same size;
  int ni = filter_responses_.ni()-maxr_;
  int nj = filter_responses_.nj()-maxr_;
  if (ni<=0||nj<=0) {
    vcl_cout << "training image too small ni or nj <= " << maxr_ << '\n';
    return false;
  }
  unsigned dim_total = dim + 2 + other_responses_.size();
  for (unsigned kk = 0; kk < pixels.size(); kk++) {
    int ii = pixels[kk].first;
    int jj = pixels[kk].second;
    if (ii < 0 || jj < 0 || ii >= ni || jj >= nj) continue;  // invalid pixels are marked -1, -1 in the input
    //vnl_vector<double> tx(dim+2);
    vnl_vector<double> tx(dim_total);
    for (unsigned f = 0; f<dim; ++f)
      tx[f]=filter_responses_.response(f)(ii,jj);
    double g = gauss_(ii,jj);
    tx[dim]=laplace_(ii,jj); tx[dim+1]=g;
    for (unsigned f = 0; f<other_responses_.size(); ++f)
      tx[dim+2+f]=(other_responses_[f])(ii,jj);
    data.push_back(tx);
  }
  return true;
}

bool sdet_texture_classifier::compute_training_data(vcl_string const& category,
                                                    vcl_string const& poly_path)
{
  vcl_vector<vgl_polygon<double> > polys;
  if (poly_path=="") {
    //create a polygon that is the bounding box of the image
    unsigned ni = laplace_.ni(), nj = laplace_.nj();
    vgl_polygon<double> temp; temp.new_sheet();
    temp.push_back(0.0, 0.0);
    temp.push_back(0.0, static_cast<double>(nj));
    temp.push_back(static_cast<double>(ni), static_cast<double>(nj));
    temp.push_back(static_cast<double>(ni), 0.0);
    polys.push_back(temp);
  }
  else {
   polys = load_polys(poly_path);
  }
  return this->compute_training_data(category, polys);
}

// execute the k means algorithm to form textons
// assumes that training data has been initialized
bool sdet_texture_classifier::compute_textons(vcl_string const& category)
{
  vul_timer t;
  // run k_means
  vcl_vector<vnl_vector<double> >& train_data = training_data_[category];
  vcl_cout << "Start k means for category " << category << " with sample size "
           << train_data.size() << '\n' << vcl_flush;
  vcl_vector<vnl_vector<double> > centers = random_centers(train_data, k_);

  unsigned converged_k = k_;
  t.mark();
  unsigned n_iter = bsta_k_means(train_data, converged_k, &centers);
  vcl_cout << "After " << n_iter << " iterations found " << converged_k
           << " cluster centers\n";
  texton_dictionary_[category]=centers;
  vcl_cout << "Compute k means in " << t.real()/1000.0 << " secs.\n";
  return true;
}
//: compute textons with k_means for all the categories with training data
void sdet_texture_classifier::compute_textons_all()
{
  for (vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator iter = training_data_.begin(); iter != training_data_.end(); iter++) 
    this->compute_textons(iter->first);
}

bool sdet_texture_classifier::
compute_textons(vcl_vector<vcl_string> const& image_paths,
                vcl_string const& category,
                vcl_vector<vcl_string> const& poly_paths)
{
  if (!image_paths.size()) {
    vcl_cout << " no images to compute textons\n";
    return false;
  }
  //load images and polygons
  vcl_vector<vil_image_view<float> > imgs;
  vcl_vector<vcl_vector< vgl_polygon<double> > >polys;
  unsigned i = 0;
  for (vcl_vector<vcl_string>::const_iterator pit = image_paths.begin();
       pit!=image_paths.end(); ++pit, ++i) {
    vil_image_resource_sptr resc = vil_load_image_resource((*pit).c_str());
    vil_image_view<float> view = scale_image(resc);
    imgs.push_back(view);
    unsigned ni = view.ni(), nj = view.nj();
    if (!poly_paths.size()||poly_paths[i]=="") {
      //create a polygon that is the bounding box of the image
      vgl_polygon<double> temp; temp.new_sheet();
      temp.push_back(0.0, 0.0);
      temp.push_back(0.0, static_cast<double>(nj));
      temp.push_back(static_cast<double>(ni), static_cast<double>(nj));
      temp.push_back(static_cast<double>(ni), 0.0);
      vcl_vector< vgl_polygon<double> > vplys;
      vplys.push_back(temp);
      polys.push_back(vplys);
    }
    else {
      vcl_vector<vgl_polygon<double> > vpys = load_polys(poly_paths[i]);
      polys.push_back(vpys);
    }
  }
  unsigned n = i;//number of images

  //compute bounding boxes
  for (unsigned i = 0; i<n; ++i) {
    double ni = imgs[i].ni(), nj = imgs[i].nj();
    vsol_box_2d_sptr box = new vsol_box_2d();
    vcl_vector<vgl_polygon<double> >& plist = polys[i];
    for (vcl_vector<vgl_polygon<double> >::iterator pit = plist.begin();
         pit != plist.end(); ++pit) {
      vcl_vector<vgl_point_2d<double> > sht = (*pit)[0];
      for (vcl_vector<vgl_point_2d<double> >::iterator xit = sht.begin();
           xit != sht.end(); ++xit)
        box->add_point(xit->x(), xit->y());
    }
    //expand box by filter radius
    double margin = this->max_filter_radius();
    double xmin = box->get_min_x()-margin, xmax = box->get_max_x()+margin;
    if (xmin<0) xmin = 0.0;  if (xmax>=ni) xmax = ni-1.0;
    double ymin = box->get_min_y()-margin, ymax = box->get_max_y()+margin;
    if (ymin<0) ymin = 0.0;  if (ymax>=nj) ymax = nj-1.0;
    unsigned i0=static_cast<unsigned>(xmin), j0 = static_cast<unsigned>(ymin);
    unsigned cni = static_cast<unsigned>(xmax-xmin+1.0);
    unsigned cnj = static_cast<unsigned>(ymax-ymin+1.0);
    //crop image
    vil_image_view<float> cview = vil_crop(imgs[i], i0, cni, j0, cnj);
    // shift polys to cropped coordinate system
    vcl_vector<vgl_polygon<double> > cplist;
    for (vcl_vector<vgl_polygon<double> >::iterator pit = plist.begin();
         pit != plist.end(); ++pit) {
      vgl_polygon<double> tpoly; tpoly.new_sheet();
      vcl_vector<vgl_point_2d<double> > sht = (*pit)[0];
      for (vcl_vector<vgl_point_2d<double> >::iterator xit = sht.begin();
           xit != sht.end(); ++xit) {
        double xp = xit->x()-xmin, yp = xit->y() - ymin;
        tpoly.push_back(xp, yp);
      }
      cplist.push_back(tpoly);
    }
    vcl_cout << "processing image(" << cview.ni() << ' ' << cview.nj() << ")\n" << vcl_flush;
    this->compute_filter_bank(cview);
    this->compute_training_data(category, cplist);
  }
  this->compute_textons(category);
  return true;
}

bool sdet_texture_classifier::save_dictionary(vcl_string const& path) const
{
  vsl_b_ofstream os(path.c_str());
  if (!os) {
    vcl_cout << "Can't open binary stream in save_dictionary\n";
    return false;
  }
  vcl_cout << "Save dictionary to " << path << '\n';
  sdet_texture_classifier_params const * tcp_ptr =
    dynamic_cast<sdet_texture_classifier_params const*>(this);
  vsl_b_write(os, *tcp_ptr);
  vsl_b_write(os, texton_dictionary_);
  vsl_b_write(os, category_histograms_);
  os.close();
  return true;
}

bool sdet_texture_classifier::load_dictionary(vcl_string const& path)
{
  vsl_b_ifstream is(path.c_str());
  if (!is) {
    vcl_cout << "Can't open binary stream in load_dictionary in " << path << vcl_endl;
    return false;
  }
  vcl_cout << "Loading texton dictionary: " << path << '\n' << vcl_flush;
  texton_dictionary_.clear();
  texton_index_.clear();
  category_histograms_.clear();
  sdet_texture_classifier_params* tcp_ptr
    = dynamic_cast<sdet_texture_classifier_params*>(this);
  vsl_b_read(is, *tcp_ptr);
  vsl_b_read(is, texton_dictionary_);
  this->compute_distances();
  this->compute_texton_index();
  vsl_b_read(is, category_histograms_);
  this->compute_texton_weights();
  this->compute_interclass_probs();
  is.close();
  return true;
}

//: save current training data, binary (includes classifier params at top of file)
bool sdet_texture_classifier::save_data(vcl_string const& path) const
{
  vsl_b_ofstream os(path.c_str());
  if (!os) {
    vcl_cout << "Can't open binary stream in save_dictionary\n";
    return false;
  }
  vcl_cout << "Save training data to " << path << '\n';
  sdet_texture_classifier_params const * tcp_ptr =
    dynamic_cast<sdet_texture_classifier_params const*>(this);
  vsl_b_write(os, *tcp_ptr);
  vsl_b_write(os, training_data_);
  os.close();
  return true;
}

int sdet_texture_classifier::data_size(vcl_string const& cat)
{
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator iter = training_data_.find(cat);
  if (iter == training_data_.end())
    return -1;
  else
    return (int)(iter->second.size());
}

//: load current training data, binary
bool sdet_texture_classifier::load_data(vcl_string const& path)
{
  vsl_b_ifstream is(path.c_str());
  if (!is) {
    vcl_cout << "Can't open binary stream in load_dictionary in " << path << vcl_endl;
    return false;
  }
  vcl_cout << "Loading training data in: " << path << '\n' << vcl_flush;
  training_data_.clear();
  sdet_texture_classifier_params* tcp_ptr
    = dynamic_cast<sdet_texture_classifier_params*>(this);
  vsl_b_read(is, *tcp_ptr);
  vsl_b_read(is, training_data_);
  is.close();

  // change the filterbank params
  filter_responses_ = brip_filter_bank(tcp_ptr->n_scales_,tcp_ptr->scale_interval_,tcp_ptr->lambda0_,tcp_ptr->lambda1_,tcp_ptr->angle_interval_,tcp_ptr->cutoff_per_);
  maxr_ = this->max_filter_radius(); 
  return true;
}

void sdet_texture_classifier::add_training_data(vcl_string const& category, vcl_vector<vnl_vector<double> >& training_data)
{
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator dit;
  dit = training_data_.find(category);
  if (dit == training_data_.end()) {
    training_data_[category]=training_data;
  }
  else {
    training_data_[category].insert(training_data_[category].end(),
                                    training_data.begin(),
                                    training_data.end());
  }
}
bool sdet_texture_classifier::get_training_data(vcl_string const& category, vcl_vector<vnl_vector<double> >& data)
{
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator dit;
  dit = training_data_.find(category);
  if (dit == training_data_.end()) {
    return false;
  }
  else {
    data.insert(data.end(), dit->second.begin(), dit->second.end());
    return true;
  }
}
//: return a list of category names for which training data is available
vcl_vector<vcl_string> sdet_texture_classifier::get_training_categories()
{
  vcl_vector<vcl_string> cats;
  for (vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator iter = training_data_.begin(); iter != training_data_.end(); iter++)
    cats.push_back(iter->first);
  return cats;
}
vcl_vector<vcl_string> sdet_texture_classifier::get_dictionary_categories()
{
  vcl_vector<vcl_string> cats;
  for (vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator iter = texton_dictionary_.begin(); iter != texton_dictionary_.end(); iter++)
    cats.push_back(iter->first);
  return cats;
}


void sdet_texture_classifier::print_dictionary() const
{
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::const_iterator it= texton_dictionary_.begin();
  for (; it!= texton_dictionary_.end(); ++it) {
    vcl_cout << " ===Category: " << (*it).first << "===\n";
    for (unsigned i = 0; i<(*it).second.size(); ++i) {
      unsigned dim = (*it).second[0].size();
      vcl_cout << "c[" << i << "]:(";
      for (unsigned f = 0; f<dim; ++f)
        vcl_cout << (*it).second[i][f] << ' ';
      vcl_cout << ")\n" << vcl_flush;
    }
  }
}

// compute the nearest distance between textons in different categories
// for the same category the distance is defined as the maximum distance
// between textons in the category
void sdet_texture_classifier::compute_distances()
{
  dist_.clear();
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::const_iterator jt= texton_dictionary_.begin();
  for (; jt!= texton_dictionary_.end(); ++jt) {
    vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::const_iterator it= texton_dictionary_.begin();
    for (; it!= texton_dictionary_.end(); ++it)
      if ((*it).first == (*jt).first) {
        //compute max distance between cluster centers within the category
        double max_dist = 0.0;
        for (unsigned j = 0; j<(*it).second.size(); ++j)
          for (unsigned i = 0; i<(*it).second.size(); ++i) {
            double d = vnl_vector_ssd((*it).second[i], (*jt).second[j]);
            //root mean square dist (RMS)
            d = vcl_sqrt(d/(*it).second[i].size());
            if (d>max_dist) max_dist = d;
          }
        dist_[(*jt).first][(*it).first]=max_dist;
      }
      else {
        //compute min distance between cluster centers in different categories
        double min_dist = vnl_numeric_traits<double>::maxval;
        for (unsigned j = 0; j<(*jt).second.size(); ++j)
          for (unsigned i = 0; i<(*it).second.size(); ++i) {
            double d = vnl_vector_ssd((*it).second[i],(*jt).second[j]);
            //root mean square dist (RMS)
            d = vcl_sqrt(d/(*it).second[i].size());
            if (d<min_dist) min_dist = d;
          }
        dist_[(*jt).first][(*it).first]= min_dist;
      }
  }
  distances_valid_ = true;
}

// compute the probability of a category, given the textons of itself or other
// categories. Provides a measure of the distinctiveness of a category.
// The texton histogram probabilities are multiplied by a weight factor that
// is based on how many categories in which a texton appears.
void sdet_texture_classifier::compute_interclass_probs()
{
  inter_prob_.clear();
  vcl_map< vcl_string, vcl_vector<float> >::const_iterator jt=
    category_histograms_.begin();
  for (; jt!= category_histograms_.end(); ++jt) {
    vcl_vector<float> const & histj = (*jt).second;
    unsigned n = histj.size();
    float prob_total = 0.0f;
    vcl_map<vcl_string, vcl_vector<float> >::const_iterator it=
      category_histograms_.begin();
    for (; it!= category_histograms_.end(); ++it) {
      float prob_sum = 0.0f;
      vcl_vector<float> const& histi = (*it).second;
      for (unsigned j = 0; j<n; ++j)
        if (histj[j]>0.0f && histi[j]>0.0f)
          prob_sum += texton_weights_[j]*histj[j];
      inter_prob_[(*jt).first][(*it).first] = prob_sum;
      prob_total += prob_sum;
    }
    it=category_histograms_.begin();
    for (; it!= category_histograms_.end(); ++it)
      inter_prob_[(*jt).first][(*it).first] /= prob_total;
  }
  inter_prob_valid_ = true;
}

// The weighting factor for textons based on
// probability of belonging to multiple categories.
// Here p = 1/Nc , where Nc is the number of categories in which
// a texton appears. The factor off accounts for the singularity
// of the log function and controls the rapidity of falloff in
// weight with Nc.
static float w(float p, float off)
{
  float t0 = -vcl_log(off), t1 = vcl_log(1.0f+off);
  float res = -vcl_log(1.0f - p + off) + t1;
  res /= (t0 + t1);
  return res;
}

// Assign a weighting factor to each texton. The weight is 1 if the
// texton appears in only one category and falls off as the number of
// categories that share the texton increase
void sdet_texture_classifier::compute_texton_weights()
{
  if (texton_index_valid_) this->compute_texton_index();
  unsigned n = texton_index_.size();
  unsigned m = category_histograms_.size();
  texton_weights_.resize(n);
  vcl_vector<vcl_vector<float> > cross_category_probs(n);
  for (unsigned i = 0; i<n; ++i)
    cross_category_probs[i] = vcl_vector<float>(m, 0.0f);

  vcl_map<vcl_string, vcl_vector<float> >::iterator hit = category_histograms_.begin();
  unsigned c = 0;//category index
  for (; hit!=category_histograms_.end(); ++hit, ++c) {
    vcl_vector<float> const& h = (*hit).second;
    for (unsigned i = 0; i<n; ++i)
      if (h[i]>0.0f)
        cross_category_probs[i][c] += 1.0f;
  }
  //normalize the category probabilities
  for (unsigned i = 0; i<n; ++i) {
    float sum = 0.0f;
    for (unsigned c = 0; c<m; ++c)
      sum += cross_category_probs[i][c];

    for (unsigned c = 0; c<m; ++c)
      cross_category_probs[i][c] /= sum;
  }
  for (unsigned i = 0; i<n; ++i) {
    float maxp = 0.0f;
    for (unsigned c = 0; c<m; ++c)
      if (cross_category_probs[i][c]>maxp)
        maxp = cross_category_probs[i][c];
    texton_weights_[i]=w(maxp, weight_offset_);
  }
  texton_weights_valid_ = true;
}

void sdet_texture_classifier::print_distances() const
{
  //don't really care if distance map is changed, since derived
  //from primary members that are const
  sdet_texture_classifier* tc = const_cast<sdet_texture_classifier*>(this);
  if (!distances_valid_) tc->compute_distances();

  vcl_cout << "Category distance matrix\n";
  vcl_map< vcl_string, vcl_map< vcl_string, double> >::const_iterator jt = dist_.begin();
  for (; jt != dist_.end(); ++jt) {
    vcl_cout << (*jt).first << " :\n";
    vcl_map< vcl_string, double>::const_iterator it = (*jt).second.begin();
    for (; it != (*jt).second.end(); ++it)
      vcl_cout << (*it).first << ":(" << (*it).second << ") ";
    vcl_cout << '\n';
  }
}

void sdet_texture_classifier::print_category_histograms() const
{
  unsigned nt = texton_index_.size();
  unsigned nh = category_histograms_.size();
  vcl_vector<vcl_string> cat_names(nh);
  vcl_vector<vcl_vector<float> > hists(nt, vcl_vector<float>(nh));
  vcl_map<vcl_string, vcl_vector<float> >::const_iterator hit = category_histograms_.begin();
  unsigned j = 0;
  for (; hit != category_histograms_.end(); ++hit, ++j) {
    cat_names[j]=(*hit).first;
    const vcl_vector<float>& h = (*hit).second;
    for (unsigned i = 0; i<h.size(); ++i)
      hists[i][j] = h[i];
  }
  for (unsigned j = 0; j<nh; ++j)
    vcl_cout << cat_names[j] << ' ';
  vcl_cout << '\n';
  for (unsigned i = 0; i<nt; ++i) {
    for (unsigned j = 0; j<nh; ++j)
      vcl_cout << hists[i][j] << ' ';
    vcl_cout << '\n';
  }
}

void sdet_texture_classifier::print_interclass_probs() const
{
  sdet_texture_classifier* ncthis = const_cast<sdet_texture_classifier*>(this);
  if (!inter_prob_valid_) ncthis->compute_interclass_probs();
  vcl_cout << "Interclass probabilities\n";
  vcl_map< vcl_string, vcl_map< vcl_string, double> >::const_iterator jt = inter_prob_.begin();
  for (; jt != inter_prob_.end(); ++jt) {
    vcl_cout << (*jt).first << " :\n";
    vcl_map< vcl_string, double>::const_iterator it = (*jt).second.begin();
    for (; it != (*jt).second.end(); ++it)
      vcl_cout << (*it).first << ":(" << (*it).second << ") ";
    vcl_cout << '\n';
  }
}

void sdet_texture_classifier::print_texton_weights() const
{
  sdet_texture_classifier* ncthis = const_cast<sdet_texture_classifier*>(this);
  if (!texton_weights_valid_) ncthis->compute_texton_weights();
  vcl_cout << "Texton weights ===>\n";
  for (vcl_vector<float>::const_iterator wit = texton_weights_.begin();
       wit != texton_weights_.end(); ++wit)
    vcl_cout << (*wit) << '\n';
}


// transfer the texton dictionary to an efficient index for sorting
// on Euclidean distance
void sdet_texture_classifier::compute_texton_index()
{
  texton_index_.clear();
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::const_iterator it= texton_dictionary_.begin();
  for (; it!= texton_dictionary_.end(); ++it) {
    vcl_string const& cat = (*it).first;
    vcl_vector<vnl_vector<double> > const & centers = (*it).second;
    for (vcl_vector<vnl_vector<double> >::const_iterator cit = centers.begin();
         cit != centers.end(); ++cit)
      texton_index_.push_back(sdet_neighbor(cat, (*cit)));
  }
}

unsigned sdet_texture_classifier::nearest_texton_index(vnl_vector<double> const& query)
{
  double min_dist = vnl_numeric_traits<double>::maxval;
  unsigned min_index = 0;
  unsigned n = texton_index_.size();
  for (unsigned i = 0; i<n; ++i) {
    double d = vnl_vector_ssd(texton_index_[i].k_mean_, query);
    if (d<min_dist) {
      min_dist = d;
      min_index = i;
    }
  }
  return min_index;
}

void sdet_texture_classifier::compute_category_histograms()
{
  // assumes that training_data and the texton index are valid
  // the index of the texton_index vector forms the bin space of
  // the category histogram
  if (!texton_index_valid_) this->compute_texton_index();
  unsigned n = texton_index_.size();
  if (!n) {
    vcl_cout << "no textons to compute category histograms\n";
    return;
  }
  else
    vcl_cout << "computing category histograms with " << n << " textons\n";
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > >::iterator dit = training_data_.begin();
  for (; dit != training_data_.end(); ++dit) {
    //histogram for the given category
    vcl_vector<float> hist(n, 0.0f);
    const vcl_string& cat = (*dit).first;
    const vcl_vector<vnl_vector<double> >& tdata = (*dit).second;
    unsigned ndata  = tdata.size();
    vcl_cout << "h[" << cat << "](" << ndata << ") "<< vcl_flush;
    float weight = 1.0f/static_cast<float>(ndata);
    //insert texton counts into the histogram
    for (vcl_vector<vnl_vector<double> >::const_iterator vit = tdata.begin();
         vit != tdata.end(); ++vit)
      this->update_hist(*vit, weight, hist);

    category_histograms_[cat]=hist;
  }
  vcl_cout << '\n';
}

void sdet_texture_classifier::
update_hist(vnl_vector<double> const& f, float weight, vcl_vector<float>& hist)
{
  unsigned indx = this->nearest_texton_index(f);
  hist[indx]+=weight;// for example, counts are normalized to probability
}
//: update the texton histogram with a vector of filter outputs, use the same weight for all the samples
void sdet_texture_classifier::update_hist(vcl_vector<vnl_vector<double> > const& f, float weight, vcl_vector<float>& hist)
{
  for (unsigned i = 0; i < f.size(); i++)
    this->update_hist(f[i], weight, hist);
}

vcl_map<vcl_string, float>  sdet_texture_classifier::
texture_probabilities(vcl_vector<float> const& hist)
{
  unsigned nt = texton_index_.size();
  vcl_map<vcl_string, float> probs;
  vcl_map<vcl_string, vcl_vector<float> >::iterator hit = category_histograms_.begin();

  for (; hit != category_histograms_.end(); ++hit) {
    const vcl_string& cat = (*hit).first;
    const vcl_vector<float>& hc = (*hit).second;
    float prob_sum = 0.0f;
    float np = 0.0f;
    for (unsigned i = 0; i<nt; ++i) {
      float w = texton_weights_[i];
      np += w;
      float vc = hc[i]*w, vh = hist[i]*w;
      prob_sum += (vc<=vh)?vc:vh;
    }
    prob_sum /= np;
    probs[cat] = prob_sum;
  }
  return probs;
}

//: return the similarity value for two histograms, this method assumes the texton dictionary is computed, there is a weight for each texton
//  the two input histograms are of the same size with the dictionary
float sdet_texture_classifier::prob_hist_intersection(vcl_vector<float> const& hist, vcl_vector<float> const& hc)
{
  unsigned nt = texton_index_.size();
  float prob_sum = 0.0f;
  float np = 0.0f;
  for (unsigned i = 0; i<nt; ++i) {
    /*float w = texton_weights_[i];
    np += w;
    float vc = hc[i]*w, vh = hist[i]*w;
    prob_sum += (vc<=vh)?vc:vh;*/
    float vc = hc[i], vh = hist[i];
    prob_sum += (vc<=vh)?vc:vh;
  }
  //prob_sum /= np;  
  return prob_sum;
}


//: get the class name and prob value with the highest probability for the given histogram
vcl_pair<vcl_string, float> sdet_texture_classifier::highest_prob_class(vcl_vector<float> const& hist)
{
  vcl_map<vcl_string, float> class_map = this->texture_probabilities(hist);
  vcl_map<vcl_string, float>::iterator iter_max = class_map.begin();
  for (vcl_map<vcl_string, float>::iterator iter = class_map.begin(); iter != class_map.end(); iter++) {
    if (iter->second > iter_max->second)
      iter_max = iter;
  }
  vcl_pair<vcl_string, float> r(iter_max->first, iter_max->second);
  return r;
}



void sdet_texture_classifier::
category_color_mix(vcl_map<vcl_string, float>  & probs,
                   vnl_vector_fixed<float, 3>& color_mix) {
  //start with max prob color
  vcl_map<vcl_string, vcl_vector<float> >::iterator hit = category_histograms_.begin();
  float prob_sum = 0.0f;
  vnl_vector_fixed<float, 3> mix(0.0f);
  vcl_string max_cat;
  for (; hit != category_histograms_.end(); ++hit) {
    const vcl_string& cat = (*hit).first;
    float p = probs[cat];
    prob_sum += p;
    mix += p*color_map_[cat];
  }
  color_mix = mix/prob_sum;
}


#if 0 //=====debug====
static bool required_block(int bidxu, int bidxv, int i,
                           int j, int block_size, int margin)
{
  int idxu = (i-margin)/block_size, idxv = (j-margin)/block_size;
  return bidxu == idxu && bidxv == idxv;
}
#endif // 0

vil_image_view<float> sdet_texture_classifier::
classify_image_blocks(vcl_string const& img_path)
{
  vul_timer t;
  vil_image_resource_sptr resc = vil_load_image_resource(img_path.c_str());
  vil_image_view<float> img = scale_image(resc);
  vcl_cout << "Classifying categories " << img_path << "\nsize(" << img.ni()
           << ' ' << img.nj() << ")pixels:[" << texton_dictionary_.size()
           << "]categories\n" << vcl_flush;
  if (!color_map_valid_)
    this->init_color_map();
  if (!texton_index_valid_)
    this->compute_texton_index();
  this->compute_filter_bank(img);
  unsigned dim = filter_responses_.n_levels();
  //vcl_cout << "texton dimension " << dim +2<< '\n';
  unsigned dim_total = dim + 2 + other_responses_.size();

  int margin = static_cast<int>(this->max_filter_radius());
  vcl_cout << "filter kernel margin " << margin << '\n';
  int ni = static_cast<int>(img.ni());
  int nj = static_cast<int>(img.nj());
  if ((ni-margin)<=0 || (nj-margin)<=0) {
    vcl_cout << "Image smaller than filter margin\n";
    return vil_image_view<float>(0, 0);
  }
  unsigned block_area = block_size_*block_size_;
  float weight = 1.0f/static_cast<float>(block_area);
  vil_image_view<float> prob(ni, nj, 3);
  prob.fill(0.5f);
  unsigned nh = texton_index_.size();
  int bidxv = 0;
  for (int j = margin; j<(nj-margin); j+=block_size_, ++bidxv) {
    int bidxu = 0;
    for (int i = margin; i<(ni-margin); i+=block_size_, ++bidxu) {
      vcl_vector<float> h(nh, 0.0f);
      for (unsigned r = 0; r<block_size_; ++r)
        for (unsigned c = 0; c<block_size_; ++c) {
          //vnl_vector<double> temp(dim+2);
          vnl_vector<double> temp(dim_total);
          for (unsigned f = 0; f<dim; ++f)
            temp[f]=filter_responses_.response(f)(i+c,j+r);
          temp[dim]=laplace_(i+c,j+r); temp[dim+1]=gauss_(i+c,j+r);
          for (unsigned f = 0; f<other_responses_.size(); ++f)
            temp[dim+2+f]=(other_responses_[f])(i+c,j+r);
          //hist bins are probabilities
          //i.e., sum h[i] = 1.0
          this->update_hist(temp, weight, h);
        }
      //finished a block
      vcl_map<vcl_string, float> texture_probs = this->texture_probabilities(h);

#if 0 //=====debug====
      int ii = 7518, jj = 2909;
      if (required_block(bidxu, bidxv, ii, jj, block_size_, margin)) {
        vcl_cout << "probs(" << i << ' ' << j << ")\n";
        float psum = 0.0;
        for (vcl_map<vcl_string, float>::iterator cit = texture_probs.begin();
             cit != texture_probs.end(); ++cit)
          psum += (*cit).second;

        for (vcl_map<vcl_string, float>::iterator cit =texture_probs.begin();
             cit != texture_probs.end(); ++cit)
          vcl_cout << (*cit).first << ' ' << ((*cit).second)/psum << '\n';
#ifdef DEBUG
        vcl_cout << " hist\n";
        for (unsigned i = 0; i<nh; ++i)
          vcl_cout << h[i]<< '\n';
#endif
      }
#endif
      vnl_vector_fixed<float, 3> color;
      //colorize output
      this->category_color_mix(texture_probs, color);
      for (unsigned r = 0; r<block_size_; ++r)
        for (unsigned c = 0; c<block_size_; ++c)
          for (unsigned b = 0; b<3; ++b)
            prob(i+c,j+r,b) = color[b];
    }
    vcl_cout << '.' << vcl_flush;
  }
  vcl_cout << "\nBlock classification took " << t.real()/1000.0 << " seconds\n" << vcl_flush;
  return prob;
}

unsigned sdet_texture_classifier::max_filter_radius()
{
  unsigned maxr = filter_responses_.invalid_border();
  unsigned lapr = gauss_radius(laplace_radius_, cutoff_per_);
  unsigned gr = gauss_radius(gauss_radius_, cutoff_per_);
  if (lapr>maxr) maxr = lapr;
  if (gr>maxr) maxr = gr;
  maxr_ = maxr;
  return maxr;
}

//: compute the texton of each pixel using the filter bank and/or other responses
void sdet_texture_classifier::compute_textons_of_pixels(vil_image_view<int>& texton_img)
{
  unsigned ni = this->filter_responses().ni();
  unsigned nj = this->filter_responses().nj();
  assert(ni != 0 && nj != 0);

  unsigned border = maxr_;
  vcl_cout << " sdet_texture_classifier::compute_textons_of_pixels() -- using border: " << border << " for an " << ni << " by " << nj << " image to compute a texton per pixel!\n";

  unsigned dim = filter_responses_.n_levels();
  assert(dim != 0);
  unsigned dim_total = dim + 2 + other_responses_.size();
  
  for (int i = border; i < ni-border; i++)
    for (int j = border; j < nj-border; j++) {
      vnl_vector<double> tx(dim_total);
      for (unsigned f = 0; f<dim; ++f)
        tx[f]=filter_responses_.response(f)(i,j);
      double g = gauss_(i,j);
      tx[dim]=laplace_(i,j); tx[dim+1]=g;
      for (unsigned f = 0; f<other_responses_.size(); ++f)
        tx[dim+2+f]=(other_responses_[f])(i,j);
      unsigned indx = this->nearest_texton_index(tx);
      texton_img(i,j) = indx;
    }
}

// === Binary I/O ===

//dummy vsl io functions to allow sdet_texture_classifier to be inserted into
//brdb as a dbvalue
void vsl_b_write(vsl_b_ostream & os, sdet_texture_classifier const &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream & is, sdet_texture_classifier &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream &os, const sdet_texture_classifier &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, sdet_texture_classifier* tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const sdet_texture_classifier* &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const sdet_texture_classifier* &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, sdet_texture_classifier_sptr& tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const sdet_texture_classifier_sptr &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const sdet_texture_classifier_sptr &tc)
{ /* do nothing */ }
