// This is gel/vifa/vifa_group_pgram.cxx
#include <vifa/vifa_group_pgram.h>
//:
// \file
#include <vnl/vnl_math.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_vector_2d.h>

#ifndef DEGTORAD
#define DEGTORAD  (vnl_math::pi / 180.0)
#endif


vifa_group_pgram::
vifa_group_pgram(imp_line_list&          lg,
                 const vifa_group_pgram_params&  old_params,
                 double              angle_range)
  : vifa_group_pgram_params(old_params)
{
  _angle_range = angle_range;
  _th_dim = (int)vcl_ceil(_angle_range/_angle_increment);
  _th_dim++; //Include both 0 and _angle_range
  _bb = new vifa_bbox;

  for (int i = 0; i < _th_dim; i++)
  {
    imp_line_list*  illp = new imp_line_list;
    _curves.push_back(illp);
  }
  this->Index(lg);
}

//------------------------------------------------------------
//: Destructor
vifa_group_pgram::
~vifa_group_pgram()
{
  for (imp_line_table::iterator ilti = _curves.begin(); ilti != _curves.end();
       ilti++)
  {
    imp_line_list*  illp = (*ilti);
    for (imp_line_iterator ili = illp->begin(); ili != illp->end(); ili++)
    {
      (*ili) = 0;
    }

    delete illp;
  }
}

//-----------------------------------------------------
//: Add an ImplicitLine to the index
void vifa_group_pgram::
Index(imp_line_sptr il)
{
  int  ang_bin = this->AngleLoc(il);
  _curves[ang_bin]->push_back(il);

  this->touch();
}

//------------------------------------------------------------
//: Add a set of ImplicitLines to the angular index
void vifa_group_pgram::
Index(imp_line_list& lg)
{
  for (imp_line_iterator ili = lg.begin(); ili != lg.end(); ili++)
    Index(*ili);
}

//------------------------------------------------------------
//: Clear all lines from the index
void vifa_group_pgram::
Clear()
{
  for (imp_line_table::iterator ilti = _curves.begin(); ilti != _curves.end();
       ilti++)
  {
    imp_line_list*  illp = (*ilti);
    for (imp_line_iterator ili = illp->begin(); ili != illp->end(); ili++)
    {
      (*ili) = 0;
    }

    delete illp;
    (*ilti) = new imp_line_list;
  }

  this->touch();
}

//-----------------------------------------------------------------
//: Compute a histogram of parallel line coverage
//
vifa_histogram_sptr vifa_group_pgram::
GetCoverageHist(void)
{
  vifa_histogram_sptr  h = new vifa_histogram(_th_dim, 0.0, _angle_range);

  float*  cnts = h->GetCounts();
  for (int i = 0; i < _th_dim; i++)
  {
    cnts[i] = this->LineCoverage(i);
  }
  return h;
}

//--------------------------------------------------------------
//: Get a populated line coverage corresponding to a given angle bin
//
vifa_line_cover_sptr vifa_group_pgram::
GetLineCover(int  angle_bin)
{
  imp_line_sptr  il = this->LineAtAngle(angle_bin);

  // Get all the lines which are within +- angular resolution of angle_bin.
  imp_line_list  lg;
  this->CollectAdjacentLines(angle_bin, lg);

  if (!lg.size())
  {
    return NULL;
  }

  // Construct a bounding box from the ROI and clip
  // the line defined by angle_bin
  double  bx;
  double  by;
  double  ex;
  double  ey;
  this->CheckUpdateBoundingBox();
  if (!vgl_clip_line_to_box(il->a(), il->b(), il->c(),
                _bb->min_x(), _bb->min_y(),
                _bb->max_x(), _bb->max_y(),
                bx, by, ex, ey))
  {
    vcl_cerr << "In vifa_group_pgram::GetLineCover(): No intersection found\n";

    return NULL;
  }

  // Here we set the clipping bounds.
  vgl_point_2d<double>  b(bx, by);
  vgl_point_2d<double>  e(ex, ey);
  il->set_points(b, e);

  // The line is cut into 1 pixel bins
  int len = int(il->length());
  if (!len)
  {
    return NULL;
  }

  vifa_line_cover_sptr  cov = new vifa_line_cover(il, len);
  for (imp_line_iterator ili = lg.begin(); ili != lg.end(); ili++)
  {
    cov->InsertLine(*ili);
  }

  return cov;
}

//--------------------------------------------------------------
//: Compute parallel line overlap on a line at the same orientation with midpoint at the center of the region of the line group.
//
double vifa_group_pgram::
LineCoverage(int  angle_bin)
{
  vifa_line_cover_sptr  lc = this->GetLineCover(angle_bin);
  double          coverage = (lc ? lc->GetCoverage() : 0.0);

  return coverage;
}

