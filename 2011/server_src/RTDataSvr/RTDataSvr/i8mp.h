#ifndef I8MEMORYPOOL_H
#define I8MEMORYPOOL_H

#include "sync.h"

#include <algorithm>
#include <map>
#include <functional>
#include <sstream>
#include <limits> //for numeric_limits
#include <assert.h>

namespace i8desk {

#ifdef USED_I8_MP

static const size_t BLOCK_SIZE  = 1024 * 512;
static const size_t ALIGNSIZE = 16;

inline size_t ALIGN(size_t x)
{
	return (((x) + (ALIGNSIZE - 1)) & ~(ALIGNSIZE - 1));
}

//任意大小的内存分配类
//Lty: 锁的类型
template<typename Lty>
class ZI8MP
{
public:

	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	ZI8MP(
		const char *name = "default i8memory pool"
		) 
		: m_size(0)
		, m_name(name)
	{
		m_lock.Create();
	}
	
	~ZI8MP()
	{
		for (BlockListItr it = m_blocks.begin();
			it != m_blocks.end(); ++it) 
		{
			//assert (it->first->used == 0);
#ifdef USE_GLOBAL_MEMORY
			::GlobalFree((HGLOBAL)it->first);
#else
			delete[] (char *)it->first;
#endif
		}
	}

	//返回size个T数组
	template<typename T>
	T *malloc(size_t size, T* = 0)
	{
		return reinterpret_cast<T *>(alloc(size * sizeof(T)));
	}

	//返回size个T数组并初始化为0
	template<typename T>
	T *calloc(size_t size, T* = 0)
	{
		if (T *p = malloc<T>(size)) {
			memset(p, 0, size * sizeof(T));
			return p;
		}
		return 0;
	}

	//重新分配size个T数组并复制原先数据
	template<typename T>
	T *realloc(T *p, size_t size)
	{
		size_t oldsize = realsize(p);
		if (size * sizeof(T) <= oldsize)
			return p;
		
		if (T *newp = malloc<T>(size)) {
			memcpy(newp, p, oldsize);
			free(p);
			return newp;
		}
		return 0;
	}

	//分配size个字节
   	char *alloc(size_t size)
	{
		I8_GUARD(ZGuard, Lty, m_lock);

		size_t asize = ALIGN(size);
		chunck *c;

		//从空闲列表中查找不小于分配尺寸的最小的块
		ChunckListItr it = m_frees.lower_bound(asize);
		if (it != m_frees.end()) {
			c = it->second;
			m_frees.erase(it);
		}  
		else {
			//没有足够大的空闲块，分配一个新片	
			c = newblock(asize);
			if (!c) {
				return 0;
			}
		}
		
		if (c->size >= asize + ALIGNSIZE + sizeof(chunck)) {
			//块太大，切割块
			cutchunck(c, asize);
		}

		//置片的使用标志
		if (!c->prev) {
			block *b = (block*)((char *)c - sizeof(block));
			if (b->used == 0)
				b->used = 1;
		}
		c->used = 1;

		return (char *)c + sizeof(chunck);
	}

	//释放
	template<typename T>
	void release(T *&p)
	{
		if (p) {
			free((void *)p);
			p = 0;
		}
	}

	void free(void *p)
	{
		if (!p)
			return;

		I8_GUARD(ZGuard, Lty, m_lock);

		assert(ischunck(p));
		chunck *c = (chunck *)((char *)p - sizeof(chunck));
		c->used = 0;

		//合并下面可能的空闲块
		chunck *next = c->next;
		if (next && next->used == 0) {
			//先将下面的空闲块从空闲队列中移除
			ChunckListItr it = m_frees.lower_bound(next->size);
			for (; it != m_frees.upper_bound(next->size); ++it) {
				if (it->second == next) {
					m_frees.erase(it);
					break;
				}
			}

			//合并
			c->next = next->next;
			c->size += next->size + sizeof(chunck);
			if (c->next) {
				c->next->prev = c;
			}
		}

		//合并上面可能的空闲块
		chunck *prev = c->prev;
		if (prev && prev->used == 0) {
			//先将上面的空闲块从空闲队列中移除
			ChunckListItr it = m_frees.lower_bound(prev->size);
			for (; it != m_frees.upper_bound(prev->size); ++it) {
				if (it->second == prev) {
					m_frees.erase(it);
					break;
				}
			}
			
			//合并
			prev->next = c->next;
			prev->size += c->size  + sizeof(chunck);

			//当前块变为合并后的块
			c = prev;
			if (c->next) {
				c->next->prev = c;
			}
		}

		//置片的使用标志
		if (c->prev == 0) {
			block *b = (block*)((char *)c - sizeof(block));
			if (c->size == b->size - sizeof(chunck)) {
				//本片全部为空，
				b->used = 0;
			}
		}

		m_frees.insert(std::make_pair<int, chunck *>(c->size, c)); 
	}

	//得到一个指针对应块的实际大小
	size_t realsize(void *p)
	{
		I8_GUARD(ZGuard, Lty, m_lock);

		chunck *c = (chunck *)((char *)p - sizeof(chunck));
		return c->size;
	}

	//回收空闲的片
	void reclaim(void)
	{
		I8_GUARD(ZGuard, Lty, m_lock);

		std::vector<block*> bl;
		for (BlockListItr it = m_blocks.begin();
			it != m_blocks.end(); ++it) 
		{
			if (it->first->used == 0) {
				bl.push_back(it->first);
			}
		}

		for (size_t i = 0; i < bl.size(); i++) {
			block *b = bl[i];
			chunck *c = (chunck *)((char *)b + sizeof(block));
			ChunckListItr it = m_frees.lower_bound(c->size);
			for (; it != m_frees.upper_bound(c->size); ++it) {
				if (it->second == c) {
					m_frees.erase(it);
					break;
				}
			}
			m_size -= b->size + sizeof(block);
			m_blocks.erase(b);
#ifdef USE_GLOBAL_MEMORY
			::GlobalFree((HGLOBAL)b);
#else
			delete[] (char *)b;
#endif      
		}
	}

