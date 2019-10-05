#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Strings/NamedProperties.hpp"
#include <map>
#include <vector>
#include <functional>

typedef NamedProperties EventArgs;
typedef bool (*EventCallbackFunction)( EventArgs& args );

class SelfRegisteringEvent;

#define COMMAND( eventName )																							\
		static bool MACRO_COMBINE(__COMMAND_,__LINE__)( EventArgs& args );												\
		static SelfRegisteringEvent MACRO_COMBINE(__COMMANDOBJ_,__LINE__)( eventName, MACRO_COMBINE(__COMMAND_,__LINE__) );\
		static bool MACRO_COMBINE(__COMMAND_,__LINE__)( EventArgs& args )			


//--------------------------------------------------------------------------
class EventFunction 
{
public:
	EventFunction( EventCallbackFunction cb )
	{
		m_object_pointer = nullptr; 
		m_function_pointer = (void*)cb; 
		m_func = cb; 
	}

	// EventFunction( thingPtr, &thing::LoadToLevel )
	template <typename OBJ_TYPE>
	EventFunction( OBJ_TYPE* obj, bool (OBJ_TYPE::*method_ptr)( EventArgs& ) )
	{
		m_object_pointer = obj; 
		m_function_pointer = &method_ptr; 

		m_func = [=]( EventArgs& args ) -> bool { return (obj->*method_ptr)(args); }; 
	}

	bool operator==( const EventFunction& other ) const; 
	bool operator!=( const EventFunction& other ) const; 
	bool operator()( EventArgs& args ) const;

private:
	void* m_object_pointer;          
	const void* m_function_pointer; 
	std::function<bool( EventArgs& args )> m_func; // a function matching our event signature
};  



//--------------------------------------------------------------------------
class EventSubscription
{
public:
	explicit EventSubscription( EventCallbackFunction callbackFunctionPtr );
	explicit EventSubscription( EventFunction callbackFunctionPtr );
	~EventSubscription();

	const EventFunction& GetCallbackFunction() const { return m_callbackFunctionPtr; };
	bool Run( EventArgs& args );

private:
	EventFunction m_callbackFunctionPtr = nullptr;
};

//------------------------------------------------------------------------
class Event
{
public:
	Event();
	Event( std::string const &commandLine );

public:
	std::string m_name; 
	EventArgs m_args;
	bool m_consumed = false;
}; 

//--------------------------------------------------------------------------
class EventSystem
{
private:
	typedef std::vector< EventSubscription* > SubList;

public:
	EventSystem();
	~EventSystem();
	void	Startup();
	void	Shutdown();
	void	BeginFrame();
	void	EndFrame();

	void	SubscribeEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback );
	void	SubscribeEventCallbackFunction( const std::string& eventName, EventFunction callback );
	void	UnsubscribeEvenCallbackFunction(  const std::string& eventName, EventCallbackFunction callback  );
	void	UnsubscribeEvenCallbackFunction(  const std::string& eventName, EventFunction callback  );
	int		FireEvent( Event& event );
	int		FireEvent( const std::string& eventName );
	int		FireEvent( const std::string& eventName, EventArgs& args );
	int		GetNumSubscribersForCommand( const std::string& eventName ) const;
	void	GetSubscribedEventsList( Strings& eventNamesWithSubsctivers ) const;

private:
	std::map< std::string, SubList > m_subsctiptions;
};



//--------------------------------------------------------------------------
// NOTE: Only intended to be used for #define COMMAND
//--------------------------------------------------------------------------
class SelfRegisteringEvent
{
	friend EventSystem;
public:
	SelfRegisteringEvent(const std::string& eventName, EventCallbackFunction callback);
	SelfRegisteringEvent(const std::string& eventName, EventFunction callback);
	~SelfRegisteringEvent();

private:
	static SelfRegisteringEvent* s_head;
	SelfRegisteringEvent* m_next = nullptr;

	std::string m_eventName;
	EventFunction m_callback;

};
//--------------------------------------------------------------------------

