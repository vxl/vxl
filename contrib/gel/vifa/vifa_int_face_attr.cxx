// This is gel/vifa/vifa_int_face_attr.cxx
#include <vtol/vtol_intensity_face.h>
#include <vifa/vifa_int_face_attr.h>


vifa_int_face_attr::
vifa_int_face_attr(sdet_fit_lines_params*  fitter_params,
                   vifa_group_pgram_params*  gpp,
                   vifa_group_pgram_params*  gpp_w,
                   vifa_norm_params*    np) :
  vifa_int_face_attr_common(fitter_params, gpp, gpp_w, 0, np),
  face_(NULL),
  cached_min_(0.0),
  cached_max_(0.0),
  cached_mean_(0.0),
  cached_var_(0.0),
  npobj_(0)
{
}

vifa_int_face_attr::
vifa_int_face_attr(vtol_intensity_face_sptr f,
                   sdet_fit_lines_params*  fitter_params,
                   vifa_group_pgram_params*  gpp,
                   vifa_group_pgram_params*  gpp_w,
                   vifa_norm_params*    np) :
  vifa_int_face_attr_common(fitter_params, gpp, gpp_w, 0, np),
  face_(f),
  cached_min_(0.0),
  cached_max_(0.0),
  cached_mean_(0.0),
  cached_var_(0.0),
  cached_2_parallel_(-1),
  cached_4_parallel_(-1),
  cached_80_parallel_(-1),
  npobj_(0)
{
  attributes_valid_ = this->ComputeAttributes();
}

vifa_int_face_attr::
~vifa_int_face_attr()
{
  delete npobj_;
}


// ------------------------------------------------------------
// Data access & computation for non-attributes
//

void vifa_int_face_attr::
SetFace(vtol_intensity_face_sptr  f)
{
  face_ = f;
  delete npobj_;
  npobj_ = 0;
  attributes_valid_ = this->ComputeAttributes();
}

edge_2d_list& vifa_int_face_attr::
GetEdges()
{
  // Don't compute again if already there
  if (!edges_.empty())
    return edges_;

  if (!face_)
  {
    vcl_cerr << "vifa_int_face_attr::GetFaces(): face_ is not set\n";
    return edges_;
  }

  edge_list*  fedges = face_->edges();
  if (fedges)
  {
    for (edge_iterator eli = fedges->begin(); eli != fedges->end(); eli++)
    {
      vtol_edge_2d_sptr  e = (*eli)->cast_to_edge_2d();

      if (e)
        edges_.push_back(e);
    }

    delete fedges;
  }

  return edges_;
}


// ------------------------------------------------------------
// Public functional methods
//

bool vifa_int_face_attr::
ComputeAttributes()
{
  ComputeCacheValues();
  attributes_valid_ = true;
  return this->valid_p();
}

// Populate a vector containing all attributes, including inherited
// ones.
bool vifa_int_face_attr::
GetAttributes(vcl_vector<float>&  attrs)
{
  // no inherited attributes
  return this->vifa_int_face_attr::GetNativeAttributes(attrs);
}

// Append the names of the attributes in the same order as in
// GetNativeAttributes.  KEEP THEM IN SYNC!
void vifa_int_face_attr::
GetAttributeNames(vcl_vector<vcl_string>&  names)
{
  names.push_back("IntMax");
  names.push_back("IntMin");
  names.push_back("IntMean");
  names.push_back("IntVar");
  names.push_back("Area");
  names.push_back("AspectRatio");
  names.push_back("PerimeterLength");
  names.push_back("WeightedPerimeterLength");
  names.push_back("Complexity");
  names.push_back("WeightedComplexity");
  names.push_back("StrongParallel");
  names.push_back("WeakParallel");
  names.push_back("TwoPeakParallel");
  names.push_back("FourPeakParallel");
  names.push_back("EightyPercentParallel");
}

// Populate a vector containing attributes native to this class (not
// inherited).  SYNC ADDITIONS TO THIS WITH THE NAMES FUNCTION ABOVE!
bool vifa_int_face_attr::
GetNativeAttributes(vcl_vector<float>& attrs)
{
  if (!this->ComputeAttributes())
  {
    vcl_cerr << "Couldn't compute single face attributes?\n";
    return false;
  }

  attrs.push_back(this->IntMax());
  attrs.push_back(this->IntMin());
  attrs.push_back(this->IntMean());
  attrs.push_back(this->IntVar());
  attrs.push_back(this->Area());
  attrs.push_back(this->AspectRatio());
  attrs.push_back(this->PerimeterLength());
  attrs.push_back(this->WeightedPerimeterLength());
  attrs.push_back(this->Complexity());
  attrs.push_back(this->WeightedComplexity());
  attrs.push_back(this->StrongParallelSal());
  attrs.push_back(this->WeakParallelSal());
  attrs.push_back(this->TwoPeakParallel());
  attrs.push_back(this->FourPeakParallel());
  attrs.push_back(this->EightyPercentParallel());
  return true;
}

// ------------------------------------------------------------
// Individual attribute computation
//

float vifa_int_face_attr::
AspectRatio()
{
  if (aspect_ratio_ < 0)
    aspect_ratio_ = face_->AspectRatio();

  return aspect_ratio_;
}

