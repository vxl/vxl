// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
// #include <Detection/gevd_contour.h>
// #include <ImageProcessing/FloatOperators.h>
// #include <ImageProcessing/pixel.h>

// #include <Topology/Vertex.h>
// #include <Topology/vtol_edge_2d.h>
// #include <DigitalGeometry/DigitalCurve.h>

// #include <cool/Timer.h>
// #include <cool/ArrayP.h>
// #include <cool/ListP.h>
// #include <iostream.h>
// #include <math.h>

#include <vil/vil_byte.h>

#include <vdgl/vdgl_digital_curve_ref.h>
#include <vdgl/vdgl_edgel_chain_ref.h>
#include <vdgl/vdgl_interpolator_ref.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>

#include "gevd_float_operators.h"
#include "gevd_contour.h"
#include "gevd_pixel.h"

const int INVALID = -1;

// Use 8 directions, with 45 degree angle in between them.

const vil_byte TWOPI = 8, /* FULLPI = 4, */ HALFPI = 2 /* , QUARTERPI = 1 */;
// const vil_byte DIR0 = 8, DIR1 = 9, DIR2 = 10, DIR3 = 11;
const int DIS[] = { 1, 1, 0,-1,-1,-1, 0, 1, // 8-connected neighbors
		    1, 1, 0,-1,-1,-1, 0, 1, // wrapped by 2PI to
		    1, 1, 0,-1,-1,-1, 0, 1};// avoid modulo operations.
const int DJS[] = { 0, 1, 1, 1, 0,-1,-1,-1,
		    0, 1, 1, 1, 0,-1,-1,-1,
		    0, 1, 1, 1, 0,-1,-1,-1};

// const int RDS[] = {0,-1, 1,-2, 2,-3, 3,-4, 4,-5, 5}; // radial search
const int RIS[] = { 1, 0,-1, 0, // spiral search for 4/8-connected
		    1,-1,-1, 1, // neighbors
		    2, 0,-2, 0,
		    2, 1,-1,-2,-2,-1, 1, 2,
		    2,-2,-2, 2,
		    3, 0,-3, 0,
		    3, 1,-1,-3,-3,-1, 1, 3,
		    3, 2,-2,-3,-3,-2, 2, 3,
		    4, 0,-4, 0};
const int RJS[] = { 0, 1, 0,-1, // rotate CW, increasing radius
		    1, 1,-1,-1,
		    0, 2, 0,-2,
		    1, 2, 2, 1,-1,-2,-2,-1,
		    2, 2,-2,-2,
		    0, 3, 0,-3,
		    1, 3, 3, 1,-1,-3,-3,-1,
		    2, 3, 3, 2,-2,-3,-3,-2,
		    0, 4, 0,-4};
const int RNS[] = { 4, 8, 12, 20, 24, 28, 36, 44, 48}; // at distinct r
const float RGS[] = { 1, 1.414213, 2, 2.236067, 2.828427, // values of gap
		      3, 3.162277, 3.605551, 4};

// - win32 - moved to here for MSVC++
const int MINLENGTH = 3;	// minimum number of pixels for a chain
const int FRAME = 4;		// border of image

bool gevd_contour::talkative = true;	// By default contour is not silent.

// -- Save parameters and create workspace for detecting contours.
// Each contour must have at least 1 pixel above min_strength,
// and its number of internal pixels must be above min_length.
// This is a heuristic hysteresis scheme that prunes weak or short
// isolated chains.
// To join a weaker contour to a stronger contour, a junction must
// have a change in response above min_jump on the stronger contour.
// This way, only strong junctions are detected.

gevd_contour::gevd_contour(float min_strength, int min_length,
		 float min_jump, float max_gap)
  : minStrength(min_strength), minLength(min_length),
    minJump(min_jump), maxSpiral(0),
    edgeMap(NULL), vertexMap(NULL)
{
  if (minStrength < 0) {
    cerr << "gevd_contour::gevd_contour -- negative min_strength: "
	 << minStrength << ". Reset to 0." << endl;
    minStrength = 0;
  }
  if (minLength < MINLENGTH) {
    cerr << "gevd_contour::gevd_contour -- too small min_length: "
	 << minLength << ". Reset to " << MINLENGTH << ".\n";
    minLength = MINLENGTH;
  }
  if (minJump < 0) {
    cerr << "gevd_contour::gevd_contour -- negative min_jump: "
	 << minJump << ". Reset to 0." << endl;
    minJump = 0;
  }
  if (minJump > minStrength) {
    cerr << "gevd_contour::gevd_contour -- too large min_jump: "
	 << min_jump << ". Reset to " << minStrength << ".\n";
    minJump = minStrength;
  }
  if (max_gap < 1) {
    cerr << "gevd_contour::gevd_contour -- too small max_gap: "
	 << max_gap << ". Reset to 1.\n";
    max_gap = 1;
  }
  if (max_gap > FRAME) {
    cerr << "gevd_contour::gevd_contour -- too large max_gap: "
	 << max_gap << ". Reset to " << FRAME << endl;
    max_gap = FRAME;
  }
  for (int i = 0; i < 9; i++)	// find number of neighbors to search
    if (max_gap <= RGS[i])	// for given gap radius
      maxSpiral= i+1;
}

// -- Free space allocated for detecting contours.

gevd_contour::~gevd_contour()
{
  delete edgeMap;		// space shared by LinkJunction/Chain
  delete vertexMap;
}

// -- Find network of linked edges and vertices, from 8-connected
// edge elements. The contours must be less than 2 pixel wide,
// for example found from non maximum suppression.
// Isolated edgels and short segments are erased.

bool
gevd_contour::FindNetwork(gevd_bufferxy& edgels,
		     const int njunction,
		     const int* junctionx, const int* junctiony,
		     vcl_vector<vtol_edge_2d_ref>*& edges,
		     vcl_vector<vtol_vertex_2d_ref>*& vertices)
{
  // make sure that if no edges are found that edges and vertices 
  // get values, to avoid seg faults, WAH
  if (!edges)
    edges = new vcl_vector<vtol_edge_2d_ref>;
  else
    edges->clear();
  if (!vertices)
    vertices = new vcl_vector<vtol_vertex_2d_ref>;
  else
    vertices->clear();

  if (talkative)
    cout << "*** Link edge elements into connected edges/vertices."
	 << endl;

  // 1. Setup lookup maps based on (x,y) integer location.
  vertexMap = gevd_float_operators::Allocate(vertexMap, edgels, bits_per_ptr);
  vertexMap->Clear();
  edgeMap = gevd_float_operators::Allocate(edgeMap, edgels, bits_per_ptr);
  edgeMap->Clear();

  // 2. Collect 4/8-connected pixels into chains
  int n = max(10*njunction, // preallocated size from junctions or
	      edgels.GetSizeX()*edgels.GetSizeY()/100); // image size
  vcl_vector<vtol_edge_2d_ref> edges2(n);
  //  edges2.set_growth_ratio(2);
  n = this->FindChains(edgels, // link pixels into chains
		       njunction, // also use junction pixels
		       junctionx, junctiony,
		       edges2);
  if (!n) 
    return false;		// empty network
  
  // 3. Sort chains longest first.
  // GEOFF: Can ignore for the moment
  //  if (edges2.size() < 1000)	// avoid O(nlogn) for very large n
  //  edges2.sort(&gevd_contour::LengthCmp); // sort longest/strongest first

  //  int i = 0;			// renumber with order in array
  //  for (edges2.reset(); edges2.next(); i++)
  //    edges2.value()->set_id(i);

  for( int i= 0; i< edges2.size(); i++)
    edges2[i]->set_id(i);

  // 4. Split/Merge chains from touching end points
  vcl_vector<vtol_vertex_2d_ref> vertices2(2*n);
  //  vertices2.set_growth_ratio(1.5);
  this->FindJunctions(edgels, // break/merge at junctions of
		      edges2, vertices2); // distinct chains

  // 5. Copy back results into global lists
  //  for (edges2.reset(); edges2.next(); )
  //    edges->push_end(edges2.value());

  for( int i= 0; i< edges2.size(); i++)
    edges->push_back( edges2[i]);

  //  for (vertices2.reset(); vertices2.next(); )
  //    vertices->push_end(vertices2.value());
  
  for( int i=0; i< vertices2.size(); i++)
    vertices->push_back( vertices2[i]);

  return true;
}


// // Return TRUE if pixel is a local maximum,
// and so is right on top of contour.

bool
Ongevd_contour(const gevd_bufferxy& edgels, const int i, const int j)
{
  float pix = (1 + sqrt(2)) * floatPixel(edgels, i, j); // fuzzy threshold
  for (vil_byte dir = 0; dir < TWOPI; dir += HALFPI) // 4-connected only
    if (floatPixel(edgels, i+DIS[dir], j+DJS[dir]) > pix)
      return false;		// should choose neighbor instead
  return true;
}

// // Delete pixel from contour, and save its location in xloc/yloc.

void
RecordPixel(int i, int j, gevd_bufferxy& edgels,
	    vcl_vector<int>& iloc, vcl_vector<int>& jloc)
{
  floatPixel(edgels, i, j) = -floatPixel(edgels, i, j); // flip sign
  iloc.push_back(i), jloc.push_back(j);
}

// // Find next best pixel on contour, searching for strongest response,
// and favoring 4-connected over 8-connected.
// Return 0, if no pixel is found, or direction in range [2*pi, 4*pi).

int
NextPixel(int& i, int& j, const gevd_bufferxy& edgels)
{
  float maxpix = 0, npix;
  int maxdir = 0, dir;
  for (dir = 0; dir < TWOPI; dir += HALFPI) // 4-connected first
    if ((npix = floatPixel(edgels, i+DIS[dir], j+DJS[dir]))
	> maxpix) {
      maxpix = npix;
      maxdir = dir+TWOPI;
    }
  if (!maxdir) {
    for (dir = 1; dir < TWOPI; dir += HALFPI) // 8-connected next
      if ((npix = floatPixel(edgels, i+DIS[dir], j+DJS[dir]))
	  > maxpix) {
	maxpix = npix;
	maxdir = dir+TWOPI;
      }
  }
  if (maxdir)			// update next strongest pixel
    i += DIS[maxdir], j += DJS[maxdir];
  return maxdir;
}


