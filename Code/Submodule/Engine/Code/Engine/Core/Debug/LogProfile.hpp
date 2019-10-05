#pragma once
#include "Engine/Core/Time/Time.hpp"


class LogProfileScope
{
public:

	LogProfileScope( const char* name );
	~LogProfileScope();

private:
	uint64_t m_statTime;
	const char* m_name;
};


