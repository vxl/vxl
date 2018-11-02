// This is gel/vifa/vifa_int_faces_attr.cxx
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include "vifa_int_faces_attr.h"
//:
// \file
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vtol/vtol_edge.h>
#include <vifa/vifa_incr_var.h>
#include <vifa/vifa_parallel.h>


// Establish the attribute vector for histogrammed attributes.  These
// are vifa_int_face_attr methods to get an attribute val for one face.  Each
// attribute type is histogrammed in this class.  Length must be
// NUM_HIST_ATTRIBUTES.
AttrFuncPtr vifa_int_faces_attr::
attr_get_funcs[] =
{
  &vifa_int_face_attr::IntMax,    // region intensity maxima
  &vifa_int_face_attr::IntMin,    // region intensity minima
  &vifa_int_face_attr::IntMean,    // region intensities
  &vifa_int_face_attr::IntVar,    // region intensity variances
  &vifa_int_face_attr::Area,      // region area
  &vifa_int_face_attr::AspectRatio,
  &vifa_int_face_attr::PerimeterLength,
  &vifa_int_face_attr::WeightedPerimeterLength,
  &vifa_int_face_attr::Complexity,
  &vifa_int_face_attr::WeightedComplexity,
  &vifa_int_face_attr_common::StrongParallelSal,
  &vifa_int_face_attr_common::WeakParallelSal,
  &vifa_int_face_attr::TwoPeakParallel,
  &vifa_int_face_attr::FourPeakParallel,
  &vifa_int_face_attr::EightyPercentParallel
};

const char* const vifa_int_faces_attr::
attr_names[] =
{
  "IntMax",
  "IntMin",
  "IntMean",
  "IntVar",
  "Area",
  "AspectRatio",
  "PerimeterLength",
  "WeightedPerimeterLength",
  "Complexity",
  "WeightedComplexity",
  "StrongParallel",
  "WeakParallel",
  "TwoPeakParallel",
  "FourPeakParallel",
  "EightyPercentParallel"
};

// Set some minimum values

float vifa_int_faces_attr::
attr_min_vals[] = {
  0.0039f,      // 1/255
  0.0039f,
  0.0039f,
  0.0001f,      //... not sure what the minimum variance should be
  1.0f,         // min area
  1.0f,         // min aspect ratio
  1.0f,         // min perimeter
  1.0f,         // min weighted perimeter
  1.0f,         // min complexity
  1.0f,         // min weighted complexity
  0.05f,        // min strong parallel
  0.05f,        // min weak parallel
  1e-5f,
  1e-5f,
  1e-5f
};

void vifa_int_faces_attr::
init()
{
  centroid_.reserve(2);
  centroid_[0] = -1;
  centroid_[1] = -1;
  perimeter_ = -1;
  weighted_perimeter_ = -1;
  // attr_map_ is allocated in ComputeSingleFaceAttributes.

  cached_2_parallel_ = -1;
  cached_4_parallel_ = -1;
  cached_80_parallel_ = -1;

  npobj_ = nullptr;

  attr_vec_.reserve(NumHistAttributes());
  for (int i=0; i < NumHistAttributes(); i++)
  {
    // can't do it below; garbage values trap in unref
    attr_vec_.push_back(nullptr);
  }

  perimeter_ = -1.f;
}

vifa_int_faces_attr::
vifa_int_faces_attr(vdgl_fit_lines_params*    fitter_params,
                    vifa_group_pgram_params*  gpp_s,
                    vifa_group_pgram_params*  gpp_w,
                    vifa_coll_lines_params*    cpp,
                    vifa_norm_params*      np,
                    vifa_int_face_attr_factory*  factory)
  : vifa_int_face_attr_common(fitter_params, gpp_s, gpp_w, cpp, np)
{
  this->init();
  factory_ = factory;
}

vifa_int_faces_attr::
vifa_int_faces_attr(iface_list&          v,
                    vdgl_fit_lines_params*      fitter_params,
                    vifa_group_pgram_params*    gpp_s,
                    vifa_group_pgram_params*    gpp_w,
                    vifa_coll_lines_params*     cpp,
                    vifa_norm_params*           np,
                    vifa_int_face_attr_factory* factory)
  : vifa_int_face_attr_common(fitter_params, gpp_s, gpp_w, cpp, np),
    faces_(v)
{
  this->init();
  factory_ = factory;
  attributes_valid_ = this->ComputeAttributes();
}

vifa_int_faces_attr::
~vifa_int_faces_attr()
{
  delete npobj_;
}

