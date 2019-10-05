#pragma once
#include "Engine/Core/Memory/MemoryManagement.hpp"

#include <mutex>


class Allocator  // or just Allocator if you prefer
{
public:
	// get memory
	virtual void* alloc(size_t size) = 0;

	// free memory; 
	virtual void free(void* ptr) = 0;


	template <typename OBJ_TYPE, typename ...ARGS>
	OBJ_TYPE* create( ARGS&& ...args )
	{
		// for a allocator meant for modern C++, the below is a bit simplistic
		// and you may want to "perfect-foward"
		// see: http://cpptruths.blogspot.com/2012/06/perfect-forwarding-of-parameter-groups.html

		void* mem = this->alloc(sizeof(OBJ_TYPE));
		if (mem != nullptr) {
			// new in place
			new (mem) OBJ_TYPE( std::forward(args)... );
		}

		return (OBJ_TYPE*)mem;
	}

	template <typename OBJ_TYPE> 
	void abliderate( OBJ_TYPE* obj )
	{
		if (obj != nullptr) {
			obj->~T();
			this->free(obj);
		}
	}

};

template <typename OBJ_TYPE>
class TrackedAllocator : public Allocator
{
public:
	virtual void* alloc(size_t size) final { return TrackedAlloc(size); }
	virtual void free(void* ptr) final { TrackedFree(ptr); }

	using value_type = OBJ_TYPE;

	TrackedAllocator() = default;
	template <class U>
	TrackedAllocator(const TrackedAllocator<U>&) {}

	OBJ_TYPE* allocate(std::size_t n) {
		return (OBJ_TYPE*)alloc( n * sizeof(OBJ_TYPE) );
	}

	void deallocate(OBJ_TYPE * ptr, std::size_t n) {
		free( ptr );
	}

	// TODO: potentially make a singleton, since no real reason to 
	// have multiple of this;  Also, TrackedAlloator is the "default" allocator of the sytem; 
};

template <typename OBJ_TYPE, typename U>
inline bool operator == (const TrackedAllocator<OBJ_TYPE>&, const TrackedAllocator<U>&) {
	return true;
}

template <typename OBJ_TYPE, typename U>
inline bool operator != (const TrackedAllocator<OBJ_TYPE>& a, const TrackedAllocator<U>& b) {
	return !(a == b);
}


//--------------------------------------------------------------------------
template <class OBJ_TYPE>
class UntrackedAllocator : public Allocator
{
public:
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef OBJ_TYPE* pointer;
	typedef const OBJ_TYPE* const_pointer;
	typedef OBJ_TYPE& reference;
	typedef const OBJ_TYPE& const_reference;
	typedef OBJ_TYPE         value_type;

	UntrackedAllocator() {}
	UntrackedAllocator(const UntrackedAllocator&) {}

	pointer allocate(size_type n, const void* = 0) {
		OBJ_TYPE* t = (OBJ_TYPE*)alloc(n * sizeof(OBJ_TYPE));
		return t;
	}

	void deallocate(void* p, size_type) {
		free( p );
	}

	void* alloc( size_type n )
	{
		return UntrackedAlloc( n );
	}

	void free( void* ptr )
	{
		if (ptr) {
			UntrackedFree(ptr);
		}
	}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }
	UntrackedAllocator<OBJ_TYPE>& operator=(const UntrackedAllocator&) { return *this; }

	size_type max_size() const { return size_t(-1); }

	template <class U>
	struct rebind { typedef UntrackedAllocator<U> other; };

	template <class U>
	UntrackedAllocator(const UntrackedAllocator<U>&) {}

	template <class U>
	UntrackedAllocator& operator=(const UntrackedAllocator<U>&) { return *this; }
};
