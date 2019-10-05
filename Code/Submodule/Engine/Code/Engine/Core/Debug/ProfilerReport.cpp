#include "Engine/Core/Debug/ProfilerReport.hpp"
#include "Engine/Core/Debug/Profiler.hpp"
#include "Engine/Core/Debug/Log.hpp"
#include "Engine/Core/Time/Time.hpp"

#include <algorithm>
#include <vector>

//--------------------------------------------------------------------------

class ReportNode;


class ProfilerReport
{
public:
	~ProfilerReport();

public:
	void append_flat_view(profiler_node_t* root);
	void append_tree_view(profiler_node_t* root);

	ReportNode* get_root() const;
	typedef bool compare_op(ReportNode const* lhs, ReportNode const* rhs);
	void sort(compare_op op);

private:
	void append( profiler_node_t* toAppend, ReportNode* appendTo );


public:
	ReportNode* head = nullptr;

public:
	bool report_state_tree = true;
	uint64_t total_time = 0;

};

//--------------------------------------------------------------------------

class ReportNode
{
public:
	ReportNode( ProfilerReport* owner ) : m_owner(owner) {};
	ReportNode( ProfilerReport* owner, profiler_node_t* node );
	~ReportNode();

public:
	typedef bool compare_op(ReportNode const* lhs, ReportNode const* rhs);
	void sort_children(compare_op op);

public:
	ReportNode* append( profiler_node_t* toAppend );

public:
	ProfilerReport* m_owner = nullptr;
	ReportNode* m_parent = nullptr;
	std::vector<ReportNode*> m_children;

	std::string m_name = "ProfilerReportRoot";
	uint m_call_count = 1;

	uint64_t m_total_hpc;
	uint64_t m_self_hpc;


	uint64_t m_alloc_count;
	uint64_t m_free_count;
	uint64_t m_bytes_allocated;
	uint64_t m_bytes_freed;


};

//								   Lable    calls tot   tot tim self   sefl tim aloc free bAloc bFree
static const char* format		= "%-32.32s %-8u %-9.3f %-15.6f %-10.3f %-15.6f %-8u %-8u %-14u %-12u";
static const char* formatHeader = "%-32.32s %-8.8s %-9.9s %-15.15s %-10.10s %-15.15s %-8.8s %-8.8s %-14.14s %-12.12s";
static uint64_t total_time = 0; 

void LogChildReport( ReportNode* node, uint indent )
{
	std::string line;
	std::string name(indent * 2, ' ');
	name += node->m_name;

	double totTime = HPCToSeconds( node->m_total_hpc );
	uint64_t ReportTotTime = node->m_owner->total_time;
	double percentOftime = totTime / HPCToSeconds( ReportTotTime );

	uint64_t self_time = node->m_total_hpc;

	for( ReportNode* child : node->m_children )
	{
		self_time -= child->m_total_hpc;
	}

	double self_time_sec = HPCToSeconds( self_time );
	double percentOfSelfTime = self_time_sec / HPCToSeconds( ReportTotTime );


	uint64_t allocs = node->m_alloc_count;
	uint64_t bytes_alloced = node->m_bytes_allocated;
	uint64_t free = node->m_free_count;
	uint64_t bytes_freed = node->m_bytes_freed;

	Logf("ProfilerReport", format, name.c_str(), node->m_call_count, percentOftime, totTime, percentOfSelfTime, self_time_sec, allocs, free, bytes_alloced, bytes_freed );

	for (ReportNode* child : node->m_children)
	{
		LogChildReport(child, indent + 1);
	}
}

//--------------------------------------------------------------------------
void LogReport( ReportNode* node, uint indent )
{
	std::string line;
	std::string name( indent * 3, ' ' );
	name += node->m_name;

	Logf( "Profiler      ", formatHeader, "LABLE", "CALLS", "TOTAL%", "TOTAL TIME(S)", "SELF%", "SELF TIME(S)", "ALLOCS", "FREED", "BYTES ALLOCED", "BYTES FREED" );

	for( ReportNode* child : node->m_children )
	{
		LogChildReport( child, indent + 1 );
	}
}

//--------------------------------------------------------------------------
/**
* TreeViewSortedByTotalTime
*/
void TreeViewSortedByTotalTime()
{
	ProfilerReport report;

	// get this threads last recorded tree
	profiler_node_t* prev_frame = ProfilerAcquirePreviousTreeForCallingThread();

	// this parses through the samples and adds them to the report
	report.append_tree_view(prev_frame);

	// release the tree so the system can potentially clean it up
	ProfilerReleaseTree(prev_frame);

	// sort by self time (passing in a lambda as my compare_op 
	report.sort([](ReportNode const* a, ReportNode const* b) {
		return a->m_total_hpc > b->m_total_hpc;
		});

	// Print
	ReportNode* root = report.get_root();
	LogReport(root, 0);
}