// ----------------------------------------------------------------
// Data access for non-attributes
// ---


float vifa_int_faces_attr::
CallAttrFunction(vifa_int_face_attr* seed_attr,int i)
{
  return (seed_attr->*(attr_get_funcs[i]))();
}

void vifa_int_faces_attr::
SetFaces(iface_list& v)
{
  // Save the new faces
  faces_ = v;

  // Reset everything
  delete npobj_;
  this->init();

  // Recompute the attributes
  attributes_valid_ = this->ComputeAttributes();
}

edge_2d_list& vifa_int_faces_attr::
GetEdges()
{
  // don't compute again if already there
  if (!edges_.empty())
    return edges_;

  if (faces_.empty())
  {
    std::cerr << "vifa_int_faces_attr::GetEdges: faces_ is not set\n";
    return edges_;
  }

  // Get edges from all faces, remove duplicates
  for (auto & face : faces_)
  {
    edge_list fedges; face->edges(fedges);
    edge_2d_iterator  edges_pos_;
    for (auto & fedge : fedges)
    {
      vtol_edge_2d*  e_ptr = fedge->cast_to_edge_2d();

      if (e_ptr)
      {
        vtol_edge_2d_sptr  e = vtol_edge_2d_sptr(e_ptr);

        edges_pos_ = std::find(edges_.begin(), edges_.end(), e);
        if (edges_pos_ == edges_.end())
          edges_.push_back(e);
      }
    }
  }

  return edges_;
}


// Compute centroid as the area-weighted mean of face centroids
// (center of mass).
void vifa_int_faces_attr::
ComputeCentroid()
{
  if ((centroid_[0] < 0) && !attr_map_.empty())
  {
    float  area_sum = 0;
    float  x_area_sum = 0;
    float  y_area_sum = 0;

    for (auto & ai : attr_map_)
    {
      float  area = ai->Area();
      area_sum += area;
      x_area_sum += area * ai->Xo();
      y_area_sum += area * ai->Yo();
    }

    if (!area_sum)
      return;

    centroid_[0] = x_area_sum / area_sum;
    centroid_[1] = y_area_sum / area_sum;
  }
}

// Compute X coord of centroid of all regions
float vifa_int_faces_attr::
Xo()
{
  if ((centroid_[0] < 0) && !attr_map_.empty())
    this->ComputeCentroid();

  return centroid_[0];
}

// Compute Y coord of centroid of all regions
float vifa_int_faces_attr::
Yo()
{
  if ((centroid_[1] < 0) && !attr_map_.empty())
    this->ComputeCentroid();

  return centroid_[1];
}

// ------------------------------------------------------------
// Attribute computation
//

// Compute single-face attributes (vifa_int_face_attr objects) for each
// individual face.
bool vifa_int_faces_attr::
ComputeSingleFaceAttributes(bool forceP)
{
  if (!forceP && attributes_valid_)
    return true;

  attr_map_.clear();
  for (auto & face : faces_)
  {
    vifa_int_face_attr_sptr fattr = factory_new_attr(face);

    if (!(fattr->valid_p()))
      return false;

    attr_map_.push_back(fattr);
  }

  return true;
}

bool vifa_int_faces_attr::
ComputeAttributes()
{
  if (!this->ComputeSingleFaceAttributes(true))
    attributes_valid_ = false;
  else
  {
    // Compute histogrammed attributes, including generation of
    // histograms
    for (int i = 0; i < NumHistAttributes(); i++)
      this->GetMeanAttr(i);

    attributes_valid_ = true;
  }

  return valid_p();
}

// Populate a vector containing all attributes, including inherited ones.
bool vifa_int_faces_attr::
GetAttributes(std::vector<float>& attrs)
{
  // No inherited attributes. The class qualifier is necessary to
  // prevent this call from going to children that define this method
  // also.
  return this->vifa_int_faces_attr::GetNativeAttributes(attrs);
}

// Populate a vector containing attributes native to this class (not
// inherited).
bool vifa_int_faces_attr::
GetNativeAttributes(std::vector<float>& attrs)
{
  if (!this->ComputeAttributes())
  {
    std::cerr << "Couldn't compute group attributes?\n";
    return false;
  }

  attrs.push_back(this->Area());
  attrs.push_back(this->PerimeterLength());
  attrs.push_back(this->WeightedPerimeterLength());
  attrs.push_back(this->Complexity());
  attrs.push_back(this->WeightedComplexity());
  attrs.push_back(this->StrongParallelSal());
  attrs.push_back(this->WeakParallelSal());
  attrs.push_back(this->TwoPeakParallel());
  attrs.push_back(this->FourPeakParallel());
  attrs.push_back(this->EightyPercentParallel());

  for (int i = 0; i < this->NumHistAttributes(); i++)
    attrs.push_back(this->GetMeanAttr(i));

  for (int i = 0; i < this->NumHistAttributes(); i++)
    attrs.push_back(this->GetSDAttr(i));

  return true;
}

