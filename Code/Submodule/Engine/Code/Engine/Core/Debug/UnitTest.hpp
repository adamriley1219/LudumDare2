#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <limits.h>

class UnitTest;
//--------------------------------------------------------------------------
// Finish construction of the function the unit test will 
// Example:
// UNITEST("Test Name", "cat", pri)
// {
//		// work
//		return true;
// }
//--------------------------------------------------------------------------
#define UNITTEST( name, category, priority )																						\
		static bool MACRO_COMBINE(__UnitTest_,__LINE__)();																			\
		static UnitTest MACRO_COMBINE(__UnitTestObj_,__LINE__)( name, category, priority, MACRO_COMBINE(__UnitTest_,__LINE__) );	\
		static bool MACRO_COMBINE(__UnitTest_,__LINE__)()																							

#define CHECK(e) if (!(e)) { return false; }

// Will run all categories, any test that has priority <= the passed in priority
void UnitTestsRunAllCategories( uint priority = INT_MAX ); 

// Runs all unit tests that are part of this category (nullptr counts as a valid category). 
void UnitTestsRun( const char* category, uint priority = INT_MAX ); 


//--------------------------------------------------------------------------
// Used for UNITTEST macro and helper functions but not intended for external use.
//--------------------------------------------------------------------------

extern UnitTest* g_unitTestList;
typedef bool (*test_work_cb)();

class UnitTest
{
public:
	UnitTest(char const* name, char const* category, uint priority, test_work_cb cb)
		: m_name(name)
		, m_category(category)
		, m_priority(priority)
		, m_callback(cb)
	{
		m_next = g_unitTestList;
		g_unitTestList = this;
	}

public:
	std::string m_name;
	std::string m_category;
	uint m_priority;
	test_work_cb m_callback;

	UnitTest* m_next = nullptr;

};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------