// This is brl/bseg/sdet/sdet_contour.cxx
#include "sdet_contour.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>   // for vcl_abs(int) and vcl_qsort()
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h> // for vcl_max()
#include <vul/vul_timer.h>
#include <vxl_config.h>
#include <vnl/vnl_math.h> // for sqrt2
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <btol/btol_vertex_algs.h>
#include <btol/btol_edge_algs.h>
#include <bdgl/bdgl_curve_algs.h>
#include <gevd/gevd_bufferxy.h>
#include <gevd/gevd_pixel.h>

#ifdef DEBUG
 bool sdet_contour::talkative_ = true;
 bool sdet_contour::debug_ = true;      // Print extensive debug messages
#else
 bool sdet_contour::talkative_ = false; // By default contour is not silent.
 bool sdet_contour::debug_ = false;
#endif

const int INVALID = -1;

// Use 8 directions, with 45 degree angle in between them.

const vxl_byte TWOPI = 8, /* FULLPI = 4, */ HALFPI = 2 /* , QUARTERPI = 1 */;
//const vxl_byte DIR0 = 8, DIR1 = 9, DIR2 = 10, DIR3 = 11;
const int DIS[] = { 1, 1, 0,-1,-1,-1, 0, 1, // 8-connected neighbors
                    1, 1, 0,-1,-1,-1, 0, 1, // wrapped by 2PI to
                    1, 1, 0,-1,-1,-1, 0, 1};// avoid modulo operations.
const int DJS[] = { 0, 1, 1, 1, 0,-1,-1,-1,
                    0, 1, 1, 1, 0,-1,-1,-1,
                    0, 1, 1, 1, 0,-1,-1,-1};

//const int RDS[] = {0,-1, 1,-2, 2,-3, 3,-4, 4,-5, 5}; // radial search
const int RIS[] = { 1, 0,-1, 0, // spiral search for 4/8-connected
                    1,-1,-1, 1, // neighbors
                    2, 0,-2, 0,
                    2, 1,-1,-2,
                    -2,-1, 1, 2,
                    2,-2,-2, 2,
                    3, 0,-3, 0,
                    3, 1,-1,-3,
                    -3,-1, 1, 3,
                    3, 2,-2,-3,
                    -3,-2, 2, 3,
                    4, 0,-4, 0};
const int RJS[] = { 0, 1, 0,-1, // rotate CW, increasing radius
                    1, 1,-1,-1,
                    0, 2, 0,-2,
                    1, 2, 2, 1,
                    -1,-2,-2,-1,
                    2, 2,-2,-2,
                    0, 3, 0,-3,
                    1, 3, 3, 1,
                    -1,-3,-3,-1,
                    2, 3, 3, 2,
                    -2,-3,-3,-2,
                    0, 4, 0,-4};
const int RNS[] = { 4, 8, 12, 20, 24, 28, 36, 44, 48}; // at distinct r
const float RGS[] = { 1.f, 1.414213f, 2.f, 2.236067f, 2.828427f, // values of gap
                      3.f, 3.162277f, 3.605551f, 4.f};

// - win32 - moved to here for MSVC++
const int MINLENGTH = 3;        // minimum number of pixels for a chain
const int FRAME = 4;            // border of image

//: A container to support sorting of edge lengths
// Will result in descending order according to length
struct sdet_contour_edge
{
  sdet_contour_edge () {};

  void set_edge(vtol_edge_2d_sptr const& e) {e_ = e;}
  vtol_edge_2d_sptr edge() {return e_;}

  double length() {return e_->curve()->length();}

 private:
  vtol_edge_2d_sptr e_;
};

//The sort compare function
static int compare(sdet_contour_edge*  ea,
                   sdet_contour_edge*  eb)
{
  if (ea->length() < eb->length())
    return +1;
  return -1;
}

//: Save parameters and create workspace for detecting contours.
// Each contour must have at least 1 pixel above min_strength,
// and its number of internal pixels must be above min_length.
// This is a heuristic hysteresis scheme that prunes weak or short
// isolated chains.
// To join a weaker contour to a stronger contour, a junction must
// have a change in response above min_jump on the stronger contour.
// This way, only strong junctions are detected.
sdet_contour::sdet_contour(float min_strength, int min_length,
                           float min_jump, float max_gap_in)
  : minStrength(min_strength), minLength(min_length),
    minJump(min_jump), maxSpiral(0),
    edgeMap(), vertexMap()
{
  if (minStrength < 0)
  {
    vcl_cerr << "sdet_contour::sdet_contour -- negative min_strength: "
             << minStrength << ". Reset to 0.\n";
    minStrength = 0;
  }
  if (minLength < MINLENGTH)
  {
    vcl_cerr << "sdet_contour::sdet_contour -- too small min_length: "
             << minLength << ". Reset to " << MINLENGTH << ".\n";
    minLength = MINLENGTH;
  }
  if (minJump < 0)
  {
    vcl_cerr << "sdet_contour::sdet_contour -- negative min_jump: "
             << minJump << ". Reset to 0.\n";
    minJump = 0;
  }
  if (minJump > minStrength)
  {
    vcl_cerr << "sdet_contour::sdet_contour -- too large min_jump: "
             << min_jump << ". Reset to " << minStrength << ".\n";
    minJump = minStrength;
  }
  if (max_gap_in < 1)
  {
    vcl_cerr << "sdet_contour::sdet_contour -- too small max_gap: "
             << max_gap_in << ". Reset to 1.\n";
    max_gap_in = 1;
  }
  if (max_gap_in > FRAME)
  {
    vcl_cerr << "sdet_contour::sdet_contour -- too large max_gap: "
             << max_gap_in << ". Reset to " << FRAME << vcl_endl;
    max_gap_in = FRAME;
  }
  max_gap = max_gap_in;
  for (int i = 0; i < 9; i++)   // find number of neighbors to search
    if (max_gap <= RGS[i])      // for given gap radius
      maxSpiral= i+1;
}


//: Free space allocated for detecting contours.
sdet_contour::~sdet_contour()
{
  delete edgeMap;               // space shared by LinkJunction/Chain
  delete vertexMap;
}


//: Find network of linked edges and vertices, from 8-connected edge elements.
// The contours must be less than 2 pixel wide,
// for example found from non maximum suppression.
// Isolated edgels and short segments are erased.
bool
sdet_contour::FindNetwork(gevd_bufferxy& edgels, bool junctionp,
                          const int njunction,
                          const int* junctionx, const int* junctiony,
                          vcl_vector<vtol_edge_2d_sptr>*& edges,
                          vcl_vector<vtol_vertex_2d_sptr >*& vertices)
{
  // make sure that if no edges are found that edges and vertices
  // get values, to avoid seg faults, WAH
  if (!edges)
    edges = new vcl_vector<vtol_edge_2d_sptr>;
  else
    edges->clear();
  if (!vertices)
    vertices = new vcl_vector<vtol_vertex_2d_sptr >;
  else
    vertices->clear();


  if (this->talkative_)
    vcl_cout << "*** Link edge elements into connected edges/vertices.\n";

  // 1. Setup lookup maps based on (x,y) integer location.
  vertexMap = new vbl_array_2d<vtol_vertex_2d_sptr>(edgels.GetSizeX(), edgels.GetSizeY());
  vertexMap->fill(NULL);
  edgeMap = new vbl_array_2d<vtol_edge_2d_sptr>(edgels.GetSizeX(), edgels.GetSizeY());
  edgeMap->fill(NULL);

  // 2. Collect 4/8-connected pixels into chains
  int n; // = vcl_max(10*njunction, // preallocated size from junctions or
  //       edgels.GetSizeX()*edgels.GetSizeY()/100); // image size
  vcl_vector<vtol_edge_2d_sptr> *edges2 = new vcl_vector<vtol_edge_2d_sptr>;
  n = this->FindChains(edgels, // link pixels into chains
                       njunction, // also use junction pixels
                       junctionx, junctiony,
                       *edges2);
  if (!n)
    return false;               // empty network
  if (junctionp) {
    // 3. Sort chains longest first.
    if (edges2->size() < 10000)     // don't sort if too many edges
    {
      sdet_contour_edge* edge_array = new sdet_contour_edge[edges2->size()];
      int i =0;
      for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges2->begin();
           eit != edges2->end(); eit++,i++)
      {
        edge_array[i].set_edge(*eit);
      }
      vcl_qsort(edge_array, i, sizeof(sdet_contour_edge) ,
                (int (*)(const void *, const void *))&compare);
      edges2->clear();
      for (int j = 0; j<i; j++)
        edges2->push_back(edge_array[j].edge());
      delete [] edge_array;
    }

    // renumber with order in array
    for (unsigned int i= 0; i< edges2->size(); i++)
      (*edges2)[i]->set_id(i);


    // 4. Split/Merge chains from touching end points
    vcl_vector<vtol_vertex_2d_sptr > vertices2;

    this->FindJunctions(edgels, // break/merge at junctions of
                        *edges2, vertices2); // distinct chains
    for (unsigned int i=0; i< vertices2.size(); i++)
      vertices->push_back( vertices2[i]);
  }

  // 5. Copy back results into global lists
  for (unsigned int i= 0; i< edges2->size(); i++)
    edges->push_back( (*edges2)[i]);

  //eliminate memory leaks
  edges2->clear();
  delete edges2;

  return true;
}


//: Return TRUE if pixel is a local maximum, and so is right on top of contour.
static bool
on_contour(const gevd_bufferxy& edgels, const int i, const int j)
{
  double pix = (1 + vnl_math::sqrt2) * floatPixel(edgels, i, j); // fuzzy threshold
  for (vxl_byte dir = 0; dir < TWOPI; dir += HALFPI) // 4-connected only
    if (floatPixel(edgels, i+DIS[dir], j+DJS[dir]) > pix)
      return false;             // should choose neighbor instead
  return true;
}


//: Delete pixel from contour, and save its location in xloc/yloc.
static void
RecordPixel(int i, int j, gevd_bufferxy& edgels,
            vcl_vector<int>& iloc, vcl_vector<int>& jloc)
{
  //JLM
  // This doesn't work if we record the same pixel twice
  //  floatPixel(edgels, i, j) = -floatPixel(edgels, i, j); // flip sign
  //  The following asserts that a pixel has been used and subsequent
  //  applications of RecordPixel cannot undo the used state.
  floatPixel(edgels, i, j) = -1;
  iloc.push_back(i), jloc.push_back(j);
#ifdef DEBUG
  if (sdet_contour::debug_)
    vcl_cout << "Recording (" << i << ' ' << j << ")\n";
#endif
}

//:
// Delete the last pixel added to iloc and jloc
//
void
ErasePixel(vcl_vector<int>& xloc, vcl_vector<int>& yloc)
{
  vcl_vector<int>::iterator xit = xloc.end();
  vcl_vector<int>::iterator yit = yloc.end();
  xloc.erase(xit-1);
  yloc.erase(yit-1);
}


//:
// Find next best pixel on contour, searching for strongest response,
// and favoring 4-connected over 8-connected.
// Return 0, if no pixel is found, or direction in range [2*pi, 4*pi).
static int
NextPixel(int& i, int& j, const gevd_bufferxy& edgels)
{
  float maxpix = 0, npix;
  int maxdir = 0, dir;
  for (dir = 0; dir < TWOPI; dir += HALFPI) // 4-connected first
    if ((npix = floatPixel(edgels, i+DIS[dir], j+DJS[dir])) > maxpix)
    {
      maxpix = npix;
      maxdir = dir+TWOPI;
    }
  if (!maxdir)
  {
    for (dir = 1; dir < TWOPI; dir += HALFPI) // 8-connected next
      if ((npix = floatPixel(edgels, i+DIS[dir], j+DJS[dir])) > maxpix)
      {
        maxpix = npix;
        maxdir = dir+TWOPI;
      }
  }
  if (maxdir)                   // update next strongest pixel
    i += DIS[maxdir], j += DJS[maxdir];
  return maxdir;
}


//:
// Find next best pixel on contour, searching for strongest response,
// and favoring 4-connected over 8-connected.
// Return 0, if no pixel is found, or direction in range [2*pi, 4*pi).
static int
next_pixel(int& i, int& j, const vbl_array_2d<vtol_vertex_2d_sptr>& vertexMap)
{
  int maxdir = 0, dir;
  for (dir = 0; dir < TWOPI; dir += HALFPI) // 4-connected first
    if (vertexMap.get(i+DIS[dir], j+DJS[dir]))
    {
      maxdir = dir+TWOPI;
      break;
    }
  if (!maxdir)
  {
    for (dir = 1; dir < TWOPI; dir += HALFPI) // 8-connected next
      if (vertexMap.get(i+DIS[dir], j+DJS[dir]))
      {
        maxdir = dir+TWOPI;
        break;
      }
  }
  if (maxdir)                   // update next strongest pixel
    i += DIS[maxdir], j += DJS[maxdir];
  return maxdir;
}