	void dump(std::stringstream& ss)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		
		ss << "---------------" << "dump " << m_name << "---------------\n";

		size_t nblock = 0;
		size_t tsize = 0;
		size_t asize = 0;
		size_t fsize = 0;
		size_t usize = 0;

		//片信息
		ss << "block detail:\n";
		int bi = 0;
		for (BlockListItr it = m_blocks.begin(); it != m_blocks.end(); ++it) {
			nblock++;
			tsize += it->second;
			usize += sizeof(block);
			ss << "[B]" << ++bi << ":" << it->first << ":" << it->second << ":" << it->first->used << "\n";
			
			int ci = 0;
			for (chunck *c = (chunck *)((char *)it->first + sizeof(block));
				c; c = c->next) 
			{
				usize += sizeof(chunck);
				ss << "\t[C]" << ++ci << ":" << c << ":" << c->size << ":" << c->used << "\n";
				if (c->used) {
					asize += c->size;
				} 
				else {
					fsize += c->size;
				}
			}
		}

		ss << "[B]total: " << nblock << "block, " << tsize << "bytes" << "\n";
		ss << "[B]cost: " << usize << ", alloc: " << asize << ", free: " << fsize << "\n";

		//空闲列表信息
		ss << "free chunck detail:\n";
		size_t nchunck = 0;
		fsize = 0;
		for (ChunckListItr it = m_frees.begin(); it != m_frees.end(); ++it) {
			ss << "\t[C]" << ++nchunck << ":" << it->second << ":" << it->first << "\n";
			fsize += it->first;
		}
		ss << "[C]total: " << nchunck << "free chunck, " << fsize << "bytes" << "\n";

		ss << "--i8mp dump end--\n" << std::ends;
	}

private:

	Lty m_lock;

#ifdef USE_PRIVATE_ALLOCATOR
	template<
		typename T
	>
	class MPAllocator
	{
	public:
		typedef T value_type;

		typedef value_type * pointer;
		typedef const value_type * const_pointer;
		typedef value_type & reference;
		typedef const value_type & const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		template <typename U> struct rebind {
			typedef MPAllocator<U> other;
		};

	public:
		MPAllocator(){}

		// default copy constructor
		// default assignment operator

		template <typename U> MPAllocator(const MPAllocator<U> &) {}

		// default destructor

		static pointer address(reference r) { return &r; }
		static const_pointer address(const_reference s) { return &s; }
		static size_type max_size() { return (std::numeric_limits<size_type>::max)(); }
		
		static void construct(const pointer ptr, const value_type & t)
		{ new (ptr) T(t); }
		static void destroy(const pointer ptr)
		{ ptr->~T(); 
			(void) ptr; }

		bool operator==(const MPAllocator &) const
		{ return true; }
		bool operator!=(const MPAllocator &) const
		{ return false; }

		static pointer allocate(const size_type n)
		{ 
			return (pointer)::GlobalAlloc(GMEM_FIXED, n*sizeof(T)); 
		}
		static pointer allocate(const size_type n, const void * const)
		{
			return allocate(n); 
		}
		static pointer allocate()
		{
			return (pointer)::GlobalAlloc(GMEM_FIXED, sizeof(T)); 
		}
		static void deallocate(const pointer ptr, const size_type)
		{
			::GlobalFree((HGLOBAL)ptr);
		}
		static void deallocate(const pointer ptr)
		{
			::GlobalFree((HGLOBAL)ptr);
		}
	};
#endif

	struct block {
		size_t size;
		int used;
		block *prev;
		block *next;
	};
	
#ifdef USE_PRIVATE_ALLOCATOR
	typedef std::map<block *, int, std::less<block *>, 
		MPAllocator<std::pair<const block *, int> > > BlockList;
#else
	typedef std::map<block *, int> BlockList;
#endif
	typedef typename BlockList::iterator BlockListItr;

	BlockList m_blocks;

	//块的结构
	struct chunck {
		size_t size;
		int used;
		chunck *prev;
		chunck *next;
	};
#ifdef USE_PRIVATE_ALLOCATOR
	typedef std::multimap<int, chunck *, std::less<int>, 
		MPAllocator<std::pair<const int, chunck *> > > ChunckList;
#else
	typedef std::multimap<int, chunck *> ChunckList;
#endif
	typedef typename ChunckList::iterator ChunckListItr;

	ChunckList m_frees;

	//总共动态分配了的空间大小
	int m_size;
	const char *m_name;

private:

	//是否属于本池
	bool ischunck(void *p)
	{
		char *c = (char *)p;

		for (BlockListItr it = m_blocks.begin();
			it != m_blocks.end(); ++it) 
		{
			char *b = (char *)it->first;
			if (c >= b + sizeof(block) + sizeof(chunck)
				&& c < b + sizeof(block) + it->second)
			{
				return true;
			}
		}
		return false;
	}

	chunck *newblock(size_t asize)
	{
		size_t bsize = asize < BLOCK_SIZE - sizeof(block) - sizeof(chunck) 
			? BLOCK_SIZE : asize + sizeof(block) + sizeof(chunck);
#ifdef USE_GLOBAL_MEMORY
		if (block *b = (block *)(::GlobalAlloc(GMEM_FIXED, bsize))) {
#else
		if (block *b = (block *)(new char[bsize])) {
#endif
			m_blocks[b] = bsize;
			m_size += bsize;

			b->size = bsize - sizeof(block);
			b->used = 1;

			chunck *c = (chunck *)((char *)b + sizeof(block));
			c->size = b->size - sizeof(chunck);
			c->prev = 0;
			c->next = 0;

			return c;
		}

		return 0;
	}

	void cutchunck(chunck *c, size_t asize)
	{
		chunck *next = (chunck *)((char *)c + asize + sizeof(chunck));
		next->size = c->size - asize - sizeof(chunck);
		c->size = asize;
		next->used = 0;
		next->prev = c;
		next->next = c->next;
		c->next = next;
		m_frees.insert(std::make_pair<int, chunck *>(next->size, next)); 
	}

};

//提供一个缺省的分配器,这个缺省的分配器使用了临界区作为锁

class DefaultI8MP
{
public:
	typedef ZI8MP<ZSync> MPty;
	static MPty& instance(void)
	{
		static std::tr1::shared_ptr<MPty> i8mp;
		if (i8mp.get() != 0)
			return *i8mp;

		i8mp.reset(new MPty);
		return *i8mp;
	}
};
#else
class DefaultI8MP
{
public:
	char *alloc(size_t size)
	{
		return new char[size];
	}	
	
