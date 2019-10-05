#include "Engine/Core/Debug/Log.hpp"

#include "Engine/Core/Memory/MPSCRingBuffer.hpp"
#include "Engine/Core/Time/Time.hpp"

#include <stdarg.h>
#include <set>
#include <string.h>
#include <mutex>
#include <shared_mutex>
#include <thread>




class LogSystem
{
public:
	std::thread m_thread;
	std::string m_filename;

	// other things this need
	// if just async queue
	MPSCRingBuffer m_buffer;

public:
	bool IsRunning() const { return m_is_running; }
	void Stop();
	void Start();

	void Flush() { m_flush = true; };
	bool m_flush = false;

public:
	bool m_is_running = false;

	bool m_blackList = true;
	std::set<std::string> m_filter;

	std::vector<log_hook_cb> m_logHooks;
};

static std::condition_variable g_Signal;
static LogSystem g_logger;
static std::shared_mutex g_filterLock;
static std::shared_mutex g_hookLock;


//--------------------------------------------------------------------------
/**
* Stop
*/
void LogSystem::Stop()
{
	m_is_running = false;
	g_Signal.notify_one();
}

//--------------------------------------------------------------------------
/**
* Start
*/
void LogSystem::Start()
{
	m_buffer.Init( 1000000 );
	m_is_running = true;
}



void LogThread()
{
	// setup
	FILE* file;
	file = fopen( g_logger.m_filename.c_str(), "wb");

	if (file == nullptr) {
		// bad juju
		// mark an error, return from thread;
		ERROR_RECOVERABLE( "Invalid file in LogThread()" );
		return;
	}

	std::mutex mutex;

	// locks/enters this mutex automatically by default
	// condition variables guarantee we have a lock when 
	// returned, so we are providing one to use; 
	std::unique_lock<std::mutex> local_lock(mutex);

	// wait for work
	while ( g_logger.IsRunning() ) 
	{
		// wait for a signal, no real condition to check
		// hence returning true (allow it to wakeup when signaled)
		g_Signal.wait(local_lock, []() { return true; });

		log_message_t* log;
		size_t total_size;
		while ( ( log = (log_message_t*) g_logger.m_buffer.TryLockRead( &total_size ) ) != nullptr )
		{
			g_hookLock.lock_shared();
			for( uint hookIdx = 0; hookIdx < g_logger.m_logHooks.size(); ++hookIdx )
			{
				if( g_logger.m_logHooks[hookIdx] )
				{
					g_logger.m_logHooks[hookIdx]( *log );
				}
			}
			g_hookLock.unlock_shared();

			fwrite( "Log:", 1, 4, file );
			fwrite( log->filter, 1, log->size_of_filter, file );
			fwrite( ":", 1, 1, file );
			fwrite( log->message, 1, log->size_of_message - 1, file );
			if( log->isCallstackValid )
			{
				fwrite( "\nCallstack:", 1, 11, file );
				Strings callstack = CallstackToString( log->callstack );
				for( std::string& string : callstack )
				{
					fwrite( "\n", 1, 1, file );
					fwrite( string.c_str(), 1, string.size(), file );
				}
			}
			fwrite( "\n", 1, 1, file );


			g_logger.m_buffer.UnlockRead( log );
		}
		if( g_logger.m_flush )
		{
			fflush( file );
			g_logger.m_flush = false;
		}
	}

	fclose( file );
}


//--------------------------------------------------------------------------
/**
* LogSystemStartup
*/
void LogSystemStartup( char const* log_file )
{
	LogSystemShutdown();
	g_logger.m_filename = log_file;
	g_logger.m_thread = std::thread( LogThread );
	g_logger.Start();
}

//--------------------------------------------------------------------------
/**
* LogSystemShutdown
*/
void LogSystemShutdown()
{
	if( g_logger.IsRunning() )
	{
		g_logger.Stop();
		g_logger.m_thread.join();
	}
}

bool IsFiltered( const char* filter )
{
	g_filterLock.lock_shared();

	bool ret = g_logger.m_filter.find( filter ) != g_logger.m_filter.end();

	if( !g_logger.m_blackList )
	{
		ret = !ret;
	}

	g_filterLock.unlock_shared();
	return ret;
}