// -- Trace and collect pixels on thin contours, stronger pixels first,
// and favoring 4-connected over 8-connected. Thinning is not used,
// and so will avoid errors because of square grid tesselation.
// A chain can not cross itself. It can only touch itself or another
// chain, in which case a junction will be found later.
// The pixels of a chain include the 2 end points.
// End points and junctions are created in gevd_contour::FindJunctions.
// Return the number of chains found.  Protected.

int
gevd_contour::FindChains(gevd_bufferxy& edgels, const int njunction,
		    const int* junctionx, const int* junctiony,
		    vcl_vector<vtol_edge_2d_ref>& edges)
{
  //  CoolTimer t;

  // 1. Save away detected junctions from extending at end points of
  // contours, without linking these contours up. This avoids random
  // order in the traversal of the contours.
  const float mark = 1;		// dummy non zero pointer
  {for (int k = 0; k < njunction; k++)
    floatPixel(*vertexMap, junctionx[k], junctiony[k]) = mark;
  }

  // 2. Trace elongated & thinned chains, stronger pixels first.
  // Virtual border of image should be inserted last.
  const int rmax = FRAME;
  const int xmax = edgels.GetSizeX()-rmax-1;
  const int ymax = edgels.GetSizeY()-rmax-1;
  vcl_vector<int> xloc(xmax+ymax), yloc(xmax+ymax); // work space for
  //  xloc.set_growth_ratio(2); yloc.set_growth_ratio(2); // collection
  {for (int j = rmax; j <= ymax; j++)
    for (int i = rmax; i <= xmax; i++)
    {
      // 2.0. Start from better pixels above noise+hysteresis
      if (floatPixel(edgels, i, j) > minStrength &&
	  Ongevd_contour(edgels, i, j)) { // right on the contour
	int x = i, y = j;

	// 2.1. Prune isolated pixels
	if (!NextPixel(x, y, edgels)) {// prune isolated pixels
	  floatPixel(edgels, i, j) = 0;
	  continue;
	}

	// 2.2. Start collecting first 3 pixels
	xloc.clear(), yloc.clear(); // collect pixels on contour
	RecordPixel(i, j, edgels, xloc, yloc);	// first pixel
	int ii = x, jj = y;
	RecordPixel(ii, jj, edgels, xloc, yloc); // second pixel
	if (NextPixel(x, y, edgels))
	  RecordPixel(x, y, edgels, xloc, yloc); // third pixel
	else {			// reach end point
	  x = i, y = j;		// revert back to start pt
	  if (NextPixel(x, y, edgels)) { // reverse collection
	    xloc.clear(), yloc.clear();
	    RecordPixel(ii, jj, edgels, xloc, yloc); // second pixel
	    RecordPixel(i, j, edgels, xloc, yloc); // first pixel
	    RecordPixel(x, y, edgels, xloc, yloc); // third pixel
	    ii = i, jj = j;
	  } else  {		// reach other end point
	    floatPixel(edgels, i, j) = 0; // prune isolated pixel-pairs
	    floatPixel(edgels, ii, jj) = 0;
	    continue;
	  }
	}

	// 2.3. Watch out for zig-zag at 2nd pixel, from LR-TD scans
	if ((x - ii)*(ii - xloc[0]) +
	    (y - jj)*(jj - yloc[0]) < 0) {
	  xloc[1] = xloc[0], yloc[1] = yloc[0]; // swap first 2 points
	  xloc[0] = ii, yloc[0] = jj; // to eliminate zig-zag
	}

	// 2.4. Collect both directions & extension points if 1-chain
	while (NextPixel(x, y, edgels)) // trace along first dir, 4-connected
	  RecordPixel(x, y, edgels, xloc, yloc); // and stronger first
	if (fabs(xloc[0]-x) > 1 || // disjoint first/last pixel
	    fabs(yloc[0]-y) > 1) { // so must be a 1-chain with end points
	  if (NextPixel(x, y, *vertexMap)) // search for extra links to
	    xloc.push_back(x), yloc.push_back(y); // detected junctions
	  x = xloc[0], y = yloc[0]; // start again from first pixel


	  //	  xloc.reverse(), yloc.reverse(); // reverse collected pixels
	  vcl_vector<int> xloctemp( xloc.size()), yloctemp( yloc.size());
	  for( int iii=0; iii< xloc.size(); iii++) xloctemp[iii]= xloc[xloc.size()-1-iii];
	  for( int jjj=0; jjj< yloc.size(); jjj++) yloctemp[jjj]= yloc[yloc.size()-1-jjj];
	  // 

	  while (NextPixel(x, y, edgels)) // trace along other dir
	    RecordPixel(x, y, edgels, xloc, yloc);
	  if (NextPixel(x, y, *vertexMap)) // search for extra links to
	    xloc.push_back(x), yloc.push_back(y); // detected junctions
	}
	int len = xloc.size();

	// 2.5. Check for isolated contours that are too short
	if (len < minLength) {	// zero or too few internal pixels
	  for (int k = 0; k < len; k++) // zero or too few internal pixels
	    floatPixel(edgels, xloc[k], yloc[k]) = 0; // prune short chains
	  continue;
	}

	// Thin zig-zags on the contours? Zig-zags happen at the 2
	// end points because of extension, or inside the contour
	// because of 4/8-connected tracing through noisy chain pixels,
	// and large shifts for subpixel locations.
	// Defer the elimination of zig-zags to gevd_contour::SubPixelAccuracy()
	// or gevd_contour::EqualizeSpacing()

	// 2.6. Create network of chains, touching, possibly ending
	// at same junction, but never crossing one another
	vtol_edge_2d_ref edge = new vtol_edge_2d();

	vdgl_edgel_chain_ref ec = new vdgl_edgel_chain;
	vdgl_interpolator_ref it = new vdgl_interpolator_linear(ec);
	vdgl_digital_curve_ref dc = new vdgl_digital_curve(it); // include end points

	for( int k=0; k< len; k++)
	  {
	    x= xloc[k];
	    y= yloc[k];
	    
	    ec->add_edgel( vdgl_edgel( x, y));
	    edgePtr( *edgeMap, x, y)= edge;
	  }

	edge->set_curve(*dc);

// 	float *cx = dc->GetX(), *cy = dc->GetY();
// 	{for (int k = 0; k < len; k++) { // collect interior pixels
// 	  x = xloc[k], y = yloc[k]; // and at 2 end points
// 	  cx[k] = x; // integral location to map to pixel location
// 	  cy[k] = y; // with truncation int().
// 	  edgePtr(*edgeMap, x, y) = edge; // can overlap only at extension
// 	}}

	LookupTableInsert(edges, edge);
      }
  }}

  // 3. Restore cache to original state
  {for (int k = 0; k < njunction; k++)  // clear all void*/float labels
    vertexPtr(*vertexMap, junctionx[k], junctiony[k]) = NULL;
  }
  {for (int j = rmax; j <= ymax; j++)
    for (int i = rmax; i <= xmax; i++)
      if (floatPixel(edgels, i, j) < 0) // undo mark put by RecordPixel
	floatPixel(edgels, i, j) = - floatPixel(edgels, i, j);
  }

  if (talkative)
    cout << "Find " << edges.size()
	 << " chains/cycles, with pixels > " << minLength
	 << " and strength > " << minStrength << endl;
      //	 << ", in " << t.real() << " msecs." << endl;
  return edges.size();	// number of chains found so far
}

// Check that end point of a weak contour touches another stronger
// contour at an internal pixel. Localize the junction to pixel accuracy
// by searching for shortest distance from end point to chain.
// Gaussian smoothing can put local maximum change in filter response
// 1 pixel away from this junction location.
// Update junction map.

bool
DetectJunction(vtol_vertex_2d& end, int& index,
	       vtol_edge_2d_ref& weaker, vtol_edge_2d_ref& stronger,
	       const int maxSpiral,
	       const gevd_bufferxy& edgels, gevd_bufferxy& edgeMap)
{
  // 0. Must be an end point of a dangling 1-chain
  if (end.numsup() > 1)		// avoid junction and 1-cycle
    return false;
  vcl_vector<vtol_edge_2d *>* edges = end.compute_edges();
  weaker = (*edges)[0];	// dangling edge must be a weaker contour
  delete edges;
  vdgl_digital_curve_ref dc = (vdgl_digital_curve*)weaker->curve().ptr();
  const int len = dc->size();
  const float *cx = dc->GetX(), *cy = dc->GetY();

  // 1. Mark off pixels at end pt to find junction of a contour to itself
  const int rfuzz = min(len, 3*MINLENGTH);
  vtol_edge_2d_ref* labels = new vtol_edge_2d*[rfuzz];
  if (&end == weaker->v1())
    for (int r = 0; r < rfuzz; r++) {
      labels[r] = edgePtr(edgeMap, int(cx[r]), int(cy[r]));
      edgePtr(edgeMap, int(cx[r]), int(cy[r])) = NULL;
    }
  else
    for (int r = 0; r < rfuzz; r++) {
      labels[r] = edgePtr(edgeMap, int(cx[len-1-r]), int(cy[len-1-r]));
      edgePtr(edgeMap, int(cx[len-1-r]), int(cy[len-1-r])) = NULL;
    }
  // 2. Find another stronger contour touched by this end point < gap.
  stronger = NULL;		// contour can join with itself
  int jx = int(end.x()), jy = int(end.y());
  {for (int l = 0, n = 0; l < maxSpiral; l++) {	// increasing radius of spiral
    float maxpix = 0; int maxn = 0;	// strongest strength at this radius
    for ( ; n < RNS[l]; n++) {
      int x = jx+RIS[n], y = jy+RJS[n];
      if (edgePtr(edgeMap, x, y) && // find another contour or itself
	  floatPixel(edgels, x, y) > maxpix) {
	maxpix = floatPixel(edgels, x, y);
	maxn = n;		// better neighbor
      }
    }
    if (maxpix) {		// location of junction on contour
      stronger = edgePtr(edgeMap, jx+RIS[maxn], jy+RJS[maxn]);
      jx += RIS[maxn], jy += RJS[maxn];
      break;
    }
  }}
  if (&end == weaker->v1())	// restore edgeMap around end point
    for (int r = 0; r < rfuzz; r++)
      edgePtr(edgeMap, int(cx[r]), int(cy[r])) = labels[r];
  else
    for (int r = 0; r < rfuzz; r++)
      edgePtr(edgeMap, int(cx[len-1-r]), int(cy[len-1-r])) = labels[r];
  delete [] labels;
  if (!stronger)		// do not find any edge in search region
    return false;

  // 3. Find index location of junction on this contour
  index = int(INVALID);
  vdgl_digital_curve_ref dc2 = (vdgl_digital_curve*) stronger->GetCurve();
  const float *cx2 = dc2->GetX(), *cy2 = dc2->GetY();
  {for (int n = 0; n < dc2->size(); n++)	// find corresponding index on contour
    if (int(cx2[n]) == jx && int(cy2[n]) == jy) {
      index = n;
      break;
    }}

#ifdef TRACE_DEBUG
  if (index == INVALID) {
    cerr << "Fail to find index on 1-chain: " << jx << "," << jy << endl;
    for (int n = 0; n < dc2->size(); n++)
      cerr << cx2[n] << "," << cy2[n] << endl;
    cerr << "End point: " << end.GetX() << "," << end.GetY() << endl;
    return false;
  }
#endif

  return true;
}

