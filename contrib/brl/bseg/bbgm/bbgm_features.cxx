//:
// \file

#include "bbgm_features.h"
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <brip/brip_line_generator.h>
//===========================================================================
// Methods for mask feature

//===========================================================================
// mask feature - static member initialization

unsigned bbgm_mask_feature::uid_ = 0; //initialize unique id counter

unsigned bbgm_mask_feature::ni() const
{
  if (mid_ == brip_rect_mask::ntypes||aid_ == brip_rect_mask::ntypes)
    return 0;
  brip_rect_mask m(static_cast<brip_rect_mask::mask_id>(mid_));
  m.set_angle_id(static_cast<brip_rect_mask::ang_id>(aid_));
  return m.ni();
}

unsigned bbgm_mask_feature::nj() const
{
  if (mid_ == brip_rect_mask::ntypes||aid_ == brip_rect_mask::ntypes)
    return 0;
  brip_rect_mask m(static_cast<brip_rect_mask::mask_id>(mid_));
  m.set_angle_id(static_cast<brip_rect_mask::ang_id>(aid_));
  return m.nj();
}

vcl_vector<vgl_point_2d<unsigned short> > bbgm_mask_feature::
pixels(unsigned i, unsigned j)
{
  vcl_vector<vgl_point_2d<unsigned short> > pix;
  if (mid_ == brip_rect_mask::ntypes||aid_ == brip_rect_mask::ntypes)
    return pix;
  brip_rect_mask m(static_cast<brip_rect_mask::mask_id>(mid_));
  m.set_angle_id(static_cast<brip_rect_mask::ang_id>(aid_));
  unsigned ni = m.ni(), nj = m.nj();
  int ri = (ni-1)/2, rj = (nj-1)/2;
  for (int jj = -rj; jj<=rj; ++jj)
    for (int ii = -ri; ii<=ri; ++ii)
      if (m(ii, jj)>0) {
        unsigned short pi = static_cast<unsigned short>(ii+i);
        unsigned short pj = static_cast<unsigned short>(jj+j);
        if (pi>=320||pj>=180) {
          vcl_cout << "Write out of bounds in mask(" << pi << ' ' << pj << ")\n";
          continue;
        }
        pix.push_back(vgl_point_2d<unsigned short>(pi, pj));
      }
  return pix;
}

//: Return a string name
// \note this is probably not portable

vcl_string bbgm_mask_feature::is_a() const
{
  return "bbgm_mask_feature";
}


bbgm_mask_feature* bbgm_mask_feature::clone() const
{
  return new bbgm_mask_feature(*this);
}


//: Return IO version number;

short bbgm_mask_feature::version() const
{
  return 1;
}


//: Binary save self to stream.

void bbgm_mask_feature::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, mid_);
  vsl_b_write(os, aid_);
  vsl_b_write(os, p_);
}


//: Binary load self from stream.
void bbgm_mask_feature::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      vsl_b_read(is, mid_);
      vsl_b_read(is, aid_);
      vsl_b_read(is, p_);
      break;
    default:
      vcl_cerr << "bbgm_mask_feature: unknown I/O version " << ver << '\n';
    }
}

// public methods
void vsl_b_write(vsl_b_ostream &os, const bbgm_mask_feature& b)
{
  b.b_write(os);
}

//: Binary load bbgm_templ_prob_image
void vsl_b_read(vsl_b_istream &is, bbgm_mask_feature& b)
{
  bbgm_mask_feature temp;
  temp.b_read(is);
  b = temp;
}

void vsl_print_summary(vcl_ostream& os, const bbgm_mask_feature & b)
{
  vcl_cerr << "bbgm_mask_feature::vsl_print_summary not implemented\n";
}

//===========================================================================
// mask pair feature - static member initialization
//===========================================================================

unsigned bbgm_mask_pair_feature::uid_ = 0; //initialize unique id counter


