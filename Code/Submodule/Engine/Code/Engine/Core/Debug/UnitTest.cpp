#include "Engine/Core/Debug/UnitTest.hpp"
#include "Engine/Core/Debug/Log.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#endif

UnitTest* g_unitTestList = nullptr;


//--------------------------------------------------------------------------
/**
* UnitTestsRunAllCategories
*/
void UnitTestsRunAllCategories( uint priority /*= INT_MAX */ )
{
	UnitTest* testIter = g_unitTestList;
	uint totalTests = 0;
	uint testspassed = 0;

	while ( testIter )
	{
		if ( testIter->m_priority <= priority )
		{
			if( testIter->m_callback() )
			{
				++testspassed;
			}
			else
			{
			#ifdef PLATFORM_WINDOWS
				DebugRenderMessage( 20.0f, Rgba::YELLOW, Rgba::YELLOW, "Failed to pass UnitTest: \"%s\", with category: \"%s\"", testIter->m_name.c_str(), testIter->m_category.c_str() );
				DebuggerPrintf( "Failed to pass UnitTest: \"%s\", with category: \"%s\"", testIter->m_name.c_str(), testIter->m_category.c_str() );
			#endif
				Logf( "UnitTest","Failed to pass UnitTest: \"%s\", with category: \"%s\"", testIter->m_name.c_str(), testIter->m_category.c_str() );
			}
			++totalTests;
		}
		testIter = testIter->m_next;
	}

	ASSERT_RECOVERABLE( totalTests == testspassed, "All the Unit Tests did not pass. Check log for more information." );

}

//--------------------------------------------------------------------------
/**
* UnitTestsRun
*/
void UnitTestsRun( const char* category, uint priority /*= INT_MAX */ )
{
	UnitTest* testIter = g_unitTestList;
	uint totalTests = 0;
	uint testspassed = 0;

	while ( testIter )
	{
		if( testIter->m_priority <= priority && testIter->m_category == category )
		{
			if( testIter->m_callback() )
			{
				++testspassed;
			}
			else
			{
			#ifdef PLATFORM_WINDOWS
				DebugRenderMessage(20.0f, Rgba::YELLOW, Rgba::YELLOW, "Failed to pass UnitTest: \"%s\", with category: \"%s\"", testIter->m_name.c_str(), testIter->m_category.c_str());
				DebuggerPrintf("Failed to pass UnitTest: \"%s\", with category: \"%s\"\n", testIter->m_name.c_str(), testIter->m_category.c_str());
			#endif
				Logf("UnitTest", "Failed to pass UnitTest: \"%s\", with category: \"%s\"", testIter->m_name.c_str(), testIter->m_category.c_str());
			}
			++totalTests;
		}
		testIter = testIter->m_next;
	}

	ASSERT_RECOVERABLE( totalTests == testspassed, "All the Unit Tests did not pass. Check log for more information." );
}