// // Confirm there is a strong jump in response near a junction.
// The location of this jump is however inaccurate, and so junctions
// can not be localized accurately along the stronger cycle.

bool
ConfirmJunctionOnCycle(int index, float threshold,
		       vtol_edge_2d& cycle, const gevd_bufferxy& edgels)
{
  vdgl_digital_curve_ref dc = (vdgl_digital_curve*) cycle.GetCurve();
  const int len = dc->size();
  const int wrap = 10*len;	// for positive index
  const int radius = 3;		// gap < 3, around junction pixel
  const float *cx = dc->GetX(), *cy = dc->GetY();
  for (int n = index-radius; n <= index+radius; n++) {
    int _n = (n-1+wrap)%len;	// modulo operations to wrap at borders
    int n_ = (n+1+wrap)%len;
    if (fabs(floatPixel(edgels, int(cx[n_]), int(cy[n_])) -
	     floatPixel(edgels, int(cx[_n]), int(cy[_n]))) >
	threshold)
      return true;
  }
  return false;
}

// // Break the cycle at given index, and create new cycle from/to
// and not including index pixel. Update the chain map accordingly.

void
BreakCycle(vtol_vertex_2d& junction, const int index,
	   vtol_edge_2d& stronger, vtol_edge_2d_ref& split,
	   gevd_bufferxy& edgeMap, gevd_bufferxy& vertexMap)
{
  vdgl_digital_curve_ref dc = (vdgl_digital_curve*) stronger.GetCurve();
  const float *cx = dc->GetX(), *cy = dc->GetY();
  const int len = dc->size();

  // 1. Move location of junction
  int jx = int(junction.GetX()), jy = int(junction.GetY());
  vertexPtr(vertexMap, jx, jy) = NULL; // erase old location
  jx = int(cx[index]), jy = int(cy[index]);
  junction.set_x(jx), junction.set_y(jy); // update new location
  vertexPtr(vertexMap, jx, jy) = &junction;
  edgePtr(edgeMap, jx, jy) = NULL;

  // 2. Create 1-cycle, including junction pixel
  split = new vtol_edge_2d();
  vdgl_digital_curve *ds = new vdgl_digital_curve(len);
  split->SetCurve(ds);
  float *sx = ds->GetX(), *sy = ds->GetY();
  int i=0;
  {for (int k = index; k < len; i++,k++) {
    sx[i] = cx[k], sy[i] = cy[k];
    edgePtr(edgeMap, int(cx[k]), int(cy[k])) = split;
  }}
  {for (int k = 0; i < len; i++,k++) {
    sx[i] = cx[k], sy[i] = cy[k];
    edgePtr(edgeMap, int(cx[k]), int(cy[k])) = split;
  }}

  split->set_v1(&junction);	// link both directions v-e
  split->set_v2(&junction);
}

// // Confirm there is a strong jump in response near a junction.
// The location of this jump is however inaccurate, and so junctions
// can not be localized accurately along the stronger chain.

bool
ConfirmJunctionOnChain(int index, float threshold,
		       vtol_edge_2d& chain, const gevd_bufferxy& edgels)
{
  vdgl_digital_curve_ref dc = (vdgl_digital_curve*) chain.GetCurve();
  const int len = dc->size()-1;
  if (len < 2*MINLENGTH-1) // will merge vertices instead of
    return false;		// breaking up chains
  const int fuzz = MINLENGTH-1; // from min length of broken chains
  const int radius = 3;		// gap < 3, around junction pixel
  const float *cx = dc->GetX(), *cy = dc->GetY();
  for (int n = max(index-radius, fuzz);
       n <= min(index+radius,len-1-fuzz); n++)
    if (fabs(floatPixel(edgels, int(cx[n+1]), int(cy[n+1])) -
	     floatPixel(edgels, int(cx[n-1]), int(cy[n-1]))) >
	threshold)
      return true;
  return false;
}


// // Break the edge at given index, and create two subchains from it.
// Update the chain map accordingly.

void
BreakChain(vtol_vertex_2d& junction, const int index,
	   vtol_edge_2d& stronger,
	   vtol_edge_2d_ref& longer, vtol_edge_2d_ref& shorter,
	   gevd_bufferxy& edgeMap, gevd_bufferxy& vertexMap)
{
  vdgl_digital_curve_ref dc = (vdgl_digital_curve*) stronger.GetCurve();
  const float *cx = dc->GetX(), *cy = dc->GetY();
  const int l0 = dc->size();
  const int l1 = index+1, l2 = l0-index;

  // 1. Move location of junction
  int jx = int(junction.x()), jy = int(junction.y());
  vertexPtr(vertexMap, jx, jy) = NULL; // erase old location
  jx = int(cx[index]), jy = int(cy[index]);
  junction.set_x(jx), junction.set_y(jy);	// update new location
  vertexPtr(vertexMap, jx, jy) = &junction;
  edgePtr(edgeMap, jx, jy) = NULL;

  // 2. Create first subchain up to and including junction pixel.
  vtol_edge_2d_ref edge1 = new vtol_edge_2d();	// create subchains, broken at junction.
  vdgl_digital_curve *dc1 = new vdgl_digital_curve(l1);
  edge1->SetCurve(dc1);
  float *cx1 = dc1->GetX(), *cy1 = dc1->GetY();
  {for (int k = 0; k < l1; k++) {
    cx1[k] = cx[k], cy1[k] = cy[k];
    edgePtr(edgeMap, int(cx1[k]), int(cy1[k])) = edge1;
  }}
  vtol_vertex_2d_ref v1 = stronger.v1();
  if (v1->numsup() == 1)	// dangling chain with end pt at v1
    edgePtr(edgeMap, int(cx1[0]), int(cy1[0])) = NULL;
  edge1->set_v1(&v1);		// link both directions v-e
  edge1->set_v2(&junction);	// unlink when stronger.UnProtect()

  // 3. Create second subchain from and including junction pixel.
  vtol_edge_2d_ref edge2 = new vtol_edge_2d();	// create second subchain
  vdgl_digital_curve *dc2 = new vdgl_digital_curve(l2);
  edge2->SetCurve(dc2);
  float *cx2 = dc2->GetX(), *cy2 = dc2->GetY();
  const float *dx = cx + index, *dy = cy + index; // after junction
  {for (int k = 0; k < l2; k++) {
    cx2[k] = dx[k], cy2[k] = dy[k];
    edgePtr(edgeMap, int(cx2[k]), int(cy2[k])) = edge2;
  }}
  vtol_vertex_2d_ref v2 = stronger.v2();
  if (v2->numsup() == 1)	// dangling chain with end pt at v2
    edgePtr(edgeMap, int(cx2[l2-1]), int(cy2[l2-1])) = NULL;
  edge2->set_v1(&junction);	// link both directions v-e
  edge2->set_v2(v2);		// unlink when stronger.UnProtect()

  if (l1 >= l2)			// sort longer/shorter chains
    longer = edge1, shorter = edge2;
  else
    longer = edge2, shorter = edge1;
}


// // Break the chain at given index, and create a loop.
// Update the chain map accordingly.