//--------------------------------------------------------------
//: Collect implicit line(s) from the angle array at orientations +- the given bin orientation.
//  Wrap around the end of the array so that 0 degrees and 180 degrees are considered parallel.
void vifa_group_pgram::
CollectAdjacentLines(int             angle_bin,
                     imp_line_list&  lg)
{
  if ((angle_bin >= 0) && (angle_bin <= (_th_dim - 1)))
  {
    if (angle_bin == 0)
    {
      // Case I - At the beginning of the array
      lg.insert(lg.end(), _curves[_th_dim - 1]->begin(),
                _curves[_th_dim - 1]->end());
      lg.insert(lg.end(), _curves[0]->begin(),
                _curves[0]->end());
      lg.insert(lg.end(), _curves[1]->begin(),
                _curves[1]->end());
    }
    else if (angle_bin == _th_dim - 1)
    {
      // Case II - At the end of the array
      lg.insert(lg.end(), _curves[_th_dim - 2]->begin(),
                _curves[_th_dim - 2]->end());
      lg.insert(lg.end(), _curves[_th_dim - 1]->begin(),
                _curves[_th_dim - 1]->end());
      lg.insert(lg.end(), _curves[0]->begin(),
                _curves[0]->end());
    }
    else
    {
      // Case III - not near ends of the array
      lg.insert(lg.end(), _curves[angle_bin - 1]->begin(),
                _curves[angle_bin - 1]->end());
      lg.insert(lg.end(), _curves[angle_bin]->begin(),
                _curves[angle_bin]->end());
      lg.insert(lg.end(), _curves[angle_bin + 1]->begin(),
                _curves[angle_bin + 1]->end());
    }
  }
  else
  {
    vcl_cerr << "vifa_group_pgram::CollectAdjacentLines(): bad angle_bin\n";
  }
}

//------------------------------------------------------------------
//: Get Total length of parallel lines adjacent and including a bin
//
double vifa_group_pgram::
GetAdjacentPerimeter(int  bin)
{
  imp_line_list  lg;
  this->CollectAdjacentLines(bin, lg);

  double  sum = 0;
  for (imp_line_iterator ili = lg.begin(); ili != lg.end(); ili++)
  {
    vgl_vector_2d<double>  v = (*ili)->point2() - (*ili)->point1();
    sum += v.length();
  }

  return sum;
}

//------------------------------------------------------
//: Compute the total length of parallel lines normalized by the total edge perimeter
double vifa_group_pgram::
norm_parallel_line_length(void)
{
  this->ComputeDominantDirs();
  if (_dominant_dirs.size() < 1)
  {
    // No basis
    return 0;
  }

  double            max_cover = 0.0;
  int              max_dir = 0;
  vcl_vector<int>::iterator  iit = _dominant_dirs.begin();
  for (; iit != _dominant_dirs.end(); iit++)
  {
    int            dir = (*iit);
    vifa_line_cover_sptr  lc = this->GetLineCover(dir);
    if (!(lc.ptr()))
    {
//      vcl_cout << "vgg::norm_parallel_line_length(): "
//               << "No line cover found for dir " << dir << vcl_endl;

      // Is this right?  What about other dominant directions?
      // return 0.0;
      continue;
    }

    double  cov = lc->GetCoverage();

    if (cov > max_cover)
    {
      max_cover = cov;
      max_dir = dir;
    }
  }

  double  per = this->GetAdjacentPerimeter(max_dir);

//  vcl_cout << "vgg::norm_parallel_line_length(): per = " << per
//           << ", _tmp1 = " << _tmp1 << vcl_endl;

  return 1.5 * per / _tmp1;
}

//---------------------------------------------------------
//: Find the angle bin corresponding to an implicit_line
int vifa_group_pgram::
AngleLoc(imp_line_sptr  il)
{
  // Compute angle index
  double  angle = il->slope_degrees();
  while (angle < 0.0)
  {
    angle += 180.0;
  }
  while (angle > 180.0)
  {
    angle -= 180.0;
  }

  if (angle > _angle_range)
  {
    vcl_cerr << "In vifa_group_pgram::AngleLoc(): angle " << angle
             << " was outside the angle range " << _angle_range << vcl_endl;

    return 0;
  }

  return (int)(vcl_floor(angle / _angle_increment));
}

//--------------------------------------------------------------
//: Define a line passing through the center of the Hough ROI and at an angle correspoinding the angle bin
imp_line_sptr vifa_group_pgram::
LineAtAngle(int  angle_bin)
{
  // Get the new line's direction unit vector
  double          ang_rad = DEGTORAD * angle_bin * _angle_increment;
  vgl_vector_2d<double>  d(vcl_cos(ang_rad), vcl_sin(ang_rad));

  // Get the new line's midpoint (bounding box centroid)
  this->CheckUpdateBoundingBox();
  vgl_point_2d<double>  m = _bb->centroid();

  // Return a new line
  imp_line_sptr      il = new imp_line(d, m);
  return il;
}

//------------------------------------------------------
//: Compute the bounding box of the current index
//
void vifa_group_pgram::
ComputeBoundingBox(void)
{
  // Reset the bounding box
  _bb->empty();

  for (imp_line_table::iterator ilti = _curves.begin(); ilti != _curves.end();
    ilti++)
  {
    imp_line_list*  illp = (*ilti);
    for (imp_line_iterator ili = illp->begin(); ili != illp->end(); ili++)
    {
      imp_line_sptr  il = (*ili);

      _bb->add(il->point1());
      _bb->add(il->point2());
    }
  }

  // Update the bounding box timestamp
  _bb->touch();
}

//---------------------------------------------------------
//: Compute the dominant directions using the coverage histogram.
//  A dominant direction is a local maximum in the coverage distribution.
//
void vifa_group_pgram::
ComputeDominantDirs(void)
{
  _dominant_dirs.clear();

  // Get the coverage histogram and do find local maxima.
  vifa_histogram_sptr  h = this->GetCoverageHist();
  vifa_histogram_sptr  h_sup = h->NonMaximumSupress(this->_max_suppress_radius, true);
  float*        cnts = h_sup->GetCounts();
  int          max_idx = h_sup->GetRes();

  // Rebuild the dominant direction array
  for (int i = 0; i < max_idx; i++)
  {
    if (cnts[i] > 0)
    {
      _dominant_dirs.push_back(i);
    }
  }

//  vcl_cout << "vgg::ComputeDominantDirs(): max_idx = " << max_idx << ", "
//           << _dominant_dirs.size() << " dominant directions found\n";
}
