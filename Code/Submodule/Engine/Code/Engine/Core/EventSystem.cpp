#include "Engine/Core/EventSystem.hpp"

SelfRegisteringEvent* SelfRegisteringEvent::s_head = nullptr;

//--------------------------------------------------------------------------
/**
* SelfRegisteringEvent
*/
SelfRegisteringEvent::SelfRegisteringEvent( const std::string& eventName, EventCallbackFunction callback )
	: m_eventName( eventName ), m_callback( EventFunction( callback ) )
{
	m_next = s_head;
	s_head = this;
}

//--------------------------------------------------------------------------
/**
* SelfRegisteringEvent
*/
SelfRegisteringEvent::SelfRegisteringEvent( const std::string& eventName, EventFunction callback )
	: m_callback(callback), m_eventName( eventName )
{
	m_next = s_head;
	s_head = this;
}

//--------------------------------------------------------------------------
/**
* ~SelfRegisteringEvent
*/
SelfRegisteringEvent::~SelfRegisteringEvent()
{

}

//--------------------------------------------------------------------------
/**
* EventSubscription
*/
EventSubscription::EventSubscription( EventCallbackFunction callbackFunctionPtr )
	: m_callbackFunctionPtr( EventFunction(callbackFunctionPtr) )
{

}

//--------------------------------------------------------------------------
/**
* EventSubscription
*/
EventSubscription::EventSubscription( EventFunction callbackFunctionPtr )
	: m_callbackFunctionPtr( callbackFunctionPtr )
{

}

//--------------------------------------------------------------------------
/**
* ~EventSubscription
*/
EventSubscription::~EventSubscription()
{

}

//--------------------------------------------------------------------------
/**
* Run
*/
bool EventSubscription::Run( EventArgs& args )
{
	return m_callbackFunctionPtr( args );
}

//--------------------------------------------------------------------------
/**
* EventSystem
*/
EventSystem::EventSystem()
{
	SelfRegisteringEvent* itr = SelfRegisteringEvent::s_head;
	while ( itr )
	{
		SubscribeEventCallbackFunction( itr->m_eventName, itr->m_callback );
		itr = itr->m_next;
	}	
}

//--------------------------------------------------------------------------
/**
* ~EventSystem
*/
EventSystem::~EventSystem()
{
	for(auto subListInMapItr : m_subsctiptions)
	{
		SubList& subList = subListInMapItr.second;
		for( int listIndex = 0; listIndex < (int) subList.size(); listIndex++ )
		{
			EventSubscription*& eventSub = subList[listIndex];
			if( eventSub )
			{
				delete eventSub;
				eventSub = nullptr;
			}
		}
	}
}

//--------------------------------------------------------------------------
/**
* Startup
*/
void EventSystem::Startup()
{

}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void EventSystem::Shutdown()
{

}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void EventSystem::BeginFrame()
{

}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void EventSystem::EndFrame()
{

}

//--------------------------------------------------------------------------
/**
* SubscribeEventCallbackFunctino
*/
void EventSystem::SubscribeEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback )
{
	EventSubscription* subToAdd = new EventSubscription( callback );
	m_subsctiptions[ eventName ].push_back( subToAdd );
}

//--------------------------------------------------------------------------
/**
* SubscribeEventCallbackFunction
*/
void EventSystem::SubscribeEventCallbackFunction( const std::string& eventName, EventFunction callback )
{
	EventSubscription* subToAdd = new EventSubscription( callback );
	m_subsctiptions[ eventName ].push_back( subToAdd );
}

//--------------------------------------------------------------------------
/**
* UnsubscribeEvenCallbackFunction
*/
void EventSystem::UnsubscribeEvenCallbackFunction( const std::string& eventName, EventCallbackFunction callback )
{
	UnsubscribeEvenCallbackFunction( eventName, EventFunction( callback ) );
}

