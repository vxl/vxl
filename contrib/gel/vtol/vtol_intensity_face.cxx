// This is gel/vtol/vtol_intensity_face.cxx
#include "vtol_intensity_face.h"
//:
// \file

#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_region.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_face_2d.h>

//---------------------------------------------------------------
//: Constructors
//
//----------------------------------------------------------------

//: A constructor from an existing face
vtol_intensity_face::vtol_intensity_face(vtol_face_2d_sptr const& f)
  : vtol_face_2d(f),
    region_(new vdgl_digital_region())
{
}

//: Copy constructor
vtol_intensity_face::vtol_intensity_face(vtol_intensity_face_sptr const& f)
  : vtol_face_2d(f->cast_to_face_2d()),
    region_(new vdgl_digital_region(f->Npix(), f->Xj(), f->Yj(), f->Ij()))
{
}

//----------------------------------------------------------------
//: A similar constructor using previously formed vtol_one_chain(s)
//    The list is structured with the outer boundary as the first
//    element of one_chains and the interior hole boundaries as
//    the remaining elements of the list.
vtol_intensity_face::vtol_intensity_face(one_chain_list & one_chains)
  : vtol_face_2d(one_chains), region_(new vdgl_digital_region())
{
}

//: Uses given 2-d vtol_one_chain(s) (not deep copy) with intensity information from dr.
vtol_intensity_face::vtol_intensity_face(vcl_vector<vtol_one_chain_sptr>* chains, vdgl_digital_region const& dr)
  : vtol_face_2d(*chains), region_(new vdgl_digital_region(dr.Npix(), dr.Xj(), dr.Yj(), dr.Ij()))
{
}

//-------------------------------------------------------------------
//: Constructor from various vtol_face_2d class/subclasses.
//    These constructors carry out a deep copy of the vtol_face_2d. Adjacent
//    face topology is lost.

vtol_intensity_face::vtol_intensity_face(vtol_face_2d_sptr const& face, int npts, float const* xp, float const* yp,
                                         unsigned short const* pix)
  :vtol_face_2d(face),
   region_(new vdgl_digital_region(npts, xp, yp, pix))
{
}

//Default Destructor
vtol_intensity_face::~vtol_intensity_face()
{
  delete region_;
}


//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vtol_intensity_face::clone() const
{
  return new vtol_intensity_face(vtol_intensity_face_sptr(const_cast<vtol_intensity_face*>(this)));
}

//----------------------------------------------------
//: A local method for the moment matrix.
//    The scatter matrix is defined
//    with a coordinate origin at the centroid while the moment matrix is
//    in the face coordinate system
//
vnl_matrix<double> vtol_intensity_face::MomentMatrix()
{
  vnl_matrix<double> M(3,3);

  M(0,0) = X2()+(Ix()*Ix()/Npix()); M(0,1) = XY()+(Ix()*Iy()/Npix()); M(0,2) = Ix();
  M(1,0) = M(0,1);                  M(1,1) = Y2()+(Iy()*Iy()/Npix()); M(1,2) = Iy();
  M(2,0) = Ix();                    M(2,1) = Iy();                    M(2,2) = Npix();
  return M;
}

//-----------------------------------------------------------
//: Compute the total face boundary perimeter.
//    Uses the length of EdgelChain(s)
// MPP 5/9/2003
// Resurrected from #ifdef'd block below for intensity face attributes
double vtol_intensity_face::perimeter()
{
  edge_list edges; this->edges(edges);
  double  p = 0.0;

  for (edge_list::iterator eit = edges.begin(); eit != edges.end(); eit++)
  {
    vsol_curve_2d_sptr  c = (*eit)->cast_to_edge_2d()->curve();
    if (c)
      p += c->length();
  }
  return p;
}

#if 0 //nobody appears to call these methods
//-----------------------------------------------
//: The extrema of the face along a line of given orientation
//
void vtol_intensity_face::extrema(vcl_vector<float>& orientation,
                                  float& fmin, float& fmax)
{
  //Iterate through the pixels of the face and determine the
  //extrema of the face projection.
  float fmin = vnl_numeric_traits<float>::maxval,
        fmax = -vnl_numeric_traits<float>::maxval;
  for (this->reset(); this->next();)
  {
    //The coordinates of each region pixel
    float xi = float(this->X())+.5, yi = float(this->Y())+.5;
    //Project onto the orientation direction
    float c = orientation.x(), s = orientation.y();
    float w = (xi*c + yi*s);
    //Update the extrema
    if (w<fmin) fmin = w;
    if (w>fmax) fmax = w;
  }
}

bool vtol_intensity_face::TaggedTransform(CoolTransform const& t)
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
Histogram_ref vtol_intensity_face::GetAdjacentRegionHistogram()
{
  //Get the adjacent faces
  float mini=this->get_min(), maxi=this->get_max();
  vcl_vector<vtol_face_2d*> adj_faces;
  this->GetAdjacentFaces(adj_faces);
  //iterate through and collect intensity bounds
  iface_list afs;
  for (vcl_vector<vtol_face_2d*>::iterator fit = adj_faces.begin();
       fit != adj_faces.end(); fit++)
  {
    vtol_intensity_face_sptr af= (*fit)->cast_to_intensity_face();
    if (!af)
      continue;
    afs.push_back(af);
    if (af->get_min() < mini) mini = af->get_min();
    if (af->get_max() > maxi) maxi = af->get_max();
  }
  //The histogram of the adjacent regions
  Histogram_ref ha = new Histogram(100, mini, maxi);
  for (iface_iterator ifit = afs.begin(); ifit != afs.end(); ifit++)
    for ((*ifit)->reset(); (*ifit)->next();)
      ha->UpCount((*ifit)->I());

  return ha;
}

//-----------------------------------------------------------
//: Compute the mean intensity of adjacent Face(s)
//
float vtol_intensity_face::GetAdjacentRegionMean()
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
    vtol_intensity_face_sptr af= (*fit)->cast_to_intensity_face();
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
#endif // 0