	template<typename T>
	T *malloc(size_t size)
	{
		return (T*)new char[size * sizeof(T)];
	}
	
	//释放
	template<typename T>
	void release(T *&p)
	{
		if (p) {
			free((void *)p);
			p = 0;
		}
	}

	void free(void *p)
	{
		if (!p)
			return;

		char *c = (char *)p;
		delete[] c;
	}

	void reclaim(void)
	{
	}

	void dump(std::stringstream& ss)
	{
		ss << "not used i8 memory pool\n" << std::ends;
	}

	static DefaultI8MP& instance(void)
	{
		static DefaultI8MP i8mp;
		return i8mp;
	}
};
#endif

#define I8MP i8desk::DefaultI8MP::instance()

#ifdef USED_I8_MP

//下面类模板适用分配固定大小的内存
//从boost::pool改编而来

template <typename SizeType>
class simple_segregated_storage
{
public:
	typedef SizeType size_type;

private:
	simple_segregated_storage(const simple_segregated_storage &);
	void operator=(const simple_segregated_storage &);

	// pre: (n > 0), (start != 0), (nextof(start) != 0)
	// post: (start != 0)
	static void * try_malloc_n(void * & start, size_type n,
		size_type partition_size);

protected:
	void * first;

	// Traverses the free list referred to by "first",
	//  and returns the iterator previous to where
	//  "ptr" would go if it was in the free list.
	// Returns 0 if "ptr" would go at the beginning
	//  of the free list (i.e., before "first")
	void * find_prev(void * ptr);

	// for the sake of code readability :)
	static void * & nextof(void * const ptr)
	{ return *(static_cast<void **>(ptr)); }

public:
	// Post: empty()
	simple_segregated_storage()
		:first(0) { }

	// pre: npartition_sz >= sizeof(void *)
	//      npartition_sz = sizeof(void *) * i, for some integer i
	//      nsz >= npartition_sz
	//      block is properly aligned for an array of object of
	//        size npartition_sz and array of void *
	// The requirements above guarantee that any pointer to a chunk
	//  (which is a pointer to an element in an array of npartition_sz)
	//  may be cast to void **.
	static void * segregate(void * block,
		size_type nsz, size_type npartition_sz,
		void * end = 0);

	// Same preconditions as 'segregate'
	// Post: !empty()
	void add_block(void * const block,
		const size_type nsz, const size_type npartition_sz)
	{
		// Segregate this block and merge its free list into the
		//  free list referred to by "first"
		first = segregate(block, nsz, npartition_sz, first);
	}

	// Same preconditions as 'segregate'
	// Post: !empty()
	void add_ordered_block(void * const block,
		const size_type nsz, const size_type npartition_sz)
	{
		// This (slower) version of add_block segregates the
		//  block and merges its free list into our free list
		//  in the proper order

		// Find where "block" would go in the free list
		void * const loc = find_prev(block);

		// Place either at beginning or in middle/end
		if (loc == 0)
			add_block(block, nsz, npartition_sz);
		else
			nextof(loc) = segregate(block, nsz, npartition_sz, nextof(loc));
	}

	// default destructor

	bool empty() const { return (first == 0); }

	// pre: !empty()
	void * malloc()
	{
		void * const ret = first;

		// Increment the "first" pointer to point to the next chunk
		first = nextof(first);
		return ret;
	}

	// pre: chunk was previously returned from a malloc() referring to the
	//  same free list
	// post: !empty()
	void free(void * const chunk)
	{
		nextof(chunk) = first;
		first = chunk;
	}

	// pre: chunk was previously returned from a malloc() referring to the
	//  same free list
	// post: !empty()
	void ordered_free(void * const chunk)
	{
		// This (slower) implementation of 'free' places the memory
		//  back in the list in its proper order.

		// Find where "chunk" goes in the free list
		void * const loc = find_prev(chunk);

		// Place either at beginning or in middle/end
		if (loc == 0)
			free(chunk);
		else
		{
			nextof(chunk) = nextof(loc);
			nextof(loc) = chunk;
		}
	}

	// Note: if you're allocating/deallocating n a lot, you should
	//  be using an ordered pool.
	void * malloc_n(size_type n, size_type partition_size);

	// pre: chunks was previously allocated from *this with the same
	//   values for n and partition_size
	// post: !empty()
	// Note: if you're allocating/deallocating n a lot, you should
	//  be using an ordered pool.
	void free_n(void * const chunks, const size_type n,
		const size_type partition_size)
	{
		add_block(chunks, n * partition_size, partition_size);
	}

