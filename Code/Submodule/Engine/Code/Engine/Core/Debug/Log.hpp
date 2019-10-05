#pragma once
#include "Engine/Core/Debug/Callstack.hpp"


struct log_message_t
{
	uint64_t hpc;
	size_t size_of_filter;
	size_t size_of_message;
	char* filter;
	char* message;
	Callstack_t callstack; // callstack if there was one
	bool isCallstackValid = false;
};

// NOTE: As with ConsolePrintf, you may use variadic templates instead if you prefer
void LogSystemStartup( char const* log_file );

// will do a final flush of the log file and properly close down
void LogSystemShutdown(); 

// Logs a message with the given filter
void Logf(char const* filter, char const* format, ...);

// Logs a message and includes the callstack on the next line(s)
void LogCallstackf(char const* filter, char const* format, ...);

// confirms all messages have been processed (committed to disk)
void LogFlush();

// Filtering
void LogEnableAll();  // all messages log
void LogDisableAll(); // not messages log
void LogEnable(char const* filter);     // this filter will start to appear in the log
void LogDisable(char const* filter);    // this filter will no longer appear in the log

// Listener Hooks
// Allow other systems to get message events (from the logger thread)
// so it can do something with them
// Example would be to call DebuggerPrintf - or to enable the dev console to print them; 

// NOTE - this message is only valid for the lifetime of the callback
// and is not guaranteed to valid after your hook returns, so be sure to 
// copy data you need!
typedef void (*log_hook_cb)(log_message_t);
void LogHook( log_hook_cb cb );
void LogUnhook( log_hook_cb cb );