// KEEP IN SYNC WITH GETNATIVEATTRIBUTES

void vifa_int_faces_attr::
GetAttributeNames(std::vector<std::string>& names)
{
  names.emplace_back("gArea");
  names.emplace_back("gPerimeterLength");
  names.emplace_back("gWeightedPerimeterLength");
  names.emplace_back("gComplexity");
  names.emplace_back("gWeightedComplexity");
  names.emplace_back("gStrongParallel");
  names.emplace_back("gWeakParallel");
  names.emplace_back("gTwoPeakParallel");
  names.emplace_back("gFourPeakParallel");
  names.emplace_back("gEightyPercentParallel");

  for (auto attr_name : attr_names)
  {
    std::string  name(attr_name);
    names.push_back("mean" + name);
  }

  for (auto attr_name : attr_names)
  {
    std::string  name(attr_name);
    names.push_back("sd" + name);
  }
}

const char* vifa_int_faces_attr::
GetBaseAttrName(int i)
{
  return vifa_int_faces_attr::attr_names[i];
}


// Create a histogram of attribute values from the supplied list of
// values.  This histogram must be deleted by the caller when done!
vifa_histogram_sptr vifa_int_faces_attr::
MakeAttrHist(std::vector<float>& attr_vals)
{
  this->ComputeSingleFaceAttributes(false);

  // Set the number of bins as sqrt(n), with a minimum of 20
  int    num_bins = std::max(20, (int)std::sqrt( static_cast<float>(attr_vals.size()) ));

  // Get value range
  float  max_val = 0;
  float  min_val = 1000000;
  for (float val : attr_vals)
  {
    if (val > max_val)
      max_val = val;

    if (val < min_val)
      min_val = val;
  }

  // Create empty histogram
  vifa_histogram_sptr val_hist = new vifa_histogram(num_bins,
                                                    min_val,
                                                    max_val);

  // Populate histogram
  for (float & attr_val : attr_vals)
    val_hist->UpCount(attr_val);

  return val_hist;
}

// Get or compute mean value of a vifa_int_face_attr attribute.  Computes
// histogram if necessary.  Not weighted by area.
float vifa_int_faces_attr::
GetMeanAttr(int attr_index)
{
  if (!attr_map_.empty())
  {
    if (!(attr_vec_[attr_index].ptr()))
    {
      attr_vec_[attr_index] = new vifa_incr_var;

      // Create list of attr vals and attr histogram
      std::vector<float>  vals(attr_map_.size());
      int          index = 0;
      for (auto ai = attr_map_.begin();
           ai != attr_map_.end(); ++ai, ++index)
      {
        vifa_int_face_attr_sptr  attr_ptr = *ai;
        vals[index] = CallAttrFunction(attr_ptr.ptr(), attr_index);
        attr_vec_[attr_index]->add_sample(vals[index]);
      }
    }

    return float(attr_vec_[attr_index]->get_mean());
  }
  else
  {
    // If no faces in neighborhood...
    return -1.f;
  }
}

// Get or compute standard deviation of a vifa_int_face_attr attribute.
// Computes histogram if necessary.  Not weighted by area.
float vifa_int_faces_attr::
GetSDAttr(int attr_index)
{
  if (!attr_map_.empty())
  {
    if (!attr_vec_[attr_index].ptr())
    {
      // Create attr histogram by side effect
      this->GetMeanAttr(attr_index);
    }

    return (float)std::sqrt(attr_vec_[attr_index]->get_var());
  }
  else
  {
    // If there are no faces in neighborhood...
    return -1;
  }
}

// Get or compute min value of a vifa_int_face_attr attribute.
// Computes histogram if necessary.  Not weighted by area.
float vifa_int_faces_attr::
GetMinAttr(int attr_index)
{
  if (!attr_map_.empty())
  {
    if (!attr_vec_[attr_index].ptr())
    {
      // Create attr histogram by side effect
      this->GetMeanAttr(attr_index);
    }

    return float(attr_vec_[attr_index]->get_min());
  }
  else
    return -1.f;
}

