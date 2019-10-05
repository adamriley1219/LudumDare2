#include "Engine/Core/Debug/Profiler.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/Memory/BlockAllocator.hpp"

#include <mutex>

//--------------------------------------------------------------------------
#include "Game/EngineBuildPreferences.hpp"
//--------------------------------------------------------------------------


COMMAND("profiler_pause")
{
	UNUSED(args);
	ProfilerPause();
	return true;
}

COMMAND("profiler_resume")
{
	UNUSED(args);
	ProfilerResume();
	return true;
}



//--------------------------------------------------------------------------

namespace ProfilerSystem
{
	static thread_local profiler_node_t* t_ActiveNode = nullptr;

	static std::vector<profiler_node_t*> m_history;
	static std::recursive_mutex m_historyLock;

	static ObjectAllocator<profiler_node_t>* m_allocator = nullptr;

	static std::atomic<double> m_maxHistorySec = 60.0;
	static std::atomic<bool> m_running = true;

	static std::atomic<size_t> m_memory = 0;
	static std::atomic<size_t> m_AllocCount = 0;
};


//--------------------------------------------------------------------------
/**
* ProfileScope
*/
ProfileScope::ProfileScope(const char* name)
{
#ifndef ENGINE_DISABLE_PROFILER
	if( !ProfilerSystem::m_running )
	{
		return;
	}
	
	ProfilerPush( name );
#endif
}

//--------------------------------------------------------------------------
/**
* ~ProfileScope
*/
ProfileScope::~ProfileScope()
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return;
	}
	ProfilerPop();
#endif
}


static profiler_node_t* AllocNode()
{
	profiler_node_t* node = ProfilerSystem::m_allocator->alloc();
	node->ref_count = 1;
	return node;
}

//--------------------------------------------------------------------------
static void FreeNode(profiler_node_t* node)
{
	std::scoped_lock<std::recursive_mutex> lk(ProfilerSystem::m_historyLock);
	uint new_count = --(node->ref_count);

	if (new_count <= 0)
	{
		ProfilerSystem::m_allocator->free(node);
	}
}

//--------------------------------------------------------------------------
static void FreeTree( profiler_node_t* root )
{
	std::scoped_lock<std::recursive_mutex> lk(ProfilerSystem::m_historyLock);
	profiler_node_t* child = root->last_child;
	
	while (child)
	{
		FreeTree(child);
		child = child->prev_sibling;
	}

	// free myself
	FreeNode(root);
}


//--------------------------------------------------------------------------
static void IncrementTree( profiler_node_t* root )
{
	std::scoped_lock<std::recursive_mutex> lk(ProfilerSystem::m_historyLock);
	profiler_node_t* child = root->last_child;

	while (child)
	{
		IncrementTree(child);
		child = child->prev_sibling;
	}

	++(root->ref_count);
}


