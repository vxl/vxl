// This is gel/vifa/vifa_int_faces_adj_attr.cxx
#include "vifa_int_faces_adj_attr.h"

vifa_int_faces_adj_attr::
vifa_int_faces_adj_attr(void)
  : vifa_int_faces_attr(),
    closure_valid_(false),
    seed_(NULL),
    seed_attr_(NULL),
    depth_(BAD_DEPTH)
{
  init();
  ComputeAttributes();
}

vifa_int_faces_adj_attr::
vifa_int_faces_adj_attr(vtol_intensity_face_sptr     seed,
                        int                          depth,
                        int                          size_filter,
                        vdgl_fit_lines_params*       fitter_params,
                        vifa_group_pgram_params*     gpp_s,
                        vifa_group_pgram_params*     gpp_w,
                        vifa_coll_lines_params*      cpp,
                        vifa_norm_params*            np,
                        vifa_int_face_attr_factory*  factory,
                        float                        junk_area_percentage)
  : vifa_int_faces_attr(fitter_params, gpp_s, gpp_w, cpp, np, factory),
    closure_valid_(false),
    seed_(seed),
    seed_attr_(NULL),
    depth_(depth),
    size_filter_(size_filter),
    junk_area_percentage_(junk_area_percentage),
    junk_count_(0)
{
  init();
  ComputeAttributes();
}

vifa_int_faces_adj_attr::
vifa_int_faces_adj_attr(vtol_intensity_face_sptr     seed,
                        int                          depth,
                        iface_list&                  neighborhood,
                        int                          size_filter,
                        vdgl_fit_lines_params*       fitter_params,
                        vifa_group_pgram_params*     gpp_s,
                        vifa_group_pgram_params*     gpp_w,
                        vifa_coll_lines_params*      cpp,
                        vifa_norm_params*            np,
                        vifa_int_face_attr_factory*  factory,
                        float                        junk_area_percentage)
  : vifa_int_faces_attr(neighborhood, fitter_params, gpp_s, gpp_w, cpp, np, factory),
    closure_valid_(false), // still need to filter on size!
    seed_(seed),
    seed_attr_(NULL),
    depth_(depth),
    size_filter_(size_filter),
    junk_area_percentage_(junk_area_percentage),
    junk_count_(0)
{
  init();
  ComputeAttributes();
}

void vifa_int_faces_adj_attr::
SetSeed(vtol_intensity_face_sptr  seed)
{
  seed_ = seed;
  closure_valid_ = false;
  seed_attr_ = factory_new_attr(seed_);
}

iface_list& vifa_int_faces_adj_attr::
GetFaces()
{
  if (!closure_valid_)
  {
    faces_.clear();
    this->compute_closure();
  }

  return faces_;
}

// Caller must delete returned list when finished
iface_list*  vifa_int_faces_adj_attr::
GetFaceList()
{
  if (!closure_valid_)
  {
    faces_.clear();
    this->compute_closure();
  }

  iface_list* v = new iface_list;
  for (iface_iterator f = faces_.begin(); f != faces_.end(); ++f)
  {
    v->push_back(*f);
  }

  return v;
}

bool vifa_int_faces_adj_attr::
ComputeAttributes()
{
  if (!closure_valid_)
    this->compute_closure();

  if (vifa_int_faces_attr::ComputeAttributes())
  {
    // Seed is not included in faces_ or attribute histograms
    seed_attr_ = factory_new_attr(seed_);
    vifa_int_face_attr*  seed_attr_ptr = seed_attr_.ptr();

    // Compute ratio of seed to neighbors for each hist attribute
    for (int i = 0; i < this->NumHistAttributes(); i++)
    {
      float  seed_val = (seed_attr_ptr->*(attr_get_funcs[i]))();

      // Instead of x/y, compute (x+1)/(y+1) since y may be near zero

      float  hist_mean = GetMeanAttr(i);
      if (hist_mean < attr_min_vals[i])
        hist_mean = attr_min_vals[i];
      mean_ratios_[i] = (seed_val + 1) / (hist_mean + 1);

      float  hist_min = GetMinAttr(i);
      if (hist_min < attr_min_vals[i])
        hist_min = attr_min_vals[i];
      min_ratios_[i] = (seed_val + 1) / (hist_min + 1);
    }

    attributes_valid_ = true;
  }

  return valid_p();
}

int vifa_int_faces_adj_attr::
NeighborhoodSize()
{
  compute_closure();
  return faces_.size();
}