vcl_vector<vgl_point_2d<unsigned short> > bbgm_mask_pair_feature::pixels()
{
  vcl_vector<vgl_point_2d<unsigned short> > pixp;
  //start of path
  float isf = static_cast<float>(i0_), jsf = static_cast<float>(j0_);
  // end point of path
  float ief = static_cast<float>(i1_), jef = static_cast<float>(j1_);
  float lif = 0, ljf = 0; //current point
  bool init = true;
  while (brip_line_generator::generate(init, isf, jsf, ief, jef,
                                       lif, ljf))
  {
    //cast the line pixel location to unsigned short
    unsigned ili = static_cast<unsigned short>(lif),
      ilj = static_cast<unsigned short>(ljf);
      if (ili>=320||ilj>=180) {
        vcl_cout << "Write out of bounds in mask pair(" << ili << ' ' << ilj << ")\n";
        continue;
      }
    pixp.push_back(vgl_point_2d<unsigned short>(ili, ilj));
  }
  //get the positive elements of masks
  bbgm_mask_feature mf0(mid_, ang0_), mf1(mid_, ang1_);
  vcl_vector<vgl_point_2d<unsigned short> > pixm0 = mf0.pixels(i0_, j0_);
  vcl_vector<vgl_point_2d<unsigned short> > pixm1 = mf0.pixels(i1_, j1_);
  vcl_vector<vgl_point_2d<unsigned short> > pix;
  for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pit = pixp.begin();
       pit!= pixp.end(); ++pit)
    pix.push_back(*pit);
  //add mask pixels (removing duplicates)
  for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pit = pixm0.begin();
       pit!= pixm0.end(); ++pit) {
    bool found = false;
    for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pot = pix.begin();
         pot != pix.end()&&!found; ++pot)
      if ((*pot)==(*pit)) {
        found = true;
      }
    if (!found) pix.push_back(*pit);
  }
  for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pit = pixm1.begin();
       pit!= pixm1.end(); ++pit) {
    bool found = false;
    for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pot = pix.begin();
         pot != pix.end()&&!found; ++pot)
      if ((*pot)==(*pit)) {
        found = true;
      }
    if (!found) pix.push_back(*pit);
  }
  return pix;
}

//===========================================================================
// Binary I/O Methods for mask pair feature

//: Return a string name
// \note this is probably not portable

vcl_string bbgm_mask_pair_feature::is_a() const
{
  return "bbgm_mask_pair_feature";
}


bbgm_mask_pair_feature* bbgm_mask_pair_feature::clone() const
{
  return new bbgm_mask_pair_feature(*this);
}


//: Return IO version number;

short bbgm_mask_pair_feature::version() const
{
  return 1;
}


//: Binary save self to stream.

void bbgm_mask_pair_feature::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, static_cast<unsigned>(mid_));
  vsl_b_write(os, static_cast<unsigned>(ang0_));
  vsl_b_write(os, static_cast<unsigned>(ang1_));
  vsl_b_write(os, i0_);
  vsl_b_write(os, j0_);
  vsl_b_write(os, i1_);
  vsl_b_write(os, j1_);
  vsl_b_write(os, id0_);
  vsl_b_write(os, id1_);
  vsl_b_write(os, p_);
}


//: Binary load self from stream.
void bbgm_mask_pair_feature::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      unsigned temp0, temp1, temp2;
      vsl_b_read(is, temp0);
      vsl_b_read(is, temp1);
      vsl_b_read(is, temp2);
      vsl_b_read(is, i0_);
      vsl_b_read(is, j0_);
      vsl_b_read(is, i1_);
      vsl_b_read(is, j1_);
      vsl_b_read(is, id0_);
      vsl_b_read(is, id1_);
      vsl_b_read(is, p_);
      mid_ = static_cast<brip_rect_mask::mask_id>(temp0);
      ang0_ = static_cast<brip_rect_mask::ang_id>(temp1);
      ang1_ = static_cast<brip_rect_mask::ang_id>(temp2);
      break;
    default:
      vcl_cerr << "bbgm_mask_pair_feature: unknown I/O version " << ver << '\n';
    }
}

// public methods
void vsl_b_write(vsl_b_ostream &os, const bbgm_mask_pair_feature& b)
{
  b.b_write(os);
}

//: Binary load bbgm_templ_prob_image
void vsl_b_read(vsl_b_istream &is, bbgm_mask_pair_feature& b)
{
  bbgm_mask_pair_feature temp;
  temp.b_read(is);
  b = temp;
}

void vsl_print_summary(vcl_ostream& os, const bbgm_mask_pair_feature & b)
{
  vcl_cerr << "bbgm_mask_pair_feature::vsl_print_summary not implemented\n";
}


//===========================================================================
// Methods for pair group features

unsigned bbgm_pair_group_feature::uid_ = 0; //initialize unique id counter

//constructor from single pair
bbgm_pair_group_feature::
bbgm_pair_group_feature(bbgm_mask_pair_feature const& mp)
{
  id_ = mp.id();
  mid_ = mp.mask_id();
  pairs_.insert(mp);
  unsigned short ci, cj;
  mp.center(ci, cj);
  ci_ = ci; cj_ = cj;
  p_ = mp();
}

//: compute a bounding box for the group based on the vertices
vgl_box_2d<unsigned> bbgm_pair_group_feature::bounding_box() const
{
  vgl_box_2d<unsigned> box;
  if (!pairs_.size())
    return box;
  vcl_set<bbgm_mask_pair_feature, fless >::const_iterator vit;
  for (vit=pairs_.begin(); vit != pairs_.end(); ++vit)
  {
    unsigned short ci, cj;
    vit->center(ci, cj);
    vgl_point_2d<unsigned> p(ci, cj);
    box.add(p);
  }
  return box;
}

