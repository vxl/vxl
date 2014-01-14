#ifndef sdet_texture_classifier_h_
#define sdet_texture_classifier_h_
//:
// \file
// \brief  A class for classifying texture regions
// \author J.L. Mundy
// \date   December 10, 2011
//
// Texture is classified using a texton dictionary. A texton is a
// k-means cluster center in a n-dimensional space of filter responses.
// The filters here are patterned after those proposed in the paper
// M. Varma and  A. Zisserman, "A Statistical Approach to Texture
// Classification from Single Images," International Journal of Computer
// Vision, Volume 62, Number 1--2, page 61--81, 2005
// In this class only 2nd order Gaussian derivatives are used for the
// anisotropic filters. The isotropic Laplace and Gaussian spot filters
// are the same as in the Varma and Zisserman work. Thus the filter space
// dimension is n_scales + 2, where n_scales is the number of scales for
// the anisotropic filters. The processing is carried out in two stages:
// 1) training - a set of texture classes is defined along with associated
//               training images. It is possible to specify a polygon or
//               multiple polygons to denote image regions corresponding to
//               the texture samples. The training images are processed to
//               produce a filter response vector at each training pixel.
//               The filter responses are randomly selected to form a
//               training set for each texture category. The training set
//               is clustered by the k-means algorithm and the resulting
//               cluster centers define the textons for the given category
//               The full set of textons define the texton dictionary.
//               Histograms of frequency of texton occurrence in each
//               training set for each category are formed. A texton histogram
//               bin is incremented if the training sample is nearest to
//               that texton. The ultimate outcome of training is the
//               set of texton histograms for each category. The bin
//               frequencies are decreased if the associated textons appear in
//               more than one category. This weighting is defined by the
//               member texton_weights_.
//
// 2) classification - A test image is decomposed into blocks and each block
//               is used to compute a texton histogram. The histogram is
//               compared with each category histogram and the probability
//               is defined by the sum of joint probabilities between the
//               test and training histograms. In this implementation the
//               joint probability is taken as the minimum of the probability
//               of corresponding bins. This approach is more invariant to
//               the large disparity in sample populations used to compute
//               the histograms, e.g. 400,000 in training vs. 4096 for
//               a test image block.

#include <sdet/sdet_texture_classifier_params.h>
#include <brip/brip_filter_bank.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_iosfwd.h>

struct sdet_neighbor
{
  sdet_neighbor(vcl_string const& category, vnl_vector<double> const& k_mean)
  : cat_(category), k_mean_(k_mean){}
  vcl_string cat_;
  vnl_vector<double> k_mean_;
};

class sdet_neighbor_less
{
 public:
  sdet_neighbor_less(vnl_vector<double> const& query): query_(query){}
  //the predicate function
  bool operator()(sdet_neighbor const& na, sdet_neighbor const& nb) const
  {
    double da = vnl_vector_ssd(na.k_mean_, query_),
      db = vnl_vector_ssd(nb.k_mean_, query_);
    return da < db;
  }
 private:
  vnl_vector<double> query_;
};

