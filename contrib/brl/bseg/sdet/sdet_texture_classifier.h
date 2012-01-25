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
// 1) training - a set of texture classes is defined along with associtated
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
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <brip/brip_filter_bank.h>
struct sdet_neighbor
{
  sdet_neighbor(vcl_string const& category, vnl_vector<double> const& k_mean):
    cat_(category), k_mean_(k_mean){}
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

class sdet_texture_classifier : public sdet_texture_classifier_params
{
 public:
  //: constructor from parameter block
  sdet_texture_classifier(sdet_texture_classifier_params const& params);

  //: compute filter responses for a given texture category training image
  bool compute_filter_bank(vil_image_view<float> const& img); 

  //: the max image border width eaten up by filter kernels
  unsigned max_filter_radius() const;

  //: retrieve the filter responses
  brip_filter_bank& filter_responses() 
    {return filter_responses_;}

  //: append to training data (current filter responses)
  bool compute_training_data(vcl_string const& category);
  bool compute_training_data(vcl_string const& category,
                             vgl_polygon<double> const& texture_region);
  bool compute_training_data(vcl_string const& category,
                             vcl_vector<vgl_polygon<double> >const& texture_regions);
  //: compute textons with k_means for the specified texture category
  bool compute_textons(vcl_string const& category);

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

  //: save dictionary, binary
  bool save_dictionary(vcl_string const& path) const;
  //: load dictionary, binary
  bool load_dictionary(vcl_string const& path);

  //: set category colors
  void set_category_colors(vcl_map< vcl_string, vnl_vector_fixed<float, 3> > const& color_map)
{color_map_ = color_map; color_map_valid_ = true;}
  //: image of category probabilities expressed as colors
  vil_image_view<float> classify_image_blocks(vcl_string const& img_path);
  //: image of atmospheric quality expressed as colors
  //: (should be done in a separate class - later JLM)
  vil_image_view<float> classify_image_blocks_qual(vcl_string const& img_path);
  vil_image_view<float> 
    classify_image_blocks_qual(vil_image_view<float> const& image);
  //: print 
  void print_dictionary() const;
  void print_distances() const;
  void print_color_map() const;
  void print_category_histograms() const;
  void print_interclass_probs() const;
  void print_texton_weights() const;
  //: debug utilities

  
 protected:
  sdet_texture_classifier();
  vcl_vector<vnl_vector<double> > 
    random_centers(vcl_vector<vnl_vector<double> > const& training_data,
                   unsigned k) const;
  void compute_distances();
  void compute_interclass_probs();
  void compute_texton_weights();
  double category_prob(vcl_string const& category,
                       vnl_vector<double> const& filt,
                       double sigma);
  void init_color_map();

  // note k in this case is k-nearest neighbor not k in k-means.
  void nearest_category(vnl_vector<double> const& query,
                        vnl_vector_fixed<float, 3>& color);
  void compute_texton_index();
  float single_category(vcl_string const& cat,vnl_vector<double> const& query,
                        double sigma, double& min_dist);
  unsigned nearest_texton_index(vnl_vector<double> const& query);

  //: update the texton histogram with a filter vector
  void update_hist(vnl_vector<double> const& f, float weight, 
                   vcl_vector<float>& hist); 
  //: compute the vector of texture probabilities
  vcl_map<vcl_string, float> texture_probabilities(vcl_vector<float> const& hist);
  //: color representing the mix of texture probabilites
  void category_color_mix(vcl_map<vcl_string, float>& probs,
                          vnl_vector_fixed<float, 3>& color_mix);
  //: color representing the mix of texture probabilites with atomspheric 
  // quality (should be removed to another class)
  void category_quality_color_mix(vcl_map<vcl_string, float>& probs,
                                  vnl_vector_fixed<float, 3>& color_mix);
  //members
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
  
};

#endif // sdet_texture_classifier_h_
