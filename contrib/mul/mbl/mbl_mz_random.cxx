#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vcl_cmath.h>

#include<mbl/mbl_mz_random.h>

mbl_mz_random::mbl_mz_random(const mbl_mz_random& r) : linear_congruential_previous(r.linear_congruential_previous), mz_array_position(r.mz_array_position),mz_borrow(r.mz_borrow), mz_previous_normal_flag(r.mz_previous_normal_flag)
  {
  for(int i=0;i<mbl_mz_array_size;++i)
    {
    mz_seed_array[i] = r.mz_seed_array[i];
    mz_array[i] = r.mz_array[i];
    }
  }

mbl_mz_random& mbl_mz_random::operator=(const mbl_mz_random& r)
  {
  linear_congruential_previous=r.linear_congruential_previous;
  mz_array_position=r.mz_array_position;
  mz_borrow=r.mz_borrow;
  mz_previous_normal_flag=r.mz_previous_normal_flag;
  for(int i=0;i<mbl_mz_array_size;++i)
    {
    mz_seed_array[i] = r.mz_seed_array[i];
    mz_array[i] = r.mz_array[i];
    }
  return *this;
  }

mbl_mz_random::mbl_mz_random() : mz_array_position(0), mz_borrow(0), mz_previous_normal_flag(0)
  {
  reseed();
  }

mbl_mz_random::~mbl_mz_random()
  {
  for(int i=0;i<mbl_mz_array_size;++i)
    {
    mz_seed_array[i] = 0;
    mz_array[i] = 0;
    }
  }

void mbl_mz_random::reseed()
  {
  reseed(vcl_time(NULL));
  }

void mbl_mz_random::reseed(unsigned long seed)
  {
  mz_array_position = 0L;
  mz_borrow = 0L;

  linear_congruential_previous = seed;
  // Use the lc generator to fill the array
  for(int i=0;i<mbl_mz_array_size;++i)
    {
    mz_seed_array[i] = linear_congruential_lrand32();
    mz_array[i] = mz_seed_array[i];
    }

  // Warm up with 1000 randoms
  for(int j=0;j<1000;j++) lrand32();
  }

void mbl_mz_random::reseed(unsigned long seed[mbl_mz_array_size])
  {
  mz_array_position = 0L;
  mz_borrow = 0L;

  for(int i=0;i<mbl_mz_array_size;++i)
    {
    mz_array[i] = seed[i];
    mz_seed_array[i] = seed[i];
    }
  }

void mbl_mz_random::restart()
  {
  mz_array_position = 0L;

  for(int i=0;i<mbl_mz_array_size;++i)
    {
    mz_array[i] = mz_seed_array[i];
    }
  }

double mbl_mz_random::normal()
{
  if (mz_previous_normal_flag)
  {
    mz_previous_normal_flag = 0;
    return mz_previous_normal;
  }
  else
  {
    double x,y,r2;
    do
    {
      x = drand32(1.0,-1.0);
      y = drand32(1.0,-1.0);
      r2 = x*x+y*y;
    }
    while(r2 >=1.0 || r2 == 0.0);
    double fac = sqrt(-2.0*log(r2)/r2);
    mz_previous_normal = x*fac;
    mz_previous_normal_flag = 1;
    return y*fac;
  }
}



//: Random value from a unit normal distribution about zero
// Uses a drand64() as its underlying generator.
// Because the function uses a probability transform, the randomness (and quantisation) is non-linearly dependant on the
// value. The further the sample is from zero, the lower the number of bits to which it is random.
double mbl_mz_random::normal64()
{
  if (mz_previous_normal_flag)
  {
    mz_previous_normal_flag = 0;
    return mz_previous_normal;
  }
  else
  {
    double x,y,r2;
    do
    {
      x = drand64(1.0,-1.0);
      y = drand64(1.0,-1.0);
      r2 = x*x+y*y;
    }
    while(r2 >=1.0 || r2 == 0.0);
    double fac = sqrt(-2.0*log(r2)/r2);
    mz_previous_normal = x*fac;
    mz_previous_normal_flag = 1;
    return y*fac;
  }
}

