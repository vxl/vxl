#include <vsol/vsol_polygon_3d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vsol/vsol_point_3d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// -- Constructor from a vcl_vector (not a geometric vector but a list of points)
// Require: new_vertices.size()>=3 and valid_vertices(new_vertices)
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(const vcl_vector<vsol_point_3d_ref> &new_vertices)
{
  // require
  assert(new_vertices.size()>=3);
  assert(valid_vertices(new_vertices));

  storage_=new vcl_vector<vsol_point_3d_ref>(new_vertices);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(const vsol_polygon_3d &other)
{
  int i;
  vsol_point_3d_ref p;
  
  storage_=new vcl_vector<vsol_point_3d_ref>(*other.storage_);
  for(i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_3d(*((*other.storage_)[i]));
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_polygon_3d::~vsol_polygon_3d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_ref vsol_polygon_3d::clone(void) const
{
  return new vsol_polygon_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return vertex `i'
// Require: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_3d_ref vsol_polygon_3d::vertex(const int i) const
{
  // require
  assert(valid_index(i));

  return (*storage_)[i];
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
// -- Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::operator==(const vsol_polygon_3d &other) const
{
  bool result;
  int i;
  int j;
  vsol_point_3d_ref p;

  result=this==&other;

  if(!result)
    {
      result=storage_->size()==other.storage_->size();
      
      if(result)
        {
          p=(*storage_)[0];
          
          result=false;
          for(i=0;i<storage_->size()&&!result;++i)
            result=*p==*(*other.storage_)[i];
          if(result)
            {
              j=i;
              i=1;
              while(i<size()&&result)
                {
                  result=(*storage_)[i]==(*storage_)[j];
                  ++i;
                  ++j;
                  if(j>=storage_->size())
                    j=0;
                }
            }
        }
    }
  return result;
}

// -- spatial object equality

bool vsol_polygon_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::REGION &&
   ((vsol_region_3d const&)obj).region_type() == vsol_region_3d::POLYGON
  ? *this == (vsol_polygon_3d const&) (vsol_region_3d const&) obj
  : false;
}

//---------------------------------------------------------------------------
// -- Has `this' not the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::operator!=(const vsol_polygon_3d &other) const
{
  return !operator==(other);
}

//---------------------------------------------------------------------------
// -- Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polygon_3d::compute_bounding_box(void)
{
  double xmin;
  double ymin;
  double zmin;
  double xmax;
  double ymax;
  double zmax;
  int i;
  double x;
  double y;
  double z;
  
  xmin=(*storage_)[0]->x();
  ymin=(*storage_)[0]->y();
  zmin=(*storage_)[0]->z();
  xmax=xmin;
  ymax=ymin;
  zmax=zmin;

  for(i=0;i<storage_->size();++i)
    {
       x=(*storage_)[i]->x();
       y=(*storage_)[i]->y();
       z=(*storage_)[i]->z();
       if(x<xmin)
         xmin=x;
       else if(x>xmax)
         xmax=x;
       if(y<ymin)
         ymin=y;
       else if(y>ymax)
         ymax=y;
       if(z<zmin)
         zmin=z;
       else if(z>zmax)
         zmax=z;
    }
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_min_x(xmin);
  _bounding_box->set_max_x(xmax);
  _bounding_box->set_min_y(ymin);
  _bounding_box->set_max_y(ymax);
  _bounding_box->set_min_z(zmin);
  _bounding_box->set_max_z(zmax);
}

//---------------------------------------------------------------------------
// -- Return the number of vertices
//---------------------------------------------------------------------------
int vsol_polygon_3d::size(void) const
{
  return storage_->size();
}
  
//---------------------------------------------------------------------------
// -- Return the area of `this'
//---------------------------------------------------------------------------
double vsol_polygon_3d::area(void) const
{
  // TO DO
  vcl_cerr << "Warning: vsol_polygon_3d::area() has not been implemented yet\n";
  return -1;
}

//---------------------------------------------------------------------------
// -- Is `this' convex ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::is_convex(void) const
{
   bool result;
   vnl_vector_fixed<double,3> *n;
   vnl_vector_fixed<double,3> *n_old;
   vnl_vector_fixed<double,3> *v1;
   vnl_vector_fixed<double,3> *v2;
   int i;
   vsol_point_3d_ref p0;
   vsol_point_3d_ref p1;
   vsol_point_3d_ref p2;

   result=storage_->size()==3; // A triangle is always convex

   if(!result)
     {
       p0=(*storage_)[0];
       p1=(*storage_)[1];
       p2=(*storage_)[2];
       v1=p0->to_vector(*p1);
       v2=p1->to_vector(*p2);
       n=new vnl_vector_fixed<double,3>(cross_3d(*v1,*v2));
       delete v2;
       delete v1;
       n_old=0;

       result=true;
       for(i=3;i<storage_->size()&&result;++i)
         {
           if(n_old!=0)
             delete n_old;
           n_old=n;
           p0=p1;
           p1=p2;
           p2=(*storage_)[i];
           v1=p0->to_vector(*p1);
           v2=p1->to_vector(*p2);
           n=new vnl_vector_fixed<double,3>(cross_3d(*v1,*v2));
           delete v2;
           delete v1;
           result=dot_product(*n_old,*n)>=0;
         }
       delete n;
     }
   return result;
}

//---------------------------------------------------------------------------
// -- Is `i' a valid index for the list of vertices ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::valid_index(const int i) const
{
  return (i>=0)&&(i<storage_->size());
}

//---------------------------------------------------------------------------
// -- Are `new_vertices' valid vertices ? That is are all vertices in the
//    same plane ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::valid_vertices(const vcl_vector<vsol_point_3d_ref> new_vertices) const
{
  if (new_vertices.size() <= 3) return true; // a triangle is always in a plane

  vsol_point_3d_ref p0=new_vertices[0];
  vsol_point_3d_ref p1=new_vertices[1];
  vsol_point_3d_ref p2=new_vertices[2];

  vnl_vector_fixed<double,3>* v1 =
     new vnl_vector_fixed<double,3>(p1->x()-p0->x(),
                                    p1->y()-p0->y(),
                                    p1->z()-p0->z());
  
  vnl_vector_fixed<double,3>* v2 =
     new vnl_vector_fixed<double,3>(p2->x()-p0->x(),
                                    p2->y()-p0->y(),
                                    p2->z()-p0->z());
  vnl_vector_fixed<double,3>* n = // normal to the plane made by the vertices
     new vnl_vector_fixed<double,3>(cross_3d(*v1,*v2));
  delete v2;
  delete v1;

  bool result=true;
  for(int i=3;i<new_vertices.size()&&result;++i)
  {
    p2=new_vertices[i];
    v2=new vnl_vector_fixed<double,3>(p2->x()-p0->x(),
                                      p2->y()-p0->y(),
                                      p2->z()-p0->z());
    result = dot_product(*n,*v2)==0;
    delete v2;
  }
  delete n;

  return result;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// -- Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::in(const vsol_point_3d_ref &p) const
{
  // TO DO
  vcl_cerr << "Warning: vsol_polygon_3d::in() has not been implemented yet\n";
  return false;
}

//---------------------------------------------------------------------------
// -- Return the unit normal vector at point `p'. Have to be deleted manually
// Require: in(p)
//---------------------------------------------------------------------------
vnl_vector_fixed<double,3> *
vsol_polygon_3d::normal_at_point(const vsol_point_3d_ref &p) const
{
  // require
  assert(in(p));

  vnl_vector_fixed<double,3> *result;
  vsol_point_3d_ref p0;
  vsol_point_3d_ref p1;
  vsol_point_3d_ref p2;

  p0=(*storage_)[0];
  p1=(*storage_)[1];
  p2=(*storage_)[2];

  vnl_vector_fixed<double,3> v1(p1->x()-p0->x(),
                                p1->y()-p0->y(),
                                p1->z()-p0->z());
  vnl_vector_fixed<double,3> v2(p2->x()-p0->x(),
                                p2->y()-p0->y(),
                                p2->z()-p0->z());
  
  result=new vnl_vector_fixed<double,3>(cross_3d(v1,v2));
  if((*result)[0]!=0||(*result)[1]!=0||(*result)[2]!=0)
    result->normalize();
  return result;
}

//***************************************************************************
// Implementation
//***************************************************************************

//---------------------------------------------------------------------------
// -- Default constructor. Do nothing. Just to enable inherance.
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(void)
{
}
