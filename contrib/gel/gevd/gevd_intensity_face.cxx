//:
// \file
#if 0
#include <list>
#include <iostream.h>
#include <math/matrix.h>
#include <cool/Vector.h>
#include <Numerics/SVD.h>
#include <Basics/Histogram.h>
#include <SpatialBasics/tolerance.h>
#include <Geometry/Curve.h>
#include <Geometry/IUPoint.h>
#include <Geometry/ImplicitLine.h>
#include <Geometry/ImplicitPlane.h>
#include <DigitalGeometry/DigitalCurve.h>
#include <Topology/Vertex.h>
#include <Topology/Edge.h>
#include <Topology/OneChain.h>
#include <Topology/Face.h>
#include <DigitalGeometry/IntensityCoef.h>
#include <Detection/IntensityFace.h>
#endif

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/algo/vnl_svd.h>
//#include <Basics/Histogram.h>
//#include <SpatialBasics/tolerance.h>
//#include <Geometry/Curve.h>
//#include <Geometry/IUPoint.h>

#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_vertex.h>
#include "gevd_intensity_face.h"

//---------------------------------------------------------------
//: Constructors
//
#if 0
// Looks like we don't need this -tpk
//----------------------------------------------------------------
//: A constructor from a set of 2-d vtol_edge(s) with DigitalCurve geometry
//    Here the vtol_edge(s) are constructed from edgel curves formed during
//    region analysis.
gevd_intensity_face::gevd_intensity_face(vcl_vector<vtol_edge*>* edges)
  :Face(edges)
{
}
#endif

//----------------------------------------------------------------
//: A similar constructor using previously formed vtol_one_chain(s)
//    The list is structured with the outer boundary as the first
//    element of one_chains and the interior hole boundaries as
//    the remaining elements of the list.
gevd_intensity_face::gevd_intensity_face(one_chain_list& one_chains)
  :vtol_face_2d(one_chains)
{
}
#if 0
//: Uses given 2-d vtol_edges (not deep copy) with intensity information from dr.
gevd_intensity_face::gevd_intensity_face(vcl_vector<vtol_edge_sptr>* edges, vdgl_digital_region& dr)
  : vtol_face_2d(edges), vdgl_digital_region(dr.Npix(), dr.Xj(), dr.Yj(), dr.Ij())
{
}
#endif
//: Uses given 2-d vtol_one_chain(s) (not deep copy) with intensity information from dr.
gevd_intensity_face::gevd_intensity_face(vcl_vector<vtol_one_chain_sptr>* chains, vdgl_digital_region& dr)
  : vtol_face_2d(*chains), vdgl_digital_region(dr.Npix(), dr.Xj(), dr.Yj(), dr.Ij())
{
}

//-------------------------------------------------------------------
//: Constructor from various vtol_face_2d class/subclasses.
//    These constructors carry out a deep copy of the vtol_face_2d. Adjacent
//    face topology is lost.

gevd_intensity_face::gevd_intensity_face(vtol_face_2d& face, int npts, float* xp, float* yp,
                             unsigned short *pix)
  :vtol_face_2d(face), vdgl_digital_region(npts, xp, yp, pix)
{
}

gevd_intensity_face::gevd_intensity_face(vtol_face_2d& face, int npts, float* xp, float* yp,
                             float* zp, unsigned short *pix)
  :vtol_face_2d(face), vdgl_digital_region(npts, xp, yp, zp, pix)
{
}
gevd_intensity_face::gevd_intensity_face(gevd_intensity_face& iface)
  :vtol_face_2d(iface), vdgl_digital_region(iface.Npix(),
                              iface.Xj(),
                              iface.Yj(),
                              iface.Zj(),
                              iface.Ij())
{
}
//Default Destructor
gevd_intensity_face::~gevd_intensity_face()
{
}


//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr gevd_intensity_face::clone(void)
{
  return new gevd_intensity_face(*this);
}

//----------------------------------------------------
//: A local method for the moment matrix.
//    The scatter matrix is defined
//    with a coordinate origin at the centroid while the moment matrix is
//    in the face coordinate system
//
vnl_matrix<double> gevd_intensity_face::MomentMatrix()
{
  if (!_scatter_matrix_valid)
    this->ComputeScatterMatrix();

  vnl_matrix<double> M(3,3);

  M(0,0) = _X2+(_Xi*_Xi/_npts); M(0,1) = _XY+(_Xi*_Yi/_npts); M(0,2) = _Xi;
  M(1,0) = M(0,1);               M(1,1) = _Y2+(_Yi*_Yi/_npts); M(1,2) = _Yi;
  M(2,0) = _Xi;                 M(2,1) = _Yi;                 M(2,2) = _npts;
  return M;
}

#if 0
//-----------------------------------------------
//: The extrema of the face along a line of given orientation
//
void gevd_intensity_face::extrema(vcl_vector<float>& orientation,
                                  float& fmin, float& fmax)
{
  //Iterate through the pixels of the face and determine the
  //extrema of the face projection.
  fmin = vnl_math::maxfloat; fmax = -vnl_math::maxfloat;
  for (this->reset(); this->next();)
    {
      //The coordinates of each region pixel
      float xi = float(this->X())+.5, yi = float(this->Y())+.5;
      //Project onto the orientation direction
      float c = orientation.x(), s = orientation.y();
      float w = (xi*c + yi*s);
      //Update the extrema
      fmin = vnl_math::min(fmin, w); fmax = vnl_math::max(fmax, w);
    }
}
#endif