	// pre: chunks was previously allocated from *this with the same
	//   values for n and partition_size
	// post: !empty()
	void ordered_free_n(void * const chunks, const size_type n,
		const size_type partition_size)
	{
		add_ordered_block(chunks, n * partition_size, partition_size);
	}
};

template <typename SizeType>
void * simple_segregated_storage<SizeType>::find_prev(void * const ptr)
{
	// Handle border case
	if (first == 0 || std::greater<void *>()(first, ptr))
		return 0;

	void * iter = first;
	while (true)
	{
		// if we're about to hit the end or
		//  if we've found where "ptr" goes
		if (nextof(iter) == 0 || std::greater<void *>()(nextof(iter), ptr))
			return iter;

		iter = nextof(iter);
	}
}

template <typename SizeType>
void * simple_segregated_storage<SizeType>::segregate(
	void * const block,
	const size_type sz,
	const size_type partition_sz,
	void * const end)
{
	// Get pointer to last valid chunk, preventing overflow on size calculations
	//  The division followed by the multiplication just makes sure that
	//    old == block + partition_sz * i, for some integer i, even if the
	//    block size (sz) is not a multiple of the partition size.
	char * old = static_cast<char *>(block)
		+ ((sz - partition_sz) / partition_sz) * partition_sz;

	// Set it to point to the end
	nextof(old) = end;

	// Handle border case where sz == partition_sz (i.e., we're handling an array
	//  of 1 element)
	if (old == block)
		return block;

	// Iterate backwards, building a singly-linked list of pointers
	for (char * iter = old - partition_sz; iter != block;
		old = iter, iter -= partition_sz)
		nextof(iter) = old;

	// Point the first pointer, too
	nextof(block) = old;

	return block;
}

// The following function attempts to find n contiguous chunks
//  of size partition_size in the free list, starting at start.
// If it succeds, it returns the last chunk in that contiguous
//  sequence, so that the sequence is known by [start, {retval}]
// If it fails, it does do either because it's at the end of the
//  free list or hits a non-contiguous chunk.  In either case,
//  it will return 0, and set start to the last considered
//  chunk.  You are at the end of the free list if
//  nextof(start) == 0.  Otherwise, start points to the last
//  chunk in the contiguous sequence, and nextof(start) points
//  to the first chunk in the next contiguous sequence (assuming
//  an ordered free list)
template <typename SizeType>
void * simple_segregated_storage<SizeType>::try_malloc_n(
	void * & start, size_type n, const size_type partition_size)
{
	void * iter = nextof(start);
	while (--n != 0)
	{
		void * next = nextof(iter);
		if (next != static_cast<char *>(iter) + partition_size)
		{
			// next == 0 (end-of-list) or non-contiguous chunk found
			start = iter;
			return 0;
		}
		iter = next;
	}
	return iter;
}

template <typename SizeType>
void * simple_segregated_storage<SizeType>::malloc_n(const size_type n,
													 const size_type partition_size)
{
	void * start = &first;
	void * iter;
	do
	{
		if (nextof(start) == 0)
			return 0;
		iter = try_malloc_n(start, n, partition_size);
	} while (iter == 0);
	void * const ret = nextof(start);
	nextof(start) = nextof(iter);
	return ret;
}

template <typename SizeType>
class PODptr
{
public:
	typedef SizeType size_type;

private:
	char *ptr;
	size_type sz;

	char * ptr_next_size() const { return (ptr + sz - sizeof(size_type)); }
	char * ptr_next_ptr() const { return (ptr_next_size() - sizeof(void *)); }

public:
	PODptr(char * const nptr, const size_type nsize) : ptr(nptr), sz(nsize) { }
	PODptr() : ptr(0), sz(0) { }