// Get or compute max value of a vifa_int_face_attr attribute.
// Computes histogram if necessary.  Not weighted by area.
float vifa_int_faces_attr::
GetMaxAttr(int attr_index)
{
  if (!attr_map_.empty())
  {
    if (!attr_vec_[attr_index].ptr())
    {
      // Create attr histogram by side effect
      this->GetMeanAttr(attr_index);
    }

    return float(attr_vec_[attr_index]->get_max());
  }
  else
    return -1.f;
}

// ---------------------------------------------------------------------
// Non-histogrammed attributes.  One value per vifa_int_faces_attr.

// area of all faces
float vifa_int_faces_attr::
Area()
{
  if (!attr_map_.empty())
  {
    area_ = 0;
    for (auto & ai : attr_map_)
      area_ += ai->Area();

    return area_;
  }
  else
    return 0.f;
}

// ratio of major moments of union of all faces
float vifa_int_faces_attr::
AspectRatio()
{
  return 0.f;
}

// a helper function
edge_list* vifa_int_faces_attr::
GetPerimeterEdges()
{
  auto*  p_edges = new edge_list;

  if (faces_.empty())
  {
    std::cerr << "no faces to calculate perimeter!\n";
    return p_edges;
  }

  // Maps edgeID to # of appearances
  std::map<int, int>      edge_count;
  std::map<int, int>::iterator  edge_count_pos;

  int  edge_index = 0;
  for (auto & face : faces_)
  {
    edge_list edges; face->edges(edges);

    for (auto e : edges)
    {
      int        e_id = e->get_id();

      if (e_id == 0)
      {
        e_id = ++edge_index;
        e->set_id(e_id);
      }

      int  count;
      edge_count_pos = edge_count.find(e_id);
      if (edge_count_pos == edge_count.end())
        count = 1;
      else
        count = edge_count_pos->second + 1;

      edge_count.insert(std::pair<int, int>(e_id, count));
    }
  }

  int  unique_count = 0;
  for (auto & face : faces_)
  {
    edge_list edges; face->edges(edges);
    for (auto e : edges)
    {
      int        count;

      edge_count_pos = edge_count.find(e->get_id());
      if (edge_count_pos == edge_count.end())
      {
        std::cerr << "Inconsistency in vifa_int_faces_attr::perimeter()?\n";
        continue;
      }
      else
        count = edge_count_pos->second;

      if (count == 1)
      {
        unique_count++;
        p_edges->push_back(e);
      }
    }
  }

  return p_edges;
}


// length of boundary of face group, in pixels.  Includes hole boundaries.
float vifa_int_faces_attr::
PerimeterLength()
{
  if (perimeter_ >= 0)
  {
    // Wary of "if ... == -1.0f"
    return perimeter_;
  }

  perimeter_ = 0;

  edge_list*  p_edges = this->GetPerimeterEdges();
  if (p_edges)
  {
    for (auto & p_edge : *p_edges)
    {
      vtol_edge_2d*  e = p_edge->cast_to_edge_2d();

      if (e)
        perimeter_ += float(e->curve()->length());
    }

    delete p_edges;
  }

  return perimeter_;
}

