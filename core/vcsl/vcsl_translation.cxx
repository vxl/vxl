#include <vcsl/vcsl_translation.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_translation::vcsl_translation(void)
{
  vector_=0;
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
  return (vector_!=0)&&(((beat_==0)&&(interpolator_==0)&&(vector_->size()==1))
                        ||
                        ((beat_!=0)&&(interpolator_!=0)
                         &&(beat_->size()==(interpolator_->size()+1))
                         &&(beat_->size()==vector_->size())));
}

//***************************************************************************
// Transformation parameters
//***************************************************************************
//---------------------------------------------------------------------------
// Set the parameters of a static translation
//---------------------------------------------------------------------------
void vcsl_translation::set_static(vnl_vector<double> &new_vector)
{
  if(vector_==0||vector_->size()!=1)
    vector_=new list_of_vectors(1);
  (*vector_)[0]=&new_vector;
  beat_=0;
  interpolator_=0;
}

//---------------------------------------------------------------------------
// Set the direction vector variation along the time
//---------------------------------------------------------------------------
void vcsl_translation::set_vector(list_of_vectors &new_vector)
{
  if(vector_!=0&&vector_->size()==1)
    delete vector_;
  vector_=&new_vector;
}

//---------------------------------------------------------------------------
// Return the angle variation along the time
//---------------------------------------------------------------------------
list_of_vectors *vcsl_translation::vector(void) const
{
  return vector_;
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

  if(beat_!=0) // dynamic
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

  if(beat_!=0) // dynamic
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
  
  if(beat_==0) // static
    result=(*vector_)[0];
  else
    {
      i=matching_interval(time);
      switch((*interpolator_)[i])
        {
        case vcsl_linear:
          result=lvi(*(*vector_)[i],*(*vector_)[i+1],i,time);
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