	bool valid() const { return (begin() != 0); }
	void invalidate() { begin() = 0; }
	char * & begin() { return ptr; }
	char * begin() const { return ptr; }
	char * end() const { return ptr_next_ptr(); }
	size_type total_size() const { return sz; }
	size_type element_size() const 
	{ return (sz - sizeof(size_type) - sizeof(void *)); }
	size_type & next_size() const 
	{ return *(reinterpret_cast<size_type *>(ptr_next_size())); }
	char * & next_ptr() const 
	{ return *(reinterpret_cast<char **>(ptr_next_ptr())); }
	PODptr next() const 
	{ return PODptr<size_type>(next_ptr(), next_size()); }
	void next(const PODptr & arg) const
	{ next_ptr() = arg.begin();
	next_size() = arg.total_size(); }
};

template <typename UserAllocator>
class pool: protected simple_segregated_storage<
	typename UserAllocator::size_type>
{
public:
	typedef UserAllocator user_allocator;
	typedef typename UserAllocator::size_type size_type;
	typedef typename UserAllocator::difference_type difference_type;

private:
	const static unsigned min_alloc_size = sizeof(void *);

	// Returns 0 if out-of-memory
	// Called if malloc/ordered_malloc needs to resize the free list
	void * malloc_need_resize();
	void * ordered_malloc_need_resize();

protected:
	PODptr<size_type> list;

	simple_segregated_storage<size_type> & store() { return *this; }
	const simple_segregated_storage<size_type> & store() const { return *this; }
	const size_type requested_size;
	size_type next_size;
	size_type start_size;

	// finds which POD in the list 'chunk' was allocated from
	PODptr<size_type> find_POD(void * const chunk) const;

	// is_from() tests a chunk to determine if it belongs in a block
	static bool is_from(void * const chunk, char * const i,
		const size_type sizeof_i)
	{
		// We use std::less_equal and std::less to test 'chunk'
		//  against the array bounds because standard operators
		//  may return unspecified results.
		// This is to ensure portability.  The operators < <= > >= are only
		//  defined for pointers to objects that are 1) in the same array, or
		//  2) subobjects of the same object [5.9/2].
		// The functor objects guarantee a total order for any pointer [20.3.3/8]
		//WAS:
		//      return (std::less_equal<void *>()(static_cast<void *>(i), chunk)
		//          && std::less<void *>()(chunk,
		//              static_cast<void *>(i + sizeof_i)));
		std::less_equal<void *> lt_eq;
		std::less<void *> lt;
		return (lt_eq(i, chunk) && lt(chunk, i + sizeof_i));
	}

	size_type alloc_size() const
	{
		return requested_size;
	}

	// for the sake of code readability :)
	static void * & nextof(void * const ptr)
	{ return *(static_cast<void **>(ptr)); }

public:
	// The second parameter here is an extension!
	// pre: npartition_size != 0 && nnext_size != 0
	explicit pool(const size_type nrequested_size,
		const size_type nnext_size = 32)
		:list(0, 0), requested_size(nrequested_size), next_size(nnext_size), start_size(nnext_size)
	{ }

	~pool() 
	{
		//purge_memory(); 
	}

	// Releases memory blocks that don't have chunks allocated
	// pre: lists are ordered
	//  Returns true if memory was actually deallocated
	bool release_memory();

	// Releases *all* memory blocks, even if chunks are still allocated
	//  Returns true if memory was actually deallocated
	bool purge_memory();

	// These functions are extensions!
	size_type get_next_size() const { return next_size; }
	void set_next_size(const size_type nnext_size) { next_size = start_size = nnext_size; }
	size_type get_requested_size() const { return requested_size; }

	// Both malloc and ordered_malloc do a quick inlined check first for any
	//  free chunks.  Only if we need to get another memory block do we call
	//  the non-inlined *_need_resize() functions.
	// Returns 0 if out-of-memory
	void * malloc()
	{
		// Look for a non-empty storage
		if (!store().empty())
			return store().malloc();
		return malloc_need_resize();
	}

	void * ordered_malloc()
	{
		// Look for a non-empty storage
		if (!store().empty())
			return store().malloc();
		return ordered_malloc_need_resize();
	}

	// Returns 0 if out-of-memory
	// Allocate a contiguous section of n chunks
	void * ordered_malloc(size_type n);

	// pre: 'chunk' must have been previously
	//        returned by *this.malloc().
	void free(void * const chunk)
	{ store().free(chunk); }

	// pre: 'chunk' must have been previously
	//        returned by *this.malloc().
	void ordered_free(void * const chunk)
	{ store().ordered_free(chunk); }

	// pre: 'chunk' must have been previously
	//        returned by *this.malloc(n).
	void free(void * const chunks, const size_type n)
	{
		const size_type partition_size = alloc_size();
		const size_type total_req_size = n * requested_size;
		const size_type num_chunks = total_req_size / partition_size +
			((total_req_size % partition_size) ? true : false);

		store().free_n(chunks, num_chunks, partition_size);
	}

	// pre: 'chunk' must have been previously
	//        returned by *this.malloc(n).
	void ordered_free(void * const chunks, const size_type n)
	{
		const size_type partition_size = alloc_size();
		const size_type total_req_size = n * requested_size;
		const size_type num_chunks = total_req_size / partition_size +
			((total_req_size % partition_size) ? true : false);

		store().ordered_free_n(chunks, num_chunks, partition_size);
	}

	// is_from() tests a chunk to determine if it was allocated from *this
	bool is_from(void * const chunk) const
	{
		return (find_POD(chunk).valid());
	}
};

template <typename UserAllocator>
bool pool<UserAllocator>::release_memory()
{
	// This is the return value: it will be set to true when we actually call
	//  UserAllocator::free(..)
	bool ret = false;

	// This is a current & previous iterator pair over the memory block list
	PODptr<size_type> ptr = list;
	PODptr<size_type> prev;

	// This is a current & previous iterator pair over the free memory chunk list
	//  Note that "prev_free" in this case does NOT point to the previous memory
	//  chunk in the free list, but rather the last free memory chunk before the
	//  current block.
	void * free_p = this->first;
	void * prev_free_p = 0;

	const size_type partition_size = alloc_size();

	// Search through all the all the allocated memory blocks
	while (ptr.valid())
	{
		// At this point:
		//  ptr points to a valid memory block
		//  free_p points to either:
		//    0 if there are no more free chunks
		//    the first free chunk in this or some next memory block
		//  prev_free_p points to either:
		//    the last free chunk in some previous memory block
		//    0 if there is no such free chunk
		//  prev is either:
		//    the PODptr whose next() is ptr
		//    !valid() if there is no such PODptr

		// If there are no more free memory chunks, then every remaining
		//  block is allocated out to its fullest capacity, and we can't
		//  release any more memory
		if (free_p == 0)
			break;

		// We have to check all the chunks.  If they are *all* free (i.e., present
		//  in the free list), then we can free the block.
		bool all_chunks_free = true;

		// Iterate 'i' through all chunks in the memory block
		// if free starts in the memory block, be careful to keep it there
		void * saved_free = free_p;
		for (char * i = ptr.begin(); i != ptr.end(); i += partition_size)
		{
			// If this chunk is not free
			if (i != free_p)
			{
				// We won't be able to free this block
				all_chunks_free = false;

				// free_p might have travelled outside ptr
				free_p = saved_free;
				// Abort searching the chunks; we won't be able to free this
				//  block because a chunk is not free.
				break;
			}

			// We do not increment prev_free_p because we are in the same block
			free_p = nextof(free_p);
		}

		// post: if the memory block has any chunks, free_p points to one of them
		// otherwise, our assertions above are still valid

		const PODptr<size_type> next = ptr.next();

		if (!all_chunks_free)
		{
			if (is_from(free_p, ptr.begin(), ptr.element_size()))
			{
				std::less<void *> lt;
				void * const end = ptr.end();
				do
				{
					prev_free_p = free_p;
					free_p = nextof(free_p);
				} while (free_p && lt(free_p, end));
			}
			// This invariant is now restored:
			//     free_p points to the first free chunk in some next memory block, or
			//       0 if there is no such chunk.
			//     prev_free_p points to the last free chunk in this memory block.

			// We are just about to advance ptr.  Maintain the invariant:
			// prev is the PODptr whose next() is ptr, or !valid()
			// if there is no such PODptr
			prev = ptr;
		}
		else
		{
			// All chunks from this block are free

			// Remove block from list
			if (prev.valid())
				prev.next(next);
			else
				list = next;

			// Remove all entries in the free list from this block
			if (prev_free_p != 0)
				nextof(prev_free_p) = free_p;
			else
				this->first = free_p;

			// And release memory
			UserAllocator::free(ptr.begin());
			ret = true;
		}

		// Increment ptr
		ptr = next;
	}

	next_size = start_size;
	return ret;
}

template <typename UserAllocator>
bool pool<UserAllocator>::purge_memory()
{
	PODptr<size_type> iter = list;

	if (!iter.valid())
		return false;

	do
	{
		// hold "next" pointer
		const PODptr<size_type> next = iter.next();

		// delete the storage
		UserAllocator::free(iter.begin());

		// increment iter
		iter = next;
	} while (iter.valid());

	list.invalidate();
	this->first = 0;
	next_size = start_size;

	return true;
}

template <typename UserAllocator>
void * pool<UserAllocator>::malloc_need_resize()
{
	// No memory in any of our storages; make a new storage,
	const size_type partition_size = alloc_size();
	const size_type POD_size = next_size * partition_size 
		+ sizeof(void *) + sizeof(size_type);
	char * const ptr = UserAllocator::malloc(POD_size);
	if (ptr == 0)
		return 0;
	const PODptr<size_type> node(ptr, POD_size);
	next_size <<= 1;

	//  initialize it,
	store().add_block(node.begin(), node.element_size(), partition_size);

	//  insert it into the list,
	node.next(list);
	list = node;

	//  and return a chunk from it.
	return store().malloc();
}

template <typename UserAllocator>
void * pool<UserAllocator>::ordered_malloc_need_resize()
{
	// No memory in any of our storages; make a new storage,
	const size_type partition_size = alloc_size();
	const size_type POD_size = next_size * partition_size 
		+ sizeof(void *) + sizeof(size_type);
	char * const ptr = UserAllocator::malloc(POD_size);
	if (ptr == 0)
		return 0;
	const PODptr<size_type> node(ptr, POD_size);
	next_size <<= 1;

	//  initialize it,
	//  (we can use "add_block" here because we know that
	//  the free list is empty, so we don't have to use
	//  the slower ordered version)
	store().add_block(node.begin(), node.element_size(), partition_size);

	//  insert it into the list,
	//   handle border case
	if (!list.valid() || std::greater<void *>()(list.begin(), node.begin()))
	{
		node.next(list);
		list = node;
	}
	else
	{
		PODptr<size_type> prev = list;

		while (true)
		{
			// if we're about to hit the end or
			//  if we've found where "node" goes
			if (prev.next_ptr() == 0
				|| std::greater<void *>()(prev.next_ptr(), node.begin()))
				break;

			prev = prev.next();
		}

		node.next(prev.next());
		prev.next(node);
	}

	//  and return a chunk from it.
	return store().malloc();
}

template <typename UserAllocator>
void * pool<UserAllocator>::ordered_malloc(const size_type n)
{
	const size_type partition_size = alloc_size();
	const size_type total_req_size = n * requested_size;
	const size_type num_chunks = total_req_size / partition_size +
		((total_req_size % partition_size) ? true : false);

	void * ret = store().malloc_n(num_chunks, partition_size);

	if (ret != 0)
		return ret;

	// Not enougn memory in our storages; make a new storage,
	if (num_chunks > next_size)
		next_size = num_chunks;
	const size_type POD_size = next_size * partition_size 
		+ sizeof(void *) + sizeof(size_type);
	char * const ptr = UserAllocator::malloc(POD_size);
	if (ptr == 0)
		return 0;
	const PODptr<size_type> node(ptr, POD_size);

	// Split up block so we can use what wasn't requested
	//  (we can use "add_block" here because we know that
	//  the free list is empty, so we don't have to use
	//  the slower ordered version)
	if (next_size > num_chunks)
		store().add_block(node.begin() + num_chunks * partition_size,
		node.element_size() - num_chunks * partition_size, partition_size);

	next_size <<= 1;

	//  insert it into the list,
	//   handle border case
	if (!list.valid() || std::greater<void *>()(list.begin(), node.begin()))
	{
		node.next(list);
		list = node;
	}
	else
	{
		PODptr<size_type> prev = list;

		while (true)
		{
			// if we're about to hit the end or
			//  if we've found where "node" goes
			if (prev.next_ptr() == 0
				|| std::greater<void *>()(prev.next_ptr(), node.begin()))
				break;

			prev = prev.next();
		}

		node.next(prev.next());
		prev.next(node);
	}

	//  and return it.
	return node.begin();
}

template <typename UserAllocator>
PODptr<typename pool<UserAllocator>::size_type>
pool<UserAllocator>::find_POD(void * const chunk) const
{
	// We have to find which storage this chunk is from.
	PODptr<size_type> iter = list;
	while (iter.valid())
	{
		if (is_from(chunk, iter.begin(), iter.element_size()))
			return iter;
		iter = iter.next();
	}

	return iter;
}


class ZI8DefaultAllocatorAdapter
{
public:
	typedef DefaultI8MP::MPty::size_type size_type;
	typedef DefaultI8MP::MPty::difference_type difference_type;
	static char *malloc(const size_type bytes) { return I8MP.alloc(bytes); }
	static void free(char * const block) { I8MP.free(block); }
};

template<size_t size, typename Lty = ZSync>
class ZI8BlockMP : private pool<ZI8DefaultAllocatorAdapter>
{
public:
	typedef pool<ZI8DefaultAllocatorAdapter> _Base;
	typedef _Base::size_type size_type;
	
