
#ifndef RAND_NUM_GENERATOR_H
#define RAND_NUM_GENERATOR_H

#include <iostream>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/utility.hpp>

#include "utility.hpp"

/**
 * Handles the generation of random numbers from
 * several distributions.
 * Uses the singleton design pattern.
 */
class RandNumGenerator : boost::noncopyable {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RandNumGenerator> RandNumGeneratorPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RandNumGeneratorPtr create();

	/**
	 * Change the seed being used.
	 * @param seed the new seed.
	 */
	inline void setSeed(const t_uint seed);

	/**
	 * Generate an int unformly at random from the range
	 * [min,max].
	 * @param min the minimum value that can be returned.
	 * @param max the maximum value that can be returned.
	 * @return an int in the range [min,max].
	 */
	inline int uniformInt(const int min, const int max);

	/**
	 * Generate a real number uniformly at random between [0,1).
	 * @return a double in the range [0,1).
	 */
	inline double uniformZeroOne();

	/**
	 * Generate a real number uniformly at random between [min,max).
	 * @param min the minimum value that can be returned (inclusive).
	 * @param max the maximum value that can be returned (exclusive).
	 * @return a double in the range [min,max).
	 */
	inline double uniformReal(const double min, const double max);

	/**
	 * Generate a random number from an exponential distribution.
	 * @param lambda the expected value of the distribution.
	 * @return a double from an exponential distribution.
	 */
	inline double exponential(const double lambda);

	/**
	 * Generate a random number from a normal distributin.
	 * @param mean the mean of the distribution.
	 * @param sigma the standard deviation of the distribution.
	 * @return a double from a normal distribution.
	 */
	inline double normalDistribution(const double mean, const double sigma);

	/**
	 * Make the output operator a friend for direct access
	 * to class state.
	 */
	friend ostream& operator<< (ostream& s, 
		const RandNumGenerator& randNumGenerator);

private:

	/// Defines which base generator will be used.  See
	/// http://www.boost.org/libs/random/random-generators.html
	/// for more details.
	typedef boost::lagged_fibonacci607 BaseGenerator;

	/// The default value used to seed the generator.
	static const t_uint m_DEFAULT_SEED;

	/// The current seed being used by the generator.
	/// @see setSeed()
	t_uint m_seed;

	/// The generator object.
	BaseGenerator m_baseGenerator;

	/// A constructor.
	RandNumGenerator();

};
typedef boost::shared_ptr<RandNumGenerator> RandNumGeneratorPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RandNumGeneratorPtr RandNumGenerator::create()
{
	RandNumGeneratorPtr rand(new RandNumGenerator());
	return rand;
}

inline void RandNumGenerator::setSeed(t_uint seed)
{
	m_seed = seed;
	m_baseGenerator.seed(m_seed);
}

inline int RandNumGenerator::uniformInt(const int min, const int max)
{
	boost::uniform_int<> uniformInt(min, max);
	boost::variate_generator<BaseGenerator&, boost::uniform_int<> >
		generator(m_baseGenerator, uniformInt);
	int randVal = generator();
	return randVal;
}

inline double RandNumGenerator::uniformZeroOne()
{
	boost::uniform_01<BaseGenerator> generator(m_baseGenerator);
	double randVal = generator();
	return randVal;
}

inline double RandNumGenerator::uniformReal(const double min, 
	const double max)
{
	boost::uniform_real<> uniformReal(min, max);
	boost::variate_generator<BaseGenerator&, boost::uniform_real<> >
		generator(m_baseGenerator, uniformReal);
	double randVal = generator();
	return randVal;
}

inline double RandNumGenerator::exponential(const double lambda)
{
	boost::exponential_distribution<> exponentialDist(lambda);
	boost::variate_generator<BaseGenerator&, 
		boost::exponential_distribution<> >
		generator(m_baseGenerator, exponentialDist);
	double randVal = generator();
	return randVal;
}

inline double RandNumGenerator::normalDistribution(const double mean, 
	const double sigma)
{
	boost::normal_distribution<> normalDist(mean, sigma);
	boost::variate_generator<BaseGenerator&, 
		boost::normal_distribution<> >
		generator(m_baseGenerator, normalDist);
	double randVal = generator();
	return randVal;

}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, 
	const RandNumGenerator& randNumGenerator)
{
	return s<< "RandNumGenerator state (pointer=" << 
		&randNumGenerator << ", seed=" << randNumGenerator.m_seed << ")" ;
}

#endif // RAND_NUM_GENERATOR_H

