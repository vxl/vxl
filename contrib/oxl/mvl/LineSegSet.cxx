#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_fstream.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_vector.txx>
#include <vcl/vcl_list.h>

#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing

#include <mvl/LineSeg.h>
#include <mvl/ImageMetric.h>
#include <mvl/HomgPoint2D.h>

////////////////#include <Geometry/ImplicitLine.h>
//////////////#include <Topology/Edge.h>

#include "LineSegSet.h"

// Default ctor
LineSegSet::LineSegSet():
  _hlines(0)
{
  _conditioner = 0;
}

// Copy ctor
LineSegSet::LineSegSet(const LineSegSet& that):
  _hlines(0)
{
  operator=(that);
}

// Assignment
LineSegSet& LineSegSet::operator=(const LineSegSet& that)
{
  _hlines = that._hlines;
  _conditioner = that._conditioner;
  return *this;
}

// Destructor
LineSegSet::~LineSegSet()
{
}

// -- Construct from ascii file
LineSegSet::LineSegSet(const char* filename, const HomgMetric& c)
{
  ifstream f(filename);
  load_ascii(f, c);
}

// -- Load lines from ASCII file
bool LineSegSet::load_ascii(istream& f, HomgMetric const& c)
{
  vnl_matrix<double> L;
  f >> L;

  int cols = L.columns();
  if (cols != 6 && cols != 4) {
    cerr << "Load failed -- there are " << L.columns() << " data per row\n";
    return false;
  }

  _conditioner = c;
  _hlines.resize(0);
  for(unsigned i = 0; i < L.rows(); ++i) {
    double x1 = L(i,0);
    double y1 = L(i,1);
    double x2 = L(i,2);
    double y2 = L(i,3);
//  double theta;
//  double avemag;
//  if (cols == 6) {
//    theta = L(i,4);
//    avemag = L(i,5);
//  } else {
//    theta = 0;
//    avemag = 0;
//  }

    HomgPoint2D p1(x1, y1);
    HomgPoint2D p2(x2, y2);
    HomgLineSeg2D line(p1, p2);
    _hlines.push_back(c.image_to_homg(line));
  }
  
  cerr << "Loaded " << _hlines.size() << " line segments\n";
  return true;
}
  
int LineSegSet::FindNearestLineIndex(double x, double y)
{
  //float mindist=100000000;
  //float dist;
  int mini=-1;

  cerr <<"LineSegSet::FindNearestLineIndex AIIEEEEE\n";
//  for (int i=0; i<int(size()); i++){
//    ImplicitLine* dl = get_iuline(i);
//    double t = ( dl->GetStartX() - P->x() ) * ( dl->GetEndX() - P->x() );
//    t +=       ( dl->GetStartY() - P->y() ) * ( dl->GetEndY() - P->y() );
//    t +=       ( dl->GetStartZ() - P->z() ) * ( dl->GetEndZ() - P->z() );
//    // i.e.: t = dot_product ( GetStartPoint() - (*p) , GetEndPoint() - (*p) ) ;
//
//    if (t<0)     // P lies inbetween the two end points
//	dist = dl->IUPoint2CurveDistance(P->x(),P->y(),P->z());  // distance to the support line
//    else
//	dist = dl->EndPointsDistance(*P);   // closest distance with endpoints
//    if (dist<mindist){
//	mindist = dist;
//	mini = i;
//    }
//  }

  return(mini);    
}

// -- Save lines to ASCII file
bool LineSegSet::save_ascii(ostream& f) const
{
  for(unsigned i = 0; i < _hlines.size(); ++i) {
    HomgLineSeg2D const& l = _hlines[i];

    vnl_double_2 p1 = _conditioner.homg_to_image(l.get_point1());
    vnl_double_2 p2 = _conditioner.homg_to_image(l.get_point2());
    
    f << p1[0] << " " << p1[1] << "\t";
    f << p2[0] << " " << p2[1] << endl;
  }
  cerr << "LineSegSet: Saved " << _hlines.size() << " line segments\n";
  return true;
}

// -- Return line selected by mouse click at (x,y) in image coordinates.
int LineSegSet::pick_line_index(double x, double y)
{
  HomgPoint2D p(x, y);
  HomgMetric metric(_conditioner);
  
  double dmin = 1e20;
  int imin = -1;
  int nlines = _hlines.size();
  for(int i = 0; i < nlines; ++i) {
    const HomgLineSeg2D& l = _hlines[i];
    HomgLineSeg2D l_decond = metric.homg_to_image(l);

    double d = l_decond.picking_distance(p);
    
    if (d < dmin) {
      dmin = d;
      imin = i;
    }
  }
  
  return imin;
}

// -- Return line selected by mouse click at (x,y) in image coordinates.
HomgLineSeg2D* LineSegSet::pick_line(double x, double y)
{
  int i = pick_line_index(x,y);
  if (i >= 0)
    return &_hlines[i];
  else
    return 0;
}
