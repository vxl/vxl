#ifndef __BRAD_APPEARANCE_NEIGHBORHOOD_INDEX_H
#define __BRAD_APPEARANCE_NEIGHBORHOOD_INDEX_H
//-----------------------------------------------------------------------------
//:
// \file
// \brief An index for images that have similar appearance to the target image
//
// The input is a set of images with raidiometric and viewpoint metadata
// The output is a set of image clusters, where a target image is associated
// with other images in the input set based on similar surface appearance.
// The appearance similarity model is based on a
// bidirectional reflectance distribution function (BRDF) and surface geometry
// distribution assumed to hold for the scene. For example the BRDF could be 
// the Lambertian model with an assumption of purely horizontal surfaces.
//
// \author J. L. Mundy
// \date March 25, 2016
//
//----------------------------------------------------------------------------
#include <map>
#include <vector>
#include <string>
#include <iosfwd>
#include <vgl/vgl_vector_3d.h>
#include <brad/brad_image_metadata.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#define BRAD_VTHR 0.05
#define BRAD_ITHR 0.2
class brad_appearance_neighborhood_index
{
 public:
 brad_appearance_neighborhood_index(): max_view_dir_thresh_(BRAD_VTHR), max_illum_dir_thresh_(BRAD_ITHR), min_overlap_thresh_(0.5), num_illum_neighbors_thresh_(2){};

  //: the inputs are are a set of illumination and view dirs on the same index domain as the 
  //  image set. That is, illumination_dir[i] corresponds to the direction for image[i]
 brad_appearance_neighborhood_index(std::vector<vgl_vector_3d<double > > const& illumination_dirs,
                                    std::vector<vgl_vector_3d<double > > const& view_dirs): illumination_dirs_(illumination_dirs),
    view_dirs_(view_dirs),max_view_dir_thresh_(BRAD_VTHR), max_illum_dir_thresh_(BRAD_ITHR), min_overlap_thresh_(0.5), num_illum_neighbors_thresh_(2)
    {this->compute_index();}

 brad_appearance_neighborhood_index(std::vector<vgl_vector_3d<double > > const& illumination_dirs,
                                    std::vector<vgl_vector_3d<double > > const& view_dirs,
                                    std::map<unsigned, std::map<unsigned, double> > const& overlap): illumination_dirs_(illumination_dirs),
    view_dirs_(view_dirs), view_overlap_(overlap), max_view_dir_thresh_(BRAD_VTHR), max_illum_dir_thresh_(BRAD_ITHR), min_overlap_thresh_(0.5), num_illum_neighbors_thresh_(2)
    {this->compute_index();}

  //: the input is a set of metadata from which the illumination and view directions can be extracted
  brad_appearance_neighborhood_index(std::vector<brad_image_metadata_sptr> const& metadata);

  brad_appearance_neighborhood_index(std::vector<brad_image_metadata_sptr> const& metadata,
                                     std::map<unsigned, std::map<unsigned, double> > const& overlap);

  //: the map describing the appearance clusters
  std::map<unsigned, std::vector<unsigned> > index() const {return index_;}

  void set_max_view_dir_thresh(double thresh){max_view_dir_thresh_ = thresh;}
  void set_max_ill_dir_thresh(double thresh){max_illum_dir_thresh_ = thresh;}
  void set_min_overlap_thresh(double thresh){min_overlap_thresh_ = thresh;}
  void set_num_illum_neighbors_thresh(unsigned num){num_illum_neighbors_thresh_=num;}

  void set_illumination_dirs(std::vector<vgl_vector_3d<double > > const& illumination_dirs){illumination_dirs_ = illumination_dirs;}
  void set_view_dirs(std::vector<vgl_vector_3d<double > > const& view_dirs){view_dirs_ = view_dirs;}
  void set_view_overlap(std::map<unsigned, std::map<unsigned, double> > const& overlap){view_overlap_=overlap;}
  void compute_index();
  bool overlap_is_specified() const {return view_overlap_.size()> static_cast<std::size_t>(0);}
  unsigned n_dirs() const {return static_cast<unsigned>(view_dirs_.size());}
  //: display contents of index  
  void print_index() const;
  void print_index_angles() const;
  void print_view_neighbors() const;
  void print_illum_neighbors() const;
  // for testing radiometric consistency
  void set_images(std::vector<vil_image_view<float> > const& imgs){imgs_ = imgs;}
  void set_cams(std::vector<vpgl_camera_double_sptr> const& cams){ cams_ = cams;}
  void print_intensities(vgl_point_3d<double> const& p) const;

  private:
  double max_view_dir_thresh_;
  double max_illum_dir_thresh_;
  double min_overlap_thresh_;
  unsigned num_illum_neighbors_thresh_;
  std::map<unsigned, std::vector<unsigned> > index_;
  std::vector<vgl_vector_3d<double> > illumination_dirs_;
  std::vector<vgl_vector_3d<double> > view_dirs_;
  std::map<unsigned, std::map<unsigned, double> > view_overlap_;
  std::map<unsigned, std::vector<unsigned> > near_view_dir_map_;//close view directions
  std::map<unsigned, std::vector<unsigned> > near_illum_dir_map_; //close illumination directions 
  // for testing purposes
  std::vector<vil_image_view<float> > imgs_;
  std::vector<vpgl_camera_double_sptr> cams_;
  //unsigned random_index(unsigned n) const; //replaced by random shuffle
  void fill_near_view_dir_map();
  // note! the view direction neighbors must be computed before calling this method
  void fill_near_illum_dir_map();
  bool pixel_intensity(vil_image_view<float> const& img, vpgl_camera_double_sptr const& cam, vgl_point_3d<double> p, float& I) const;
};


#endif
