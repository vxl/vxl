#include <vcsl/vcsl_translation.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_translation::vcsl_translation(void)
{
  _vector=0;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_translation::~vcsl_translation()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Is `this' invertible at time `time'?
// REQUIRE: valid_time(time)
//---------------------------------------------------------------------------
bool vcsl_translation::is_invertible(const double time) const
{
  // require
  assert(valid_time(time));

  return true;
}

//---------------------------------------------------------------------------
// Is `this' correctly set ?
//---------------------------------------------------------------------------
bool vcsl_translation::is_valid(void) const
{
  return (_vector!=0)&&(((_beat==0)&&(_interpolator==0)&&(_vector->size()==1))
                        ||
                        ((_beat!=0)&&(_interpolator!=0)
                         &&(_beat->size()==(_interpolator->size()+1))
                         &&(_beat->size()==_vector->size())));
}

//***************************************************************************
// Transformation parameters
//***************************************************************************
//---------------------------------------------------------------------------
// Set the parameters of a static translation
//---------------------------------------------------------------------------
void vcsl_translation::set_static(vnl_vector<double> &new_vector)
{
  if(_vector==0||_vector->size()!=1)
    _vector=new list_of_vectors(1);
  (*_vector)[0]=&new_vector;
  _beat=0;
  _interpolator=0;
}

//---------------------------------------------------------------------------
// Set the direction vector variation along the time
//---------------------------------------------------------------------------
void vcsl_translation::set_vector(list_of_vectors &new_vector)
{
  if(_vector!=0&&_vector->size()==1)
    delete _vector;
  _vector=&new_vector;
}

//---------------------------------------------------------------------------
// Return the angle variation along the time
//---------------------------------------------------------------------------
list_of_vectors *vcsl_translation::vector(void) const
{
  return _vector;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// Image of `v' by `this'
// REQUIRE: is_valid()
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_translation::execute(const vnl_vector<double> &v,
                                              const double time) const
{
  // require
  assert(is_valid());

  vnl_vector<double> *result;
  vnl_vector<double> *value;
  int i;

  value=vector_value(time);
  result=new vnl_vector<double>(v.size());
  for(i=0;i<v.size();++i)
    result->put(i,v.get(i)+value->get(i));

  if(_beat!=0) // dynamic
    delete value;

  return result;
}

//---------------------------------------------------------------------------
// Image of `v' by the inverse of `this'
// REQUIRE: is_valid()
// REQUIRE: is_invertible(time)
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_translation::inverse(const vnl_vector<double> &v,
                                              const double time) const
{
  // require
  assert(is_valid());
  assert(is_invertible(time));

  vnl_vector<double> *result;
  vnl_vector<double> *value;
  int i;

  value=vector_value(time);
  result=new vnl_vector<double>(v.size());
  for(i=0;i<v.size();++i)
    result->put(i,v.get(i)-value->get(i));

  if(_beat!=0) // dynamic
    delete value;

  return result;
}

//---------------------------------------------------------------------------
// Compute the value of the parameter at time `time'
//---------------------------------------------------------------------------
vnl_vector<double> *vcsl_translation::vector_value(const double time) const
{
  vnl_vector<double> *result;
  int i;
  
  if(_beat==0) // static
    result=(*_vector)[0];
  else
    {
      i=matching_interval(time);
      switch((*_interpolator)[i])
        {
        case vcsl_linear:
          result=lvi(*(*_vector)[i],*(*_vector)[i+1],i,time);
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