//:
// Trace and collect pixels on thin contours, stronger pixels first,
// and favoring 4-connected over 8-connected. Thinning is not used,
// and so will avoid errors because of square grid tessellation.
// A chain can not cross itself. It can only touch itself or another
// chain, in which case a junction will be found later.
// The pixels of a chain include the 2 end points.
// End points and junctions are created in sdet_contour::FindJunctions.
// Return the number of chains found.  Protected.
int
sdet_contour::FindChains(gevd_bufferxy& edgels, const int njunction,
                         const int* junctionx, const int* junctiony,
                         vcl_vector<vtol_edge_2d_sptr>& edges)
{
  vul_timer t;

  // 1. Save away detected junctions from extending at end points of
  // contours, without linking these contours up. This avoids random
  // order in the traversal of the contours.
  vtol_vertex_2d_sptr mark = new vtol_vertex_2d;      // dummy non zero pointer
  for (int k = 0; k < njunction; k++)
    vertexMap->put(junctionx[k], junctiony[k], mark);

  // 2. Trace elongated & thinned chains, stronger pixels first.
  // Virtual border of image should be inserted last.
  const int rmax = FRAME;
  const int xmax = edgels.GetSizeX()-rmax-1;
  const int ymax = edgels.GetSizeY()-rmax-1;
  vcl_vector<int> xloc(xmax+ymax), yloc(xmax+ymax); // work space for

  for (int j = rmax; j <= ymax; j++)
    for (int i = rmax; i <= xmax; i++)
    {
      // 2.0. Start from better pixels above noise+hysteresis
      if (floatPixel(edgels, i, j) > minStrength &&
          on_contour(edgels, i, j))    // right on the contour
      {
        int x = i, y = j;

        // 2.1. Prune isolated pixels
        if (!NextPixel(x, y, edgels))  // prune isolated pixels
        {
          floatPixel(edgels, i, j) = 0;
          continue;
        }
        // 2.2. Start collecting first 3 pixels
        xloc.clear(), yloc.clear(); // collect pixels on contour
        RecordPixel(i, j, edgels, xloc, yloc);  // first pixel
        int ii = x, jj = y;
        RecordPixel(ii, jj, edgels, xloc, yloc); // second pixel
        if (NextPixel(x, y, edgels))
          RecordPixel(x, y, edgels, xloc, yloc); // third pixel
        else {                  // reach end point
          x = i, y = j;         // revert back to start pt
          if (NextPixel(x, y, edgels)) { // reverse collection
            xloc.clear(), yloc.clear();
            RecordPixel(ii, jj, edgels, xloc, yloc); // second pixel
            RecordPixel(i, j, edgels, xloc, yloc); // first pixel
            RecordPixel(x, y, edgels, xloc, yloc); // third pixel
            ii = i, jj = j;
          } else  {             // reach other end point
            floatPixel(edgels, i, j) = 0; // prune isolated pixel-pairs
            floatPixel(edgels, ii, jj) = 0;
            continue;
          }
        }

        // 2.3. Watch out for zig-zag at 2nd pixel, from LR-TD scans
        //Check to see if the contour doubles back on itself
        //if so, eliminate the middle pixel causing the jag
        int dprod = (x - ii)*(ii - xloc[0]) + (y - jj)*(jj - yloc[0]);
        if (dprod < 0)
        {
          if (sdet_contour::debug_)
            vcl_cout << "dps(" << xloc[0] << ' ' << yloc[0] << ")(" << xloc[1]
                     << ' ' << yloc[1] << ")(" << x << ' ' << y << ")= "
                     << dprod << '\n' << vcl_flush;
          //Replace the 2nd pixel in the chain with x-y
          ErasePixel(xloc, yloc);
          ErasePixel(xloc, yloc);
          RecordPixel(x, y, edgels, xloc, yloc);
        }
        // 2.4. Collect both directions & extension points if 1-chain
        // trace along first dir, 4-connected and stronger first
        // Scanning forward --->
        int niii = ii, njjj = jj;//the i-2 pixel
        int nii = x, njj=y;
        while (NextPixel(x, y, edgels))
        {
          //Check to see if the contour doubles back on itself
          //if so, eliminate the middle pixel causing the jag
          dprod = (x - nii)*(nii - niii) + (y - njj)*(njj - njjj);
          if (dprod < 0)
          {
            if (sdet_contour::debug_)
              vcl_cout << "dpf(" << niii << ' ' << njjj << ")(" << nii << ' '
                       << njj << ")(" << x << ' ' << y << ")= " << dprod
                       << '\n' << vcl_flush;
            //Erase the last pixel and replace it with (x, y)
            ErasePixel(xloc,  yloc);
            RecordPixel(x, y,edgels, xloc, yloc);
          }
          else
            RecordPixel(x, y, edgels, xloc, yloc);
          niii = nii; njjj = njj;
          nii = x; njj = y;
        }
        // disjoint first/last pixel
        // so must be an open chain with different end points
        if (vcl_abs(xloc[0]-x) > 1 ||
            vcl_abs(yloc[0]-y) > 1)
        {
          // include a vertex location if
          // there was one detected at the end of the chain
          if (next_pixel(x, y, *vertexMap))
            xloc.push_back(x), yloc.push_back(y);

          // start again from first pixel

          x = xloc[0], y = yloc[0];

          //reversing the vectors, xloc and yloc
          vcl_vector<int> xloctemp( xloc.size()), yloctemp( yloc.size());
          for (unsigned int iii=0; iii< xloc.size(); iii++)
            xloctemp[iii]= xloc[xloc.size()-1-iii];
          for (unsigned int jjj=0; jjj< yloc.size(); jjj++)
            yloctemp[jjj]= yloc[yloc.size()-1-jjj];

          //now copy the reversed vector back into xloc, yloc.
          for (unsigned int jk=0; jk<xloc.size(); jk++)
            xloc[jk]=xloctemp[jk];
          for (unsigned int jk=0; jk<yloc.size(); jk++)
            yloc[jk]=yloctemp[jk];

          // Scanning backwards from the first point <----
          int il = xloc.size();
          nii = xloc[il-1];
          njj = yloc[il-1];
          niii = xloc[il-2];
          njjj = yloc[il-2];
          while (NextPixel(x, y, edgels)) // trace along other dir
          {
            //Check to see if the contour doubles back on itself
            //if so, eliminate the middle pixel causing the jag
            dprod = (x - nii)*(nii - niii)+(y - njj)*(njj - njjj);
            if (dprod < 0)
            {
              if (sdet_contour::debug_)
                vcl_cout << "dpr(" << niii << ' ' << njjj << ")(" << nii << ' '
                         << njj << ")(" << x << ' ' << y << ")= " << dprod
                         << '\n' << vcl_flush;
              //Erase the last pixel and replace it with (x, y)
              ErasePixel(xloc, yloc);
              RecordPixel(x, y, edgels, xloc, yloc);
            }
            else
              RecordPixel(x, y, edgels, xloc, yloc);
            niii = nii; njjj = njj;
            nii = x; njj = y;
          }
          // add in an edgel for the junction at the end
          // if it exists.
          if (next_pixel(x, y, *vertexMap))
            xloc.push_back(x), yloc.push_back(y); // detected junctions
        }
        int len = xloc.size();
        // 2.5. Check for isolated contours that are too short
        if (len < minLength) {  // zero or too few internal pixels
          for (int k = 0; k < len; k++) // zero or too few internal pixels
            floatPixel(edgels, xloc[k], yloc[k]) = 0; // prune short chains
          continue;
        }

        // 2.6. Create topological network of chains, touching,
        //      possibly ending at same junction, but never
        //      crossing one another
        vtol_edge_2d_sptr edge = new vtol_edge_2d();
        vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain;
        vdgl_interpolator_sptr it = new vdgl_interpolator_linear(ec);
        vdgl_digital_curve_sptr dc = new vdgl_digital_curve(it);

        for (int k=0; k< len; k++)
        {
          x= xloc[k];
          y= yloc[k];
          ec->add_edgel( vdgl_edgel( x, y));
          edgeMap->put(x, y, edge);
        }
        edge->set_curve(*dc->cast_to_curve());
        LookupTableInsert(edges, edge);
      }
    }
  // 3. Restore cache to original state
  //    Placeholder vertices had been added to
  //    flag junctions but will be replaced with
  //    actual vertices in a later step
  for (int k = 0; k < njunction; k++)  // clear all void*/float labels
    vertexMap->put(junctionx[k], junctiony[k],NULL);
  for (int j = rmax; j <= ymax; j++)
    for (int i = rmax; i <= xmax; i++)
      if (floatPixel(edgels, i, j) < 0) // undo marks placed by RecordPixel
        floatPixel(edgels, i, j) = - floatPixel(edgels, i, j);

  if (talkative_)
    vcl_cout << "Find " << edges.size()
             << " chains/cycles, with pixels > " << minLength
             << " and strength > " << minStrength
             << ", in " << t.real() << " msecs.\n";

  return edges.size();  // number of chains found so far
}


//:
// The inputs are: endv, edgels, maxSpiral, and edgeMap.
// The outputs are: index, weaker and stronger.
// endv is a vertex corresponding to a dangling end of an edge.
// i.) If the end vertex is bounding more than one edge, the routine
//     returns false.
// ii.) Otherwise a spiral search is carried out around the end with a radius
// given by maxSpiral.  Some of the nearby points on the edge connected to endv,
// are erased so that they are not found in the search.
// iii.) The edgel with maximum strength is found.  If there is none, the
//       routine returns false.
// iv.) The edge containing the found edgel is called "stronger" and the
//      the location on that edge where the edgel was found is "index"
bool sdet_contour:: DetectJunction(vtol_vertex_2d_sptr const& endv, int& index,
                                   vtol_edge_2d_sptr& weaker,
                                   vtol_edge_2d_sptr& stronger,
                                   const int maxSpiral,
                                   const gevd_bufferxy& edgels)
{
  // 0. Must be an end point of a dangling 1-chain
  if (endv->numsup() > 1)         // avoid junction and 1-cycle
    return false;
  vcl_vector<vtol_edge_sptr>* edges = endv->edges();
  weaker = (*edges)[0]->cast_to_edge_2d();      // dangling edge must be a weaker contour
  delete edges;
  vdgl_digital_curve_sptr dc = weaker->curve()->cast_to_vdgl_digital_curve();

  const int len = dc->get_interpolator()->get_edgel_chain()->size();

  // 1. Mark off pixels at end pt to find junction of a contour to itself
  //Erase the edge pointers for rfuzz positions in the edgeMap corresponding
  //to the last rfuzz edgels in the weaker edge.  It looks like "labels"
  //caches the old edge pointers
  const int rfuzz = vcl_min(len, 3*MINLENGTH);
  vtol_edge_2d_sptr* labels = new vtol_edge_2d_sptr[rfuzz];
  //make sure the "end" of the edge corresponds to the correct end of the
  //edgel chain, i.e. "endv" might be either v1 or v2 -- the first or last
  //point on the edgel chain.
  if (endv == weaker->v1()->cast_to_vertex_2d())    //erase the first part
    for (int r = 0; r < rfuzz; r++)
    {
      vdgl_edgel edgel= dc->get_interpolator()->get_edgel_chain()->edgel( r);
      labels[r] = edgeMap->get( int(edgel.get_x()), int(edgel.get_y()));
      edgeMap->put(int(edgel.get_x()), int(edgel.get_y()), NULL);
    }
  else //erase the last part
    for (int r = 0; r < rfuzz; r++)
    {
      vdgl_edgel edgel= dc->get_interpolator()->get_edgel_chain()->edgel(len-1-r);
      labels[r] = edgeMap->get( int( edgel.get_x()), int( edgel.get_y()));
      edgeMap->put(int(edgel.get_x()), int(edgel.get_y()), NULL);
    }

  // 2. Find another stronger contour touched by this end point < gap.
  //Sprial around the end location, increasing the radius
  //searching for the strongest edge strength on some other chain
  //It can't be near the end of the weaker chain nearby since we
  //erased the pixels corresponding to it.
  stronger = NULL;              // contour can join with itself
  int jx = int(endv->x()), jy = int(endv->y());
  for (int l = 0, n = 0; l < maxSpiral; l++)    // increasing radius of spiral
  {
    float maxpix = 0; int maxn = 0;     // strongest strength at this radius
    for (; n < RNS[l]; n++)
    {
      int x = jx+RIS[n], y = jy+RJS[n];
      if (edgeMap->get(x, y) && // find another contour or itself
          floatPixel(edgels, x, y) > maxpix)
      {
        maxpix = floatPixel(edgels, x, y);
        maxn = n;               // better neighbor
      }
    }
    if (maxpix) {               // location of junction on contour
      stronger = edgeMap->get(jx+RIS[maxn], jy+RJS[maxn]);
      jx += RIS[maxn], jy += RJS[maxn];
      break;
    }
  }
  // restore edgeMap around end point (undo step 1)
  if (endv == weaker->v1()->cast_to_vertex_2d())
    for (int r=0; r< rfuzz; r++)
    {
      vdgl_edgel edge= dc->get_interpolator()->get_edgel_chain()->edgel(r);
      edgeMap->put(int( edge.get_x()), int( edge.get_y()), labels[r]);
    }
  else
    for (int r=0; r< rfuzz; r++)
    {
      vdgl_edgel edgel= dc->get_interpolator()->get_edgel_chain()->edgel(len-1-r);
      edgeMap->put(int( edgel.get_x()), int( edgel.get_y()),labels[r]);
    }
  delete [] labels;

  if (!stronger)                // do not find any edge in search region
    return false;

  // 3. Find index location of junction on this contour
  // We have found an edgel on a "stronger" edge at location maxn on the
  // search spiral, i.e. at jx and jy.
  index = int(INVALID);
  vdgl_digital_curve_sptr dc2 =(stronger->curve()->cast_to_vdgl_digital_curve());
  vdgl_edgel_chain_sptr ec = dc2->get_interpolator()->get_edgel_chain();
  index = bdgl_curve_algs::closest_point(ec, jx, jy);
  //
  //If we are within s pixels of either end then don't bother to split the
  //edge unless the stronger curve is a cycle without a vertex
  //In the case of a cycle there is no real origin to be used as
  //a junction later during merging tests
  const int s = 3;
  if ((index<=s || index+s+1 > (int)ec->size())&&
      stronger->v1()&&stronger->v2())
    return false;
  if (sdet_contour::debug_)
    vcl_cout << "Closest index to (" << endv->x() << ' '
             << endv->y() << ") is " << index
             << " corresponding to " << ec->edgel(index)
             << "size = " << ec->size() << vcl_endl;

  return true;
}