vgl_polygon<double> bbgm_pair_group_feature::convex_hull() const
{
  vcl_vector<vgl_point_2d<double> > points;
  vcl_set<bbgm_mask_pair_feature, fless >::const_iterator vit;
  for (vit=pairs_.begin(); vit != pairs_.end(); ++vit)
  {
    unsigned short ci, cj;
    vit->center(ci, cj);
    vgl_point_2d<double> p(ci, cj);
    points.push_back(p);
  }
  if (points.size()<3)
    return vgl_polygon<double>();
  vgl_convex_hull_2d<double> h(points);

  return h.hull();
}

vcl_vector<vgl_point_2d<unsigned short> > bbgm_pair_group_feature::pixels()
{
  vcl_vector<vgl_point_2d<unsigned short> > pix;
  vcl_set<bbgm_mask_pair_feature, fless >::iterator pit = pairs_.begin();
  for (; pit!=pairs_.end(); ++pit)
  {
    bbgm_mask_pair_feature mpf = *pit;
    vcl_vector<vgl_point_2d<unsigned short> > pixp = mpf.pixels();
    for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pxt = pixp.begin();
      pxt!= pixp.end(); ++pxt) {
      bool found = false;
      for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pot = pix.begin();
           pot != pix.end()&&!found; ++pot)
        if ((*pot)==(*pxt)) {
          found = true;
        }
      if (!found) pix.push_back(*pxt);
    }
    //insert the pixels of the star
    unsigned short im, jm;
    mpf.center(im, jm);
    //start of path
    float isf = static_cast<float>(ci_), jsf = static_cast<float>(cj_);
    // end point of path
    float ief = static_cast<float>(im), jef = static_cast<float>(jm);
    float lif = 0, ljf = 0; //current point
    bool init = true;
    while (brip_line_generator::generate(init, isf, jsf, ief, jef,
                                         lif, ljf))
    {
      //cast the line pixel location to unsigned short
      unsigned short ili = static_cast<unsigned short>(lif),
        ilj = static_cast<unsigned short>(ljf);
      vgl_point_2d<unsigned short> pt(ili, ilj);
      if (ili>=320||ilj>=180) {
        vcl_cout << "Write out of bounds in pair group(" << ili << ' ' << ilj << ")\n";
        continue;
      }
      bool found = false;
      for (vcl_vector<vgl_point_2d<unsigned short> >::iterator pot = pix.begin();
           pot != pix.end()&&!found; ++pot)
        if ((*pot)==pt) {
          found = true;
        }
      if (!found) pix.push_back(pt);
    }
  }
  return pix;
}


//: Return a string name
// \note this is probably not portable

vcl_string bbgm_pair_group_feature::is_a() const
{
  return "bbgm_pair_group_feature";
}


bbgm_pair_group_feature* bbgm_pair_group_feature::clone() const
{
  return new bbgm_pair_group_feature(*this);
}


//: Return IO version number;

short bbgm_pair_group_feature::version() const
{
  return 1;
}


//: Binary save self to stream.

void bbgm_pair_group_feature::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, static_cast<unsigned>(mid_));
  vsl_b_write(os, ci_);
  vsl_b_write(os, cj_);
  //have to implement explicitly
  //since general vsl_set_io doesn't take a predicate.
  unsigned n = pairs_.size();
  vsl_b_write(os, n);
  vcl_set<bbgm_mask_pair_feature, fless>::const_iterator vit;
  vit = pairs_.begin();
  for (; vit!= pairs_.end(); ++vit)
    vsl_b_write(os, *vit);
  vsl_b_write(os, p_);
}


//: Binary load self from stream.
void bbgm_pair_group_feature::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      unsigned temp;
      vsl_b_read(is, temp);
      vsl_b_read(is, ci_);
      vsl_b_read(is, cj_);
      unsigned n;
      vsl_b_read(is, n);
      for (unsigned i = 0; i<n; ++i) {
        bbgm_mask_pair_feature mpf;
        vsl_b_read(is, mpf);
        pairs_.insert(mpf);
      }
      vsl_b_read(is, p_);
      mid_ = static_cast<brip_rect_mask::mask_id>(temp);
      break;
    default:
      vcl_cerr << "bbgm_pair_group_feature: unknown I/O version " << ver << '\n';
    }
}

// public methods
void vsl_b_write(vsl_b_ostream &os, const bbgm_pair_group_feature& b)
{
  b.b_write(os);
}

//: Binary load bbgm_templ_prob_image
void vsl_b_read(vsl_b_istream &is, bbgm_pair_group_feature& b)
{
  bbgm_pair_group_feature temp;
  temp.b_read(is);
  b = temp;
}