float vifa_int_face_attr::
PerimeterLength()
{
  if (peri_length_ < 0)
    peri_length_ = face_->perimeter();

  return peri_length_;
}

float vifa_int_face_attr::
WeightedPerimeterLength()
{
  if (weighted_peri_length_ < 0)
  {
    // Block-copied from intensity face rather than undergo the pain
    // of adding a "weighted" boolean parameter
    edge_list*  edges = face_->edges();
    float    p = 0.0;
    float    intensity_sum = 1.0;

    if (edges)
    {
      for (edge_iterator eit = edges->begin(); eit != edges->end(); eit++)
      {
        vtol_edge_2d_sptr  e = (*eit)->cast_to_edge_2d();

        if (e)
        {
          // Leave at default of 1.0 if no adjacent face
          float  int_grad =
            get_contrast_across_edge(e->cast_to_edge(), 1.0);

          p += e->curve()->length() * int_grad;
          intensity_sum += int_grad;
        }
      }

      // Clean up returned edge list
      delete edges;
    }

    weighted_peri_length_ = p / intensity_sum;
  }

  return weighted_peri_length_;
}

float vifa_int_face_attr::
Complexity()
{
  float  area = this->Area();
  float  len = this->PerimeterLength();

  if ((complexity_ < 0) && (len >= 0) && (area >= 0))
  {
    complexity_ = len * len / area;
  }

  return complexity_;
}

float vifa_int_face_attr::
WeightedComplexity()
{
  float  area = this->Area();
  float  len = this->WeightedPerimeterLength();

  if (weighted_complexity_ < 0 && len >= 0 && area >= 0)
    weighted_complexity_ = len * len / area;

  return weighted_complexity_;
}

float vifa_int_face_attr::
TwoPeakParallel()
{
  if (cached_2_parallel_ < 0)
  {
    SetNP();

    float  max_angle;
    float  std_dev;
    float  scale;
    for (int i = 0; i < 1; i++)
    {
      npobj_->map_gaussian(max_angle, std_dev, scale);
      npobj_->remove_gaussian(max_angle, std_dev, scale);
    }

    cached_2_parallel_ = npobj_->area();
  }

  return cached_2_parallel_;
}

float vifa_int_face_attr::
FourPeakParallel()
{
  if (cached_4_parallel_ < 0)
  {
    SetNP();

    float  max_angle;
    float  std_dev;
    float  scale;
    for (int i = 0; i < 3; i++)
    {
      npobj_->map_gaussian(max_angle, std_dev, scale);
      npobj_->remove_gaussian(max_angle, std_dev, scale);
    }

    cached_4_parallel_ = npobj_->area();
  }

  return cached_4_parallel_;
}

float vifa_int_face_attr::
EightyPercentParallel()
{
  if (cached_80_parallel_ < 0)
  {
    SetNP();

    for (int i=0; i < 20 && npobj_->area() > 0.3; ++i)
    {
      float  max_angle, std_dev, scale;
      npobj_->map_gaussian(max_angle, std_dev, scale);
      npobj_->remove_gaussian(max_angle, std_dev, scale);
      cached_80_parallel_ = i;
    }
  }

  return float(cached_80_parallel_);
}

void vifa_int_face_attr::
ComputeCacheValues()
{
  float  real_min = face_->get_min();
  float  real_max = face_->get_max();

  // Sanity check -- can happen if face has 0 pixels...
  if (real_min > real_max)
  {
    real_min = 0.0;
    real_max = 1.0;  // Superstitous, probably could also be zero?
  }

  int        max_bins = int(real_max - real_min + 1);
  int        nbins = (face_->Npix() > max_bins) ? max_bins : face_->Npix();
  vifa_histogram  intensity_hist(nbins, real_min, real_max);

  for (int i = 0; i < face_->Npix(); i++)
  {
    float  pval = (face_->Ij())[i];
    intensity_hist.UpCount(pval);
  }

#if 0
  vcl_cout << "vifa::ComputeCacheValues(): start dump:\n";
  intensity_hist.Print();
  vcl_cout << "LowClipVal: " << intensity_hist.LowClipVal(0.1) << vcl_endl
           << "HighClipVal: " << intensity_hist.LowClipVal(0.9) << vcl_endl
           << "face_->Io(): " << face_->Io() << vcl_endl
           << "face_->Npix(): " << face_->Npix() << vcl_endl;
  np_->print_info();
  vcl_cout << "vifa::ComputeCacheValues(): end dump\n";
#endif

  cached_min_ = normalize_intensity(intensity_hist.LowClipVal(0.1f));
  cached_max_ = normalize_intensity(intensity_hist.LowClipVal(0.9f));
  cached_mean_ = normalize_intensity(face_->Io());

  float          sum = 0.0;
  const unsigned short*  pvals = face_->Ij();

  for (int i = 0; i < face_->Npix(); i++)
  {
    float  mapped_pval = normalize_intensity(pvals[i]);
    float  delta = mapped_pval - cached_mean_;
    sum += delta * delta;
  }

  cached_var_ = sum / face_->Npix();
}

void vifa_int_face_attr::
SetNP()
{
  if (npobj_)
    npobj_->reset();
  else
  {
    vcl_vector<vtol_intensity_face_sptr>  faces;
    faces.push_back(face_);

    const bool  contrast = true;
    npobj_ = new vifa_parallel(faces, contrast);
  }
}
