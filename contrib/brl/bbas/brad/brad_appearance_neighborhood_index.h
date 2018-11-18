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
#include <utility>
#include <vector>
#include <string>
#include <iosfwd>
#include <vgl/vgl_vector_3d.h>
#include <brad/brad_image_metadata.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#define BRAD_VTHR_LOW 0.20
#define BRAD_VTHR_HIGH 0.35
//#define BRAD_ITHR 0.03
#define BRAD_ITHR 0.15
class brad_appearance_neighborhood_index
{
 public:
 brad_appearance_neighborhood_index(): min_view_dir_thresh_(BRAD_VTHR_LOW), max_view_dir_thresh_(BRAD_VTHR_HIGH), illum_dir_thresh_(BRAD_ITHR), min_overlap_thresh_(0.5), num_illum_neighbors_thresh_(2){};

  //: the inputs are are a set of illumination and view dirs on the same index domain as the
  //  image set. That is, illumination_dir[i] corresponds to the direction for image[i]
 brad_appearance_neighborhood_index(std::vector<vgl_vector_3d<double > >  illumination_dirs,
                                    std::vector<vgl_vector_3d<double > >  view_dirs)
    : min_view_dir_thresh_(BRAD_VTHR_LOW)
    , max_view_dir_thresh_(BRAD_VTHR_HIGH)
    , illum_dir_thresh_(BRAD_ITHR)
    , min_overlap_thresh_(0.5)
    , num_illum_neighbors_thresh_(2)
    , illumination_dirs_(std::move(illumination_dirs))
    , view_dirs_(std::move(view_dirs))
    {this->compute_index();}

 brad_appearance_neighborhood_index(std::vector<vgl_vector_3d<double > >  illumination_dirs,
                                    std::vector<vgl_vector_3d<double > >  view_dirs,
                                    std::map<unsigned, std::map<unsigned, double> >  overlap)
    : min_view_dir_thresh_(BRAD_VTHR_LOW)
    , max_view_dir_thresh_(BRAD_VTHR_HIGH)
    , illum_dir_thresh_(BRAD_ITHR)
    , min_overlap_thresh_(0.5)
    , num_illum_neighbors_thresh_(2)
    , illumination_dirs_(std::move(illumination_dirs))
    , view_dirs_(std::move(view_dirs))
    , view_overlap_(std::move(overlap))
    {this->compute_index();}

  //: the input is a set of metadata from which the illumination and view directions can be extracted
  brad_appearance_neighborhood_index(std::vector<brad_image_metadata_sptr> const& metadata);

  brad_appearance_neighborhood_index(std::vector<brad_image_metadata_sptr> const& metadata,
                                     std::map<unsigned, std::map<unsigned, double> > const& overlap);

  //: the map describing the appearance clusters
  std::map<unsigned, std::vector<unsigned> > index() const {return index_;}
  const std::map<unsigned, std::vector<unsigned> >& const_index() const {return index_;}
  //: the appearance cluster for a specified index
  std::vector<unsigned> index(unsigned indx) const;
  void set_min_view_dir_thresh(double thresh){min_view_dir_thresh_ = thresh;}
  void set_max_view_dir_thresh(double thresh){max_view_dir_thresh_ = thresh;}
  void set_ill_dir_thresh(double thresh){illum_dir_thresh_ = thresh;}
  void set_min_overlap_thresh(double thresh){min_overlap_thresh_ = thresh;}
  void set_num_illum_neighbors_thresh(unsigned num){num_illum_neighbors_thresh_=num;}
  void set_illumination_dirs(std::vector<vgl_vector_3d<double > > const& illumination_dirs){illumination_dirs_ = illumination_dirs;}
  void set_view_dirs(std::vector<vgl_vector_3d<double > > const& view_dirs){view_dirs_ = view_dirs;}
  void set_view_overlap(std::map<unsigned, std::map<unsigned, double> > const& overlap){view_overlap_=overlap;}
  void compute_index();
  bool force_single_index(unsigned index);
  bool overlap_is_specified() const {return view_overlap_.size()> static_cast<std::size_t>(0);}
  unsigned n_dirs() const {return static_cast<unsigned>(view_dirs_.size());}
  unsigned most_nadir_view() const;
  std::vector<unsigned> views_in_nadir_order() const;
  vgl_vector_3d<double> view_dir(unsigned indx) const{return view_dirs_[indx];}
  //: display contents of index
  void print_index() const;
  void print_index_angles() const;
  void print_view_neighbors() const;
  void print_illum_neighbors() const;
  //: operator for sorting
  bool operator () (unsigned va, unsigned vb) const;
 private:
  double min_view_dir_thresh_;
  double max_view_dir_thresh_;
  double illum_dir_thresh_;
  double min_overlap_thresh_;
  unsigned num_illum_neighbors_thresh_;
  std::map<unsigned, std::vector<unsigned> > index_;
  std::vector<vgl_vector_3d<double> > illumination_dirs_;
  std::vector<vgl_vector_3d<double> > view_dirs_;
  std::map<unsigned, std::map<unsigned, double> > view_overlap_;
  std::map<unsigned, std::vector<unsigned> > valid_view_dir_map_;//view directions in valid range
  std::map<unsigned, std::vector<unsigned> > close_view_dir_map_;//close view directions
  std::map<unsigned, std::vector<unsigned> > close_target_dir_map_;//close view directions
  std::map<unsigned, std::vector<unsigned> > near_illum_dir_map_; //close illumination directions

  void fill_view_dir_maps();
  // note! the view direction neighbors must be computed before calling this method
  void fill_near_illum_dir_map();
};

#endif