void
LoopChain(vtol_vertex_2d& junction, const int index,
	  vtol_edge_2d& chain,
	  vtol_edge_2d_ref& straight, vtol_edge_2d_ref& curled,
	  gevd_bufferxy& edgeMap, gevd_bufferxy& vertexMap)
{
  vdgl_digital_curve_ref dc = (vdgl_digital_curve*) chain.GetCurve();
  const float *cx = dc->GetX(), *cy = dc->GetY();
  const int l0 = dc->size();

  // 1. Move location of junction
  int jx = int(junction.x()), jy = int(junction.y());
  vertexPtr(vertexMap, jx, jy) = NULL; // erase old location
  jx = int(cx[index]), jy = int(cy[index]);
  junction.set_x(jx), junction.set_y(jy);	// update new location
  vertexPtr(vertexMap, jx, jy) = &junction;
  edgePtr(edgeMap, jx, jy) = NULL;

  // 1. Find straight/curled chains
  straight = new vtol_edge_2d(), curled = new vtol_edge_2d();
  const int l1 = index+1, l2 = l0-index;
  if (&junction == chain.v1()) { // first subchain is curled
    vdgl_digital_curve *c = new vdgl_digital_curve(l1);
    curled->SetCurve(c);
    float *x = c->GetX(), *y = c->GetY();
    {for (int k = 0; k < l1; k++) {
      x[k] = cx[k], y[k] = cy[k];
      edgePtr(edgeMap, int(x[k]), int(y[k])) = curled;
    }}
    curled->set_v1(&junction);
    curled->set_v2(&junction);
    const float *dx = cx + index, *dy = cy + index; // after junction
    c = new vdgl_digital_curve(l2);	// second subchain is straight
    straight->SetCurve(c);
    x = c->GetX(), y = c->GetY();
    {for (int k = 0; k < l2; k++) {
      x[k] = dx[k], y[k] = dy[k];
      edgePtr(edgeMap, int(x[k]), int(y[k])) = straight;
    }}
    if (chain.v2()->numsup()==1)
      edgePtr(edgeMap, int(x[l2-1]), int(y[l2-1])) = NULL;
    straight->set_v1(&junction);
    straight->set_v2(chain.v2());
  } else {			// first subchain is straight
    vdgl_digital_curve *c = new vdgl_digital_curve(l1);
    straight->SetCurve(c);
    float *x = c->GetX(), *y = c->GetY();
    {for (int k = 0; k < l1; k++) {
      x[k] = cx[k], y[k] = cy[k];
      edgePtr(edgeMap, int(x[k]), int(y[k])) = straight;
    }}
    if (chain.v1()->numsup()==1)
      edgePtr(edgeMap, int(x[0]), int(y[0])) = NULL;
    straight->set_v1(chain.v1());
    straight->set_v2(&junction);
    const float *dx = cx + index, *dy = cy + index; // after junction
    c = new vdgl_digital_curve(l2);	// second subchain is curled
    curled->SetCurve(c);
    x = c->GetX(), y = c->GetY();
    {for (int k = 0; k < l2; k++) {
      x[k] = dx[k], y[k] = dy[k];
      edgePtr(edgeMap, int(x[k]), int(y[k])) = curled;
    }}
    curled->set_v1(&junction);
    curled->set_v2(&junction);
  }
}

// // Find number of rays connected to a vertex.

int
NumConnectedRays(vtol_vertex_2d& v)
{
  int nray = 0;		
  vcl_vector<vtol_edge_2d_ref>* segs = v.Edges();
  //  for (segs->reset(); segs->next(); ) 
  for( int i=0; i< segs.size(); i++)
    {
      if (segs[i]->v1() == &v) nray++; // 1 for 1-chain
      if (segs[i]->v2() == &v) nray++; // 2 for 1-cycle
    }

  delete segs;
  return nray;
}


// // Detect touching another junction or end point,
// from an end point of a dangling chain.

vtol_vertex_2d_ref
DetectTouch(const vtol_vertex_2d& end, const int maxSpiral,
	    gevd_bufferxy& vertexMap)
{
  const int jx = int(end.x()), jy = int(end.y());
  for (int l = 0, n = 0; l < maxSpiral; l++) { 	// increasing radius of spiral
    vtol_vertex_2d_ref other = NULL;	// prefer junction over endpt
    int maxray = 0;		// largest number of rays
    for ( ; n < RNS[l]; n++) {  // 4- then 8-connected
      vtol_vertex_2d_ref nbr = vertexPtr(vertexMap, jx+RIS[n], jy+RJS[n]);
      int nray = (nbr != NULL ? NumConnectedRays(*nbr) : 0);
      if (nray > maxray) {
	maxray = nray;		// number of rays connected to it
	other = nbr;		// better neighbor
      }
    }
    if (maxray) 		// find larger/other junction
      return other;
  }
  return NULL;
}

// // Find dangling edges connected to vertex

vtol_edge_2d_ref
DanglingEdge(vtol_vertex_2d& v)
{
  vcl_vector<vtol_edge_2d_ref>* segs = v.Edges();
  vtol_edge_2d_ref e = NULL;
  if (segs->size()==1) 
    e = segs->value();
  delete segs;
  return e;
}

// // Merge 2 end points of a same chain.
// Update global maps.

void
MergeEndPtsOfChain(vtol_vertex_2d& endpt, vtol_vertex_2d& other, vtol_edge_2d& common,
		   gevd_bufferxy& edgeMap, gevd_bufferxy&vertexMap)
{ 
  int px = int(other.x()), py = int(other.y());
  vertexPtr(vertexMap, px, py) = NULL; // erase old location
  edgePtr(edgeMap, px, py) = &common;  		
  if (common.v1() == &other)  // remove links to other endpt
    common.set_v1(&endpt); 
  else
    common.set_v2(&endpt);	
}		

// // Merge 2 touching chains into 1, deleting the 2 touching end points
// and their chains. Smooth away short kinks is delayed for later.
// Update global maps.

void
MergeEndPtTouchingEndPt(vtol_vertex_2d& end1, vtol_vertex_2d& end2,
			vtol_edge_2d_ref& merge, vtol_edge_2d_ref& longer, vtol_edge_2d_ref& shorter,
			gevd_bufferxy& edgeMap, gevd_bufferxy& vertexMap)
{
  // 1. Retrieve the dangling edges/chains
  vcl_vector<vtol_edge_2d_ref>* edges = end1.Edges();
  vtol_edge_2d_ref edge1 = edges->get();	// dangling edges
  delete edges;
  edges = end2.Edges();
  vtol_edge_2d_ref edge2 = edges->get();
  delete edges;

  // 2. Create merged edge/chain
  vdgl_digital_curve_ref dc1 = (vdgl_digital_curve*)edge1->GetCurve();
  const int l1 = dc1->size();
  vdgl_digital_curve_ref dc2 = (vdgl_digital_curve*)edge2->GetCurve();
  const int l2 = dc2->size();
  const int len = l1+l2;
  merge = new vtol_edge_2d();
  vdgl_digital_curve_ref dc = new vdgl_digital_curve(len);
  merge->SetCurve(dc);
  float *cx = dc->GetX(), *cy = dc->GetY();
  vtol_vertex_2d *v1, *v2;		// vertices of merge edge
  int k = 0;			// index in merge array
  const float *cx1 = dc1->GetX(), *cy1 = dc1->GetY();
  if (edge1->v2() == &end1) {
    for (int i = 0; i < l1; i++, k++)
      cx[k] = cx1[i], cy[k] = cy1[i];
    v1 = edge1->v1();
  } else {			// reverse collection
    for (int i = l1-1; i >= 0; i--, k++)
      cx[k] = cx1[i], cy[k] = cy1[i];
    v1 = edge1->v2();
  }
  merge->set_v1(v1);
  const float *cx2 = dc2->GetX(), *cy2 = dc2->GetY();
  if (edge2->v1() == &end2) {
    for (int i = 0; i < l2; i++, k++)
      cx[k] = cx2[i], cy[k] = cy2[i];
    v2 = edge2->v2();
  } else {			// reverse collection
    for (int i = l2-1; i >= 0; i--, k++)
      cx[k] = cx2[i], cy[k] = cy2[i];
    v2 = edge2->v1();
  }
  merge->set_v2(v2);

  // 3. Update global maps
  vertexPtr(vertexMap, int(end1.x()), int(end1.y())) = NULL;
  vertexPtr(vertexMap, int(end2.x()), int(end2.y())) = NULL;
  const int last = len-1;
  for (k = 1; k < last; k++)
    edgePtr(edgeMap, int(cx[k]), int(cy[k])) = merge;
  if (edgePtr(edgeMap, int(cx[0]), int(cy[0])))
    edgePtr(edgeMap, int(cx[0]), int(cy[0])) = merge;
  if (edgePtr(edgeMap, int(cx[last]), int(cy[last])))
    edgePtr(edgeMap, int(cx[last]), int(cy[last])) = merge;

  if (l1 >= l2)			// sort out length of deleted subchains
    longer = edge1, shorter = edge2;
  else
    longer = edge2, shorter = edge1;
}

// // Merge an end point into a touching junction.
// Update global maps.

void
MergeEndPtTouchingJunction(vtol_vertex_2d &endpt, vtol_vertex_2d& junction,
			   gevd_bufferxy& edgeMap, gevd_bufferxy&vertexMap)
{
  vcl_vector<vtol_edge_2d_ref>* edges = endpt.Edges();
  vtol_edge_2d_ref edge = edges->get();	// dangling edge terminating at end pt
  delete edges;
  int px = int(endpt.x()), py = int(endpt.y());
  vertexPtr(vertexMap, px, py) = NULL; // erase old location
  edgePtr(edgeMap, px, py) = edge;
  if (edge->v1() == &endpt)  // change the links both directions v-e
    edge->set_v1(&junction);	// unlink when endpt.UnProtect()
  else
    edge->set_v2(&junction);
}


// -- Find junctions from end points touching at an interior point
// of a chain, with detectable jump in filter response.
// Localize these junctions on the stronger contour to pixel accuracy,
// and break stronger chain into subchains.
// Also merge end points touching another end point or junction.
// Return the number of end points and junctions bounding
// all chains/cycles detected in gevd_contour::FindChains.
// Deletion/insertion to the network must be done completely,
// so that the connectivity links are updated.  Protected.

