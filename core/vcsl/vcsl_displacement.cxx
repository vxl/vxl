#include <vcsl/vcsl_displacement.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_displacement::vcsl_displacement(void)
{
  _point=0;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_displacement::~vcsl_displacement()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_displacement::is_valid(void) const
{
  return (_point!=0)&&(_axis!=0)&&(_angle!=0)
    &&(
       ((_beat==0)&&(_interpolator==0)&&(_point->size()==1)&&
        (_axis->size()==1)&&(_angle->size()==1)
        )
       ||
       (
        (_beat!=0)&&(_interpolator!=0)
        &&(_beat->size()==(_interpolator->size()+1))
        &&(_beat->size()==_point->size())
        &&(_beat->size()==_axis->size())
        &&(_beat->size()==_angle->size())
        ));
}

//***************************************************************************
// Transformation parameters
//***************************************************************************

//---------------------------------------------------------------------------
// Set the point for a static displacement
//---------------------------------------------------------------------------
void vcsl_displacement::set_static_point(vnl_vector<double> &new_point)
{
  if(_point==0||_point->size()!=1)
    _point=new list_of_vectors(1);
  (*_point)[0]=&new_point;
  _beat=0;
  _interpolator=0;
}

//---------------------------------------------------------------------------
// Set the variation of the point of the axis along the time
//---------------------------------------------------------------------------
void vcsl_displacement::set_point(list_of_vectors &new_point)
{
  if(_point!=0&&_point->size()==1)
    delete _point;
  _point=&new_point;
}

//---------------------------------------------------------------------------
// Return the variation of the point of the axis along the time
//---------------------------------------------------------------------------
list_of_vectors *vcsl_displacement::point(void) const
{
  return _point;
}
  
//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_displacement::execute(const vnl_vector<double> &v,
                                               const double time) const
{
  // require
  assert(is_valid());

  vnl_vector<double> *result;
  
  vnl_quaternion<double> *q;
  vnl_vector<double> *translation;
  vnl_vector<double> *tmp;

  int i;
  
  translation=vector_value(time);

  tmp=new vnl_vector<double>(3);
  if(_mode_2d)
    tmp->put(2,0);
  for(i=0;i<v.size();++i)
    tmp->put(i,v.get(i)-translation->get(i));

  q=quaternion(time);

  result=new vnl_vector<double>(q->rotate(*tmp));
  delete q;
  delete tmp;

  tmp=result;
  if(_mode_2d)
    result=new vnl_vector<double>(2);
  for(i=0;i<v.size();++i)
    result->put(i,tmp->get(i)+translation->get(i));
  if(_mode_2d)
     delete tmp;
  if(_beat!=0)
    delete translation;
  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_displacement::inverse(const vnl_vector<double> &v,
                                               const double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));

  vnl_vector<double> *result;
  
  vnl_quaternion<double> *q0;
  vnl_quaternion<double> *q1;
  vnl_vector<double> *translation;
  vnl_vector<double> *tmp;
  int i;

  translation=vector_value(time);

  tmp=new vnl_vector<double>(3);
  if(_mode_2d)
    tmp->put(2,0);
  for(i=0;i<v.size();++i)
    tmp->put(i,v.get(i)-translation->get(i));

  q0=quaternion(time);
  q1=new vnl_quaternion<double>(q0->conjugate());
  delete q0;

  result=new vnl_vector<double>(q1->rotate(*tmp));
  delete q1;
  delete tmp;

  tmp=result;
  if(_mode_2d)
    result=new vnl_vector<double>(2);

  for(i=0;i<v.size();++i)
    result->put(i,tmp->get(i)+translation->get(i));
  if(_mode_2d)
    delete tmp;
  if(_beat!=0)
    delete translation;
  return result;
}

//---------------------------------------------------------------------------
// Compute the value of the vector at time `time'
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_displacement::vector_value(const double time) const
{
  vnl_vector<double> *result;
  int i;

  if(_beat==0) // static
    result=(*_point)[0];
  else
    {
      i=matching_interval(time);
      switch((*_interpolator)[i])
        {
        case vcsl_linear:
          result=lvi(*(*_point)[i],*(*_point)[i+1],i,time);
          break;
        case vcsl_cubic:
          assert(false); // Not yet implemented
          break;
        case vcsl_spline:
          assert(false); // Not yet implemented
          break;
        default:
          assert(false); // Impossible
          break;
        }
    }
  return result;
}
