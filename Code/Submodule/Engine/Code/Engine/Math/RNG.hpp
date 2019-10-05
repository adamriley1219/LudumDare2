#pragma once

class RNG
{
public:
	RNG( unsigned int seed = 0 );
	~RNG();

	int GetRandomIntLessThan( int maxNotInclusice );
	int GetRandomIntInRange( int minInclusive, int maxInclusive );
	float GetRandomFloatZeroToOne();
	float GetRandomFloatInRange( float minInclusive, float maxInclusive );
	bool PercentChance( float chanceOfReturningTrue );
	void Seed( unsigned int newSeed );

private:
	unsigned int m_seed = 0;
	unsigned int m_posision = 0;
};