int
gevd_contour::FindJunctions(gevd_bufferxy& edgels,
		       vcl_vector<vtol_edge_2d_ref>& edges,
		       vcl_vector<vtol_vertex_2d_ref>& vertices)
{
  //  CoolTimer t;
  if (!edges.size()) {
    cerr << "gevd_contour::FindChains must precede gevd_contour::FindJunctions."
	 << endl;
    return 0;
  }

  // 1. Create end points or junctions, for all 1-chains.
  const float connect_fuzz = 2;

  
  //  for (edges.reset(); edges.next(); ) {
  for( int i=0; i< edges.size(); i++) 
    {
      vtol_edge_2d_ref edge = edges[i];
      vdgl_digital_curve_ref dc = (vdgl_digital_curve*) edge->GetCurve();
      const float *cx = dc->GetX(), *cy = dc->GetY();
      const int last = dc->size()-1;
      if (fabs(cx[0]-cx[last]) > connect_fuzz || // disjoint first/last pixel
	  fabs(cy[0]-cy[last]) > connect_fuzz) { // so must be a 1-chain
	int x = int(cx[0]), y = int(cy[0]);
	vtol_vertex_2d_ref v1 = vertexPtr(*vertexMap, x, y);
	if (!v1) {		// check for collision
	  v1 = new vtol_vertex_2d(cx[0], cy[0]); // 1st point in chain
	  vertexPtr(*vertexMap, x, y) = v1;
	  LookupTableInsert(vertices, v1);
	} else
	  edgePtr(*edgeMap, x, y) = NULL; // erase junction point
	edge->set_v1(v1);		// link both directions v-e
	x = int(cx[last]), y = int(cy[last]);
	vtol_vertex_2d_ref v2 = vertexPtr(*vertexMap, x, y);
	if (!v2) {		// check for collision
	  v2 = new vtol_vertex_2d(cx[last], cy[last]); // last point in chain
	  vertexPtr(*vertexMap, x, y) = v2;
	  LookupTableInsert(vertices, v2);
	} else
	  edgePtr(*edgeMap, x, y) = NULL; // erase junction point
	edge->set_v2(v2);		// link both directions v-e
      }
  }
//   cout << "Create " << vertices.size()
//        << " end points/junctions for chains only." << endl;

  // 2. Localize a junction, when an end point of a dangling contour
  // touches another contour or itself at an interior point.
  int jcycle = 0, jchain = 0;	// number of junctions with cycle/chain

  //  for (vertices.reset(); vertices.next(); ) {
  for( int i=0; i< vertices.size(); i++)
    {
      vtol_vertex_2d_ref end = vertices[i];
      vtol_edge_2d *weaker = NULL, *stronger = NULL; // weaker touches stronger
      int index;			// location on stronger contour
      if (DetectJunction(*end, index,
			 weaker, stronger, maxSpiral,
			 edgels, *edgeMap)) {
	if (!stronger->v1()) { // touch 1-cycle
	  if (ConfirmJunctionOnCycle(index, minJump,
				     *stronger, edgels)) {
	    vtol_edge_2d_ref split = NULL;		// cycle is now split at junction
	    BreakCycle(*end, index,
		       *stronger,
		       split,	// find split 1-cycle
		       *edgeMap, *vertexMap);	// mutate v-e links
	    LookupTableReplace(edges, stronger, split);
	    jcycle++;		// remove original edge
	  }
	} else {			// touch itself or another 1-chain
	  if (ConfirmJunctionOnChain(index, minJump,
				     *stronger, edgels)) {
	    if (weaker == stronger) {
	      vtol_edge_2d *straight = NULL, *curled = NULL;
	      LoopChain(*end, index, // break its own chain
			*stronger, // and make a loop
			straight, curled,
			*edgeMap, *vertexMap);
	      LookupTableReplace(edges, stronger, straight);
	      LookupTableInsert(edges, curled);
	      jchain++;
	    } else {
	      vtol_edge_2d *longer = NULL, *shorter = NULL;
	      BreakChain(*end, index, // break another stronger chain in 2
			 *stronger,
			 longer, shorter, // find sub chains
			 *edgeMap, *vertexMap);	// mutate v-e links
	      LookupTableReplace(edges, stronger, longer);
	      LookupTableInsert(edges, shorter);
	      jchain++;
	    }
	  }
	}
      }
    }
  //   cout << "Find junctions with "
  //        << jcycle << " cycles and " << jchain << " chains,"
  //        << " with jump > " << minJump << endl;

  // 3. Merge touching end points, into a larger junction/chain.
  int dendpt = 0, dchain = 0;	// number of deleted endpt/chain

  //  for (vertices.reset(); vertices.next(); ) {
  for( int i=0; i< vertices.size(); i++){
    vtol_vertex_2d_ref end = vertices[i]; // search from dangling end pt only
    if (end != NULL &&		// skip deleted vertices
	NumConnectedRays(*end) == 1) { // end point of dangling 1-chain
      vtol_vertex_2d_ref end2 = DetectTouch(*end, maxSpiral, *vertexMap);
      if (end2 != NULL) {	// find end points nearby
	if (NumConnectedRays(*end2) == 1) { // found another dangling end point
	  vtol_edge_2d_ref seg = DanglingEdge(*end);
	  if (seg == DanglingEdge(*end2)) { // end points of 1-cycle
	    MergeEndPtsOfChain(*end, *end2, *seg,
			       *edgeMap, *vertexMap);
	    LookupTableRemove(vertices, end2);
	    dendpt++;		
	  } else {		// end points of 2 distinct 1-chains
// 	    cout << "end=" << *end << endl
// 		 << "endpt2=" << *end2 << endl;
	    vtol_edge_2d *merge=NULL, *longer=NULL, *shorter=NULL; // merge 2 different edges		
	    MergeEndPtTouchingEndPt(*end, *end2, // merge 2 subchains
				    merge, longer, shorter, // deleting
				    *edgeMap, *vertexMap); // end points
// 	    cout << "merge=" << *merge << endl
// 		 << "longer=" << *longer << endl
// 		 << "shorter=" << *shorter << endl;
// 	    cout << "merge.v1=" << *merge->v1() << endl
// 		 << "merge.v2=" << *merge->v2() << endl;
	    LookupTableReplace(edges, longer, merge);
	    LookupTableRemove(edges, shorter);
	    LookupTableRemove(vertices, end);
	    LookupTableRemove(vertices, end2);
	    dendpt += 2, dchain += 1;
	  }
	} else {		// merge into another junction
// 	  cout << "end=" << *end << endl
// 	       << "junction2=" << *end2 << endl;
	  MergeEndPtTouchingJunction(*end, *end2,
				     *edgeMap, *vertexMap);
	  LookupTableRemove(vertices, end);
	  dendpt++;
	}
      }
    }
  }
//   cout << "Merge and delete " << dendpt
//        << " end points and " << dchain << " edges" << endl;
  if (dchain)			// eliminate holes in global arrays
    LookupTableCompress(edges);
  if (dendpt)
    LookupTableCompress(vertices);

  // 4. Insert virtual junction for isolated 1-cycles
  int ncycle = 0;
  //  for (edges.reset(); edges.next(); ) {

  for( int i=0; i< edges.size(); i++)
    {
      vtol_edge_2d_ref edge = edges[i];
      if (!edge->v1()) {	// vertices not created from 1.
	vdgl_digital_curve_ref dc = (vdgl_digital_curve*) edge->GetCurve();
	const float *cx = dc->GetX(), *cy = dc->GetY();
	const int last = dc->size()-1;
	vtol_vertex_2d_ref v = new vtol_vertex_2d((cx[0]+cx[last])/2, (cy[0]+cy[last])/2);
	edge->set_v1(v); edge->set_v2(v); // link both directions v-e
	vertexPtr(*vertexMap, int(v->x()), int(v->y())) = v;
	LookupTableInsert(vertices, v);
	ncycle++;
      }
    }
//   cout << "Create " << ncycle
//        << " virtual end points for isolated cycles." << endl;
  //  if (talkative)
  //    cout << "All junctions found in " << t.real() << " msecs." << endl;
  return vertices.size();
}

// -- Insert subpixel accurary into the pixels on the edges/vertices.
// Truncating float locations with int(xy) should map to the original
// pixel locations. No interpolation is done at junctions of 3 or more
// contours, so a junction can have location error up to 1-2 pixel,
// tangential to the strong contour.