	explicit ZI8BlockMP(size_type next_size = 32) 
		: _Base(size, next_size) 
	{
	}
	~ZI8BlockMP()
	{

	}
	void *alloc(void)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::malloc(); 
	}
	void free(void * const chunk)
	{ 
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::free(chunk); 
	}
	void * malloc()
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::malloc(); 
	}
	void * ordered_malloc()
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::ordered_malloc(); 
	}
	void * ordered_malloc(size_type n)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::ordered_malloc(n); 
	}
	void ordered_free(void * const chunk)
	{ 
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::ordered_free(chunk); 
	}
	void free(void * const chunks, const size_type n)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::free(chunks, n); 
	}
	void ordered_free(void * const chunks, const size_type n)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::ordered_free(chunks, n); 
	}
	void reclaim(void)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::release_memory();
	}
private:
	Lty m_lock;
};

#else

template<size_t size, typename Lty = ZSync>
class ZI8BlockMP
{
public:
	void *alloc(void)
	{
		return I8MP.alloc(size);
	}
	void free(void * const chunck)
	{ 
		I8MP.free(chunck);
	}
	void * malloc()
	{
		return I8MP.alloc(size);
	}
	void * ordered_malloc()
	{
		return I8MP.alloc(size);
	}
	void * ordered_malloc(size_t n)
	{
		return I8MP.alloc(size*n);
	}
	void ordered_free(void * const chunk)
	{ 
		I8MP.free(chunck);
	}
	void free(void * const chunks, const size_t n)
	{
		I8MP.free(chunck);
		(void)n;
	}
	void ordered_free(void * const chunks, const size_t n)
	{
		I8MP.free(chunck);
		(void)n;
	}
	void reclaim(void)
	{
	}
};
#endif

