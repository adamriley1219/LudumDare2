#include "Engine/Math/RawNoise.hpp"





//-----------------------------------------------------------------------------------------------
// Fast hash of an int32 into a different (unrecognizable) uint32.
//
// Returns an unsigned integer containing 32 reasonably-well-scrambled bits, based on the hash
//	of a given (signed) integer input parameter (position/index) and [optional] seed.  Kind of
//	like looking up a value in an infinitely large table of previously generated random numbers.
//
// The bit-noise constants and bit-shifts were evolved by a genetic algorithm using the
//	"BigCrush" statistical tests for fitness, and have so far produced excellent test results.
//
// I call this particular approach SquirrelNoise (version 4).
//
unsigned int Get1dNoiseUint(int positionX, unsigned int seed)
{
	constexpr unsigned int BIT_NOISE1 = 0xd2a80a23;
	constexpr unsigned int BIT_NOISE2 = 0xa884f197;
	constexpr unsigned int BIT_NOISE3 = 0x1b56c4e9;

	unsigned int mangledBits = (unsigned int)positionX;
	mangledBits *= BIT_NOISE1;
	mangledBits += seed;
	mangledBits ^= (mangledBits >> 7);
	mangledBits += BIT_NOISE2;
	mangledBits ^= (mangledBits >> 8);
	mangledBits *= BIT_NOISE3;
	mangledBits ^= (mangledBits >> 11);
	return mangledBits;
}


//-----------------------------------------------------------------------------------------------
unsigned int Get2dNoiseUint(int indexX, int indexY, unsigned int seed)
{
	constexpr int PRIME_NUMBER = 198491317; // Large prime number with non-boring bits
	return Get1dNoiseUint(indexX + (PRIME_NUMBER * indexY), seed);
}

//-----------------------------------------------------------------------------------------------
unsigned int Get3dNoiseUint(int indexX, int indexY, int indexZ, unsigned int seed)
{
	constexpr int PRIME1 = 198491317; // Large prime number with non-boring bits
	constexpr int PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
	return Get1dNoiseUint(indexX + (PRIME1 * indexY) + (PRIME2 * indexZ), seed);
}

//-----------------------------------------------------------------------------------------------
unsigned int Get4dNoiseUint(int indexX, int indexY, int indexZ, int indexT, unsigned int seed)
{
	constexpr int PRIME1 = 198491317; // Large prime number with non-boring bits
	constexpr int PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
	constexpr int PRIME3 = 357239; // Large prime number with distinct and non-boring bits
	return Get1dNoiseUint(indexX + (PRIME1 * indexY) + (PRIME2 * indexZ) + (PRIME3 * indexT), seed);
}

//-----------------------------------------------------------------------------------------------
float Get1dNoiseZeroToOne(int index, unsigned int seed)
{
	constexpr double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
	return (float)(ONE_OVER_MAX_UINT * (double)Get1dNoiseUint(index, seed));
}

//-----------------------------------------------------------------------------------------------
float Get2dNoiseZeroToOne(int indexX, int indexY, unsigned int seed)
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
	return (float)(ONE_OVER_MAX_UINT * (double)Get2dNoiseUint(indexX, indexY, seed));
}

//-----------------------------------------------------------------------------------------------
float Get3dNoiseZeroToOne(int indexX, int indexY, int indexZ, unsigned int seed)
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
	return (float)(ONE_OVER_MAX_UINT * (double)Get3dNoiseUint(indexX, indexY, indexZ, seed));
}

//-----------------------------------------------------------------------------------------------
float Get4dNoiseZeroToOne(int indexX, int indexY, int indexZ, int indexT, unsigned int seed)
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
	return (float)(ONE_OVER_MAX_UINT * (double)Get4dNoiseUint(indexX, indexY, indexZ, indexT, seed));
}


//-----------------------------------------------------------------------------------------------
float Get1dNoiseNegOneToOne(int index, unsigned int seed)
{
	const double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
	return (float)(ONE_OVER_MAX_INT * (double)(int)Get1dNoiseUint(index, seed));
}


//-----------------------------------------------------------------------------------------------
float Get2dNoiseNegOneToOne(int indexX, int indexY, unsigned int seed)
{
	const double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
	return (float)(ONE_OVER_MAX_INT * (double)(int)Get2dNoiseUint(indexX, indexY, seed));
}


//-----------------------------------------------------------------------------------------------
float Get3dNoiseNegOneToOne(int indexX, int indexY, int indexZ, unsigned int seed)
{
	const double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
	return (float)(ONE_OVER_MAX_INT * (double)(int)Get3dNoiseUint(indexX, indexY, indexZ, seed));
}


//-----------------------------------------------------------------------------------------------
float Get4dNoiseNegOneToOne(int indexX, int indexY, int indexZ, int indexT, unsigned int seed)
{
	const double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
	return (float)(ONE_OVER_MAX_INT * (double)(int)Get4dNoiseUint(indexX, indexY, indexZ, indexT, seed));
}