//--------------------------------------------------------------------------
/**
* Logf
* Log struct, then format, then filter into the RingBuffer
*/
void Logf( char const* filter, char const* format, ... )
{
	// do not work if the message is currently filtered; 
	// you could alternatively do this on the thread if you're
	// allowing for seperate filters for console/files/etc...
	if ( IsFiltered(filter) || !g_logger.IsRunning() )
	{
		return;
	}

	log_message_t log;

	// lock enough space to write log information; 
	// look into `vsnprintf` for determining buffer size needs
	// for the format/arguments...
	va_list variableArgumentList;
	va_list copyArgs;
	va_start(variableArgumentList, format);
	va_copy(copyArgs, variableArgumentList);

	log.size_of_message = (size_t)vsnprintf(nullptr, 0, format, copyArgs) + 1;
	va_end(copyArgs);

	log.size_of_filter = strlen(filter);
	size_t total_size = log.size_of_message + log.size_of_filter + sizeof(log_message_t);

	// get callstack; 
	log.callstack = GetCallstack(3);
	log.isCallstackValid = false;

	// Time of message
	log.hpc = GetCurrentTimeHPC();

	// allocate it using a thread-safe ring-buffer
	byte* buf = (byte*)g_logger.m_buffer.LockWrite(total_size);

	// Set pointers into the buffer
	log.message = (char*)(buf + sizeof(log_message_t));
	log.filter = (log.message + log.size_of_message);

	// sub allocate and finalize the message
	memcpy(buf, &log, sizeof(log_message_t));

	// Put message into the buffer
	vsnprintf(log.message, log.size_of_message, format, variableArgumentList);
	va_end(variableArgumentList);

	// Put filter into the buffer
	memcpy(log.filter, filter, log.size_of_filter);

	// mark us as finished writing - so worker thread can pick it up
	g_logger.m_buffer.UnlockWrite(buf);

	// Let the thread know it has work; 
	g_Signal.notify_one();
}

//--------------------------------------------------------------------------
/**
* LogCallstackf
*/
void LogCallstackf( char const* filter, char const* format, ... )
{
	// do not work if the message is currently filtered; 
	// you could alternatively do this on the thread if you're
	// allowing for seperate filters for console/files/etc...
	if ( IsFiltered(filter) || !g_logger.IsRunning() )
	{
		return;
	}

	log_message_t log;

	// lock enough space to write log information; 
	// look into `vsnprintf` for determining buffer size needs
	// for the format/arguments...
	va_list variableArgumentList;
	va_list lengthOne;
	va_start( variableArgumentList, format );
	va_copy( lengthOne, variableArgumentList );

	log.size_of_message = (size_t)vsnprintf( nullptr, 0, format, lengthOne ) + 1;
	va_end( lengthOne );

	log.size_of_filter = strlen(filter);
	size_t total_size = log.size_of_message + log.size_of_filter + sizeof(log_message_t);

	// get callstack; 
	log.callstack = GetCallstack(3);
	log.isCallstackValid = true;

	// Time of message
	log.hpc = GetCurrentTimeHPC();

	// allocate it using a thread-safe ring-buffer
	byte* buf = (byte*)g_logger.m_buffer.LockWrite(total_size);

	// Set pointers into the buffer
	log.message = (char*)(buf + sizeof(log_message_t));
	log.filter = (log.message + log.size_of_message);

	// sub allocate and finalize the message
	memcpy( buf, &log, sizeof(log_message_t) );

	// Put message into the buffer
	vsnprintf( log.message, log.size_of_message, format, variableArgumentList );
	va_end( variableArgumentList );

	// Put filter into the buffer
	memcpy( log.filter, filter, log.size_of_filter );

	// mark us as finished writing - so worker thread can pick it up
	g_logger.m_buffer.UnlockWrite(buf);

	// Let the thread know it has work; 
	g_Signal.notify_one();
}

//--------------------------------------------------------------------------
/**
* LogFlush
*/
void LogFlush()
{
	g_logger.Flush();
	g_Signal.notify_one();
	while ( g_logger.m_flush )
	{
		std::this_thread::yield();
	}
}

//--------------------------------------------------------------------------
/**
* LogEnableAll
*/
void LogEnableAll()
{
	g_filterLock.lock();

	g_logger.m_filter.clear();
	g_logger.m_blackList = true;

	g_filterLock.unlock();
}

//--------------------------------------------------------------------------
/**
* LogDisableAll
*/
void LogDisableAll()
{
	g_filterLock.lock();

	g_logger.m_filter.clear();
	g_logger.m_blackList = false;

	g_filterLock.unlock();
}

//--------------------------------------------------------------------------
/**
* LogEnable
*/
void LogEnable( char const* filter )
{
	g_filterLock.lock();

	if( g_logger.m_blackList )
	{
		g_logger.m_filter.erase( filter );
	}
	else
	{
		g_logger.m_filter.insert( filter );
	}

	g_filterLock.unlock();
}

//--------------------------------------------------------------------------
/**
* LogDisable
*/
void LogDisable( char const* filter )
{
	g_filterLock.lock();

	if ( g_logger.m_blackList )
	{
		g_logger.m_filter.insert(filter);
	}
	else
	{
		g_logger.m_filter.erase(filter);
	}
	
	g_filterLock.unlock();
}

//--------------------------------------------------------------------------
/**
* LogHook
*/
void LogHook( log_hook_cb cb )
{
	g_hookLock.lock();
	for( log_hook_cb& hook : g_logger.m_logHooks )
	{
		if( !hook )
		{
			hook = cb;
			g_hookLock.unlock();
			return;
		}
	}
	g_logger.m_logHooks.push_back( cb );
	g_hookLock.unlock();
}

//--------------------------------------------------------------------------
/**
* LogUnhook
*/
void LogUnhook( log_hook_cb cb )
{
	g_hookLock.lock();
	for ( log_hook_cb& hook : g_logger.m_logHooks )
	{
		if ( hook == cb )
		{
			hook = nullptr;
			g_hookLock.unlock();
			return;
		}
	}
	g_hookLock.unlock();
}