//: Move a junction to lie on the intersecting digital curve
//  Refine the intersection position to double precision
bool sdet_contour::move_junction(vtol_vertex_2d_sptr const& junction,
                                 int& index,
                                 vdgl_digital_curve_sptr const & dc)
{
  if (!junction)
    return false;
  int jx = int(junction->x()), jy = int(junction->y());
  vertexMap->put(jx, jy, NULL); // erase old location
  //get new location
  vdgl_edgel_chain_sptr ec = dc->get_interpolator()->get_edgel_chain();
  jx = int((*ec)[index].x());
  jy = int((*ec)[index].y());
  //move the junction
  junction->set_x(jx), junction->set_y(jy); // update new vertex location

  //fill out the arrays
  vertexMap->put(jx, jy, junction);
  edgeMap->put(jx, jy, NULL);
  return true;
}

//:
// when a vertex position is moved, an edge's edgel chain must potentially
// be replaced.  old_x and old_y is the original end position.
// It is possible that the vertex can move up to 4 pixels.  The new
// position is the location of v.
// Edgels are added from the end of the old digital_curve to the new
// vertex position.
void sdet_contour::update_edgel_chain(vtol_edge_2d_sptr const& edge,
                                      const int old_x, const int old_y,
                                      vtol_vertex_2d_sptr& v)
{
  if (!edge||!v)
  {
    vcl_cout << "In update_edgel_chain - null inputs\n";
    return;
  }
  //The new vertex position
  double x = v->x(), y = v->y();
  //Access the old edgel chain
  vdgl_digital_curve_sptr dc_old= edge->curve()->cast_to_vdgl_digital_curve();
  vdgl_edgel_chain_sptr ec_old= dc_old->get_interpolator()->get_edgel_chain();
  int N = ec_old->size();
  //Create a new digital curve
  vdgl_edgel_chain_sptr  ec = new vdgl_edgel_chain;
  vdgl_interpolator_sptr it = new vdgl_interpolator_linear(ec);
  vsol_curve_2d_sptr dc = (vsol_curve_2d*)(new vdgl_digital_curve(it));

  // Cases
  // A. The edge is a cycle
  if (edge->v1()==edge->v2())
  {
    vcl_cout << "Cycle case not implemented\n";
    return;
  }
  // B. The edge is open
  // Determine which end of the digital curve is closest to v
  //  int end_index = bdgl_curve_algs::closest_end(ec_old, x, y);
  //  if (end_index == 0)
  if (v==edge->v1()->cast_to_vertex_2d())
  {    //index = 0
    //vcl_cout << "update at v1\n";
    //start the chain with the new vertex location
    vdgl_edgel ed(x, y, bdgl_curve_algs::synthetic);//mark as synthetic edgel

    ec->add_edgel(ed);

    //add in a linear segment to reach old location
    int npix =
      bdgl_curve_algs::add_straight_edgels(ec, old_x, old_y,
                                           sdet_contour::debug_);

    if (!npix)
      return;//nothing was needed

    //mark the edge map at the new edgel locations
    for (int i=1; i<npix; i++)
      edgeMap->put(int((*ec)[i].x()),int((*ec)[i].y()),edge);

    //fill out the rest of the edgel chain
    for (int index = 0; index<N; index++)
      ec->add_edgel((*ec_old)[index]);

    //replace the curve on the edge
    edge->set_curve(*dc);
    return;
  }

  if (v==edge->v2()->cast_to_vertex_2d())
  {
    //vcl_cout << "update at v2\n";
    //copy the chain
    for (int index = 0; index<N; index++)
      ec->add_edgel((*ec_old)[index]);

    //Add in a linear segment to reach to new vertex location
    //from old vertex location, (*ec_old)[N-1].
    int npix =
      bdgl_curve_algs::add_straight_edgels(ec, x, y,
                                           sdet_contour::debug_);

    if (!npix)
      return;//nothing was needed
    //mark the edge map at the new locations
    for (int i=N; i<N+npix; i++)
      edgeMap->put(int((*ec)[i].x()),int((*ec)[i].y()),edge);

    //replace the curve on the edge
    edge->set_curve(*dc);
    return;
  }
}

void fill_cycle_gap(vdgl_edgel_chain_sptr const & ec)
{
  if (!ec)
    return;
  int x0 = int((*ec)[0].x()), y0 = int((*ec)[0].y());
  bdgl_curve_algs::add_straight_edgels(ec, x0, y0,
                                       sdet_contour::debug_);
}

#if 0 // unused local function
static bool
ConfirmJunctionOnCycle(int index, float threshold,
                       vtol_edge_2d& cycle, const gevd_bufferxy& edgels)
{
  if (sdet_contour::debug_)
    vcl_cerr << "ConfirmJunctionOnCycle() not run: returning 'TRUE'\n";

#if 1 // JLM
  return true;
#else
  vdgl_digital_curve_sptr dc = cycle.curve()->cast_to_digital_curve();
  const int len = dc->get_interpolator()->get_edgel_chain()->size();
  const int wrap = 10*len;      // for positive index
  const int radius = 3;         // gap < 3, around junction pixel

  for (int n = index-radius; n <= index+radius; n++)
  {
    int nm = (n-1+wrap)%len;    // modulo operations to wrap at borders
    int np = (n+1+wrap)%len;

    vdgl_edgel edgel_m= dc->get_interpolator()->get_edgel_chain()->edgel( nm);
    vdgl_edgel edgel_p= dc->get_interpolator()->get_edgel_chain()->edgel( np);

    if (vcl_fabs(floatPixel(edgels, int( edgel_p.x()), int( edgel_p.y())) -
                 floatPixel(edgels, int( edgel_m.x()), int( edgel_m.y())))
        > threshold)
      return true;
  }
  return false;
#endif // 1
}
#endif // 0

//:
// Break the cycle at given index, and create new cycle from/to
// and not including index pixel.
//
//           stronger (no vertex initially)
//         ------------
//         |          |
//         |          |
// 0-------0 junction |
//         |          |
//         |          |
//         ------------
//             split -  the new edge
//
void sdet_contour::BreakCycle(vtol_vertex_2d_sptr const& junction,
                              int& index, vtol_edge_2d_sptr const& stronger,
                              vtol_edge_2d_sptr & split)
{
  //Get the old curve
  vdgl_digital_curve_sptr old_dc =
    (stronger->curve()->cast_to_vdgl_digital_curve());
  vdgl_edgel_chain_sptr old_ec = old_dc->get_interpolator()->get_edgel_chain();
  const int N = old_ec->size();

  // 1. Move location of junction
  move_junction(junction, index, old_dc);

  // 2. Create a new edge (a cycle)
  split = new vtol_edge_2d();

  //  The new curve
  vdgl_edgel_chain* es = new vdgl_edgel_chain;
  vdgl_interpolator* it =
    new vdgl_interpolator_linear(vdgl_edgel_chain_sptr(es));
  vdgl_digital_curve *ds =
    new vdgl_digital_curve( vdgl_interpolator_sptr( it));
  split->set_curve(*( vsol_curve_2d *) ds);

  //insert edgels from index to N-1
  //starting at v1()
  for (int k = index; k <N; k++)
  {
    es->add_edgel((*old_ec)[k]);
    int x = int((*old_ec)[k].x()), y = int((*old_ec)[k].y());
    edgeMap->put(x, y, split);
    if (sdet_contour::debug_)
      vcl_cout << "BreakCycle: edge1 edgel at (" << x << ' ' << y << ")\n";
  }
  //insert edgels from 0 to and including index
  for (int k = 0; k <= index; k++)
  {
    es->add_edgel((*old_ec)[k]);
    int x = int((*old_ec)[k].x()), y = int((*old_ec)[k].y());
    edgeMap->put(x, y, split);
    if (sdet_contour::debug_)
      vcl_cout << "BreakCycle: edge1 edgel at (" << x << ' ' << y << ")\n";
  }

  split->set_v1(junction->cast_to_vertex());
  split->set_v2(junction->cast_to_vertex());
  int x = int(junction->x());
  int y = int(junction->y());
  vertexMap->put(x, y, junction);
  //stronger is no longer of interest to v1 and v2.
  //  btol_edge_algs::unlink_all_inferiors_twoway(stronger); done in lookup tab
}

#if 0 // unused local function
//: Confirm there is a strong jump in response near a junction.
// The location of this jump is however inaccurate, and so junctions
// can not be localized accurately along the stronger chain.
static bool
ConfirmJunctionOnChain(int index, float threshold,
                       vtol_edge_2d& chain, const gevd_bufferxy& edgels)
{
  if (sdet_contour::debug_)
    vcl_cerr << "ConfirmJunctionOnChain() not run: returning 'TRUE'\n";

#if 1 // JLM
  return true;
#else
  vdgl_digital_curve_sptr dc = chain.curve()->cast_to_digital_curve();
  const int len = dc->get_interpolator()->get_edgel_chain()->size()-1;

  if (len < 2*MINLENGTH-1) // will merge vertices instead of
    return false;          // breaking up chains

  const int fuzz = MINLENGTH-1; // from min length of broken chains
  const int radius = 3;         // gap < 3, around junction pixel
  //search a neighborhood around index on the input chain
  //for long chains this amounts to index+-radius
  for (int n = vcl_max(index-radius, fuzz); n <= vcl_min(index+radius,len-1-fuzz); n++)
  {
    //for each point in the neighborhood of index
    //get edgels on each side
    vdgl_edgel cp1= dc->get_interpolator()->get_edgel_chain()->edgel(n+1);
    vdgl_edgel cm1= dc->get_interpolator()->get_edgel_chain()->edgel(n-1);
    //if there is a location where the difference is above
    // a threshold then the junction can be formed.
    if (vcl_fabs(floatPixel(edgels, int(cp1.x()), int(cp1.y())) -
                 floatPixel(edgels, int(cm1.x()), int(cm1.y())))
        > threshold)
    {
      return true;
    }
  }
  return false;
#endif // 1
}
#endif // 0

vtol_vertex_2d_sptr get_vertex_at_index(vtol_edge_2d_sptr& e, int index)
{
  vdgl_digital_curve_sptr dc = e->curve()->cast_to_vdgl_digital_curve();
  if (!dc)
    return 0;
  vdgl_edgel ed = dc->get_interpolator()->get_edgel_chain()->edgel( index);
  vtol_vertex_2d_sptr v = new vtol_vertex_2d(ed.x(), ed.y());
  return v;
}

bool find_vertex(vtol_vertex_2d_sptr& v,
                 vcl_vector<vtol_vertex_2d_sptr>& vertices)
{
  for (vcl_vector<vtol_vertex_2d_sptr>::iterator vit = vertices.begin();
       vit != vertices.end(); vit++)
    if ((*vit)==v)
      return true;
  return false;
}