//: This method determines if a vtol_face_2d is a Hole of another vtol_face_2d.
bool
gevd_intensity_face::IsHoleP()
{
  vcl_vector<vtol_edge_sptr>* edges = outside_boundary_edges();
  if (edges->size() == 0)
    return false;
  vtol_edge_sptr e = edges->front();
  bool ret = false;
  const vcl_vector<vtol_topology_object_sptr>* chains = e->superiors();
  for (vcl_vector<vtol_topology_object_sptr>::const_iterator i = chains->begin();
       i != chains->end(); ++i)
    {
      vtol_one_chain* onec = (*i)->cast_to_one_chain();
      if (onec->numsup() > 0)
      {
        ret = true;
        break;
      }
    }
  delete edges;
  return ret;
}

#if 0 //nobody appears to call this method
//-----------------------------------------------------------
//: Compute the total face boundary perimeter.
//    Uses the length of EdgelChain(s)
float gevd_intensity_face::perimeter()
{
  vcl_vector<vtol_edge_sptr>* edges = this->edges();
  //vcl_cout << "Number of Edges = " << edges->length() << vcl_endl;
  float p = 0;
  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges->begin();
       eit != edges->end(); eit++)
    {
      vsol_curve_2d_sptr c = (*eit)->cast_to_edge_2d()->curve();
      if (!c)
        continue;
      vsol_curve_2d::vsol_curve_2d_type type = c->curve_type();
      switch(type)
        {
        case vsol_curve_2d::DIGITAL_CURVE:
          {
            vdgl_digital_curve_sptr dc = (vdgl_digital_curve_sptr)c;
            float l = dc->length();
            p += l;
          }
          break;
        //the boundary could be fit with lines and the Edge geometry replaced
        case GeometryObject::IMPLICITDIGITALLINE:
          {
            ImplicitLine* il = (ImplicitLine*)c;
            float l = il->GetLength();
            p += l;
          }
          break;
        default:
          vcl_cout << "In gevd_intensity_face::perimeter() - warning:"
                   << " Edge does not have known geometry" << vcl_endl;
        break;
        }
    }
  return p;
}

bool gevd_intensity_face::TaggedTransform(CoolTransform const& t)
{
  if (this->GetId())
    return true;
  //First transform the boundary of the vtol_face_2d
  if (!vtol_face_2d::TaggedTransform(t))//this sets the vtol_face_2d Id
    return false;
  //Then transform the interior pixels, no Id change
  return vdgl_digital_region::Transform(t);
}

//-----------------------------------------------------------
//: Compute the histogram of the intensities of the adjacent faces
//
Histogram_ref gevd_intensity_face::GetAdjacentRegionHistogram()
{
    //Get the adjacent faces
  float mini=this->get_min(), maxi=this->get_max();
  vcl_vector<vtol_face_2d*> adj_faces;
  this->GetAdjacentFaces(adj_faces);
  //iterate through and collect intensity bounds
  list<gevd_intensity_face_ref> afs;
  for (vcl_vector<vtol_face_2d*>::iterator fit = adj_faces.begin();
       fit != adj_faces.end(); fit++)
    {
      gevd_intensity_face_ref af=
        (gevd_intensity_face*)((*fit)->CastTogevd_intensity_face());
      if (!af)
        continue;
      afs.push_back(af);
      mini = vnl_math::min(mini, af->get_min());
      maxi = vnl_math::max(maxi, af->get_max());
    }
  //The histogram of the adjacent regions
  Histogram_ref ha = new Histogram(100, mini, maxi);
  for (list<gevd_intensity_face_ref>::iterator ifit = afs.begin();
       ifit != afs.end(); ifit++)
    for ((*ifit)->reset(); (*ifit)->next();)
      ha->UpCount((*ifit)->I());

  return ha;
}

//-----------------------------------------------------------
//: Compute the mean intensity of adjacent Face(s)
//
float gevd_intensity_face::GetAdjacentRegionMean()
{
    //Get the adjacent faces
  vcl_vector<vtol_face_2d*> adj_faces;
  this->adjacent_faces(adj_faces);
  //iterate through and accumulate the mean
  float area =0;
  float mean = 0;
  for (vcl_vector<vtol_face_2d*>::iterator fit = adj_faces.begin();
       fit != adj_faces.end(); fit++)
    {
      gevd_intensity_face_ref af=
        (gevd_intensity_face*)((*fit)->CastTogevd_intensity_face());
      if (!af)
        continue;
      float n = af->Npix();
      area += n;
      mean += n*af->Io();
    }
  if (!area)
    return 0;
  mean/=area;
  return mean;
}
#endif

//-------------------------------------------------------------
//: Update the bounding box, a member of SpatialObject.
//    The algorithm uses the bounding boxes of the vtol_edge(s) forming
//    the boundary of the Face.  Maybe this should be done at the
//    Face level.. but for now.
void gevd_intensity_face::compute_bounding_box()
{
  vcl_vector<vtol_edge_sptr>* edges = this->edges();
  vtol_edge_sptr e;
  this->set_min_x(0); this->set_max_x(0);
  this->set_min_y(0); this->set_max_y(0);

  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges->begin();eit != edges->end(); eit++)
  {
    e = (*eit);
    this->grow_minmax_bounds(*(e->get_bounding_box()));
  }
  delete edges;
}