void vsl_print_summary(vcl_ostream& os, const bbgm_pair_group_feature & b)
{
  vcl_cerr << "bbgm_pair_group_feature::vsl_print_summary not implemented\n";
}

bool pair_intersect(bbgm_mask_pair_feature const& mp0,
                    bbgm_mask_pair_feature const& mp1,
                    bool plus_intersect_only)
{
  // first see if they share a common mask
  unsigned id00 = mp0.id0(), id01 = mp0.id1();
  unsigned id10 = mp1.id0(), id11 = mp1.id1();
  if (id00==id10||id00==id11||id01==id00||id01==id11)
    return true;
  //check if the masks have intersecting positive elements
  brip_rect_mask::mask_id mid = mp0.mask_id();
  brip_rect_mask::ang_id ang00 = mp0.ang0();
  brip_rect_mask::ang_id ang01 = mp0.ang1();
  brip_rect_mask::ang_id ang10 = mp1.ang0();
  brip_rect_mask::ang_id ang11 = mp1.ang1();
  unsigned short i00, j00, i01, j01, i10, j10, i11, j11;
  mp0.x0(i00, j00);   mp0.x1(i01, j01);
  mp1.x0(i10, j10);   mp1.x1(i11, j11);
  if (plus_intersect_only) {
    if (brip_rect_mask::intersect(mid, ang00, i00, j00, ang10, i10, j10))
      return true;
    if (brip_rect_mask::intersect(mid, ang00, i00, j00, ang11, i11, j11))
      return true;
    if (brip_rect_mask::intersect(mid, ang01, i01, j01, ang10, i10, j10))
      return true;
    if (brip_rect_mask::intersect(mid, ang01, i01, j01, ang11, i11, j11))
      return true;
  }
  else {
    if (brip_rect_mask::intersect_domain(mid, ang00, i00, j00, ang10, i10, j10))
      return true;
    if (brip_rect_mask::intersect_domain(mid, ang00, i00, j00, ang11, i11, j11))
      return true;
    if (brip_rect_mask::intersect_domain(mid, ang01, i01, j01, ang10, i10, j10))
      return true;
    if (brip_rect_mask::intersect_domain(mid, ang01, i01, j01, ang11, i11, j11))
      return true;
  }
  return false;
}

bbgm_pair_group_feature pair_group_merge(bbgm_pair_group_feature const& pg0,
                                         bbgm_pair_group_feature const& pg1,
                                         float p_path)
{
  //collect the vertices
  const vcl_set<bbgm_mask_pair_feature, fless>& pairs0 =
    pg0.pairs();
  double pr0 = pg0();

  const vcl_set<bbgm_mask_pair_feature, fless>& pairs1 = pg1.pairs();
  double pr1 = pg1();

  vcl_set<bbgm_mask_pair_feature, fless> merged_verts;

  vcl_set<bbgm_mask_pair_feature, fless>::const_iterator pit;

  for (pit = pairs0.begin(); pit != pairs0.end(); ++pit)
    merged_verts.insert(*pit);

  for (pit = pairs1.begin(); pit != pairs1.end(); ++pit)
    merged_verts.insert(*pit);
  double np = merged_verts.size();
  if (np==0) return pg0;
  double dp = vcl_pow(pr0, (np-1.0)/np);
  dp *= vcl_pow(pr1, 1.0/np);
  float p = static_cast<float>(dp)*p_path;
  bbgm_pair_group_feature pgm;
  pgm.set_prob(merged_verts, p);
  return pgm;
}

bbgm_pair_group_feature
pair_group_max_union(bbgm_pair_group_feature const& pg0,
                     bbgm_pair_group_feature const& pg1)
{
  //collect the mask pairs in each group
  const vcl_set<bbgm_mask_pair_feature, fless>& pairs0 = pg0.pairs();

  const vcl_set<bbgm_mask_pair_feature, fless>& pairs1 = pg1.pairs();

  //the pairs in the union
  vcl_set<bbgm_mask_pair_feature, fless> merged_verts;

  vcl_set<bbgm_mask_pair_feature, fless>::const_iterator pit;

  float pmax = 0.0f;
  for (pit = pairs0.begin(); pit != pairs0.end(); ++pit) {
    merged_verts.insert(*pit);
    if ((*pit)()>pmax)
      pmax = (*pit)();
  }
  for (pit = pairs1.begin(); pit != pairs1.end(); ++pit) {
    merged_verts.insert(*pit);
    if ((*pit)()>pmax)
      pmax = (*pit)();
  }
  bbgm_pair_group_feature pgm;
  pgm.set_prob(merged_verts, pmax);
  return pgm;
}
