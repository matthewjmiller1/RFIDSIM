
#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
using namespace std;
#include <math.h>
#include <boost/shared_ptr.hpp>

/**
 * \file utility.hpp
 * This includes some \c typedef's, functions, and constants
 * that are used globally.
 */

/// Shorten the name for unsigned primitives.
typedef unsigned int t_uint; 

/// Shorten the name for unsigned primitives.
typedef unsigned long t_ulong; 

/// Shorten the name for unsigned primitives.
typedef unsigned char t_uchar;

/// Smart pointer that clients should use.
typedef boost::shared_ptr<ostream> ostreamPtr;

/**
 * Convert a power value to decibels (dB).
 * The denominator of the ratio is 1.0.
 * @param powerValue the original power value.
 * @return the power value converted to dB.
 */
inline double powerToDecibels(double powerValue)
{
	assert(powerValue != 0.0);
	return (10.0 * log10(powerValue / 1.0));
}

/**
 * Convert a decibel (dB) value to a power value.
 * The denominator of the ratio is 1.0.
 * @param decibelValue the value in dB.
 * @return the dB value converted to its original power value.
 */
inline double decibelsToPower(double decibelValue)
{
	return (1.0 * pow(10, (decibelValue / 10.0)));
}

/// Speed of light constant in meters per seconds (m/s).
const double SPEED_OF_LIGHT = 299792458.0;

/// The constant for PI.
const double PI = 3.14159265;

#endif // UTILITY_H

