#ifndef mbl_mz_random_h
#define mbl_mz_random_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Aaron Kotcheff
// \brief A superior random number generator

#include<assert.h>
// #include<stddef.h>


const int mbl_mz_array_size = 37;

//: A superior random number generator
// Implements a new random number generator that
// recently appeared in the literature. It generates 32 bit
// numbers with a higher degree of randomness than previous
// generators and has a cycle of 10^354 i.e. so huge that in
// practice it never cycles.
// For the mathematics behind it see:
// "A New Class of Random Number Generators" G. Marsaglia and A. Zaman,
// Annals of Applied Probability 1991, Vol. 1, No. 3, 462.
// \author Aaron Kotcheff
class mbl_mz_random
{
	private:
		enum {linear_congruential_multiplier = 1664525, mz_previous1 = 24};
		unsigned long linear_congruential_previous;
		unsigned long mz_seed_array[mbl_mz_array_size];
		unsigned long mz_array[mbl_mz_array_size];
		int mz_array_position;
		int mz_borrow;
		unsigned long linear_congruential_lrand32() {return (linear_congruential_previous = (linear_congruential_previous*linear_congruential_multiplier + 1));}

		double mz_previous_normal;
		int mz_previous_normal_flag;

	public:
			//: Default constructor
			// Initializes the random number generator non-deterministically.
			// i.e. it will generate a different series of random numbers each
			// time the program is run.
		mbl_mz_random();

			//: Destructor
		~mbl_mz_random();

			//: Construct with seed
			//  Initializes the random number generator deterministically
			//  using a single ulong as the 'seed'. A linear congruential
			//  generator is used to generate the 37 ulongs needed
			//  as the real seed. The same seed will produce the
			//  same series of random numbers.
		mbl_mz_random(unsigned long seed) : mz_array_position(0L), mz_borrow(0), mz_previous_normal_flag(0) {reseed(seed);}

			//: Construct with seed
			//  Initializes the random number generator deterministically
			//  using 37 ulongs as the 'seed'. The same seed will
			//  produce the same series of random numbers.
		mbl_mz_random(unsigned long seed[mbl_mz_array_size]) : mz_array_position(0L), mz_borrow(0), mz_previous_normal_flag(0) {reseed(seed);}

			//: Copy constructor
			//  Initializes/sets the random number generator to exactly
			//  the same state as the argument, i.e. both will generate exactly
			//  the same series of random numbers from then on.
		mbl_mz_random(const mbl_mz_random&);

			//: Copy operator
			//  Initializes/sets the random number generator to exactly
			//  the same state as the argument, i.e. both will generate exactly
			//  the same series of random numbers from then on.

		mbl_mz_random& operator=(const mbl_mz_random&);
           //: Starts a new non-deterministic sequence from an already declared generator.
		void reseed();

			//: Starts a new deterministic sequence from an already declared generator using the provided seed.
		void reseed(unsigned long);

			//: Starts a new deterministic sequence from an already declared generator using the provided seed.
		void reseed(unsigned long[mbl_mz_array_size]);

			//: This restarts the sequence of random numbers.
			//  Restarts so that it repeats
			//  from the point at which you declared the generator, last
			//  intialized it, or last called a 'reseed'.
		void restart();

			//: Generates a random unsigned long.
		unsigned long lrand32();

			//: Generates a random unsigned long in [0,a]
		int lrand32(int a, int=0);

			//: Generates a random unsigned long in [b,a]
		int lrand32(int a, int b, int&);

			//:  Generates a random double in the range b <= x <= a with 32 bit randomness.
			//   drand32(1,0) is random down to about the 10th decimal place.
		double drand32(double a, double b);

			//: Generates a random double in the range b <= x <= a with 64 bit randomness,
			//  Completely random down to the accuracy of a double.
		double drand64(double a, double b);

			//: Random value from a unit normal distribution about zero
			// Uses a drand32() as its underlying generator.
			// Because the function uses a probability transform, the randomness (and quantisation) is non-linearly dependant on the
			// value. The further the sample is from zero, the lower the number of bits to which it is random.
		double normal();

			//: Random value from a unit normal distribution about zero
			// Uses a drand64() as its underlying generator.
			// Because the function uses a probability transform, the randomness (and quantisation) is non-linearly dependant on the
			// value. The further the sample is from zero, the lower the number of bits to which it is random.
		double normal64();
	};

inline unsigned long mbl_mz_random::lrand32()
	{
	unsigned long p1 = mz_array[(mbl_mz_array_size + mz_array_position - mz_previous1)%mbl_mz_array_size];
	unsigned long p2 = p1 - mz_array[mz_array_position] - mz_borrow;
	if (p2 < p1) mz_borrow = 0;
	if (p2 > p1) mz_borrow = 1;
	mz_array[mz_array_position] = p2;
	mz_array_position = (++mz_array_position)%mbl_mz_array_size;
	return p2;
	}

inline int mbl_mz_random::lrand32(int upper, int lower)
	{
	assert(upper > lower);

	// Note: we have to reject some numbers otherwise we get a very slight bias
	// towards the lower part of the range lower - upper. See below

	unsigned long range = upper-lower+1;
	unsigned long denom = 0xffffffff/range;
	unsigned long ran;
	while ((ran=lrand32())>=denom*range);
	return lower + ran/denom;
	}


inline int mbl_mz_random::lrand32(int upper, int lower, int &count)
	{
	assert(upper > lower);

	// Note: we have to reject some numbers otherwise we get a very slight bias
	// towards the lower part of the range lower - upper. Hence this is a "count"
	// version of the above function that returns the number of lrand32()
	// calls made.

	unsigned long range = upper-lower+1;
	unsigned long denom = 0xffffffff/range;
	unsigned long ran;
	count = 1;
	while ((ran=lrand32())>=denom*range) ++count;
	return lower + ran/denom;
	}

inline double mbl_mz_random::drand32(double upper = 1.0, double lower = 0.0)
	{
	assert(upper > lower);
	return  (double(lrand32())/0xffffffff)*(upper-lower) + lower;
	}

inline double mbl_mz_random::drand64(double upper = 1.0, double lower = 0.0)
	{
	assert(upper > lower);
	return  (double(lrand32())/0xffffffff + double(lrand32())/(double(0xffffffff)*double(0xffffffff)))*(upper-lower) + lower;
	}


#endif // mbl_mz_random_h