// Populate a vector containing all attributes, including inherited ones.
bool vifa_int_faces_adj_attr::
GetAttributes(vcl_vector<float>& attrs)
{
  if (vifa_int_faces_attr::GetAttributes(attrs))
    return this->vifa_int_faces_adj_attr::GetNativeAttributes(attrs);
  else
    return false;
}

// Append the attribute names to a vector in the same order as
// GetAttributes.  KEEP IN SYNC WITH GetAttributes!!!
void vifa_int_faces_adj_attr::
GetAttributeNames(vcl_vector<vcl_string>& names)
{
  vifa_int_faces_attr::GetAttributeNames(names);

  for (int i = 0; i < NUM_HIST_ATTRIBUTES; i++)
  {
    vcl_string  name(attr_names[i]);
    names.push_back("ratio" + name);
  }

  for (int i = 0; i < NUM_HIST_ATTRIBUTES; i++)
  {
    vcl_string  name(attr_names[i]);
    names.push_back("minRatio" + name);
  }
}

// Populate a vector containing attributes native to this class (not
// inherited).  KEEP IN SYNC WITH GETNAMES ABOVE!
// Assumes that
bool vifa_int_faces_adj_attr::
GetNativeAttributes(vcl_vector<float>& attrs)
{
  if (!this->ComputeAttributes())
  {
    vcl_cerr << "Couldn't compute group adj attributes?\n";
    return false;
  }

  for (int i = 0; i < this->NumHistAttributes(); i++)
  {
    attrs.push_back(this->GetRatioAttr(i));
  }

  for (int i = 0; i < this->NumHistAttributes(); i++)
  {
    attrs.push_back(this->GetMinRatioAttr(i));
  }

  // what about Collinearity()???

  return true;
}

bool vifa_int_faces_adj_attr::
compute_closure()
{
//  vcl_cout << " In ifsaa::compute_closure()...\n";

  if (!closure_valid_)
  {
    if (faces_.empty())
    {
//      vcl_cout << "faces_ is empty -- rebuilding\n";

      if ((seed_) && (depth_ != BAD_DEPTH))
      {
        // Temporarily add the seed to the list (front), preventing the
        // neighborhood construction from adding it back accidentally.
        faces_.push_back(seed_);

        // Build the neighborhood (recursively)
        this->compute_closure_step(0, seed_);

        // Remove the seed, leaving the true neighborhood
        faces_.erase(faces_.begin());
      }
    }

//    vcl_cout << faces_.size() << " face(s)" << endl;

    float    original_area = 0;
    float    junk_area = 0;
    int      original_nbrhood_size = faces_.size();
    float    area_threshold = (seed_->Npix() * junk_area_percentage_);
    iface_list  keep_faces;
    for (iface_iterator f = faces_.begin(); f != faces_.end(); ++f)
    {
      original_area += (*f)->Npix();

      if ((*f)->Npix() >= area_threshold)
        keep_faces.push_back(*f);
      else
      {
        junk_count_++;
        junk_area += (*f)->Npix();
      }
    }

//    vcl_cout << keep_faces.size() << " keep_face(s)" << endl;

    faces_ = keep_faces;
    junk_percent_ = (float)junk_count_ / original_nbrhood_size;
    junk_area_ratio_ = junk_area / original_area;
    closure_valid_ = true;
  }

#if 0
  vcl_cout << "Final faces_: " << faces_.size() << vcl_endl;

  for (iface_iterator f = faces_.begin(); f != faces_.end(); ++f)
    vcl_cout << "  (" << (*f)->Xo() << ", " << (*f)->Yo() << "), "
             << (*f)->Npix() << " pixels\n";
#endif

//  vcl_cout << "Leaving ifsaa::compute_closure()\n";

  return closure_valid_;
}

float vifa_int_faces_adj_attr::
Collinearity()
{
  compute_closure();
  get_collinear_lines();

  float    u_len = 0.0f;
  float    w_len = 0.0f;
  float    coll = 0.0f;
  edge_list edges; seed_->edges(edges);

  for (edge_iterator  ei = edges.begin(); ei != edges.end(); ei++)
  {
    vtol_edge_2d*  e = (*ei)->cast_to_edge_2d();

    if (e)
    {
      vifa_coll_lines_sptr  clr = get_line_along_edge(e);

      if (clr)
      {
        u_len += float(e->curve()->length());
        w_len += float(clr->spanning_length());
      }
    }
  }

  if (u_len > 0.0f)
    coll = w_len / u_len;

  return coll;
}

