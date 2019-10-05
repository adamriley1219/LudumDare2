#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <queue>
#include <mutex>
#include <thread>

template <typename TYPE>
class AsyncQueue
{
public:
	void Enqueue( TYPE const &v ); 
	bool Dequeue( TYPE* out ); 

	bool IsEmpty();
	uint GetCount();

private:
	std::queue<TYPE> m_queue; 
	std::mutex m_mutex;

};

//--------------------------------------------------------------------------
/**
* GetCount
*/
template <typename TYPE>
uint AsyncQueue<TYPE>::GetCount()
{
	return m_queue.size();
}

//--------------------------------------------------------------------------
/**
* IsEmpty
*/
template <typename TYPE>
bool AsyncQueue<TYPE>::IsEmpty()
{
	return m_queue.empty();
}

//--------------------------------------------------------------------------
/**
* Enqueue
*/
template <typename TYPE>
void AsyncQueue<TYPE>::Enqueue( const TYPE& toQueue )
{
	std::lock_guard<std::mutex> locky( m_mutex ); 
	m_queue.push( toQueue );
}

//--------------------------------------------------------------------------
/**
* Dequeue
*/
template <typename TYPE>
bool AsyncQueue<TYPE>::Dequeue( TYPE* out )
{
	std::lock_guard<std::mutex> locky( m_mutex ); 
	if ( IsEmpty() ) 
	{
		return false; 
	} 
	else 
	{
		*out = m_queue.front(); 
		m_queue.pop();
		return true;
	} 
}