float vifa_int_faces_attr::
WeightedPerimeterLength()
{
  if (weighted_perimeter_ >= 0)
    return weighted_perimeter_;

//  std::cout << "In vifsa::WeightedPerimeterSum()...\n";

  weighted_perimeter_ = 0;

  edge_list*  p_edges = this->GetPerimeterEdges();
  if (p_edges)
  {
    // uh-oh, we've lost the face information for defining
    // "the other face"...

    float  weighted_perimeter_sum = 0;
    float  contrast_sum = 0;
    for (auto & p_edge : *p_edges)
    {
      vtol_edge_2d*  e = p_edge->cast_to_edge_2d();

      if (e)
      {
        face_list edge_faces; e->faces(edge_faces);
        iface_list  in_faces;
        iface_list  out_faces;

//        std::cout << edge_faces->size() << " faces found" << endl;

        for (auto & edge_face : edge_faces)
        {
          vtol_intensity_face*  int_f = edge_face->cast_to_intensity_face();

          if (!int_f)
          {
            std::cerr << "vifa_int_faces_attr::WeightedPerimeterLength() -"
                     << " Face is not an intensity face\n";
            continue;
          }

          bool in_face = false;
          for (auto & face : faces_)
          {
            if (*face == *int_f)
            {
              in_face = true;
              in_faces.push_back(int_f);
              break;
            }
          }

          if (!in_face)
            out_faces.push_back(int_f);
        }

//        std::cout << in_faces.size() << " in_faces, " << out_faces.size()
//                 << " out_faces\n";

        //  this might emphasize large, high-gradient faces too much...

        float  i_intensity_sum = 0;
        float  i_area_sum = 0;
        for (auto & in_face : in_faces)
        {
          i_intensity_sum += (in_face->Io() * in_face->Npix());
          i_area_sum += in_face->Npix();
        }

//        std::cout << "i_intensity_sum = " << i_intensity_sum
//                 << ", i_area_sum = " << i_area_sum << std::endl;

        float  i_intensity = (i_area_sum > 0) ?  i_intensity_sum / i_area_sum : 0.f;

        float  o_intensity_sum = 0;
        float  o_area_sum = 0;
        for (auto & out_face : out_faces)
        {
          o_intensity_sum += (out_face->Io() * out_face->Npix());
          o_area_sum += out_face->Npix();
        }

//        std::cout << "o_intensity_sum = " << o_intensity_sum
//                 << ", o_area_sum = " << o_area_sum << std::endl;

        float  o_intensity = (o_area_sum > 0) ?  o_intensity_sum / o_area_sum : 0.f;

        float  intensity_gradient = std::fabs(i_intensity - o_intensity);

//        std::cout << "intensity_gradient = " << intensity_gradient
//                 << ", curve length = " << e->curve()->length() << std::endl;

        weighted_perimeter_sum +=
          float(e->curve()->length()) * intensity_gradient;
        contrast_sum += intensity_gradient;
      }
      else
        std::cerr << "(*eit)->cast_to_edge_2d() returned NULL\n";
    }

//    std::cout << "weighted_perimeter_sum = " << weighted_perimeter_sum
//             << ", contrast_sum = " << contrast_sum << std::endl;

    weighted_perimeter_ = weighted_perimeter_sum / contrast_sum;
    delete p_edges;
  }

//  std::cout << "Leaving vifsa::WeightedPerimeterSum()\n";

  return weighted_perimeter_;
}

// edge length^2 / detection area
float vifa_int_faces_attr::
Complexity()
{
  if (this->Area() <= 0)
    return 0.f;

  float  p = this->PerimeterLength();
  return p * p / this->Area();
}


float vifa_int_faces_attr::
WeightedComplexity()
{
  if (this->Area() <= 0)
    return 0.f;

  float  wp = this->WeightedPerimeterLength();
  return wp * wp / this->Area();
}

void vifa_int_faces_attr::
SetNP()
{
  if (npobj_)
    npobj_->reset();
  else
    npobj_ = new vifa_parallel(faces_, true);
}

float vifa_int_faces_attr::
TwoPeakParallel()
{
  if (cached_2_parallel_ < 0)
  {
    SetNP();
    float  max_angle;
    float  std_dev;
    float  scale;

    for (int i=0; i<1; i++)
    {
      npobj_->map_gaussian(max_angle, std_dev, scale);
      npobj_->remove_gaussian(max_angle, std_dev, scale);
    }

    cached_2_parallel_ = npobj_->area();
  }

  return cached_2_parallel_;
}

float vifa_int_faces_attr::
FourPeakParallel()
{
  if (cached_4_parallel_ < 0)
  {
    SetNP();
    float  max_angle;
    float  std_dev;
    float  scale;

    for (int i=0; i<3; i++)
    {
      npobj_->map_gaussian(max_angle, std_dev, scale);
      npobj_->remove_gaussian(max_angle, std_dev, scale);
    }

    cached_4_parallel_ = npobj_->area();
  }

  return cached_4_parallel_;
}

float vifa_int_faces_attr::
EightyPercentParallel()
{
  if (cached_80_parallel_ < 0)
  {
    SetNP();

    for (int i=0; i < 20 && npobj_->area() > 0.3f; ++i)
    {
      float  max_angle, std_dev, scale;
      npobj_->map_gaussian(max_angle, std_dev, scale);
      npobj_->remove_gaussian(max_angle, std_dev, scale);
      cached_80_parallel_ = float(i);
    }
  }

  return cached_80_parallel_;
}

vifa_int_face_attr_sptr vifa_int_faces_attr::
factory_new_attr(vtol_intensity_face_sptr face)
{
  if (factory_)
    return factory_->obtain_int_face_attr(face,
                                          fitter_params_.ptr(),
                                          gpp_s_.ptr(),
                                          gpp_w_.ptr(),
                                          np_.ptr());
  else
    return new vifa_int_face_attr(face,
                                  fitter_params_.ptr(),
                                  gpp_s_.ptr(),
                                  gpp_w_.ptr(),
                                  np_.ptr());
}