void
gevd_contour::SubPixelAccuracy(vcl_vector<vtol_edge_2d_ref>& edges,
			  vcl_vector<vtol_vertex_2d_ref>& vertices,
			  const gevd_bufferxy& locationx,
			  const gevd_bufferxy& locationy)
{
  //CoolTimer t;
 if (talkative)
   cout << "Insert subpixel accuracy into edges/vertices";
 
 // 1. Subpixel accurary for end points
 //  for (vertices.reset(); vertices.next(); ) {
 for( int i=0; i< vertices.size(); i++)
   {
     vtol_vertex_2d_ref vert = vertices[i];
     int x = int(vert->x()), y = int(vert->y());
     vert->set_x(x + floatPixel(locationx, x, y));
     vert->set_y(y + floatPixel(locationy, x, y));
   }

 // 2. Subpixel accurary for chain pixels
 // for (edges.reset(); edges.next(); ) {
 for( int i=0; i< edges.size(); i++)
   {
   vtol_edge_2d_ref edge = edges[i];
   vdgl_digital_curve_ref dc = (vdgl_digital_curve*) edge->GetCurve();
   float *cx = dc->GetX(), *cy = dc->GetY();
   int x, y;
   for (int k = 0; k < dc->size(); k++) {
     x = int(cx[k]), y = int(cy[k]);
     cx[k] += floatPixel(locationx, x, y);
     cy[k] += floatPixel(locationy, x, y);
   }
 }

  // 3. Thin zig-zags on the contours? Zig-zags happen at
  // the 2 end points because of extension, or inside the contour
  // because of 4/8-connected tracing through noisy chain pixels,
  // and large shifts for subpixel locations.
  // Implement only if experiments prove zig-zags are excessive

 //  if (talkative)
 //    cout << ", in " << t.real() << " msecs." << endl;
}
//------------------------------------------------------------
// -- Generate an Edge with a vdgl_digital_curve representing a straight line
//    between the specified vertices.
//    
static vtol_edge_2d_ref DigitalEdge(vtol_vertex_2d_ref vs, vtol_vertex_2d_ref ve)
{
  IUPoint* ps = vs->GetPoint();
  IUPoint* pe = ve->GetPoint();
  vdgl_digital_curve_ref dc = new vdgl_digital_curve(ps, pe);
  vtol_edge_2d_ref e = new vtol_edge_2d(vs, ve, dc);
  return e;
}
// -- Insert virtual edges and vertices to enforce closure
// of the regions beyond the rectangular image border.
// The location of the border is at 3 pixels away from the
// real image border, because of kernel radius in convolution
// and non maximum suppression. Virtual border of image should be
// inserted after gevd_contour::FindChains() and gevd_contour::FindJunctions().
//
// JLM - February 1999  Modified this routine extensively to 
// move the border to the actual image ROI bounds.  Chain endpoints
// are extended to intersect with the border.  These changes were
// made to support region segmentation from edgels.
void
gevd_contour::InsertBorder(vcl_vector<vtol_edge_2d_ref>& edges,
		      vcl_vector<vtol_vertex_2d_ref>& vertices)
{
  //  CoolTimer t;
  //1.00 Save Edges along the border
  vcl_vector<vtol_vertex_2d_ref> xmin_verts;
  vcl_vector<vtol_vertex_2d_ref> xmax_verts;
  vcl_vector<vtol_vertex_2d_ref> ymin_verts;
  vcl_vector<vtol_vertex_2d_ref> ymax_verts;
  //  vcl_vector<Edge*> edge_extensions;

  if (talkative)
    cout << "Insert virtual border to enforce closure";

  // 1. Create 4 corners vertices
  const int rmax = FRAME;	// border of image
  const int xmax = vertexMap->GetSizeX()-rmax-1;
  const int ymax = vertexMap->GetSizeY()-rmax-1;
  int cx[] = {rmax, xmax, rmax, xmax}; // coordinates of 4 corners
  int cy[] = {rmax, ymax, ymax, rmax};
  int d;
  // 1. Collect Vertices along each border
  //1.0 Generate Corner Vertices
  vtol_vertex_2d_ref V00 = new vtol_vertex_2d(rmax, rmax);
  vtol_vertex_2d_ref V01 = new vtol_vertex_2d(rmax, ymax);
  vtol_vertex_2d_ref V10 = new vtol_vertex_2d(xmax, rmax);
  vtol_vertex_2d_ref V11 = new vtol_vertex_2d(xmax, ymax);
  xmin_verts.push_back(V00); 
  xmax_verts.push_back(V10);
  ymin_verts.push_back(V00); 
  ymax_verts.push_back(V01); 
  // 1.1 ymin, ymax edges
  for (d = 0; d < 2; d++) 
    {
      int x, y = cy[d];
      for (x = rmax; x<=xmax; x++)
	{
          vtol_vertex_2d_ref v = vertexPtr(*vertexMap, x, y); 
	  if(v)
	    vertexPtr(*vertexMap, x, y)=NULL;
	  else continue;
	  if(d)
	    ymax_verts.push_back(v);
	  else
	    ymin_verts.push_back(v);
	}
    }
  // 1.2 xmin, xmax edges
  for (d = 0; d < 2; d++) 
    {
      int x = cx[d], y;
      for (y = rmax; y<=ymax; y++)
	{
	  vtol_vertex_2d_ref v = vertexPtr(*vertexMap, x, y); 
	  if(v)
	    vertexPtr(*vertexMap, x, y)=NULL;
	  else continue;
	  if(d)
	    xmax_verts.push_back(v);
	  else
	    xmin_verts.push_back(v);
	}
    }

  xmin_verts.push_back(V01); 
  xmax_verts.push_back(V11);
  ymin_verts.push_back(V10); 
  ymax_verts.push_back(V11); 

  // 2.0 Merge vertices
  // 2.1  along ymin and ymax
  for (d = 0; d < 2; d++) 
    {
      vcl_vector<vtol_vertex_2d_ref>* verts = &ymin_verts;
      if(d)
	verts = &ymax_verts;
      int len = (*verts).size();
      if(len<3) continue;
      //potential merge at xmin
      vtol_vertex_2d_ref pre_v = (*verts)[0];
      vtol_vertex_2d_ref v = (*verts)[1];
      int x = int(v->x());
      int pre_x = int(pre_v->x());
      if((x-pre_x)<3)
 	{
  	  merge_references(pre_v,v);
  	  (*verts).remove(v);
 	  vertices.remove(v);
	  //	  v->UnProtect();
 	  len--;
 	}
      //potential merge at xmax
      pre_v = (*verts)[len-2];
      v = (*verts)[len-1];
      pre_x = int(pre_v->x());
        if((xmax+rmax-pre_x)<3)
  	{
  	  merge_references(v,pre_v);
  	  (*verts).remove(pre_v);
  	  vertices.remove(pre_v);
	  //	  pre_v->UnProtect();
  	  len--;
	}
    }

  // 2.1  along xmin and xmax
  for (d = 0; d < 2; d++) 
    {
      vcl_vector<vtol_vertex_2d_ref>* verts = &xmin_verts;
      if(d)
	verts = &xmax_verts;
      int len = (*verts).size();
      if(len<3) continue;
      //potential merge at ymin
      vtol_vertex_2d_ref pre_v = (*verts)[0];
      vtol_vertex_2d_ref v = (*verts)[1];
      int y = int(v->GetY()), pre_y = int(pre_v->GetY());
      if((y-pre_y)<3)
 	{
 	  merge_references(pre_v,v);
 	  (*verts).remove(v);
 	  vertices.remove(v);
	  // 	  v->UnProtect();
 	  len--;
 	}
      //potential merge at ymax
      pre_v = (*verts)[len-2];
      v = (*verts)[len-1];
      pre_y = int(pre_v->GetY());
      if((ymax+rmax-pre_y)<3)
 	{
 	  merge_references(v,pre_v);
 	  (*verts).remove(pre_v);
 	  vertices.remove(pre_v);
	  //  	  pre_v->UnProtect();
 	  len--;
 	}
    }
  // 2.0 Move the vertices to the bounds of the ROI
  int iv,  len = xmin_verts.size();
  float xmi = 0, xmx = (xmax + rmax);
  float ymi = 0, ymx = (ymax + rmax);
  for(iv=1; iv<len-1; iv++)
    {
      vtol_vertex_2d_ref v = xmin_verts[iv];
      IUPoint* p = new IUPoint(*(v->GetPoint()));
      p->set_x(xmi);
      vtol_vertex_2d_ref vp = new vtol_vertex_2d(p);
      vertices.push_back(vp);// vp->Protect();
      xmin_verts[iv] = vp;
      vtol_edge_2d_ref e = DigitalEdge(v, vp);
      edges.push_back(e);//  e->Protect();
    }

  len = xmax_verts.size();
  for(iv=0; iv<len; iv++)
    {
      vtol_vertex_2d_ref v = xmax_verts[iv];
      if(iv!=0&&iv!=(len-1))
	{
	  IUPoint* p = new IUPoint(*(v->GetPoint()));
	  p->set_x(xmx);
	  vtol_vertex_2d_ref vp = new vtol_vertex_2d(p);
	  vertices.push_back(vp); // vp->Protect();
	  xmax_verts[iv] = vp;
	  vtol_edge_2d_ref e = DigitalEdge(v, vp);
	  edges.push_back(e); // e->Protect();
	}
    }
  len = ymin_verts.size();
  for(iv=0; iv<len; iv++)
    {
      vtol_vertex_2d_ref v = ymin_verts[iv];
      if(iv!=0&&iv!=(len-1))
	{
	  IUPoint* p = new IUPoint(*(v->GetPoint()));
	  p->set_y(ymi);
	  vtol_vertex_2d_ref vp = new vtol_vertex_2d(p);
	  vertices.push_back(vp); // vp->Protect();
	  ymin_verts[iv] = vp;
	  vtol_edge_2d_ref e = DigitalEdge(v, vp);
	  edges.push_back(e); // e->Protect();
	}
    }
  len = ymax_verts.size();
  for(iv=0; iv<len; iv++)
    {
      vtol_vertex_2d_ref v = ymax_verts[iv];
      if(iv!=0&&iv!=(len-1))
	{
	  IUPoint* p = new IUPoint(*(v->GetPoint()));
	  p->set_y(ymx);
	  vtol_vertex_2d_ref vp = new vtol_vertex_2d(p);
	  vertices.push_back(vp); // vp->Protect();
	  ymax_verts[iv] = vp;
	  vtol_edge_2d_ref e = DigitalEdge(v, vp);
	  edges.push_back(e); // e->Protect();
	}
    }
  V00->set_x(0);  V00->set_y(0); vertices.push_back(V00); 
  //  V00->Protect();  
  V01->set_x(0);  V01->set_y(ymax+rmax); vertices.push_back(V01);
  //  V01->Protect();  
  V10->set_x(xmax+rmax);  V10->set_y(0); vertices.push_back(V10);
  //  V10->Protect();
  V11->set_x(xmax+rmax);  V11->set_y(ymax+rmax); vertices.push_back(V11);
  //  V11->Protect();
  //4.0 Now we have properly placed vertices.  Next we scan and generate
  //edges. along the border.
  //4.1 along ymin and ymax
  for (d = 0; d < 2; d++) 
    {
      vcl_vector<vtol_vertex_2d_ref>* verts = &ymin_verts;
      if(d)
	verts = &ymax_verts;
      int len = (*verts).size();
      if(len<2)
	{
	  cout <<"In gevd_contour::InsertBorder() - too few vertices" << endl; 
	  return;
	}
      for(int i = 0; i<len-1; i++)
	{
	  vtol_vertex_2d_ref v = (*verts)[i];
	  vtol_vertex_2d_ref vp = (*verts)[i+1];
	  vtol_edge_2d_ref e = DigitalEdge(v, vp);
	  edges.push_back(e); // e->Protect();
	}
    }
  //4.2 along xmin and xmax
  for (d = 0; d < 2; d++) 
    {
      //      int x;
      vcl_vector<vtol_vertex_2d_ref>* verts = &xmin_verts;
      if(d)
	verts = &xmax_verts;
      int len = (*verts).size();
      if(len<2)
	{
	  cout <<"In gevd_contour::InsertBorder() - too few vertices" << endl; 
	  return;
	}
      for(int i = 0; i<len-1; i++)
	{
	  vtol_vertex_2d_ref v = (*verts)[i];
	  vtol_vertex_2d_ref vp = (*verts)[i+1];
	  vtol_edge_2d_ref e = DigitalEdge(v, vp);
	  edges.push_back(e); // e->Protect();
      }
    }
  //  if (talkative)
  //    cout << ", in " << t.real() << " msecs." << endl;
}

