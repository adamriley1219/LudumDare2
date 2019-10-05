#include "Engine/Math/RNG.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RawNoise.hpp"


//--------------------------------------------------------------------------
/**
* RNG
*/
RNG::RNG( unsigned int seed /*= 0*/ )
	: m_seed( seed )
	, m_posision( 0 )
{

}

//--------------------------------------------------------------------------
/**
* ~RNG
*/
RNG::~RNG()
{

}

//--------------------------------------------------------------------------
/**
* GetRandomIntLessThan
*/
int RNG::GetRandomIntLessThan( int maxNotInclusice )
{
	unsigned int randomUint = Get1dNoiseUint( m_posision, m_seed );
	m_posision++;
	return randomUint % maxNotInclusice;
}

//--------------------------------------------------------------------------
/**
* GetRandomIntInRange
*/
int RNG::GetRandomIntInRange( int minInclusive, int maxInclusive )
{
	if( minInclusive == maxInclusive  )
	{
		return maxInclusive;
	}
	if( minInclusive > maxInclusive)
	{
		ERROR_RECOVERABLE( "RNG::GetRandomIntInRange was called with bad values." );
		return -1;
	}
	// Shift min to 0
	int shift = maxInclusive + 1; // to include the max
	shift -= minInclusive;
	unsigned int randomUint = Get1dNoiseUint( m_posision, m_seed );
	m_posision++;
	shift = randomUint % shift;

	// Shift back
	return shift + minInclusive;
}


//--------------------------------------------------------------------------
/**
* GetRandomFloatZeroToOne
*/
float RNG::GetRandomFloatZeroToOne()
{
	return Get1dNoiseZeroToOne( m_posision++, m_seed );
}

//--------------------------------------------------------------------------
/**
* GetRandomFloatInRange
*/
float RNG::GetRandomFloatInRange( float minInclusive, float maxInclusive )
{
	if( minInclusive == maxInclusive  )
	{
		return maxInclusive;
	}
	if( minInclusive > maxInclusive)
	{
		ERROR_RECOVERABLE( "RNG::GetRandomIntInRange was called with bad values." );
		return -1.0f;
	}

	float randNum = GetRandomFloatZeroToOne();	// number 0.0f - 1.0f
	randNum *= (maxInclusive - minInclusive);		// scale by range
	return randNum + minInclusive;				// Shift over by min 
}

//--------------------------------------------------------------------------
/**
* Seed
*/
void RNG::Seed( unsigned int newSeed )
{
	m_seed = newSeed;
	m_posision = 0;
}