bool find_edge(vtol_edge_2d_sptr& e,
               vcl_vector<vtol_edge_2d_sptr>& edges)
{
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    if ((*eit)==e)
      return true;
  return false;
}

void print_edge_lookup_table(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  int ei=0;
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++, ei++)
  {
    if (!*eit)
    {
      vcl_cout << "edge[" << ei << "]= null\n";
      continue;
    }
    vcl_cout<< "edge["<< ei << "]= " << **eit
            << *((*eit)->v1()->cast_to_vertex_2d())
            << ' ' << *((*eit)->v2()->cast_to_vertex_2d()) << '\n';
  }
}

//: Break the edge at given index, and create two subchains from it.
//
//               junction
//  0---------------o----------------------0
//                index                  nedgels-1
//       edge1                edge2
//
//
void sdet_contour::BreakChain(vtol_vertex_2d_sptr const& junction,
                              int& index,
                              vtol_edge_2d_sptr const& stronger,
                              vtol_edge_2d_sptr& longer,
                              vtol_edge_2d_sptr& shorter)
{
  vdgl_digital_curve_sptr dc = stronger->curve()->cast_to_vdgl_digital_curve();

  const int N = dc->get_interpolator()->get_edgel_chain()->size();

  // 1. Move the location of junction to lie on stronger's digital_curve
  move_junction(junction, index, dc);

  // 2. Create first subchain up to and including junction pixel.
  vtol_edge_2d_sptr edge1 = new vtol_edge_2d();
  vdgl_edgel_chain *ec= new vdgl_edgel_chain;
  vdgl_interpolator *it= new vdgl_interpolator_linear( ec);
  vdgl_digital_curve *dc1 = new vdgl_digital_curve( it);
  edge1->set_curve(*dc1);

  //insert the edgels in first subchain, edge1
  // include index in edge 1
  vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();
  vdgl_edgel_chain *cxy1= ec;
  for (int k = 0; k <= index; k++)
  {
    cxy1->add_edgel( (*cxy)[k] );
    (*cxy1)[k] = (*cxy)[k];
    int x = int((*cxy1)[k].x()), y = int((*cxy1)[k].y());
    edgeMap->put(x , y,  edge1);
    if (sdet_contour::debug_)
      vcl_cout << "BreakChain: edge1 edgel at (" << x << ' ' << y << ")\n";
  }

  //set the vertices of edge 1
  // have to get rid of stronger from the list of superiors of v1
  edge1->set_v1(stronger->v1()->cast_to_vertex());

  // junction keeps its original superiors, + edge1
  edge1->set_v2(junction->cast_to_vertex());

  //mark vertex and edge arrays for edge 1
  int x11 = int(edge1->v1()->cast_to_vertex_2d()->x());
  int y11 = int(edge1->v1()->cast_to_vertex_2d()->y());
  int x12 = int(edge1->v2()->cast_to_vertex_2d()->x());
  int y12 = int(edge1->v2()->cast_to_vertex_2d()->y());
  edgeMap->put(x11, y11, NULL);
  edgeMap->put(x12, y12, NULL);
  vertexMap->put(x11, y11, edge1->v1()->cast_to_vertex_2d());
  vertexMap->put(x12, y12, edge1->v2()->cast_to_vertex_2d());

  // 3. Create second subchain from and including junction pixel.
  vtol_edge_2d_sptr edge2 = new vtol_edge_2d();    // create second subchain
  vdgl_edgel_chain *ec2= new vdgl_edgel_chain;
  vdgl_interpolator *it2= new vdgl_interpolator_linear( ec2);
  vdgl_digital_curve *dc2= new vdgl_digital_curve( it2);
  edge2->set_curve(*dc2);

  //insert the edgels into edge2
  //start at index since that edgel will be v1 of edge2
  vdgl_edgel_chain *cxy2= ec2;
  for (int k = index; k < N; k++)
  {
    cxy2->add_edgel((*cxy)[k]);
    int x = int((*cxy)[k].x()), y = int((*cxy)[k].y());
    edgeMap->put( x, y, edge2);
    if (sdet_contour::debug_)
      vcl_cout << "BreakChain: edge2 edgel at (" << x << ' ' << y << ")\n";
  }

  // have to remove stronger from superiors of v2
  edge2->set_v2(stronger->v2()->cast_to_vertex());
  // junction keeps the orginal superiors + edge2
  edge2->set_v1(junction->cast_to_vertex());

  //mark vertex and edge arrays for edge 2
  int x21 = int(edge2->v1()->cast_to_vertex_2d()->x());
  int y21 = int(edge2->v1()->cast_to_vertex_2d()->y());
  int x22 = int(edge2->v2()->cast_to_vertex_2d()->x());
  int y22 = int(edge2->v2()->cast_to_vertex_2d()->y());
  edgeMap->put(x21, y21, NULL);
  edgeMap->put(x22, y22, NULL);
  vertexMap->put(x21, y21, edge2->v1()->cast_to_vertex_2d());
  vertexMap->put(x22, y22, edge2->v2()->cast_to_vertex_2d());

  //Here is where we get rid of the stale superior (stronger) of v1 and v2

  //  btol_edge_algs::unlink_all_inferiors_twoway(stronger);
  // done in lookup table
  if (cxy1->size() >= cxy2->size())  // sort longer/shorter chains
    longer = edge1, shorter = edge2;
  else
    longer = edge2, shorter = edge1;
}


//: Break the chain at given index, and create a loop.
// This case occurs when the junction is caused by a chain touching itself
//
//      straight     junction
//  0-------------------0--------
//                      |       |  curled
//                      |       |
//                      ---------
void
sdet_contour::LoopChain(vtol_vertex_2d_sptr const& junction, int& index,
                        vtol_edge_2d_sptr const& chain,
                        vtol_edge_2d_sptr& straight,
                        vtol_edge_2d_sptr& curled)
{
  vdgl_digital_curve_sptr dc = chain->curve()->cast_to_vdgl_digital_curve();
  vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();
  const int N = cxy->size();

  // 1. Move location of junction
  int old_x = int(junction->x()), old_y = int(junction->y());
  move_junction(junction, index, dc);

  // 2. Find straight/curled chains
  straight = new vtol_edge_2d(), curled = new vtol_edge_2d();

  //
  // The touching end of chain is v1 so
  // first subchain is curled and forms a cycle
  //         v1 index    v2
  //   -------0----------0
  //  |       x
  //  |       | potential gap
  //   --------
  //    curled
  if (junction == chain->v1()->cast_to_vertex_2d())
  {
    vdgl_edgel_chain *ec= new vdgl_edgel_chain;
    vdgl_interpolator *it= new vdgl_interpolator_linear( ec);
    vdgl_digital_curve *c= new vdgl_digital_curve( it);
    curled->set_curve(*c);
    vdgl_edgel_chain *xy= ec;

    //fill in potential gap starting at index
    if (!(int(junction->x())==old_x&&int(junction->y())==old_y))
    {
      //add an edgel at index
      ec->add_edgel(vdgl_edgel((*cxy)[index].x(), (*cxy)[index].y()));

      //add in a linear segment to reach old v1's position
      int npix =
        bdgl_curve_algs::add_straight_edgels(ec, old_x, old_y,
                                             sdet_contour::debug_);

      //mark the edge map at the new edgel locations
      for (int i=1; i<npix; i++)
        edgeMap->put(int((*xy)[i].x()),int((*xy)[i].y()),curled);
    }
    //include index as other endpoint of curled
    for (int k = 0; k <= index; k++)
    {
      xy->add_edgel( (*cxy)[k] );
      edgeMap->put( int((*cxy)[k].x()), int((*cxy)[k].y()), curled);
    }
    //same v1 and v2 since curled is a cycle
    curled->set_v1(junction->cast_to_vertex());
    curled->set_v2(junction->cast_to_vertex());
    int x = int(junction->x()), y = int(junction->y());
    edgeMap->put(x, y, NULL);
    vertexMap->put(x, y, junction);
    //
    //define the straight section (ends in v2)
    //
    ec= new vdgl_edgel_chain;
    it= new vdgl_interpolator_linear( ec);
    c = new vdgl_digital_curve( it);    // second subchain is straight
    straight->set_curve(*c);
    xy= ec;
    //start at index
    for (int k = index; k < N; k++)
    {
      xy->add_edgel( (*cxy)[k] );
      edgeMap->put( int((*cxy)[k].x()), int((*cxy)[k].y()), straight);
    }
    //set vertices of new straight section
    straight->set_v1(junction->cast_to_vertex());
    straight->set_v2(chain->v2());

    //fill out maps
    x = int(straight->v2()->cast_to_vertex_2d()->x());
    y = int(straight->v2()->cast_to_vertex_2d()->y());
    edgeMap->put(x, y, NULL);
    vertexMap->put(x, y, straight->v2()->cast_to_vertex_2d());
  }
  else // The first subchain is straight, second is curled
  {
    //
    // v1         v2 index
    //  0-----------0--------
    //   potential  x        |
    //      gap     |        |
    //               --------
    //                 curled
    vdgl_edgel_chain *ec= new vdgl_edgel_chain;
    vdgl_interpolator *it= new vdgl_interpolator_linear( ec);
    vdgl_digital_curve *c= new vdgl_digital_curve( it);
    straight->set_curve(*c);
    vdgl_edgel_chain *xy= ec;
    //include index in the straight section
    for (int k = 0; k <=index; k++)
    {
      xy->add_edgel( (*cxy)[k] );
      edgeMap->put( int((*cxy)[k].x()), int((*cxy)[k].y()), straight);
    }

    //set the vertices of straight
    straight->set_v1(chain->v1());
    straight->set_v2(junction->cast_to_vertex());

    //fill out the maps
    int x = int(straight->v1()->cast_to_vertex_2d()->x());
    int y = int(straight->v1()->cast_to_vertex_2d()->y());
    edgeMap->put(x, y, NULL);
    vertexMap->put(x, y, straight->v1()->cast_to_vertex_2d());

    //construct the curled section
    ec= new vdgl_edgel_chain;
    it= new vdgl_interpolator_linear( ec);
    c = new vdgl_digital_curve( it);    // second subchain is curled
    curled->set_curve(*c);
    xy = ec;

    int nc = 0;//length of curled section up to gap
    // start at index
    for (int k = index; k < N; k++, nc++)
    {
      xy->add_edgel( (*cxy)[k] );
      edgeMap->put( int((*cxy)[k].x()), int((*cxy)[k].y()), curled);
    }

    //fill in potential gap starting at v2
    if (!(int(junction->x())==old_x&&int(junction->y())==old_y))
    {
      //add in a linear segment to reach new junction position
      int new_x = int(junction->x()), new_y = int(junction->y());
      int npix =
        bdgl_curve_algs::add_straight_edgels(ec, new_x, new_y,
                                             sdet_contour::debug_);
      //mark the edge map at the new edgel locations
      for (int i=0; i<npix; i++)
        edgeMap->put(int((*xy)[nc+i].x()),int((*xy)[nc+i].y()),curled);
    }

    // set the vertices of curled
    curled->set_v1(junction->cast_to_vertex());
    curled->set_v2(junction->cast_to_vertex());

    //fill out the maps
    x = int(junction->x());
    y = int(junction->y());
    edgeMap->put(x, y, NULL);
    vertexMap->put(x, y, junction);
  }
  //done in lookup table
  //btol_edge_algs::unlink_all_inferiors_twoway(chain);
}

//: Determine if a vertex is in the border strip.
//  The strip supports the edge detection kernel
//
bool sdet_contour::near_border(vtol_vertex_2d_sptr const&  v)
{
  const int xmin = FRAME;
  const int ymin = FRAME;
  const int xmax = vertexMap->rows()-FRAME-1;
  const int ymax = vertexMap->columns()-FRAME-1;
  int x = int(v->x()), y = int(v->y());
  return x<=xmin||x>=xmax||y<=ymin||y>=ymax;
}

//:
// Detect touching another junction or end point,
// from an end point of a dangling chain by
// searching in a spiral pattern.
// Find the neighboring vertex with the largest number of
// incident edges.
//
vtol_vertex_2d_sptr
sdet_contour::DetectTouch(vtol_vertex_2d_sptr const& endv,
                          const int maxSpiral)
{
  const int jx = int(endv->x()), jy = int(endv->y());
  for (int l = 0, n = 0; l < maxSpiral; l++)    // increasing radius of spiral
  {
    int bx=0, by=0;
    vtol_vertex_2d_sptr  best_neighbor = NULL;  // prefer junction over endpt
    int max_edges = 0;             // max number of edges
    for (; n < RNS[l]; n++)    // 4- then 8-connected
    {
      int x = jx+RIS[n], y = jy+RJS[n];
      vtol_vertex_2d_sptr nbr = vertexMap->get(x, y);
      int nedges = 0;
      if (nbr)
        nedges = nbr->numsup();
      if (nedges > max_edges)
      {
        max_edges = nedges;    // number of edges connected to it
        best_neighbor = nbr;   // better neighbor
        bx = x; by = y; //best detected location, for debug purposes
      }
    }
    if (sdet_contour::debug_)
      vcl_cout << "(bx,by) = (" << bx << ' ' << by << ")\n";
    if (max_edges)
      return best_neighbor;
  }
  return NULL;
}