class sdet_texture_classifier : public sdet_texture_classifier_params,
                                public vbl_ref_count
{
 public:
  //: constructor from parameter block
  sdet_texture_classifier(sdet_texture_classifier_params const& params);

  //: compute filter responses for a given texture category training image
  bool compute_filter_bank(vil_image_view<float> const& img);
  
  //: check the folder if already computed using the image name, otherwise compute and save
  bool compute_filter_bank_color_img(vcl_string const& filter_folder, vcl_string const& img_name);

  //: check the folder if already computed using the image name, otherwise compute and save
  bool compute_filter_bank_float_img(vcl_string const& filter_folder, vcl_string const& img_name, float max_val = -1.0f);

  //: helper function to compute gauss response of an image using params of the instance, assumes the output image is properly initialized
  void compute_gauss_response(vil_image_view<float> const& img, vil_image_view<float>& out_gauss);

  //: the max image border width eaten up by filter kernels, also updates maxr_
  unsigned max_filter_radius();

  //: retrieve the filter responses
  brip_filter_bank& filter_responses()
    {return filter_responses_;}

  //: append to the vector of other_responses_
  //  it may be necessary to increase the dimensionality using another source of info than the original image
  // this method checks whether an other response with this name is already computed and saved
  // is_smooth options used to specify whether gauss smooth will apply on the input image
  void add_gauss_response(vil_image_view<float>& img_f, vcl_string const& filter_folder, vcl_string const& img_name, vcl_string const& response_name, bool const& is_smooth = true);

  //: append to training data (current filter responses).
  //  randomly select training samples from full training image for category
  bool compute_training_data(vcl_string const& category);
  //: randomly select training samples from within the specified region
  bool compute_training_data(vcl_string const& category,
                             vgl_polygon<double> const& texture_region);
  //: randomly select training samples from within the specified regions
  bool compute_training_data(vcl_string const& category,
                             vcl_vector<vgl_polygon<double> >const& texture_regions);
  //: randomly select training samples from within a region loaded from file
  bool compute_training_data(vcl_string const& category,
                             vcl_string const& poly_path);

  //: extract trainig data for the pixels in the array
  bool compute_training_data(vcl_string const& category, vcl_vector<vcl_pair<int, int> >const& pixels);
  //: extract filter outputs for the specified pixels 
  bool compute_data(vcl_vector<vcl_pair<int, int> >const& pixels, vcl_vector<vnl_vector<double> >& data);

  //: compute textons with k_means for the specified texture category
  bool compute_textons(vcl_string const& category);
  //: compute textons with k_means for all the categories with training data
  void compute_textons_all();

  unsigned get_number_of_textons() { return (unsigned)texton_index_.size(); }

  //: compute textons from set of images (and polygons).
  //  If polygon_paths is empty or some element contains a null string
  //  the entire image(s) is(are) used
  //  This method is the main driver function to carry out training
  bool compute_textons(vcl_vector<vcl_string> const& image_paths,
                       vcl_string const& category,
                       vcl_vector<vcl_string> const& poly_paths=
                       vcl_vector<vcl_string>());

  //: The texton histograms derived from the training data
  void compute_category_histograms();

  //: save texton dictionary, binary (includes classifier params at top of file)
  bool save_dictionary(vcl_string const& path) const;
  //: load dictionary, binary
  bool load_dictionary(vcl_string const& path);

  //: save current training data, binary (includes classifier params at top of file)
  bool save_data(vcl_string const& path) const;
  //: load current training data, binary
  bool load_data(vcl_string const& path);

  //: save filter responses
  bool save_filter_responses(vcl_string const& dir);
  bool load_filter_responses(vcl_string const& dir);

  bool save_other_filter_responses(vcl_string const& dir);
  bool load_other_filter_responses(vcl_string const& dir);

  int data_size(vcl_string const& cat);
  void add_training_data(vcl_string const& cat, vcl_vector<vnl_vector<double> >& data);
  bool get_training_data(vcl_string const& cat, vcl_vector<vnl_vector<double> >& data);
  
  //: clear all the training data for all categories
  void clear_training_data() { training_data_.clear(); }

  //: return a list of category names for which training data is available
  vcl_vector<vcl_string> get_training_categories();
  vcl_vector<vcl_string> get_dictionary_categories();

  //: set category colors
  void set_category_colors(vcl_map< vcl_string, vnl_vector_fixed<float, 3> > const& color_map)
  {color_map_ = color_map; color_map_valid_ = true;}
  //: image of category probabilities expressed as colors
  vil_image_view<float> classify_image_blocks(vcl_string const& img_path);

  //: print
  void print_dictionary() const;
  void print_distances() const;
  void print_color_map() const;
  void print_category_histograms() const;
  void print_interclass_probs() const;
  void print_texton_weights() const;

  // === testing utilities ===

  //: update the texton histogram with a vector of filter outputs, use the same weight for all the samples
  void update_hist(vcl_vector<vnl_vector<double> > const& f, float weight, vcl_vector<float>& hist);

  //: update the texton histogram with a filter vector
  void update_hist(vnl_vector<double> const& f, float weight, vcl_vector<float>& hist);

  //: get the class name and prob value with the highest probability for the given histogram
  vcl_pair<vcl_string, float> highest_prob_class(vcl_vector<float> const& hist);

  //: return the similarity value for two histograms, this method assumes the texton dictionary is computed, there is a weight for each texton
  //  the two input histograms are of the same size with the dictionary
  float prob_hist_intersection(vcl_vector<float> const& hist1, vcl_vector<float> const& hist2);

  //: concatanates the texton dictionaries of all the categories, so we can read the total number of textons in the dictionary
  void compute_texton_index();

  float laplace_response(unsigned i, unsigned j) { return laplace_(i,j); }
  float gauss_response(unsigned i, unsigned j) { return gauss_(i,j); }

  static vcl_vector<vgl_polygon<double> > load_polys(vcl_string const& poly_path);

  //: compute the texton of each pixel using the filter bank and/or other responses
  void compute_textons_of_pixels(vil_image_view<int>& texton_img);

  // ===  debug utilities ===

 protected:
  sdet_texture_classifier();
  vil_image_view<float> scale_image(vil_image_resource_sptr const& resc);
  vcl_vector<vnl_vector<double> >
    random_centers(vcl_vector<vnl_vector<double> > const& training_data,
                   unsigned k) const;
  void compute_distances();
  void compute_interclass_probs();
  void compute_texton_weights();

  void init_color_map();

  unsigned nearest_texton_index(vnl_vector<double> const& query);

  //: compute the vector of texture probabilities
  vcl_map<vcl_string, float> texture_probabilities(vcl_vector<float> const& hist);
  //: color representing the mix of texture probabilites
  void category_color_mix(vcl_map<vcl_string, float>& probs,
                          vnl_vector_fixed<float, 3>& color_mix);
  //: color representing the mix of texture probabilites with atomspheric quality (should be removed to another class)
  void category_quality_color_mix(vcl_map<vcl_string, float>& probs,
                                  vnl_vector_fixed<float, 3>& color_mix);
  // === members ===
  brip_filter_bank filter_responses_;
  vil_image_view<float> laplace_;
  vil_image_view<float> gauss_;
  vil_image_view<float> frac_counts_;
  // the training data for a given category
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > > training_data_;
  // the texton dictionary. The class is identified by a string name
  vcl_map< vcl_string, vcl_vector<vnl_vector<double> > > texton_dictionary_;
  // min distance between categories if different, max distance if the same
  vcl_map< vcl_string, vcl_map< vcl_string, double> > dist_;
  bool distances_valid_;
  // inter class texton probability
  vcl_map< vcl_string, vcl_map< vcl_string, double> > inter_prob_;
  bool inter_prob_valid_;
  // category color table
  vcl_map< vcl_string, vnl_vector_fixed<float, 3> > color_map_;
  bool color_map_valid_;
  vcl_vector<sdet_neighbor> texton_index_;
  bool texton_index_valid_;
  vcl_map<vcl_string, vcl_vector<float> > category_histograms_;
  vcl_vector<float> texton_weights_;
  bool texton_weights_valid_;
  unsigned maxr_;

  vcl_vector<vil_image_view<float> > other_responses_;
  vcl_vector<vcl_string > other_responses_names_;
};
#include <sdet/sdet_texture_classifier_sptr.h>
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, sdet_texture_classifier const &tc);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, sdet_texture_classifier &tc);

void vsl_print_summary(vcl_ostream &os, const sdet_texture_classifier &tc);

void vsl_b_read(vsl_b_istream& is, sdet_texture_classifier* tc);

void vsl_b_write(vsl_b_ostream& os, const sdet_texture_classifier* &tc);

void vsl_print_summary(vcl_ostream& os, const sdet_texture_classifier* &tc);

void vsl_b_read(vsl_b_istream& is, sdet_texture_classifier_sptr& tc);

void vsl_b_write(vsl_b_ostream& os, const sdet_texture_classifier_sptr &tc);

void vsl_print_summary(vcl_ostream& os, const sdet_texture_classifier_sptr &tc);
#endif // sdet_texture_classifier_h_