#ifdef USED_I8_MP

//这个类模板用于构建特定对象
//from boost::object_pool
// T must have a non-throwing destructor
template <typename T, typename UserAllocator>
class object_pool: protected pool<UserAllocator>
{
public:
	typedef T element_type;
	typedef UserAllocator user_allocator;
	typedef typename pool<UserAllocator>::size_type size_type;
	typedef typename pool<UserAllocator>::difference_type difference_type;

protected:
	pool<UserAllocator> & store() { return *this; }
	const pool<UserAllocator> & store() const { return *this; }

	// for the sake of code readability :)
	static void * & nextof(void * const ptr)
	{ return *(static_cast<void **>(ptr)); }

public:
	// This constructor parameter is an extension!
	explicit object_pool(const size_type next_size = 32)
		:pool<UserAllocator>(sizeof(T), next_size) { }

	~object_pool();

	// Returns 0 if out-of-memory
	element_type * malloc()
	{ return static_cast<element_type *>(store().ordered_malloc()); }
	void free(element_type * const chunk)
	{ store().ordered_free(chunk); }
	bool is_from(element_type * const chunk) const
	{ return store().is_from(chunk); }

	element_type * construct()
	{
		element_type * const ret = malloc();
		if (ret == 0)
			return ret;
		try { new (ret) element_type(); }
		catch (...) { free(ret); throw; }
		return ret;
	}


	template <typename T0>
	element_type * construct(const T0 & a0)
	{
		element_type * const ret = malloc();
		if (ret == 0)
			return ret;
		try { new (ret) element_type(a0); }
		catch (...) { free(ret); throw; }
		return ret;
	}
	template <typename T0, typename T1>
	element_type * construct(const T0 & a0, const T1 & a1)
	{
		element_type * const ret = malloc();
		if (ret == 0)
			return ret;
		try { new (ret) element_type(a0, a1); }
		catch (...) { free(ret); throw; }
		return ret;
	}

	template <typename T0, typename T1, typename T2>
	element_type * construct(const T0 & a0, const T1 & a1, const T2 & a2)
	{
		element_type * const ret = malloc();
		if (ret == 0)
			return ret;
		try { new (ret) element_type(a0, a1, a2); }
		catch (...) { free(ret); throw; }
		return ret;
	}

	void destroy(element_type * const chunk)
	{
		chunk->~T();
		free(chunk);
	}

	// These functions are extensions!
	size_type get_next_size() const { return store().get_next_size(); }
	void set_next_size(const size_type x) { store().set_next_size(x); }
};

template <typename T, typename UserAllocator>
object_pool<T, UserAllocator>::~object_pool()
{
	// handle trivial case
	if (!this->list.valid())
		return;

	PODptr<size_type> iter = this->list;
	PODptr<size_type> next = iter;

	// Start 'freed_iter' at beginning of free list
	void * freed_iter = this->first;

	const size_type partition_size = this->alloc_size();

	do
	{
		// increment next
		next = next.next();

		// delete all contained objects that aren't freed

		// Iterate 'i' through all chunks in the memory block
		for (char * i = iter.begin(); i != iter.end(); i += partition_size)
		{
			// If this chunk is free
			if (i == freed_iter)
			{
				// Increment freed_iter to point to next in free list
				freed_iter = nextof(freed_iter);

				// Continue searching chunks in the memory block
				continue;
			}

			// This chunk is not free (allocated), so call its destructor
			static_cast<T *>(static_cast<void *>(i))->~T();
			// and continue searching chunks in the memory block
		}

		// free storage
		UserAllocator::free(iter.begin());

		// increment iter
		iter = next;
	} while (iter.valid());

	// Make the block list empty so that the inherited destructor doesn't try to
	//  free it again.
	this->list.invalidate();
}

template<typename T, typename Lty = ZSync>
class ZI8ObjectMP : private object_pool<T, ZI8DefaultAllocatorAdapter>
{
public:
	typedef object_pool<T, ZI8DefaultAllocatorAdapter> _Base;
	using _Base::size_type;
	using _Base::element_type;

	explicit ZI8ObjectMP(size_type next_size = 32) 
		: _Base(next_size) 
	{
	}

	element_type * malloc()
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::malloc();
	}
	void free(element_type * const chunk)
	{ 
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::free(chunk);
	}
	bool is_from(element_type * const chunk) const
	{ 
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::is_from(chunk);
	}
	element_type * construct()
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::construct();
	}
	template <typename T0>
	element_type * construct(const T0 & a0)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::construct(a0);
	}
	template <typename T0, typename T1>
	element_type * construct(const T0 & a0, const T1 & a1)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::construct(a0, a1);
	}
	template <typename T0, typename T1, typename T2>
	element_type * construct(const T0 & a0, const T1 & a1, const T2 & a2)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return _Base::construct(a0, a1, a2);
	}
	void destroy(element_type * const chunk)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::destroy(chunk);
	}
	void reclaim(void)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		_Base::release_memory();
	}