//: If there is only one edge connected to v then return it, otherwise return null
//
vtol_edge_2d_sptr
DanglingEdge(vtol_vertex_2d_sptr const& v)
{
  vcl_vector<vtol_edge_sptr>* segs = v->edges();
  vtol_edge_sptr e = NULL;

  if (segs->size()==1)
    e = (*segs)[0];

  delete segs;
  return e->cast_to_edge_2d();
}


//: Merge 2 end points of a same chain creating a cycle
//
//       endpt
//         o------------------
//         o----             |
//      other   \            |
//               ------------
//
//  The removed vertex is the either endpt or other, depending
//  on the direction of the edge
//
bool
sdet_contour::MergeEndPtsOfChain(vtol_vertex_2d_sptr const& endpt,
                                 vtol_vertex_2d_sptr const& other,
                                 vtol_vertex_2d_sptr& removed_vert)
{
  if (sdet_contour::debug_)
    vcl_cout << " Merging end points of same edge " << *endpt << ' '
             << *other << '\n';

  vcl_vector<vtol_edge_sptr>* edges = endpt->edges();
  // dangling edge terminating at endpt
  vtol_edge_2d_sptr edge = (*edges)[0]->cast_to_edge_2d();
  delete edges;
  vdgl_digital_curve_sptr dc = edge->curve()->cast_to_vdgl_digital_curve();
  vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();
  int N = cxy->size();

  //replace the old edge
  //
  // need to consider 2 combinations
  //          endpt               other         removed_vert
  //             0-------------------0
  //  case a     v2                 v1             endpt
  //  case b     v1                 v2             other

  while (true)
  {
    //case self_a: v2 corresponds to endpt
    //add edgels to reach other
    if (edge->v2()->cast_to_vertex_2d() == endpt)
    {
      //Fill in edges across the gap endpt->other
      double xe = other->x();
      double ye = other->y();
      int nedgls =
        bdgl_curve_algs::add_straight_edgels(cxy, xe, ye,
                                             sdet_contour::debug_);
      for (int i = N; i<N+nedgls; i++)
        edgeMap->put( int((*cxy)[i].x()), int((*cxy)[i].y()), edge);
      edge->set_v2(other->cast_to_vertex());
      removed_vert = endpt;
      vertexMap->put(int(endpt->x()), int(endpt->y()), NULL);
      break;
    }

    //case jb, v1 corresponds to endpt
    if (edge->v1()->cast_to_vertex_2d() == endpt)
    {
      //Fill in edges across the gap other->endpt
      double xe = endpt->x();
      double ye = endpt->y();
      int nedgls =
        bdgl_curve_algs::add_straight_edgels(cxy, xe, ye,
                                             sdet_contour::debug_);
      for (int i = N; i<N+nedgls; i++)
        edgeMap->put( int((*cxy)[i].x()), int((*cxy)[i].y()), edge);
      edge->set_v2(endpt->cast_to_vertex());
      removed_vert = other;
      vertexMap->put(int(other->x()), int(other->y()), NULL);
      break;
    }
  }
  return true;
}


//:
//  Merge two different chains by inserting a link from end1 to end2
//  end1 and end2 are touching according to the predicate DetectTouch
//  end1 and end2 are each incident to a single, different edge
//
//        shorter   gap         longer
//    0-------------o  o----------------------0
//              end1    end2
//
void
sdet_contour::MergeEndPtTouchingEndPt(vtol_vertex_2d_sptr const& end1,
                                      vtol_vertex_2d_sptr const& end2,
                                      vtol_edge_2d_sptr& merge,
                                      vtol_edge_2d_sptr& longer,
                                      vtol_edge_2d_sptr& shorter)
{
  // 1. Retrieve the dangling edges/chains

  // The single edge connected to end1
  vcl_vector<vtol_edge_sptr>* edges = end1->edges();
  vtol_edge_2d_sptr edge1 = (*edges)[0]->cast_to_edge_2d();
  delete edges;

  // The single edge connected to end2
  edges = end2->edges();
  vtol_edge_2d_sptr edge2 = (*edges)[0]->cast_to_edge_2d();
  delete edges;

  // 2. Create merged edge/chain
  vdgl_digital_curve_sptr dc1 = edge1->curve()->cast_to_vdgl_digital_curve();
  const int l1 = dc1->get_interpolator()->get_edgel_chain()->size();
  vdgl_digital_curve_sptr dc2 = edge2->curve()->cast_to_vdgl_digital_curve();
  const int l2 = dc2->get_interpolator()->get_edgel_chain()->size();

  //edgel chain for edge1
  vdgl_edgel_chain_sptr cxy1= dc1->get_interpolator()->get_edgel_chain();

  //edgel chain for edge2
  vdgl_edgel_chain_sptr cxy2= dc2->get_interpolator()->get_edgel_chain();

  //The new edge
  merge = new vtol_edge_2d();
  vdgl_edgel_chain *ec = new vdgl_edgel_chain;
  vdgl_interpolator *it = new vdgl_interpolator_linear( ec);
  vdgl_digital_curve *dc = new vdgl_digital_curve(it);

  merge->set_curve(*dc);
  //
  // need to consider 4 combinations
  //                   dc1         end1  end2     dc2
  //             0-------------------0    0----------------------0
  //  case a     v1                 v2    -                      -
  //  case b     v2                 v1    -                      -
  //  case c     -                  -     v1                    v2
  //  case d     -                  -     v2                    v1
  //
  //debug

  if (sdet_contour::debug_)
  {
    vcl_cout << "end1 " << *end1 << '\n'
             << "end2 " << *end2 << '\n'
             << "edge1-v1 " << *(edge1->v1())
             << "  edge1-v2 " << *(edge1->v2()) << '\n'
             << "edge2-v1 " << *(edge2->v1())
             << "  edge2-v2 " << *(edge2->v2()) << '\n'
             << " (*cxy1)[0] ="<< (*cxy1)[0] << '\n'
             << " (*cxy1)[l1-1] =" <<(*cxy1)[l1-1] << '\n'

             << " (*cxy2)[0] = "<<(*cxy2)[0] << '\n'
             << " (*cxy1)[l2-1] =" << (*cxy2)[l2-1] << '\n';
  }

  vdgl_edgel_chain *cxy= ec;    // new edgel chain

  //Fill in edgels up to end1
  while (true)
  {
    //case a: v2 corresponds to end1
    if (edge1->v2()->cast_to_vertex_2d() == end1)
    {
      if (sdet_contour::debug_)
        vcl_cout << "Case a\n";
      for (int i = 0; i < l1; i++)
      {
        cxy->add_edgel( (*cxy1)[i] );
        if (sdet_contour::debug_)
          vcl_cout << "merge edgel " << (*cxy1)[i] << '\n';
      }
      merge->set_v1(edge1->v1());
      break;
    }
    //case b, v1 corresponds to end1, reverse the original edge1 chain
    if (edge1->v1()->cast_to_vertex_2d() == end1)
    {
      if (sdet_contour::debug_)
        vcl_cout << "Case b\n";
      for (int i = l1-1; i >= 0; --i)
      {
        cxy->add_edgel((*cxy1)[i]);
        if (sdet_contour::debug_)
          vcl_cout << "merge edgel " << (*cxy1)[i] << '\n';
      }
      merge->set_v1(edge1->v2());
      break;
    }
  }
  //
  //Fill in edges across the gap end1->end2
  double xe = end2->cast_to_vertex_2d()->x();
  double ye = end2->cast_to_vertex_2d()->y();
  bdgl_curve_algs::add_straight_edgels(cxy, xe, ye,
                                       sdet_contour::debug_);
  //note that end1 is now accounted for in the merged chain

  while (true)
  {
    //case c: v1 corresponds to end2
    if (edge2->v1()->cast_to_vertex_2d() == end2)
    {
      if (sdet_contour::debug_)
        vcl_cout << "Case c\n";
      for (int i = 1; i < l2; i++)//don't need edge2->v1() i=1
      {
        cxy->add_edgel( (*cxy2)[i] );
        if (sdet_contour::debug_)
          vcl_cout << "merge edgel " << (*cxy2)[i] << '\n';
      }
      merge->set_v2(edge2->v2());
      break;
    }
    //case d: v2 corresponds to end2, reverse the chain
    if (edge2->v2()->cast_to_vertex_2d() == end2)
    {
      if (sdet_contour::debug_)
        vcl_cout << "Case d\n";
      for (int i = l2-2; i >= 0; i--)// don't need edge2->v2() i = l2-2
      {
        cxy->add_edgel( (*cxy2)[i] );
        if (sdet_contour::debug_)
          vcl_cout << "merge edgel " << (*cxy2)[i] << '\n';
      }
      merge->set_v2(edge2->v1());
      break;
    }
  }

  // 3. Update global maps
  vertexMap->put(int(end1->x()), int(end1->y()), NULL);
  vertexMap->put(int(end2->x()), int(end2->y()), NULL);
  const int last = cxy->size()-1;
  for (int k = 1; k < last; k++)
    edgeMap->put( int((*cxy)[k].x()), int((*cxy)[k].y()), merge);
  if (edgeMap->get( int((*cxy)[0].x()), int((*cxy)[0].y())))
    edgeMap->put( int((*cxy)[0].x()), int((*cxy)[0].y()), merge);
  if (edgeMap->get( int((*cxy)[last].x()), int((*cxy)[last].y())))
    edgeMap->put( int((*cxy)[last].x()), int((*cxy)[last].y()), merge);

  if (l1 >= l2)                 // sort out length of deleted subchains
    longer = edge1, shorter = edge2;
  else
    longer = edge2, shorter = edge1;
  if (sdet_contour::debug_)
  {
    vcl_cout << "longer " << *(longer->v1()->cast_to_vertex_2d())
             << ' ' << *(longer->v2()->cast_to_vertex_2d()) << '\n'
             << "shorter " << *(shorter->v1()->cast_to_vertex_2d())
             << ' ' << *(shorter->v2()->cast_to_vertex_2d()) << '\n';
  }
}