// Get the ratio of an vifa_int_face_attr attribute to the seed.
float vifa_int_faces_adj_attr::
GetRatioAttr(int attr_index)
{
  return mean_ratios_[attr_index];
}

// Get the ratio of the seed's vifa_int_face_attr to the minimum neighbor
float vifa_int_faces_adj_attr::
GetMinRatioAttr(int attr_index)
{
  return min_ratios_[attr_index];
}

void vifa_int_faces_adj_attr::
init()
{
  junk_percent_ = 0.0f;
  junk_area_ratio_ = 0.0f;

  int    n = NumHistAttributes();
  mean_ratios_.reserve(n);
  min_ratios_.reserve(n);

  // initialize to -1.
  for (int i = 0; i < n; i++)
    mean_ratios_[i] = min_ratios_[i] = -1;
}

bool vifa_int_faces_adj_attr::
add_unique_face(iface_list&               facelist,
                vtol_intensity_face_sptr  face,
                int                       size_filter)
{
  for (iface_iterator check = facelist.begin();
      check != facelist.end(); ++check)
  {
    if ((check->ptr()->Xo() == face->Xo()) &&
        (check->ptr()->Yo() == face->Yo()))
      return false;
  }

  if (face->Npix() >= size_filter)
  {
    facelist.push_back(face);
    return true;
  }

  return false;
}

void vifa_int_faces_adj_attr::
compute_closure_step(int                       current_depth,
                     vtol_intensity_face_sptr  seed)
{
#ifdef CCS_DEBUG
  vcl_cout << "ccs: depth " << current_depth << " of " << depth_;

  if (seed)
    vcl_cout << " at seed " << seed->Xo() << ", " << seed->Yo();
  else
    vcl_cout << " with null seed";
#endif

  if ((current_depth >= depth_) || (!seed))
  {
#ifdef CCS_DEBUG
    vcl_cout << "...bailing\n";
#endif
    return;
  }
#ifdef CCS_DEBUG
  else
    vcl_cout << vcl_endl;
#endif

  // Get all the faces adjacent to the seed face
  iface_list*  adj_faces = get_adjacent_faces(seed);
  if (adj_faces)
  {
    // For each adjacent face...
    iface_iterator fi = adj_faces->begin();
    for (; fi != adj_faces->end(); fi++)
    {
      vtol_intensity_face_sptr  adj_face_sptr = (*fi);

      // Try to add the adjacent face to the master face list
      if (this->add_unique_face(faces_, adj_face_sptr, size_filter_))
      {
        // It's a new face, so recursively find its neighbors
        this->compute_closure_step(current_depth + 1, adj_face_sptr);
      }
    }

    delete adj_faces;
  }
  else
    vcl_cerr << "vifsaa::compute_closure_step(): No adj. faces found!\n";
}

vtol_intensity_face_sptr vifa_int_faces_adj_attr::
get_adjacent_face_at_edge(vtol_intensity_face_sptr&  known_face,
                          vtol_edge_2d*              e)
{
  vtol_intensity_face_sptr  adj_face = 0;
  face_list faces; e->faces(faces);

  // Expect only one or two intensity faces for 2-D case
  if (faces.size() == 2)
  {
    vtol_intensity_face*  f1 = faces[0]->cast_to_intensity_face();
    vtol_intensity_face*  f2 = faces[1]->cast_to_intensity_face();

    if (f1 && f2 && *known_face == *f1)
      adj_face = f2;
    else if (f1 && f2 && *known_face == *f2)
      adj_face = f1;
    else
    {
      // Known face does not contain the
      // given edge -- leave result NULL
    }
  }

  return adj_face;
}

iface_list* vifa_int_faces_adj_attr::
get_adjacent_faces(vtol_intensity_face_sptr&  known_face)
{
  iface_list*    faces = NULL;

  if (known_face.ptr())
  {
    edge_list edges; known_face->edges(edges);
    faces = new iface_list;

    for (edge_iterator ei = edges.begin(); ei != edges.end(); ei++)
    {
      vtol_edge_2d* e = (*ei)->cast_to_edge_2d();
      if (e)
      {
        vtol_intensity_face_sptr  other_f =
          this->get_adjacent_face_at_edge(known_face, e);

        if (other_f.ptr())
          this->add_unique_face(*faces, other_f, 0);
      }
    }
  }

  return faces;
}
