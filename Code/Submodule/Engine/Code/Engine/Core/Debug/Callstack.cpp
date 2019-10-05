#include "Engine/Core/Debug/Callstack.hpp"

#if defined( _WIN32 )
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"

#define WIN32_LEAN_AND_MEAN		
#include <windows.h>	
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")


//--------------------------------------------------------------------------
/**
* CallstackGet
*/
Callstack_t GetCallstack( uint skip_frames /*= 0*/ )
{
	Callstack_t toRet;
	toRet.m_depth = CaptureStackBackTrace( skip_frames, MAX_TRACE, toRet.m_trace, (PDWORD) &toRet.m_hash );
	return toRet;
}

//--------------------------------------------------------------------------
/**
* CallstackToString
*/
std::vector<std::string> CallstackToString( const Callstack_t& cs )
{
	std::vector<std::string> toRet;
	HANDLE processHandle = GetCurrentProcess();

	bool pass = SymInitialize( processHandle, NULL, true );

	if( pass )
	{
		for (uint stackIdx = 0; stackIdx < cs.m_depth; ++stackIdx)
		{
			SYMBOL_INFO* symInfo = (SYMBOL_INFO*) TrackedAlloc( sizeof(SYMBOL_INFO) + (1026 - 1) * sizeof(TCHAR) );
			symInfo->MaxNameLen = 1026;
			symInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

			IMAGEHLP_LINE64 lineInfo;
			lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			
			DWORD offset;
			DWORD64 displacement;
			bool sucessSym = SymFromAddr( processHandle, (DWORD64)cs.m_trace[stackIdx], &displacement, symInfo );
			bool sucessLine = SymGetLineFromAddr( processHandle, (DWORD64)cs.m_trace[stackIdx], &offset, &lineInfo );
			if ( sucessLine && sucessSym )
			{
				Strings filePathSplit = SplitStringOnDelitmiter( lineInfo.FileName, "\\" );

				std::string filePath = filePathSplit[filePathSplit.size() - 1];
				for( uint splitIdx = 0; splitIdx < (uint) filePathSplit.size(); ++splitIdx )
				{
					if( filePathSplit[splitIdx] == "Code" )
					{
						filePath.clear();
						filePath = filePathSplit[splitIdx];
						for( uint startString = splitIdx + 1; startString < (uint) filePathSplit.size(); ++startString )
						{
							filePath = Stringf( "%s/%s", filePath.c_str(), filePathSplit[startString].c_str() );
						}
						break;
					}
				}

				toRet.push_back(Stringf("%s(%u,%u): %s", filePath.c_str(), lineInfo.LineNumber, offset, symInfo->Name));
			}
			TrackedFree(symInfo);
		}
		SymCleanup( processHandle );
	}

	return toRet;
}

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
/**
* PrintCallstackAtLocation
*/
void PrintCallstackAtLocation()
{
	DebuggerPrintf("Print Callstack At Loaction:\n");
	Callstack_t cs = GetCallstack(2);
	std::vector<std::string> stackInfo = CallstackToString(cs);
	PrintToOutput(stackInfo);
}

#else
//--------------------------------------------------------------------------
/**
* CallstackGet
*/
Callstack_t GetCallstack( uint skip_frames /*= 0*/ )
{
	return Callstack_t();
}

//--------------------------------------------------------------------------
/**
* CallstackToString
*/
std::vector<std::string> CallstackToString( Callstack_t const& cs )
{

}

//--------------------------------------------------------------------------
/**
* PrintCallstackAtLocation
*/
void PrintCallstackAtLocation()
{

}
#endif

/**
* PrintToOutput
*/
void PrintToOutput(const std::vector<std::string>& lines)
{
	for (std::string line : lines)
	{
		DebuggerPrintf("%s\n", line.c_str());
	}
}

//--------------------------------------------------------------------------
/**
* operator==
*/
bool Callstack_t::operator==(const Callstack_t& compare) const
{
	return compare.m_hash == m_hash;
}

//--------------------------------------------------------------------------
/**
* operator!=
*/
bool Callstack_t::operator!=(const Callstack_t& compare) const
{
	return compare.m_hash != m_hash;
}