//----------------------------------------------------------
void ProfileReleaseTree( profiler_node_t* node )
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return;
	}


	uint new_count = --(node->ref_count);

	if (new_count == 0) {
		FreeTree(node);
	}
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerSystemInit
*/
bool ProfilerSystemInit()
{
#ifndef ENGINE_DISABLE_PROFILER
	ProfilerSystem::m_running = true;
	ProfilerSetMaxHistoryTime( 1.0f );
	ProfilerSystem::m_allocator = new ObjectAllocator<profiler_node_t>();
	ProfilerSystem::m_history.resize( 60 * 500 );
	return false;
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerSystemDeinit
*/
void ProfilerSystemDeinit()
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return;
	}
	SAFE_DELETE(ProfilerSystem::m_allocator);
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerSetMaxHistoryTime
*/
void ProfilerSetMaxHistoryTime( double seconds )
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return;
	}
	ProfilerSystem::m_maxHistorySec = seconds;
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerPause
*/
void ProfilerPause()
{
#ifndef ENGINE_DISABLE_PROFILER
	ProfilerSystem::m_running = false;
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerResume
*/
void ProfilerResume()
{
#ifndef ENGINE_DISABLE_PROFILER
	ProfilerSystem::m_running = true;
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerPush
*/
void ProfilerPush( char const* label )
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running && ProfilerSystem::t_ActiveNode == nullptr)
	{
		return;
	}

	profiler_node_t* node = AllocNode();
	profiler_node_t*& activeNode = ProfilerSystem::t_ActiveNode;

	node->thread_id = std::this_thread::get_id();
	node->start_time = GetCurrentTimeHPC();

	size_t labelLength = strlen(label) + 1;
	memcpy( node->label, label, labelLength < 32 ? labelLength : 32 );
	
	if( activeNode )
	{
		activeNode->add_child(node);
	}
	activeNode = node;
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerPop
*/
void ProfilerPop()
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running && ProfilerSystem::t_ActiveNode == nullptr)
	{
		return;
	}

	if (ProfilerSystem::t_ActiveNode == nullptr) 
	{
		ERROR_RECOVERABLE("void ProfilerPop() (Profiler) Not ment to pop on a tree with nothing in it.");
		return;
	}

	// finalize
	ProfilerSystem::t_ActiveNode->end_time = GetCurrentTimeHPC();
	ProfilerSystem::t_ActiveNode->life_time = GetCurrentTimeHPC() - ProfilerSystem::t_ActiveNode->start_time;

	ProfilerSystem::t_ActiveNode->memory_remaining = ((int)ProfilerSystem::t_ActiveNode->memory_alloced - (int)ProfilerSystem::t_ActiveNode->memory_freed);

	// this is where I am now
	profiler_node_t* old_active = ProfilerSystem::t_ActiveNode;
	ProfilerSystem::t_ActiveNode = ProfilerSystem::t_ActiveNode->parent;
	if (ProfilerSystem::t_ActiveNode == nullptr) 
	{
		std::scoped_lock<std::recursive_mutex> hlk( ProfilerSystem::m_historyLock );
		for (profiler_node_t*& root : ProfilerSystem::m_history)
		{
			if( !root )
			{
				root = old_active;
				return;
			}
		}
		ProfilerSystem::m_history.push_back(old_active);
	}
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerAllocation
*/
void ProfilerAllocation( size_t byte_size /*= 0*/ )
{
#ifndef ENGINE_DISABLE_PROFILER
	if( !ProfilerSystem::m_running )
	{
		return;
	}
	if( ProfilerSystem::t_ActiveNode )
	{
		ProfilerSystem::t_ActiveNode->memory_alloced += byte_size;
		++(ProfilerSystem::t_ActiveNode->alloc_count);
	}
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerFree
*/
void ProfilerFree(  size_t byte_size /*= 0*/ )
{
#ifndef ENGINE_DISABLE_PROFILER
	if( !ProfilerSystem::m_running )
	{
		return;
	}
	if( ProfilerSystem::t_ActiveNode )
	{
		ProfilerSystem::t_ActiveNode->memory_freed += byte_size;
		++(ProfilerSystem::t_ActiveNode->free_count);
	}
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerBeginFrame
*/
void ProfilerBeginFrame( char const* label /*= "frame"*/ )
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return;
	}
	ASSERT_RECOVERABLE( ProfilerSystem::t_ActiveNode == nullptr, "ProfilerBeginFrame:  ProfilerSystem::t_ActiveNode != nullptr" );

	ProfilerPush(label);
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerEndFrame
*/
void ProfilerEndFrame()
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return;
	}
	ProfilerPop();

	ASSERT_RECOVERABLE( ProfilerSystem::t_ActiveNode == nullptr, "ProfilerEndFrame:  ProfilerSystem::t_ActiveNode != nullptr" );

	std::scoped_lock<std::recursive_mutex> ls(ProfilerSystem::m_historyLock);
	for( profiler_node_t*& root : ProfilerSystem::m_history )
	{
		if( root )
		{
			double timeSinceDeath = HPCToSeconds( GetCurrentTimeHPC() - root->end_time );
			if( timeSinceDeath > ProfilerSystem::m_maxHistorySec )
			{
				FreeTree(root);
				root = nullptr;
			}
		}
	}
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerAcquirePreviousTree
*/
profiler_node_t* ProfilerAcquirePreviousTree( std::thread::id id, uint history /*= 0 */ )
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return nullptr;
	}
	std::scoped_lock<std::recursive_mutex> hlk( ProfilerSystem::m_historyLock );
	std::vector<profiler_node_t*> threadTrees;

	for( profiler_node_t* root : ProfilerSystem::m_history )
	{
		if ( root->thread_id == id )
		{
			threadTrees.push_back(root);
		}
	}

	// loop through gHistory
	// count up to history all trees from thread::id id
	// return that tree or null if there wasn't enough; }
	if( threadTrees.size() < history )
	{
		return nullptr;
	}
	else
	{
		return threadTrees[threadTrees.size() - 1 - history];
	}
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerAcquirePreviousTreeForCallingThread
*/
profiler_node_t* ProfilerAcquirePreviousTreeForCallingThread( uint history /*= 0 */ )
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return nullptr;
	}
	std::scoped_lock<std::recursive_mutex> hlk(ProfilerSystem::m_historyLock);
	std::vector<profiler_node_t*> threadTrees;
	std::thread::id threadID = std::this_thread::get_id();
	for (profiler_node_t* root : ProfilerSystem::m_history)
	{
		if( root )
		{
			if (root->thread_id == threadID)
			{
				threadTrees.push_back(root);
			}
		}
	}

	// loop through gHistory
	// count up to history all trees from thread::id id
	// return that tree or null if there wasn't enough; }
	if (threadTrees.size() < history)
	{
		return nullptr;
	}
	else
	{
		profiler_node_t* treeToRet = threadTrees[threadTrees.size() - 1 - history];
		IncrementTree(treeToRet);
		return threadTrees[threadTrees.size() - 1 - history];
	}
#endif
}

//--------------------------------------------------------------------------
/**
* ProfilerReleaseTree
*/
void ProfilerReleaseTree( profiler_node_t* node )
{
#ifndef ENGINE_DISABLE_PROFILER
	if (!ProfilerSystem::m_running)
	{
		return;
	}
	std::scoped_lock<std::recursive_mutex> hlk(ProfilerSystem::m_historyLock);

	FreeTree(node);
#endif
}