// // Convolve array elements with [1 0 1]/2, replacing
// center pixel by average of 2 neighbors.
// This will make the spacing between pixels almost equal
// and prune away small zig-zags.

void
EqualizeElements(float* elmts, const int n,
		 const float v1, const float v2)
{
  float p0 = elmts[0], p1 = elmts[1], p2 = elmts[2]; // setup pipeline
  elmts[0] = (v1 + p1) / 2;	// touching first vertex
  for (int i = 1; i < n-2; i++) {
    elmts[i] = (p0 + p2)/2;
    p0 = p1; p1 = p2; p2 = elmts[i+2]; // faster with circular list
  }
  if (n>1) elmts[n-2] = (p0 + p2)/2;	// last convolution
  if (n>0) elmts[n-1] = (p1 + v2) / 2;	// touching second vertex
}

// -- Make the spacing of the chain pixels nearly equal by
// smoothing their locations with the average filter  [1 0 1]/2.
// This will reduce square grid tesselation artifacts, and
// lead to more accurate estimation of the tangent direction,
// and local curvature angle, from finite differences in location.
// It is also useful to avoid weight artifacts in curve fitting
// caused by the periodic clustering of pixels along the chain.
// Truncating the float locations with int() will no longer map
// to the original pixels of the discrete chains.

void
gevd_contour::EqualizeSpacing(vcl_vector<vtol_edge_2d_ref>& chains)
{
  //  CoolTimer t;
  if (talkative)
    cout << "Equalize the spacing between pixels in chains";
  //  for (chains.reset(); chains.next(); ) {
  for( int i= 0; i< chains.size(); i++)
    {
      vtol_edge_2d_ref e = chains[i];
      vdgl_digital_curve_ref dc = (vdgl_digital_curve*) e->GetCurve();
      const int len = dc->size();
      if (len > 2*MINLENGTH) 
	{	// not necessary for short chains
	  vtol_vertex_2d *v1 = e->v1(), *v2 = e->v2();
	  EqualizeElements(dc->GetX(), len, v1->x(), v2->x());
	  EqualizeElements(dc->GetY(), len, v1->y(), v2->y());
	}
    }

  //  if (talkative)
  //    cout << ", in " << t.real() << " msecs." << endl;
}

// -- Set depth for intensity images, default to 0.

void
gevd_contour::SetDepth(vcl_vector<vtol_edge_2d_ref>& edges,
		  vcl_vector<vtol_vertex_2d_ref>& vertices,
		  float depth)
{
  //  CoolTimer t;
  if (talkative)
    cout << "Set depth for all pixels in edges/vertices to " << depth;
  //  for (vertices.reset(); vertices.next(); )

  for( int i=0; i< vertices.size(); i++)
    vertices[i]->SetZ(depth);

  //  for (edges.reset(); edges.next(); ) {
  for( int i=0; i< edges.size(); i++)
    {
      vdgl_digital_curve_ref dc = (vdgl_digital_curve*) edges[i]->GetCurve();
      float *cz = dc->GetZ();
      for (int k = 0; k < dc->size(); k++)
	cz[k] = depth;
    }

  //  if (talkative)
  //    cout << ", in " << t.real() << " msecs." << endl;
}


// -- For range images, the contours are 3D not just 2D.
// So, insert the depth z at fold edges, by interpolating the depth
// values in the original range image.

void
gevd_contour::InterpolateDepth(vcl_vector<vtol_edge_2d_ref>& edges,
			  vcl_vector<vtol_vertex_2d_ref>& vertices,
			  const gevd_bufferxy& image)
{
  //  CoolTimer t;
  if (talkative)
    cout << "Interpolate depth for all edges/vertices";
  //  for (vertices.reset(); vertices.next(); ) {
  for( int i=0; i< vertices.size(); i++)
    {
      vtol_vertex_2d_ref v = vertices[i];
      v->SetZ(floatPixel(image, int(v->x()), int(v->y())));
    }
  //  for (edges.reset(); edges.next(); ) {
  for( int i=0; i< edges.size(); i++)
    {
      vdgl_digital_curve_ref dc = (vdgl_digital_curve*) edges[i]->GetCurve();
      float *cx = dc->x(), *cy = dc->y(), *cz = dc->GetZ();
      for (int k = 0; k < dc->size(); k++)
	cz[k] = floatPixel(image, int(cx[k]), int(cy[k]));
    }

//  if (talkative)
//     cout << ", in " << t.real() << " msecs." << endl;
}

// -- Translate all the pixels in the edges and vertices by (tx, ty, tz).
// If the image is extracted from an ROI, a translation of
// (roi->GetOrigX(), roi->GetOrigY(), 0) must be done to have
// coordinates in the reference frame of the original image.
// Add 0.5 if you want to display location at center of pixel
// instead of upper-left corner.

void
gevd_contour::Translate(vcl_vector<vtol_edge_2d_ref>& edges, // translate loc to center
		   vcl_vector<vtol_vertex_2d_ref>& vertices,
		   const float tx, const float ty, const float tz)
{
  //  CoolTimer t;
  if (talkative)
    cout << "Translate edges/vertices";
  //  for (vertices.reset(); vertices.next(); ) {
  for( int i=0; i< vertices.size(); i++) {
    vtol_vertex_2d_ref vert = vertices[i];
    vert->set_x(vert->x() + tx);
    vert->set_y(vert->y() + ty);
  }
  //  for (edges.reset(); edges.next(); ) {
  for( int i=0; i< edges.size(); i++) {
    vtol_edge_2d_ref edge = edges[i];
    vdgl_digital_curve_ref dc = (vdgl_digital_curve*) edge->GetCurve();
    float *cx = dc->GetX(), *cy = dc->GetY();
    for (int k = 0; k < dc->size(); k++) {
      cx[k] += tx; 
      cy[k] += ty;
    }
  }
  if (tz != 0) {
    //  for (vertices.reset(); vertices.next(); ) {
    for( int i=0; i< vertices.size(); i++)
      {
	vtol_vertex_2d_ref vert = vertices[i];
	vert->SetZ(vert->GetZ() + tz);
      }
    //  for (edges.reset(); edges.next(); ) {
    for( int i=0; i< edges.size(); i++) {
      vtol_edge_2d_ref edge = edges[i];
      vdgl_digital_curve_ref dc = (vdgl_digital_curve*) edge->GetCurve();
      float* cz = dc->GetZ();
      for (int k = 0; k < dc->size(); k++)
	cz[k] += tz;
    }
  }
  //  if (talkative)
  //    cout << ", in " << t.real() << " msecs." << endl;
}

// -- Remove and delete all elements in global lists, and set
// the global lists to NULL. Remove all digital chains of edges.
// Edges and vertices are removed with UnProtect().

void
gevd_contour::ClearNetwork(vcl_vector<vtol_edge_2d_ref>*& edges,
		      vcl_vector<vtol_vertex_2d_ref>*& vertices)
{
  if (edges) {
    // for (edges->reset(); edges->next(); ) {
    for( int i=0; i< edges.size(); i++) {
      vtol_edge_2d_ref edge = edges[i];
      Curve* dc = NULL;        // retrieve digital chain dc
      edge->SetCurve(NULL, dc); // and remove it from edge
      //      edge->UnProtect();        // delete edge 
      //      delete ((vdgl_digital_curve_ref) dc); // delete dc
    }
    delete edges; edges = NULL;
  }
  if (vertices) {
    //for (vertices->reset(); vertices->next(); )
    //    for( int i=0; i< vertices.size(); i++)
    //      vertices[i]->UnProtect();
    delete vertices; vertices = NULL;
  }
}


// -- Create a mapping from image pixel to edges.

gevd_bufferxy*
gevd_contour::CreateEdgeMap(vcl_vector<vtol_edge_2d_ref>& edges,
		       const int sizex, const int sizey)
{
  gevd_bufferxy* map = new gevd_bufferxy(sizex, sizey, bits_per_ptr);
  map->Clear();
  //  for (edges.reset(); edges.next(); ) {
  for( int i= 0; i< edges.size(); i++) {
    vtol_edge_2d_ref e = edges[i];
    vdgl_digital_curve_ref dc = (vdgl_digital_curve*) e->GetCurve();
    const float *x = dc->GetX(), *y = dc->GetY();
    for (int i = 0; i < dc->size(); i++)
      ptrPixel(*map, int(x[i]+0.5), int(y[i]+0.5)) = e;
  }
  return map;
}


// -- Mask the detected edge elements and junctions with a given
// mask array, using AND operation, for ROI with arbitrary shapes.

void
gevd_contour::MaskEdgels(const gevd_bufferxy& mask,
		    gevd_bufferxy& edgels, // edge elements AND with mask
		    int& njunction, // junctions AND with mask
		    int* junctionx, int* junctiony)
{
  int x, y;
  for (y = 0; y < edgels.GetSizeY(); y++)
    for (x = 0; x < edgels.GetSizeX(); x++)
      if (floatPixel(edgels, x, y) && // is edge element
	  !bytePixel(mask, x, y)) // is not in mask
	floatPixel(edgels, x, y) = 0; // remove edgel not in mask
  int j = 0;
  for (int i = 0; i < njunction; i++) {
    x = junctionx[i], y = junctiony[i];
    if (bytePixel(mask, x, y)) { // keep junction in mask
      junctionx[j] = x, junctiony[j] = y, j++;
    }
  }
  njunction = j;
}


// Set the orientation at each edgel on all digital curves to a continous
// orientation value, which is consistent with C. Rothwell's EdgeDetector.
// That is theta = (180/M_PI)*atan2(dI/dy, dI/dx)
//