//: Merge an isolated end point into a nearby junction.
//
//                     ------------
//                    |            |
//             endpt  |            |         O
//    0-----------o   0 junction   0         |
//                    |                      |
//                     ----------------------
//
bool sdet_contour::
MergeEndPtTouchingJunction(vtol_vertex_2d_sptr const& endpt,
                           vtol_vertex_2d_sptr const& junction,
                           vtol_edge_2d_sptr& old_edge,
                           vtol_edge_2d_sptr& new_edge)
{
  if (sdet_contour::debug_)
    vcl_cout << "Merge at Junction e" << *endpt<< " j"  << *junction << '\n';
  vcl_vector<vtol_edge_sptr>* edges = endpt->edges();
  // dangling edge terminating at end pt
  old_edge = (*edges)[0]->cast_to_edge_2d();
  delete edges;
  vdgl_digital_curve_sptr old_dc = old_edge->curve()->cast_to_vdgl_digital_curve();
  vdgl_edgel_chain_sptr old_cxy= old_dc->get_interpolator()->get_edgel_chain();
  int N = old_cxy->size();

  new_edge = new vtol_edge_2d();
  vdgl_edgel_chain *cxy = new vdgl_edgel_chain;
  vdgl_interpolator *it = new vdgl_interpolator_linear(cxy);
  vdgl_digital_curve *dc = new vdgl_digital_curve(it);
  new_edge->set_curve(*dc);

  int xs, ys;
  //replace the old edge
  while (true)
  {
    //case ja: v2 corresponds to endpt
    //copy edgels up to v2
    if (old_edge->v2()->cast_to_vertex_2d() == endpt)
    {
      if (sdet_contour::debug_)
        vcl_cout << "Case ja\n";
      for (int i = 0; i+1 < N; ++i)
      {
        cxy->add_edgel( (*old_cxy)[i] );
        if (sdet_contour::debug_)
          vcl_cout << "junction edgel " << (*old_cxy)[i] << '\n';
        edgeMap->put( int((*old_cxy)[i].x()),
                      int((*old_cxy)[i].y()), new_edge);
      }
      //set v1 to old v1
      new_edge->set_v1(old_edge->v1());
      xs = int(old_edge->v1()->cast_to_vertex_2d()->x());
      ys = int(old_edge->v1()->cast_to_vertex_2d()->y());
      vertexMap->put(xs, ys, new_edge->v1()->cast_to_vertex_2d());
      edgeMap->put(xs, ys, NULL);
      break;
    }
    //case jb, v1 corresponds to endpt
    //must reverse the original edge1 chain
    if (old_edge->v1()->cast_to_vertex_2d() == endpt)
    {
      if (sdet_contour::debug_)
        vcl_cout << "Case jb\n";
      for (int i = N-1; i >=0; --i)
      {
        cxy->add_edgel((*old_cxy)[i]);
        if (sdet_contour::debug_)
          vcl_cout << "junction edgel " << (*old_cxy)[i] << '\n';
        edgeMap->put( int((*old_cxy)[i].x()),
                      int((*old_cxy)[i].y()), new_edge);
      }

      //set new v1 to old v2
      new_edge->set_v1(old_edge->v2());
      xs = int(old_edge->v2()->cast_to_vertex_2d()->x());
      ys = int(old_edge->v2()->cast_to_vertex_2d()->y());
      vertexMap->put(xs, ys, new_edge->v1()->cast_to_vertex_2d());
      edgeMap->put(xs, ys, NULL);
      break;
    }
  }
  //At this point we have copied the old edge up to the gap at endpt
  //the new edge has v2 at the position of endpt.
  //Now we add edgels to reach the junction
  double xe = junction->cast_to_vertex_2d()->x();
  double ye = junction->cast_to_vertex_2d()->y();
  int nedgls =
    bdgl_curve_algs::add_straight_edgels(cxy, xe, ye,
                                         sdet_contour::debug_);

  //Check for self-intersection
  //Intersections in a 3x3 window around the
  //final point (xs, ys) do not count since
  //there are intrinsically collisions near the vertex
  bool self_intersects = false;//JLM added -1 need to check!
  for (int i = N; i<(N+nedgls-1)&&!self_intersects; i++)
  {
    int x = int((*cxy)[i].x()), y = int((*cxy)[i].y());
#define WARN(x,y) vcl_cerr << "Warning: edgel "<<i<<" is at ("<<x<<','<<y\
                           <<") which is outside of edge map of size "\
                           <<edgeMap->rows()<<'x'<<edgeMap->cols()<<'\n'
    if (x < 0) { WARN(x,y); x = 0; }
    if (y < 0) { WARN(x,y); y = 0; }
    if (x >= edgeMap->rows()) { WARN(x,y); x = edgeMap->rows()-1; }
    if (y >= edgeMap->cols()) { WARN(x,y); y = edgeMap->cols()-1; }
#undef WARN

    if (sdet_contour::debug_)
      vcl_cout << " intersecting (" << i << ")(" << vnl_math_abs(x-xe)
               << ' ' << vnl_math_abs(y-ye) << ")\n";

    self_intersects = self_intersects ||
      ((edgeMap->get(x, y)==new_edge)&&
       ((vnl_math_abs(x-xe)>1)||(vnl_math_abs(y-ye)>1)));

    if (!self_intersects)
      edgeMap->put(x, y,new_edge);
  }

  if (sdet_contour::debug_&&self_intersects)
    vcl_cout << "merge endpoint touching junction - self-intersection\n";

  if (self_intersects)
    return false;

  //set v2 of the new edge to be the junction
  new_edge->set_v2(junction->cast_to_vertex());
  int x = int(junction->x()), y = int(junction->y());
  vertexMap->put(x, y, junction);
  vertexMap->put(int(endpt->x()), int(endpt->y()), NULL);

  edgeMap->put(x, y, NULL);

  return true;
}


//:
// Find junctions from end points touching at an interior point
// of a chain, with detectable jump in filter response.
// Localize these junctions on the stronger contour to pixel accuracy,
// and break stronger chain into subchains.
// Also merge end points touching another end point or junction.
// Return the number of end points and junctions bounding
// all chains/cycles detected in sdet_contour::FindChains.
// Deletion/insertion to the network must be done completely,
// so that the connectivity links are updated.  Protected.
int
sdet_contour::FindJunctions(gevd_bufferxy& edgels,
                            vcl_vector<vtol_edge_2d_sptr>& edges,
                            vcl_vector<vtol_vertex_2d_sptr >& vertices)
{
  vul_timer t;

  if (!edges.size())
  {
    vcl_cerr << "sdet_contour::FindChains must precede sdet_contour::FindJunctions.\n";
    return 0;
  }
  // 1. Create vertices at the end of edges (digital_curve geometry)
  const float connect_fuzz = 2;

  for (unsigned int i=0; i< edges.size(); i++)
  {
    vtol_edge_2d_sptr edge = edges[i];
    vdgl_digital_curve_sptr dc = edge->curve()->cast_to_vdgl_digital_curve();
    vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();
    //the index of the last edgel in the curve
    const int last = cxy->size()-1;
    //the edge might be a closed cycle
    // test for disjoint first/last pixel
    if (vcl_fabs((*cxy)[0].x()-(*cxy)[last].x()) > connect_fuzz ||
        vcl_fabs((*cxy)[0].y()-(*cxy)[last].y()) > connect_fuzz)
    { // so not closed cycle
      int x = int((*cxy)[0].x()), y = int((*cxy)[0].y());
      vtol_vertex_2d_sptr v1 = vertexMap->get(x, y);
      //see if there is a vertex for v1 at x,y
      if (!v1)
      {
        // If not, add a new vertex, for v1
        // 1st point in chain
        v1 = new vtol_vertex_2d((*cxy)[0].x(), (*cxy)[0].y());
        vertexMap->put(x, y, v1);
        LookupTableInsert(vertices, v1);
      }
      else//othewise erase the edgel at x,y
      {
        edgeMap->put( x, y, NULL); // erase junction point
      }

      edge->set_v1(v1->cast_to_vertex());         // link both directions v-e
      if (sdet_contour::debug_)
        vcl_cout << "adding vertex (" << x << ' ' << y
                 << ")(" << v1->numsup() << ")\n";
      x = int((*cxy)[last].x()), y = int((*cxy)[last].y());

      vtol_vertex_2d_sptr v2 = vertexMap->get(x, y);
      //see if there is a vertex for v2 at x,y
      if (!v2)
      {
        // If not, add a new vertex, for v2
        // last point in chain
        v2 = new vtol_vertex_2d((*cxy)[last].x(), (*cxy)[last].y());
        vertexMap->put(x, y, v2);
        LookupTableInsert(vertices, v2);
      }
      else//othewise erase the edgel at x,y
      {
        edgeMap->put( x, y, NULL); // erase junction point
      }

      edge->set_v2(v2->cast_to_vertex());  // link both directions v-e
      if (sdet_contour::debug_)
        vcl_cout << "adding vertex (" << x << ' ' << y
                 << ")(" << v2->numsup() << ")\n";
    }
    else // is a closed cycle but with a potential gap of connect_fuzz
      fill_cycle_gap(cxy);
  }
  //
  // At this point, all edges have vertices defined at the endpoints
  // of the digital curve except for 1-edge cycles
  //
  // 2. Localize a junction, when an end point of a dangling contour
  // touches another contour or itself at an interior point.
  //
  // If the end vertex does form a junction, e.g. with DetectJunction
  // then the digital_curve "weaker" must also be updated
  //
  int jcycle = 0, jchain = 0;   // number of junctions with cycle/chain
  for (unsigned int i=0; i< vertices.size(); i++)
  {
    //continue; //JLM no junctions
    vtol_vertex_2d_sptr  endv = vertices[i];
    vtol_edge_2d_sptr weaker = NULL, stronger = NULL;
    int index; // location on stronger contour
    if (DetectJunction(endv, index, weaker, stronger, maxSpiral, edgels))
    {
      if (sdet_contour::debug_)
        vcl_cout << "detected junction near (" << endv->x() <<' '<< endv->y()
                 << ")\n";

      int old_x = int(endv->x()), old_y = int(endv->y());//before end moves

      //If v1 is NULL then the edge is a cycle

      if (!stronger->v1())
      {
        // cycle is now split at junction
        vtol_edge_2d_sptr split = NULL;
        BreakCycle(endv, index, stronger, split);
        LookupTableReplace(edges, stronger, split);

        //Replace the mutated weaker digital curve
        //since the endpoint may have moved
        update_edgel_chain(weaker, old_x, old_y, endv);

        if (sdet_contour::debug_) {
          vcl_cout << "new position on cycle (" << endv->x() << ' '
                   << endv->y() << ")\n";
        }
        jcycle++;             // remove original edge
      }
      else if (weaker == stronger)                  // touch itself or another 1-chain
      {
        vtol_edge_2d_sptr straight = NULL, curled = NULL;
        // break own chain and make a loop
        // edgel chain gaps are updated internally
        LoopChain(endv, index, stronger, straight, curled);

        LookupTableReplace(edges, stronger, straight);
        LookupTableInsert(edges, curled);

        if (sdet_contour::debug_)
          vcl_cout << "new position on loop chain (" << endv->x()
                   << ' ' << endv->y()<< ")\n";
        jchain++;
      }
      else
      {
        vtol_edge_2d_sptr longer = NULL, shorter = NULL;
        BreakChain(endv, index, stronger,longer, shorter);
        LookupTableReplace(edges, stronger, longer);
        LookupTableInsert(edges, shorter);

        //Replace the mutated weaker digital curve
        //since the endpoint may have moved
        update_edgel_chain(weaker, old_x, old_y, endv);
        if (sdet_contour::debug_)
          vcl_cout << "old position on chain (" << old_x
                   << ' ' << old_y
                   << ")  new position on chain (" << endv->x()
                   << ' ' << endv->y()<< ")(" << endv->numsup() <<")\n";

        jchain++;
      }
    }
  }

  if (talkative_)
    vcl_cout << "Find junctions with "
             << jcycle << " cycles and " << jchain << " chains, with jump > "
             << minJump << " and maxSpiral " << maxSpiral << vcl_endl;
  // 3. Merge touching end points, into a larger junction/chain.
  int dendpt = 0, dchain = 0;   // number of deleted endpt/chain

  for (unsigned int i=0; i< vertices.size(); i++)
  {
    //continue; //JLM no merge
    // search from dangling end pt only
    vtol_vertex_2d_sptr  end1 = vertices[i];

    // skip deleted vertices, i.e., !end
    // and only consider end point of dangling 1-chain
    // but not within border strip
    if (end1 && end1->numsup() == 1 && !near_border(end1))
    {
      if (sdet_contour::debug_)
        vcl_cout << "merge target end1(" << end1->x() << ' '
                 << end1->y() << ")\n";
      // find another vertex nearby
      vtol_vertex_2d_sptr  end2 = DetectTouch(end1, maxSpiral);
      if (end2)
      {
        //Case a. The other end point has only one edge
        if (end2->numsup() == 1)
        {
          vtol_edge_2d_sptr seg = DanglingEdge(end1);
          //Case a1. The edge on the other endpoint is the same as seg
          if (seg == DanglingEdge(end2))
          {
            vtol_vertex_2d_sptr removed_vert = NULL;
            if (MergeEndPtsOfChain(end1, end2, removed_vert))
            {
              LookupTableRemove(vertices, removed_vert);
              if (sdet_contour::debug_)
                vcl_cout << "cycle endpt1=" << *end1 << vcl_endl
                         << "cycle endpt2=" << *end2 << vcl_endl;
              dendpt++;
            }
          }
          else
          {
            //Case a2. The edge on the other endpoint is different from seg
            if (sdet_contour::debug_)
              vcl_cout << "endpt1=" << *end1 << vcl_endl
                       << "endpt2=" << *end2 << vcl_endl;

            vtol_edge_2d_sptr merge=NULL, longer=NULL, shorter=NULL;
            // merge 2 different edges
            MergeEndPtTouchingEndPt(end1, end2,
                                    merge, longer, shorter);
            if (sdet_contour::debug_)
              vcl_cout << "merge=" << *merge << vcl_endl
                       << "longer=" << *longer << vcl_endl
                       << "shorter=" << *shorter << vcl_endl
                       << "merge.v1=" << *merge->v1() << vcl_endl
                       << "merge.v2=" << *merge->v2() << vcl_endl;

            LookupTableReplace(edges, longer, merge);
            LookupTableRemove(edges, shorter);
            LookupTableRemove(vertices, end1);
            LookupTableRemove(vertices, end2);
            dendpt += 2, dchain += 1;
          }
        }
        else
        {
          //Case b. The other junction has more than 2 edges
          if (sdet_contour::debug_)
            vcl_cout << "junction endpt1=" << *end1 << vcl_endl
                     << "junction endpt2=" << *end2 << vcl_endl;
          vtol_edge_2d_sptr old_edge=NULL, new_edge=NULL;
          if (MergeEndPtTouchingJunction(end1, end2, old_edge, new_edge))
          {
            LookupTableReplace(edges, old_edge, new_edge);
            LookupTableRemove(vertices, end1);
            dendpt++;
          }
        }
      } //end if (end2->numsup()==1)
    } //end if (end2)
  }

  if (sdet_contour::debug_)
    vcl_cout << "Merge and delete " << dendpt
             << " end points and " << dchain << " edges\n";
  if (dchain)                   // eliminate holes in global arrays
    LookupTableCompress(edges);
  if (dendpt)
    LookupTableCompress(vertices);

  // 4. Insert virtual junction for isolated 1-cycles
  int ncycle = 0;
  for (unsigned int i=0; i< edges.size(); i++)
  {
    //continue; //JLM no virtual junctions
    vtol_edge_2d_sptr edge = edges[i];
    if (!edge->v1())    // vertices not created from 1.
    {
      vdgl_digital_curve_sptr dc = edge->curve()->cast_to_vdgl_digital_curve();
      vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();

      const int last = cxy->size()-1;
      vtol_vertex_2d_sptr  v =
        new vtol_vertex_2d(((*cxy)[0].x()+(*cxy)[last].x())/2,
                           ((*cxy)[0].y()+(*cxy)[last].y())/2);
      edge->set_v1(v->cast_to_vertex()); edge->set_v2(v->cast_to_vertex());
      vertexMap->put(int(v->x()), int(v->y()), v);
      LookupTableInsert(vertices, v);
      ncycle++;
    }
  }
  if (sdet_contour::debug_)
    vcl_cout << "Create " << ncycle
             << " virtual end points for isolated cycles.\n";

  if (talkative_)
    vcl_cout << "All junctions found in " << t.real() << " msecs.\n";

  return vertices.size();
}