private:
	Lty m_lock;
};

#else

template<typename T, typename Lty = ZSync>
class ZI8ObjectMP 
{
public:
	typedef T element_type;

	element_type * malloc()
	{
		return new char[sizeof(T)];
	}
	void free(element_type * const chunk)
	{ 
		delete[] (char*)chunk;
	}
	bool is_from(element_type * const chunk) const
	{ 
		return true;
	}
	element_type * construct()
	{
		return new T();
	}
	template <typename T0>
	element_type * construct(const T0 & a0)
	{
		return new T(a0);
	}
	template <typename T0, typename T1>
	element_type * construct(const T0 & a0, const T1 & a1)
	{
		return new T(a0, a1);
	}
	template <typename T0, typename T1, typename T2>
	element_type * construct(const T0 & a0, const T1 & a1, const T2 & a2)
	{
		return new T(a0, a1, a2);
	}
	void destroy(element_type * const chunk)
	{
		delete chunk;
	}

	void reclaim(void)
	{
	}
};


#endif


#if defined(USED_I8_MP)
#define DECLARE_I8_NEW_DELETE_OP() \
	public: \
		static void *operator new(size_t size); \
		static void operator delete(void *p, size_t size); \
		static void ReclaimObjectPool(void); \
	private: 

#define IMPLMENT_I8_NEW_DELETE_OP(X) \
		typedef i8desk::ZI8BlockMP<sizeof(X)> X##ObjectMPty; \
		static X##ObjectMPty *Get##X##Allocator(void) \
		{ \
			static X##ObjectMPty *allocator = 0; \
            if (allocator) \
				return allocator; \
			return allocator = new X##ObjectMPty; \
		} \
		void *X::operator new(size_t size) \
		{ \
			assert(size == sizeof(X)); \
			return Get##X##Allocator()->alloc(); \
		} \
		void X::operator delete(void *p, size_t size) \
		{ \
			assert(size == sizeof(X)); \
			Get##X##Allocator()->free(p); \
		}  \
		void X::ReclaimObjectPool(void) \
		{ \
			X##ObjectMPty *allocator = Get##X##Allocator(); \
			allocator->reclaim(); \
			delete allocator; \
		}
#else
#define DECLARE_I8_NEW_DELETE_OP() 
#define IMPLMENT_I8_NEW_DELETE_OP(X) 
#endif

#ifdef USED_I8_MP
//from boost::pool_allocator.hpp

template <typename T,
typename UserAllocator,
typename Mutex,
unsigned NextSize>
class pool_allocator
{
public:
	typedef T value_type;
	typedef UserAllocator user_allocator;
	typedef Mutex mutex;
	const static unsigned next_size = NextSize;

	typedef value_type * pointer;
	typedef const value_type * const_pointer;
	typedef value_type & reference;
	typedef const value_type & const_reference;
	typedef typename pool<UserAllocator>::size_type size_type;
	typedef typename pool<UserAllocator>::difference_type difference_type;

	template <typename U>
	struct rebind
	{
		typedef pool_allocator<U, UserAllocator, Mutex, NextSize> other;
	};

public:
	pool_allocator()
	{
	}

	// default copy constructor

	// default assignment operator

	// not explicit, mimicking std::allocator [20.4.1]
	template <typename U>
	pool_allocator(const pool_allocator<U, UserAllocator, Mutex, NextSize> &)
	{
	}

	// default destructor

	static pointer address(reference r)
	{ return &r; }
	static const_pointer address(const_reference s)
	{ return &s; }
	static size_type max_size()
	{ return (std::numeric_limits<size_type>::max)(); }

	static void construct(const pointer ptr, const value_type & t)
	{
		new (ptr) T(t); 
	}
	static void destroy(const pointer ptr)
	{
		ptr->~T();
		(void) ptr; // avoid unused variable warning
	}

	bool operator==(const pool_allocator &) const
	{ return true; }
	bool operator!=(const pool_allocator &) const
	{ return false; }

	static pointer allocate(const size_type n)
	{
		if (n == 1)
			return (pointer)mp.malloc();
		return (pointer)mp.ordered_malloc(n);
	}
	static pointer allocate(const size_type n, const void * const)
	{
		return allocate(n); 
	}
	static pointer allocate()
	{
		return (pointer)mp.malloc();
	}
	static void deallocate(const pointer ptr, const size_type n)
	{
		if (n == 1)
			mp.free(ptr);
		else
			mp.free(ptr, n);
	}
	static void deallocate(const pointer ptr)
	{
		mp.free(ptr);
	}

	typedef ZI8BlockMP<sizeof(T), Mutex> MPty;
private:
	static MPty mp;
};

template <typename T,
	typename UserAllocator,
	typename Mutex,
	unsigned NextSize>
__declspec(selectany) 
typename pool_allocator<T, UserAllocator, Mutex, NextSize>::MPty 
pool_allocator<T, UserAllocator, Mutex, NextSize>::mp;

template<
    typename UserAllocator,
    typename Mutex,
    unsigned NextSize>
class pool_allocator<void, UserAllocator, Mutex, NextSize>
{
public:
    typedef void*       pointer;
    typedef const void* const_pointer;
    typedef void        value_type;
    template <class U> struct rebind {
	typedef pool_allocator<U, UserAllocator, Mutex, NextSize> other;
    };
};

template <typename T,
    typename UserAllocator = ZI8DefaultAllocatorAdapter,
    typename Mutex = ZSync,
    unsigned NextSize = 32>
class ZI8PoolAllocator : public pool_allocator<T, UserAllocator, Mutex, NextSize>
{
};

#else 

template<typename T> 
class ZI8PoolAllocator : public std::allocator<T>
{
};

#endif



} //namespace i8desk

#endif //I8MEMORYPOOL_H