//--------------------------------------------------------------------------
/**
* UnsubscribeEvenCallbackFunction
*/
void EventSystem::UnsubscribeEvenCallbackFunction( const std::string& eventName, EventFunction callback )
{
	auto subMapItr = m_subsctiptions.find( eventName );
	if( subMapItr != m_subsctiptions.end() )
	{
		SubList& subList = subMapItr->second;
		for( int listIndex = 0; listIndex < (int) subList.size(); listIndex++ )
		{
			EventSubscription*& eventSub = subList[listIndex];
			if( eventSub && callback == eventSub->GetCallbackFunction() )
			{
				delete eventSub;
				eventSub = nullptr;
			}
		}
	}
}

//--------------------------------------------------------------------------
/**
* FireEvent
*/
int EventSystem::FireEvent( const std::string& eventName )
{
	EventArgs args;
	return FireEvent( eventName, args );
}

//--------------------------------------------------------------------------
/**
* FireEvent
*/
int EventSystem::FireEvent( const std::string& eventName, EventArgs& args )
{
	int subFuncRanCount = 0;

	auto subMapItr = m_subsctiptions.find( eventName );
	if( subMapItr != m_subsctiptions.end() )
	{
		SubList& subList = subMapItr->second;
		for( int subIndex = 0; subIndex < (int) subList.size(); subIndex++ )
		{
			EventSubscription*& eventSub = subList[subIndex];
			if( eventSub )
			{
				++subFuncRanCount;
				if( eventSub->Run(args) )
				{
					break;
				}
			}
		}
	}

	return subFuncRanCount;
}

//--------------------------------------------------------------------------
/**
* FireEvent
*/
int EventSystem::FireEvent( Event& event )
{
	return FireEvent( event.m_name, event.m_args );
}

//--------------------------------------------------------------------------
/**
* GetNumSubscribersForCommand
*/
int EventSystem::GetNumSubscribersForCommand( const std::string& eventName ) const
{	
	auto subMapItr = m_subsctiptions.find( eventName );
	if( subMapItr != m_subsctiptions.end() )
	{
		return (int) subMapItr->second.size();
	}
	return 0;
}

//--------------------------------------------------------------------------
/**
* GetSubscribedEventsList
*/
void EventSystem::GetSubscribedEventsList( Strings& eventNamesWithSubsctivers ) const
{
	for(auto subListInMapItr : m_subsctiptions)
	{
		const std::string& subList = subListInMapItr.first;
		eventNamesWithSubsctivers.push_back( subList );
	}
}

//--------------------------------------------------------------------------
/**
* Event
*/
Event::Event()
{

}

//--------------------------------------------------------------------------
/**
* Event
*/
Event::Event( std::string const &commandLine )
{
	size_t split;
	std::string curText = commandLine;
	
	split = curText.find_first_of( " " );

	if( split != std::string::npos && !curText.empty() )
	{
		m_name = std::string( curText, 0, split );

		
		std::string theArgs( curText, split + 1 );
		std::vector<std::string> argStrings = SplitStringOnDelitmiter( theArgs.c_str(), " " );
		
		for( int stringIdx = 0; stringIdx < (int) argStrings.size(); ++stringIdx )
		{
			std::vector<std::string> arg = SplitStringOnDelitmiter( argStrings[stringIdx].c_str(), "=" );

			if( (int) arg.size() == 2 )
			{
				m_args.SetValue( arg[0], arg[1] );
			}
			else
			{
				ERROR_RECOVERABLE( Stringf( "There were bad args in the given string: %s", theArgs.c_str() ) );
			}
		}
	}
	else
	{
		m_name = commandLine;
	}

	
}

//--------------------------------------------------------------------------
/**
* operator!=
*/
bool EventFunction::operator!=( const EventFunction& other ) const
{
	return !( *this == other );
}

//--------------------------------------------------------------------------
/**
* operator==
*/
bool EventFunction::operator==( const EventFunction& other ) const
{
	return m_object_pointer == other.m_object_pointer 
		&& m_function_pointer == other.m_function_pointer;
}

//--------------------------------------------------------------------------
/**
* operator()
*/
bool EventFunction::operator()( EventArgs& args ) const
{
	return m_func( args );
}