//: Insert subpixel accuracy into the pixels on the edges/vertices.
// Truncating float locations with int(xy) should map to the original
// pixel locations. No interpolation is done at junctions of 3 or more
// contours, so a junction can have location error up to 1-2 pixel,
// tangential to the strong contour.
void
sdet_contour::SubPixelAccuracy(vcl_vector<vtol_edge_2d_sptr>& edges,
                               vcl_vector<vtol_vertex_2d_sptr >& vertices,
                               const gevd_bufferxy& locationx,
                               const gevd_bufferxy& locationy)
{
  //return;//JLM no subpixel
  vul_timer t;
  if (talkative_)
    vcl_cout << "Insert subpixel accuracy into edges/vertices";

  // 1. Subpixel accuracy for end points
  for (unsigned int i=0; i< vertices.size(); i++)
  {
    vtol_vertex_2d_sptr  vert = vertices[i];
    int x = int(vert->x()), y = int(vert->y());
    vert->set_x(x + floatPixel(locationx, x, y));
    vert->set_y(y + floatPixel(locationy, x, y));
  }

  // 2. Subpixel accuracy for chain pixels
  for (unsigned int i=0; i< edges.size(); i++)
  {
    vtol_edge_2d_sptr edge = edges[i];
    if (!edge) continue;
    vdgl_digital_curve_sptr dc = edge->curve()->cast_to_vdgl_digital_curve();
    if (!dc) continue;
    if (!dc->get_interpolator()) continue;
    vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();
    if (!cxy) continue;

    for (unsigned int k = 0; k < cxy->size(); ++k)
    {
      // if ((*cxy)[k].get_grad()<0)
      int x = int((*cxy)[k].x()), y = int((*cxy)[k].y());
      double tempx= (*cxy)[k].x()+ floatPixel( locationx, x, y);
      double tempy= (*cxy)[k].y()+ floatPixel( locationy, x, y);
      (*cxy)[k].set_x( tempx);
      (*cxy)[k].set_y( tempy);
    }
  }

  // 3. Thin zig-zags on the contours? Zig-zags happen at
  // the 2 end points because of extension, or inside the contour
  // because of 4/8-connected tracing through noisy chain pixels,
  // and large shifts for subpixel locations.
  // Implement only if experiments prove zig-zags are excessive

  if (talkative_)
    vcl_cout << ", in " << t.real() << " msecs.\n";
}


//:
//  Generate an Edge with a vdgl_digital_curve representing a straight line
//  between the specified vertices.
vtol_edge_2d_sptr DigitalEdge(vtol_vertex_2d_sptr const& vs,
                              vtol_vertex_2d_sptr const& ve)
{
  vsol_curve_2d_sptr dc= new vdgl_digital_curve(vs->point(), ve->point());
  return new vtol_edge_2d(vs, ve, dc);
}


//:
// Insert virtual edges and vertices to enforce closure
// of the regions beyond the rectangular image border.
// The location of the border is at 3 pixels away from the
// real image border, because of kernel radius in convolution
// and non maximum suppression. Virtual border of image should be
// inserted after sdet_contour::FindChains() and sdet_contour::FindJunctions().
//
// JLM - February 1999  Modified this routine extensively to
// move the border to the actual image ROI bounds.  Chain endpoints
// are extended to intersect with the border.  These changes were
// made to support region segmentation from edgels.
void
sdet_contour::InsertBorder(vcl_vector<vtol_edge_2d_sptr>& edges,
                           vcl_vector<vtol_vertex_2d_sptr >& vertices)
{
  //return;//JLM no borders
  vul_timer t;
  bool merge = true;//merge close vertices
  //1. Save Edges along the border
  vcl_vector<vtol_vertex_2d_sptr > xmin_verts;
  vcl_vector<vtol_vertex_2d_sptr > xmax_verts;
  vcl_vector<vtol_vertex_2d_sptr > ymin_verts;
  vcl_vector<vtol_vertex_2d_sptr > ymax_verts;

  if (talkative_)
    vcl_cout << "Insert virtual border to enforce closure";

  // 2. Create 4 corners vertices
  const int rmax = FRAME;       // border of image
  const int xmax = vertexMap->rows()-rmax-1;
  const int ymax = vertexMap->columns()-rmax-1;
  int cx[] = {rmax, xmax, rmax, xmax}; // coordinates of 4 corners
  int cy[] = {rmax, ymax, ymax, rmax};
  int d;
  // 3. Collect Vertices along each border
  //3.0 Generate Corner Vertices
  vtol_vertex_2d_sptr  V00 = new vtol_vertex_2d(rmax, rmax);
  vtol_vertex_2d_sptr  V01 = new vtol_vertex_2d(rmax, ymax);
  vtol_vertex_2d_sptr  V10 = new vtol_vertex_2d(xmax, rmax);
  vtol_vertex_2d_sptr  V11 = new vtol_vertex_2d(xmax, ymax);
  xmin_verts.push_back(V00);
  xmax_verts.push_back(V10);
  ymin_verts.push_back(V00);
  ymax_verts.push_back(V01);
  // 3.1 ymin, ymax edges
  for (d = 0; d < 2; d++)
  {
    int x, y = cy[d];
    for (x = rmax; x<=xmax; x++)
    {
      vtol_vertex_2d_sptr  v = vertexMap->get(x, y);
      if (v)
        vertexMap->put(x, y, NULL);
      else continue;
      if (d)
        ymax_verts.push_back(v);
      else
        ymin_verts.push_back(v);
    }
  }
  // 3.2 xmin, xmax edges
  for (d = 0; d < 2; d++)
  {
    int x = cx[d], y;
    for (y = rmax; y<=ymax; y++)
    {
      vtol_vertex_2d_sptr  v = vertexMap->get(x, y);
      if (v)
        vertexMap->put(x, y, NULL);
      else continue;
      if (d)
        xmax_verts.push_back(v);
      else
        xmin_verts.push_back(v);
    }
  }

  xmin_verts.push_back(V01);
  xmax_verts.push_back(V11);
  ymin_verts.push_back(V10);
  ymax_verts.push_back(V11);

  // 4. Merge vertices
  // 4.1  along ymin and ymax
  for (d = 0; d < 2; d++)
  {
    vcl_vector<vtol_vertex_2d_sptr >* verts = &ymin_verts;
    if (d)
      verts = &ymax_verts;
    int len = (*verts).size();
    if (len<3) continue;
    //potential merge at xmin
    vtol_vertex_2d_sptr  pre_v = (*verts)[0];
    vtol_vertex_2d_sptr  v = (*verts)[1];
    int x = int(v->x());
    int pre_x = int(pre_v->x());
    if (merge&&(x-pre_x)<3)
    {
      vtol_vertex_sptr pv = pre_v->cast_to_vertex(),
        vv = v->cast_to_vertex();
      btol_vertex_algs::merge_superiors(pv, vv);

      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= verts->begin();
           it!= verts->end(); ++it)
      {
        if (*it == v)
        {
          verts->erase( it);
          break;
        }
      }
      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= vertices.begin();
           it!= vertices.end(); ++it)
      {
        if (*it == v)
        {
          vertices.erase( it);
          break;
        }
      }
      len--;
    }
    //potential merge at xmax
    pre_v = (*verts)[len-2];
    v = (*verts)[len-1];
    pre_x = int(pre_v->x());
    if (merge&&(xmax+rmax-pre_x)<3)
    {
      vtol_vertex_sptr pv = pre_v->cast_to_vertex(),
        vv = v->cast_to_vertex();
      btol_vertex_algs::merge_superiors(pv, vv);

      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= verts->begin();
           it!= verts->end(); ++it)
      {
        if (*it == pre_v)
        {
          verts->erase( it);
          break;
        }
      }
      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= vertices.begin();
           it!= vertices.end(); ++it)
      {
        if (*it == pre_v)
        {
          vertices.erase( it);
          break;
        }
      }
      len--;
    }
  }

  // 4.2  along xmin and xmax
  for (d = 0; d < 2; d++)
  {
    vcl_vector<vtol_vertex_2d_sptr >* verts = &xmin_verts;
    if (d)
      verts = &xmax_verts;
    int len = (*verts).size();
    if (len<3) continue;
    //potential merge at ymin
    vtol_vertex_2d_sptr  pre_v = (*verts)[0];
    vtol_vertex_2d_sptr  v = (*verts)[1];

    int y = int(v->y()), pre_y = int(pre_v->y());
    if (merge&&(y-pre_y)<3)
    {
      vtol_vertex_sptr pv = pre_v->cast_to_vertex(),
        vv = v->cast_to_vertex();
      btol_vertex_algs::merge_superiors(pv, vv);
      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= verts->begin();
           it!= verts->end(); ++it)
      {
        if (*it == v)
        {
          verts->erase( it);
          break;
        }
      }
      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= vertices.begin();
           it!= vertices.end(); ++it)
      {
        if (*it == v)
        {
          vertices.erase( it);
          break;
        }
      }
      len--;
    }
    //potential merge at ymax
    pre_v = (*verts)[len-2];
    v = (*verts)[len-1];
    pre_y = int(pre_v->y());
    if (merge&&(ymax+rmax-pre_y)<3)
    {
      vtol_vertex_sptr pv = pre_v->cast_to_vertex(),
        vv = v->cast_to_vertex();
      btol_vertex_algs::merge_superiors(pv, vv);
      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= verts->begin();
           it!= verts->end(); ++it)
      {
        if (*it == pre_v)
        {
          verts->erase( it);
          break;
        }
      }
      for (vcl_vector<vtol_vertex_2d_sptr >::iterator it= vertices.begin();
           it!= vertices.end(); ++it)
      {
        if (*it == pre_v)
        {
          vertices.erase( it);
          break;
        }
      }
      len--;
    }
  }

  // 5. Move the vertices to the bounds of the ROI
  float xmi = 0, xmx = float(xmax + rmax);
  float ymi = 0, ymx = float(ymax + rmax);
  for (unsigned int iv=1; iv+1<xmin_verts.size(); ++iv)
  {
    vtol_vertex_2d_sptr  v = xmin_verts[iv];
    if (!v) continue;
    vtol_vertex_2d_sptr  vp = new vtol_vertex_2d(xmi, v->y());
    vertices.push_back(vp);// vp->Protect();
    xmin_verts[iv] = vp;

    vtol_edge_2d_sptr e = DigitalEdge(v, vp);
    edges.push_back(e);//  e->Protect();
  }

  for (unsigned int iv=1; iv+1<xmax_verts.size(); ++iv)
  {
    vtol_vertex_2d_sptr v = xmax_verts[iv];
    if (!v) continue;
    vtol_vertex_2d_sptr  vp = new vtol_vertex_2d( xmx, v->y());
    vertices.push_back(vp); // vp->Protect();
    xmax_verts[iv] = vp;
    vtol_edge_2d_sptr e = DigitalEdge(v, vp);
    edges.push_back(e); // e->Protect();
  }
  for (unsigned int iv=1; iv+1<ymin_verts.size(); ++iv)
  {
    vtol_vertex_2d_sptr  v = ymin_verts[iv];
    if (!v) continue;
    vtol_vertex_2d_sptr  vp = new vtol_vertex_2d(v->x(), ymi);
    vertices.push_back(vp); // vp->Protect();
    ymin_verts[iv] = vp;
    vtol_edge_2d_sptr e = DigitalEdge(v, vp);
    edges.push_back(e); // e->Protect();
  }
  for (unsigned int iv=1; iv+1<ymax_verts.size(); ++iv)
  {
    vtol_vertex_2d_sptr  v = ymax_verts[iv];
    if (!v) continue;
    vtol_vertex_2d_sptr  vp = new vtol_vertex_2d( v->x(), ymx);
    vertices.push_back(vp); // vp->Protect();
    ymax_verts[iv] = vp;
    vtol_edge_2d_sptr e = DigitalEdge(v, vp);
    edges.push_back(e); // e->Protect();
  }
  V00->set_x(0);  V00->set_y(0); vertices.push_back(V00);
  V01->set_x(0);  V01->set_y(ymax+rmax); vertices.push_back(V01);
  V10->set_x(xmax+rmax);  V10->set_y(0); vertices.push_back(V10);
  V11->set_x(xmax+rmax);  V11->set_y(ymax+rmax); vertices.push_back(V11);

  //6. Now we have properly placed vertices.  Next we scan and generate
  //edges. along the border.
  //6.1 along ymin and ymax
  for (d = 0; d < 2; ++d)
  {
    vcl_vector<vtol_vertex_2d_sptr >* verts = &ymin_verts;
    if (d)
      verts = &ymax_verts;
    unsigned int len = (*verts).size();
    if (len<2)
    {
      vcl_cout <<"In sdet_contour::InsertBorder() - too few vertices\n";
      return;
    }
    for (unsigned int i=0; i+1<len; ++i)
    {
      vtol_vertex_2d_sptr  v = (*verts)[i];
      vtol_vertex_2d_sptr  vp = (*verts)[i+1];
      vtol_edge_2d_sptr e = DigitalEdge(v, vp);
      edges.push_back(e); // e->Protect();
    }
  }
  //6.2 along xmin and xmax
  for (d = 0; d < 2; ++d)
  {
    vcl_vector<vtol_vertex_2d_sptr >* verts = &xmin_verts;
    if (d)
      verts = &xmax_verts;
    unsigned int len = (*verts).size();
    if (len<2)
    {
      vcl_cout <<"In sdet_contour::InsertBorder() - too few vertices\n";
      return;
    }
    for (unsigned int i = 0; i+1<len; ++i)
    {
      vtol_vertex_2d_sptr  v = (*verts)[i];
      vtol_vertex_2d_sptr  vp = (*verts)[i+1];
      vtol_edge_2d_sptr e = DigitalEdge(v, vp);
      edges.push_back(e); // e->Protect();
    }
  }

  if (talkative_)
    vcl_cout << ", in " << t.real() << " msecs.\n";
}