//--------------------------------------------------------------------------
/**
* FlatViewSortedBySelfTime
*/
void FlatViewSortedBySelfTime()
{
	ProfilerReport report;

	// get this threads last recorded tree
	profiler_node_t* prev_frame = ProfilerAcquirePreviousTreeForCallingThread();

	// this parses through the samples and adds them to the report
	report.append_flat_view(prev_frame);

	// release the tree so the system can potentially clean it up
	ProfilerReleaseTree(prev_frame);

	// sort by self time (passing in a lambda as my compare_op 
	report.sort([](ReportNode const* a, ReportNode const* b) {
		return a->m_total_hpc > b->m_total_hpc;
		});

	// Print
	ReportNode* root = report.get_root();
	LogReport(root, 0);
}

//--------------------------------------------------------------------------
/**
* ~ProfilerReport
*/
ProfilerReport::~ProfilerReport()
{
	SAFE_DELETE(head);
}

//--------------------------------------------------------------------------
/**
* append_flat_view
*/
void ProfilerReport::append_flat_view( profiler_node_t* root )
{
	if( report_state_tree )
	{
		report_state_tree = false;
		total_time = 0;
		SAFE_DELETE(head);
	}
	if (!head)
	{
		head = new ReportNode( this );
	}
	total_time += root->life_time;
	append( root, head );
}

//--------------------------------------------------------------------------
/**
* append_tree_view
*/
void ProfilerReport::append_tree_view( profiler_node_t* root )
{
	if( !report_state_tree )
	{
		report_state_tree = true;
		total_time = 0;
		SAFE_DELETE(head);
	}
	if( !head )
	{
		head = new ReportNode( this);
	}
	total_time += root->life_time;
	append( root, head );
}

//--------------------------------------------------------------------------
/**
* get_root
*/
ReportNode* ProfilerReport::get_root() const
{
	return head;
}

void swap( ReportNode* a, ReportNode* b )
{
	ReportNode* tempParent = a->m_parent;
	std::vector<ReportNode*> tempChildren = a->m_children;

	a->m_parent = b->m_parent;
	a->m_children = b->m_children;

	b->m_parent = tempParent;
	b->m_children = tempChildren;
}

//--------------------------------------------------------------------------
/**
* sort
*/
void ProfilerReport::sort(compare_op op)
{
	get_root()->sort_children(op);
}



//--------------------------------------------------------------------------
/**
* ReportNode
*/
ReportNode::ReportNode( ProfilerReport* owner, profiler_node_t* node )
	: m_owner( owner )
{
	m_name = node->label;
	m_total_hpc = node->life_time;

	m_alloc_count = node->alloc_count;
	m_free_count = node->free_count;
	m_bytes_allocated = node->memory_alloced;
	m_bytes_freed = node->memory_freed;
}


//--------------------------------------------------------------------------
/**
* ~ReportNode
*/
ReportNode::~ReportNode()
{
	for( ReportNode*& child : m_children )
	{
		SAFE_DELETE(child);
	}
}

//--------------------------------------------------------------------------
/**
* sort_children
*/
void ReportNode::sort_children(compare_op op)
{
	for( ReportNode* child : m_children )
	{
		child->sort_children(op);
	}
	
	std::sort(m_children.begin(), m_children.end(), op);
}

//--------------------------------------------------------------------------
/**
* append
* Returns the node that is associated with toAppend
*/
ReportNode* ReportNode::append( profiler_node_t* toAppend )
{
	for( ReportNode* child : m_children )
	{
		if( child->m_name == toAppend->label )
		{
			++(child->m_call_count);
			return child;
		}
	}
	ReportNode* newNode = new ReportNode( this->m_owner, toAppend );
	m_children.push_back( newNode );
	return newNode;
}

void treeAppend( profiler_node_t* toAppend, ReportNode* appendTo )
{
	ReportNode* appendedNode = appendTo->append(toAppend);
	profiler_node_t* attachChild = toAppend->last_child;
	while( attachChild )
	{
		treeAppend( attachChild, appendedNode );
		attachChild = attachChild->prev_sibling;
	}
}

void flatAppend( profiler_node_t* toAppend, ReportNode* appendTo )
{
	appendTo->append(toAppend);
	profiler_node_t* attachChild = toAppend->last_child;
	while (attachChild)
	{
		flatAppend(attachChild, appendTo);
		attachChild = attachChild->prev_sibling;
	}
}

//--------------------------------------------------------------------------
/**
* append
*/
void ProfilerReport::append( profiler_node_t* toAppend, ReportNode* appendTo )
{
	if( report_state_tree )
	{
		treeAppend( toAppend, appendTo );
	}
	else
	{
		flatAppend( toAppend, appendTo );
	}
}
