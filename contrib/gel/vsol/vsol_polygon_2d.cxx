#include <vsol/vsol_polygon_2d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vsol/vsol_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// -- Constructor from a vcl_vector (not a geometric vector but a list of points)
// Require: new_vertices.size()>=3
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices)
{
  // require
  assert(new_vertices.size()>=3);

  storage_=new vcl_vector<vsol_point_2d_sptr>(new_vertices);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(const vsol_polygon_2d &other)
{
  //vsol_point_2d_sptr p;

  storage_=new vcl_vector<vsol_point_2d_sptr>(*other.storage_);
  for(unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_2d(*((*other.storage_)[i]));
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_polygon_2d::~vsol_polygon_2d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_polygon_2d::clone(void) const
{
  return new vsol_polygon_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return vertex `i'
// Require: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polygon_2d::vertex(const int i) const
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
bool vsol_polygon_2d::operator==(const vsol_polygon_2d &other) const
{
  bool result = (this==&other);

  if(!result)
    {
      result = (storage_->size()==other.storage_->size());
      if(result)
        {
          vsol_point_2d_sptr p=(*storage_)[0];

          result=false;
          unsigned int i=0;
          for(;i<storage_->size()&&!result;++i)
            result = (*p==*(*other.storage_)[i]);
          if(result)
            {
              unsigned int j=i;
              i=1;
              while(i<size()&&result)
                {
                  result = ((*storage_)[i]==(*storage_)[j]);
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

bool vsol_polygon_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::REGION &&
   ((vsol_region_2d const&)obj).region_type() == vsol_region_2d::POLYGON
  ? *this == (vsol_polygon_2d const&) (vsol_region_2d const&) obj
  : false;
}


//---------------------------------------------------------------------------
// -- Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_polygon_2d::operator!=(const vsol_polygon_2d &other) const
{
  return !operator==(other);
}

//---------------------------------------------------------------------------
// -- Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polygon_2d::compute_bounding_box(void)
{
  double xmin=(*storage_)[0]->x();
  double ymin=(*storage_)[0]->y();
  double xmax=xmin;
  double ymax=ymin;

  for(unsigned int i=0;i<storage_->size();++i)
    {
       double x=(*storage_)[i]->x();
       double y=(*storage_)[i]->y();
       if(x<xmin)
         xmin=x;
       else if(x>xmax)
         xmax=x;
       if(y<ymin)
         ymin=y;
       else if(y>ymax)
         ymax=y;
    }

  if(_bounding_box==0)
    _bounding_box=new vsol_box_2d();
  _bounding_box->set_min_x(xmin);
  _bounding_box->set_max_x(xmax);
  _bounding_box->set_min_y(ymin);
  _bounding_box->set_max_y(ymax);
}

//---------------------------------------------------------------------------
// -- Return the number of vertices
//---------------------------------------------------------------------------
int vsol_polygon_2d::size(void) const
{
  return storage_->size();
}

//---------------------------------------------------------------------------
// -- Return the area of `this'
//---------------------------------------------------------------------------
double vsol_polygon_2d::area(void) const
{
  vcl_cerr << "Warning: vsol_polygon_2d::area() has not yet been implemented\n";
  return -1; // TO DO
}

//---------------------------------------------------------------------------
// -- Is `this' convex ?
//---------------------------------------------------------------------------
bool vsol_polygon_2d::is_convex(void) const
{
  bool result;

  double nz_old; // cote of the previous normal
  double nz; // cote of the current normal
  double dx;
  double dy;
  double tmp;
  double x_old;
  double y_old;
  double x;
  double y;
  int i;

  result=size()==3; // A triangle is always convex

  // Initialization: computation of the first normal

  if(!result)
    {
      x_old=(*storage_)[0]->x();
      y_old=(*storage_)[0]->y();
      x=(*storage_)[1]->x();
      y=(*storage_)[1]->y();
      dx=x-x_old;
      dy=y-y_old;

      nz=dx;
      x_old=x;
      x=x=(*storage_)[2]->x();
      dx=x-x_old;
      tmp=dx*dy;
      y_old=y;
      y=(*storage_)[2]->y();
      dy=y-y_old;
      nz=nz*dy-tmp;

      result=true;
      for(i=3;i<size()&&result;++i)
        {
          nz_old=nz;
          nz=dx;
          x_old=x;
          x=x=(*storage_)[i]->x();
          dx=x-x_old;
          tmp=dx*dy;
          y_old=y;
          y=(*storage_)[i]->y();
          dy=y-y_old;
          nz=nz*dy-tmp;
          result=(nz<0&&nz_old<0)||(nz>0&&nz_old>0);
        }
      if(result)
        {
          nz_old=nz;
          nz=dx;
          x_old=x;
          x=x=(*storage_)[0]->x();
          dx=x-x_old;
          tmp=dx*dy;
          y_old=y;
          y=(*storage_)[0]->y();
          dy=y-y_old;
          nz=nz*dy-tmp;
          result=(nz<0&&nz_old<0)||(nz>0&&nz_old>0);
        }
    }

  return result;
}

//---------------------------------------------------------------------------
// -- Is `i' a valid index for the list of vertices ?
//---------------------------------------------------------------------------
bool vsol_polygon_2d::valid_index(const int i) const
{
  return (i>=0)&&(i<storage_->size());
}

//***************************************************************************
// Implementation
//***************************************************************************

//---------------------------------------------------------------------------
// -- Default constructor. Do nothing. Just to enable inherance.
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(void)
{
}