void
gevd_contour::SetEdgelData(gevd_bufferxy& grad_mag, gevd_bufferxy& angle, vcl_vector<vtol_edge_2d_ref>& edges)
{
  //  for(edges.reset(); edges.next();)
  for( int i=0; i< edges.size(); i++)
    {
      vtol_edge_2d_ref e = edges[i];
      vdgl_digital_curve_ref dc = (e->GetCurve())->CastTovdgl_digital_curve();
      if(dc)
	{
	  int len = dc->size();
	  float* xpos = dc->GetX();
	  float* ypos = dc->GetY();
	  float* th = dc->GetTheta();
	  float* gr = dc->GetGrad();
	  for(int i = 0; i < len; i++)
	    {
	      int ix = int(xpos[i]);
	      int iy = int(ypos[i]);

	      // Debugging : RIH
	      // Routine crashes with iy < 0.
	      if (iy < 0 || ix < 0 || 
	   	  ix >= grad_mag.GetSizeX() || 
		  iy >= grad_mag.GetSizeY())
	         {
         	 cerr << "***********  ERROR  : (ix, iy) = (" << 
			ix << ", " << iy << ")" << endl;
		 if (ix < 0) ix = 0;
	      	 if (iy < 0) iy = 0;
	 	 if (ix >= grad_mag.GetSizeX()) ix = grad_mag.GetSizeX()-1;
	 	 if (iy >= grad_mag.GetSizeY()) iy = grad_mag.GetSizeY()-1;
	         }

	      gr[i] = floatPixel(grad_mag, ix, iy);
	      th[i] = floatPixel(angle, ix, iy);
	    }
	}
    }    
}


// - Find closed regions from connected boundaries, and
// eliminate bridge or dangling edges/vertices from network.
// Return number of closed regions found.

// int
// gevd_contour::ClosedRegions(vcl_vector<Edge*>& edges,
// 		       vcl_vector<vtol_vertex_2d_ref>& vertices)
// {
//   return 0;
// }



// -- Compare function to sort the edges by their length in pixels,
// largest first.

int
gevd_contour::LengthCmp(vtol_edge_2d_ref const& dc1, vtol_edge_2d_ref const& dc2)
{
  vdgl_digital_curve_ref c1 = (vdgl_digital_curve*) ((vtol_edge_2d_ref)dc1)->GetCurve();
  vdgl_digital_curve_ref c2 = (vdgl_digital_curve*) ((vtol_edge_2d_ref)dc2)->GetCurve();
  return c2->size() - c1->size();
}



// -- Create a 2-way lookup table from list elements in set,
// using array and get_id/set_id.

vcl_vector<vtol_edge_2d_ref>*
gevd_contour::CreateLookupTable(vcl_vector<vtol_edge_2d_ref>& set)
{
  vcl_vector<vtol_edge_2d_ref>* set2 =
    new vcl_vector<vtol_edge_2d_ref>(2*set.size()); // preallocate space
  //  set2->set_growth_ratio(1.5);	// growth ratio for table
  //  for (set.reset(); set.next(); )
  for( int i=0; i< set.size(); i++)
    gevd_contour::LookupTableInsert(*set2, set[i]);
  return set2;
}

// -- As above for vertices.

vcl_vector<vtol_vertex_2d_ref>*
gevd_contour::CreateLookupTable(vcl_vector<vtol_vertex_2d_ref>& set)
{
  vcl_vector<vtol_vertex_2d_ref>* set2 =
    new vcl_vector<vtol_vertex_2d_ref>(2*set.size()); // preallocate space
  //  set2->set_growth_ratio(1.5);	// growth ratio for table
  //  for (set.reset(); set.next(); )
  for( int i=0; i< set.size(); i++)
    gevd_contour::LookupTableInsert(*set2, set[i]);
  return set2;
}

// -- Insert topology object in 2-way lookup table,
// using Id and dynamic array. Protect it in the network.

void
gevd_contour::LookupTableInsert(vcl_vector<vtol_edge_2d_ref>& set,
			   vtol_edge_2d_ref elmt)
{
  //  elmt->Protect();		// reference count
  elmt->set_id(set.size());	// index in global array
  set.push_back(elmt);		// push_back at end of array
}

// -- As above for vertices.

void
gevd_contour::LookupTableInsert(vcl_vector<vtol_vertex_2d_ref>& set,
			   vtol_vertex_2d_ref elmt)
{
  //  elmt->Protect();		// reference count
  elmt->set_id(set.size());	// index in global array
  set.push_back(elmt);		// push at end of array
}

// -- Replace deleted by inserted in 2-way lookup table.
// Also remove object from the network.

void
gevd_contour::LookupTableReplace(vcl_vector<vtol_edge_2d_ref>& set,
			    vtol_edge_2d_ref deleted, vtol_edge_2d_ref inserted)
{
  //  inserted->Protect();
  const int i = deleted->get_id();
  inserted->set_id(i);
  set[i] = inserted;		// replace in global array
  deleted->unlink_all_inferiors_twoway(deleted);
  //  deleted->UnProtect();		// reference count
}

// -- As above for vertices.

void
gevd_contour::LookupTableReplace(vcl_vector<vtol_vertex_2d_ref>& set,
			    vtol_vertex_2d_ref deleted, vtol_vertex_2d_ref inserted)
{
  //  inserted->Protect();
  const int i = deleted->get_id();
  inserted->set_id(i);
  set.put(inserted, i);
  set[i] = inserted;		// replace in global array
  //  deleted->UnProtect();		// reference count
}

// -- Remove topology object from 2-way lookup table
// leaving a empty hole. Also remove object from the network.

void
gevd_contour::LookupTableRemove(vcl_vector<vtol_edge_2d_ref>& set,
			   vtol_edge_2d_ref elmt)
{
  set[elmt->get_id()] = NULL;	// remove from global array
  //  elmt->UnProtect();		// reference count
}

// -- As above for vertices.

void
gevd_contour::LookupTableRemove(vcl_vector<vtol_vertex_2d_ref>& set,
			   vtol_vertex_2d_ref elmt)
{
  set[elmt->get_id()] = NULL; // remove from global array
  //  elmt->UnProtect();		// reference count
}

// -- Eliminate empty holes in the lookup table.

void
gevd_contour::LookupTableCompress(vcl_vector<vtol_edge_2d_ref>& set)
{
  int i = 0;
  for (int j = set.size()-1; i <= j; i++)
    if (!set[i]) {	// find empty hole
      vtol_edge_2d_ref last = NULL;
      for (; i < j; j--)
	if (set[j]) {
	  last = set[j]; j--; // remove from the end
	  break;
	}
      if (last) {
	last->set_id(i);		// move it to the front
	set[i] = last;
      } else
	break;			// no more elements
    }
  set.set_length(i);		// reset the length
}

// -- As above for vertices.

void
gevd_contour::LookupTableCompress(vcl_vector<vtol_vertex_2d_ref>& set)
{
  int i = 0;
  for (int j = set.size()-1; i <= j; i++)
    if (!set[i]) {		// find empty hole
      vtol_vertex_2d_ref last = NULL;
      for (; i < j; j--)
	if (set[j]) {
	  last = set[j]; j--; // remove from the end
	  break;
	}
      if (last) {
	last->set_id(i);		// move it to the front
	set[i] = last;
      } else
	break;			// no more elements
    }
  set.set_length(i);		// reset the length
}

// -- Check a few obvious invariants, and return number of errors.
// 0. Network has closure of all vertices and edges.
// 1. No 2 vertices touch: endpt/endpt, endpt/junction or junction/junction
// 2. No vertex connecting 2 edges, each vertex has 1 or >= 3 edges,
//    except the 4 corners of image border.
// 3. Each edge has >= 3 internal pixels.
// 4. A chain can touch/join with itself or with a stronger chain.
//    Junction is created only if the local change in filter response
//    is greater than some noise threshold, set by the user.
//    Junction is created only if the 2 broken up chains have
//    lengths >= 3.

int
gevd_contour::CheckInvariants(vcl_vector<vtol_edge_2d_ref>& edges, 
			 vcl_vector<vtol_vertex_2d_ref>& vertices)
{
  int nerror = 0;
  
  // 0. Check that vertices of all edges have been listed
  const int unmark = -1;
  //  for (edges.reset(); edges.next(); ) // unmark all network
  for( int i=0; i< edges.size(); i++)
    edges[i]->set_id(unmark);
  //  for (vertices.reset(); vertices.next(); )
  for( int i=0; i< edges.size(); i++)
    vertices[i]->set_id(unmark);  
  //  for (edges.reset(); edges.next(); ) {	// check vs of all edges
  for( int i=0; i< edges.size(); i++) {
    vtol_edge_2d_ref e = edges[i];
    vtol_vertex_2d_ref v1 = e->v1();
    if (v1->get_id() != unmark) {
      cout << *v1 << ": v1 is not in vertex list" << endl;
      nerror++;
    }
    vtol_vertex_2d_ref v2 = e->v2();
    if (v2->get_id() != unmark) {
      cout << *v2 << ": v2 is not in vertex list" << endl;
      nerror++;
    }
  }
  //  for (vertices.reset(); vertices.next(); ) {	// check es of all vertices
  for( int i=0; i< vertices.size(); i++) {
    vcl_vector<vtol_edge_2d_ref>* es = vertices[i]->Edges();
    //    for (es->reset(); es->next(); )
    for( int j=0; j< es.size(); j++) {
      if (es[j]->get_id() != unmark) { 
	cout << *es->value() << ": e is not in edge list" << endl;
	nerror++;
      }
  }
  int id;			// mark id with index in global list
  //  for (edges.reset(), id = 0; edges.next(); id++) // mark all network
  for( int i=0; i< edges.size(); i++)
    edges[i]->set_id(id);
  //  for (vertices.reset(), id = 0; vertices.next(); id++)
  for( id=0; id< vertices.size(); id++)
    vertices[id]->set_id(id);  
  
  return nerror;
}