//:
// Convolve array elements with [1 0 1]/2, replacing
// center pixel by average of 2 neighbors.
// This will make the spacing between pixels almost equal
// and prune away small zig-zags.
void
EqualizeElements(double* elmts, int n, double v1, double v2)
{
  double p0 = elmts[0], p1 = elmts[1], p2 = elmts[2]; // setup pipeline
  elmts[0] = (v1 + p1) / 2;     // touching first vertex
  for (int i = 1; i+2 < n; ++i)
  {
    elmts[i] = (p0 + p2)/2;
    p0 = p1; p1 = p2; p2 = elmts[i+2]; // faster with circular list
  }
  if (n>1) elmts[n-2] = (p0 + p2)/2;   // last convolution
  if (n>0) elmts[n-1] = (p1 + v2)/2;   // touching second vertex
}


//:
// Make the spacing of the chain pixels nearly equal by
// smoothing their locations with the average filter  [1 0 1]/2.
// This will reduce square grid tessellation artifacts, and
// lead to more accurate estimation of the tangent direction,
// and local curvature angle, from finite differences in location.
// It is also useful to avoid weight artifacts in curve fitting
// caused by the periodic clustering of pixels along the chain.
// Truncating the float locations with int() will no longer map
// to the original pixels of the discrete chains.
void
sdet_contour::EqualizeSpacing(vcl_vector<vtol_edge_2d_sptr>& chains)
{
  vul_timer t;

  if (talkative_)
    vcl_cout << "Equalize the spacing between pixels in chains\n";

  for (unsigned int i= 0; i< chains.size(); ++i)
  {
    vtol_edge_2d_sptr e = chains[i];
    vdgl_digital_curve_sptr dc = e->curve()->cast_to_vdgl_digital_curve();
    const int len = dc->get_interpolator()->get_edgel_chain()->size();
    if (len > 2*MINLENGTH)
    {   // not necessary for short chains
      vtol_vertex_sptr v1 = e->v1(), v2 = e->v2();

      vcl_vector<double> cx(len);
      vcl_vector<double> cy(len);

      for (int qq=0; qq<len; ++qq)
      {
        vdgl_edgel e= dc->get_interpolator()->get_edgel_chain()->edgel( qq);
        cx[qq]= e.x();
        cy[qq]= e.y();
      }

      EqualizeElements(&cx[0], len, v1->cast_to_vertex_2d()->x(), v2->cast_to_vertex_2d()->x());
      EqualizeElements(&cy[0], len, v1->cast_to_vertex_2d()->y(), v2->cast_to_vertex_2d()->y());

      for (int qq=0; qq<len; ++qq)
      {
        vdgl_edgel e( cx[qq], cy[qq]);
        dc->get_interpolator()->get_edgel_chain()->set_edgel( qq, e);
      }
    }
  }
  if (talkative_)
    vcl_cout << ", in " << t.real() << " msecs.\n";
}


//: Translate all the pixels in the edges and vertices by (tx, ty).
// If the image is extracted from an ROI, a translation of
// (roi->GetOrigX(), roi->GetOrigY()) must be done to have
// coordinates in the reference frame of the original image.
// Add 0.5 if you want to display location at center of pixel
// instead of upper-left corner.
void
sdet_contour::Translate(vcl_vector<vtol_edge_2d_sptr>& edges, // translate loc to center
                        vcl_vector<vtol_vertex_2d_sptr >& vertices,
                        float tx, float ty)
{
  vul_timer t;

  if (talkative_)
    vcl_cout << "Translate edges/vertices\n";

  for (unsigned int i=0; i< vertices.size(); ++i)
  {
    vtol_vertex_2d_sptr  vert = vertices[i];
    vert->set_x(vert->x() + tx);
    vert->set_y(vert->y() + ty);
  }
  for (unsigned int i=0; i< edges.size(); ++i)
  {
    vtol_edge_2d_sptr edge = edges[i];
    vdgl_digital_curve_sptr dc = edge->curve()->cast_to_vdgl_digital_curve();

    vdgl_edgel_chain_sptr cxy= dc->get_interpolator()->get_edgel_chain();
    for (unsigned int k = 0; k < cxy->size(); ++k)
    {
      vdgl_edgel e= (*cxy)[k];

      e.set_x( e.x()+tx);
      e.set_y( e.y()+ty);

      cxy->set_edgel( k, e);
    }
  }

  if (talkative_)
    vcl_cout << ", in " << t.real() << " msecs.\n";
}


//:
// Remove and delete all elements in global lists, and set
// the global lists to NULL. Remove all digital chains of edges.
// Edges and vertices are removed with UnProtect().
void
sdet_contour::ClearNetwork(vcl_vector<vtol_edge_2d_sptr>*& edges,
                           vcl_vector<vtol_vertex_2d_sptr >*& vertices)
{
  delete edges; edges = NULL;
  delete vertices; vertices = NULL;
}


//:
// Set the orientation at each edgel on all digital curves to a continuous
// orientation value, which is consistent with C. Rothwell's EdgeDetector.
// That is theta = (180/M_PI)*atan2(dI/dy, dI/dx)
//
void
sdet_contour::SetEdgelData(gevd_bufferxy& grad_mag, gevd_bufferxy& angle, vcl_vector<vtol_edge_2d_sptr>& edges)
{
  for (unsigned int i=0; i< edges.size(); ++i)
  {
    vtol_edge_2d_sptr e = edges[i];
    vdgl_digital_curve_sptr dc= e->curve()->cast_to_vdgl_digital_curve();

    if (dc)
    {
      vdgl_edgel_chain_sptr xypos= dc->get_interpolator()->get_edgel_chain();

      int len = xypos->size();

      for (int i = 0; i < len; ++i)
      {
        int ix = int((*xypos)[i].x());
        int iy = int((*xypos)[i].y());

        // Debugging : RIH
        // Routine crashes with iy < 0.
        if (iy < 0 || ix < 0 ||
            ix >= grad_mag.GetSizeX() ||
            iy >= grad_mag.GetSizeY())
        {
          vcl_cerr << "***********  ERROR  : (ix, iy) = ("
                   << ix << ", " << iy << ")\n";
          if (ix < 0) ix = 0;
          if (iy < 0) iy = 0;
          if (ix >= grad_mag.GetSizeX()) ix = grad_mag.GetSizeX()-1;
          if (iy >= grad_mag.GetSizeY()) iy = grad_mag.GetSizeY()-1;
        }

        (*xypos)[i].set_grad( floatPixel( grad_mag, ix, iy));
        (*xypos)[i].set_theta( floatPixel( angle, ix, iy));
#if 0
        gr[i] = floatPixel(grad_mag, ix, iy);
        th[i] = floatPixel(angle, ix, iy);
#endif
      }
    }
  }
}


//:
// Insert topology object in 2-way lookup table,
// using Id and dynamic array. Protect it in the network.
void
sdet_contour::LookupTableInsert(vcl_vector<vtol_edge_2d_sptr>& set,
                                vtol_edge_2d_sptr elmt)
{
  elmt->set_id(set.size());     // index in global array
  set.push_back(elmt);          // push_back at end of array
}


//: As above for vertices.
void
sdet_contour::LookupTableInsert(vcl_vector<vtol_vertex_2d_sptr >& set,
                                vtol_vertex_2d_sptr  elmt)
{
  elmt->set_id(set.size());     // index in global array
  set.push_back(elmt);          // push at end of array
}


//: Replace deleted by inserted in 2-way lookup table.
// Also remove object from the network.
void
sdet_contour::LookupTableReplace(vcl_vector<vtol_edge_2d_sptr>& set,
                                 vtol_edge_2d_sptr deleted, vtol_edge_2d_sptr inserted)
{
  const int i = deleted->get_id();
  inserted->set_id(i);
  set[i] = inserted;            // replace in global array
  btol_edge_algs::unlink_all_inferiors_twoway(deleted);
}


//: As above for vertices.
void
sdet_contour::LookupTableReplace(vcl_vector<vtol_vertex_2d_sptr >& set,
                                 vtol_vertex_2d_sptr  deleted, vtol_vertex_2d_sptr  inserted)
{
  const int i = deleted->get_id();
  inserted->set_id(i);
  set[i] = inserted;            // replace in global array
}


//: Remove topology object from 2-way lookup table leaving an empty hole.
// Also remove object from the network.
void
sdet_contour::LookupTableRemove(vcl_vector<vtol_edge_2d_sptr>& set,
                                vtol_edge_2d_sptr elmt)
{
  btol_edge_algs::unlink_all_inferiors_twoway(elmt);
  set[elmt->get_id()] = NULL;   // remove from global array
}


//: As above for vertices.
void
sdet_contour::LookupTableRemove(vcl_vector<vtol_vertex_2d_sptr >& set,
                                vtol_vertex_2d_sptr  elmt)
{
  set[elmt->get_id()] = NULL;   // remove from global array
}


//: Eliminate empty holes in the lookup table.
void
sdet_contour::LookupTableCompress(vcl_vector<vtol_edge_2d_sptr>& set)
{
  vcl_vector<vtol_edge_2d_sptr> temp;
  //eliminate null edges
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = set.begin();
       eit != set.end(); eit++)
    if (*eit)
      temp.push_back(*eit);
  int i = 0;
  set.clear();
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = temp.begin();
       eit != temp.end(); eit++, i++)
  {
    (*eit)->set_id(i);
    set.push_back(*eit);
  }
}

//: As above for vertices.
void
sdet_contour::LookupTableCompress(vcl_vector<vtol_vertex_2d_sptr>& set)
{
  vcl_vector<vtol_vertex_2d_sptr> temp;
  //eliminate null edges
  for (vcl_vector<vtol_vertex_2d_sptr>::iterator vit = set.begin();
       vit != set.end(); vit++)
    if (*vit)
      temp.push_back(*vit);
  int i = 0;
  set.clear();
  for (vcl_vector<vtol_vertex_2d_sptr>::iterator vit = temp.begin();
       vit != temp.end(); vit++, i++)
  {
    (*vit)->set_id(i);
    set.push_back(*vit);
  }
}
