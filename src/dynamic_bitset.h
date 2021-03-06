// July 2013, Jairo Andres Velasco Romero, jairov(at)javerianacali.edu.co
#pragma once

#include <vector>
#include <assert.h>
#include <stdint.h>
#include <boost/functional/hash.hpp>

template<typename Block, typename Index>
struct bitutil
{
	static void bs(Block* b, Index idx)
	{
		*b |= Block(1) << idx;
	}
		
	static void bc(Block* b, Index idx)
	{
		*b &= ~(Block(1) << idx);
	}

	static bool bt(Block b, Index idx)
	{
		Block mask = Block(1) << idx;
		return (b & mask) != 0;
	}

	static bool bts(Block* b, Index idx)
	{
		auto r = bt(*b, idx);
		bs(b, idx);
		return r;
	}

	static bool btc(Block* b, Index idx)
	{
		auto r = bt(*b, idx);
		bc(b, idx);
		return r;
	}

	/*
	Martin L�uter (1997), Charles E. Leiserson, Harald Prokop, Keith H. Randall
	"Using de Bruijn Sequences to Index a 1 in a Computer Word"
	*/
	static bool bsf(Block b, Index* i)
	{
		static const uint8_t index64[64] = {
			0,  1, 48,  2, 57, 49, 28,  3,
			61, 58, 50, 42, 38, 29, 17,  4,
			62, 55, 59, 36, 53, 51, 43, 22,
			45, 39, 33, 30, 24, 18, 12,  5,
			63, 47, 56, 27, 60, 41, 37, 16,
			54, 35, 52, 21, 44, 32, 23, 11,
			46, 26, 40, 15, 34, 20, 31, 10,
			25, 14, 19,  9, 13,  8,  7,  6
		};

		static_assert(sizeof(b) <= 8, "Max size of block is 64-bit");
		if(b==0) return false;
		const uint64_t debruijn64 = 0x03f79d71b4cb0a89;
		*i = index64[((b & -b) * debruijn64) >> 58];
		return true;
	}

	static Index popcnt(Block x)
	{
		static_assert(sizeof(x) <= 8, "Max size of block is 64-bit");
		Index pop;
		for(pop=0; x; pop++)
			x &= x - 1;
		return pop;
	}
};
// 64-bit AMD64 versions
#ifdef _MSC_VER
#include <intrin.h>

template<typename Index>
struct bitutil<uint64_t, Index>
{
	static void bs(uint64_t* b, Index idx)
	{
		_bittestandset64(reinterpret_cast<int64_t*>(b), idx);
	}

	static void bc(uint64_t* b, Index idx)
	{
		_bittestandreset64(reinterpret_cast<int64_t*>(b), idx);
	}

	static bool bt(uint64_t b, Index idx)
	{
		return _bittest64(reinterpret_cast<int64_t*>(&b), idx) != 0;
	}

	static bool bts(uint64_t* b, Index idx)
	{
		return _bittestandset64(reinterpret_cast<int64_t*>(b), idx) != 0;
	}

	static bool btc(uint64_t* b, Index idx)
	{
		return _bittestandreset64(reinterpret_cast<int64_t*>(b), idx) != 0;
	}

	static Index popcnt(uint64_t x)
	{
		return static_cast<Index>(__popcnt64(x));
	}

	static bool bsf(uint64_t b, Index* i)
	{
		unsigned long ii;
		auto r = _BitScanForward64(&ii, b) != 0;
		*i = static_cast<Index>(ii);
		return r;
	}
};

// 32-bit x86 versions
template<typename Index>
struct bitutil<uint32_t, Index>
{
	static void bs(uint32_t* b, Index idx)
	{
		_bittestandset(reinterpret_cast<int32_t*>(b), idx);
	}

	static void bc(uint32_t* b, Index idx)
	{
		_bittestandreset(reinterpret_cast<int32_t*>(b), idx);
	}

	static bool bt(uint32_t b, Index idx)
	{
		return _bittest(reinterpret_cast<int32_t*>(&b), idx) != 0;
	}

	static bool bts(uint32_t* b, Index idx)
	{
		return _bittestandset(reinterpret_cast<int32_t*>(b), idx) != 0;
	}

	static bool btc(uint32_t* b, Index idx)
	{
		return _bittestandreset(reinterpret_cast<int32_t*>(b), idx) != 0;
	}

	static Index popcnt(uint32_t x)
	{
		return static_cast<Index>(__popcnt(x));
	}

	static bool bsf(uint32_t b, Index* i)
	{
		unsigned long ii;
		auto r = _BitScanForward(&ii, b);
		*i = static_cast<Index>(ii);
		return r;
	}
};
#elif __GNUC__

template<typename Index>
struct bitutil<uint64_t, Index>
{
	static void bs(uint64_t* b, Index idx)
	{
		uint8_t i = static_cast<uint8_t>(idx);
		*b |= uint64_t(1) << i;
	}

	static void bc(uint64_t* b, Index idx)
	{
		uint8_t i = static_cast<uint8_t>(idx);
		*b &= ~(uint64_t(1) << i);
	}

	static bool bt(uint64_t b, Index idx)
	{
		uint8_t i = static_cast<uint8_t>(idx);
		return ((b >> i) & 1) != 0;
	}

	static bool bts(uint64_t* b, Index idx)
	{
		bool flag = bt(*b, idx); 
		bs(b, idx);
		return flag;
	}

	static bool btc(uint64_t* b, Index idx)
	{
		bool flag = bt(*b, idx); 
		bc(b, idx);
		return flag;
	}

	static Index popcnt(uint64_t x)
	{
		return static_cast<Index>(__builtin_popcountll(x));
	}

	static bool bsf(uint64_t b, Index* i)
	{	
		auto r = __builtin_ffsll(b);
		if(r != 0) 
		{
			*i = static_cast<Index>(r - 1);
			return true;
		}
		return false;
	}
};


template<typename Index>
struct bitutil<uint32_t, Index>
{
	static void bs(uint32_t* b, Index idx)
	{
		uint8_t i = static_cast<uint8_t>(idx);
		*b |= uint32_t(1) << i;
	}

	static void bc(uint32_t* b, Index idx)
	{
		uint8_t i = static_cast<uint8_t>(idx);
		*b &= ~(uint32_t(1) << i);
	}

	static bool bt(uint32_t b, Index idx)
	{
		uint8_t i = static_cast<uint8_t>(idx);
		return ((b >> i) & 1) != 0;
	}

	static bool bts(uint32_t* b, Index idx)
	{
		bool flag = bt(*b, idx); 
		bs(b, idx);
		return flag;
	}

	static bool btc(uint32_t* b, Index idx)
	{
		bool flag = bt(*b, idx); 
		bc(b, idx);
		return flag;
	}

	static Index popcnt(uint32_t x)
	{
		return static_cast<Index>(__builtin_popcountl(x));
	}

	static bool bsf(uint32_t b, Index* i)
	{	
		auto r = __builtin_ffsl(b);
		if(r != 0) 
		{
			*i = static_cast<Index>(r - 1);
			return true;
		}
		return false;
	}
};


#endif

// Declarations 

template<class _Parent>
class dynamic_bitset_iterator;

template<typename ElementType, typename Block, typename Allocator = std::allocator<Block>>
class dynamic_bitset;

// Definitions

template<class _Parent>
class dynamic_bitset_iterator
{
public:
	typedef dynamic_bitset_iterator<_Parent> iterator;
	typedef typename _Parent::element_type element_type;
	typedef typename _Parent::block_type block_type;
	typedef typename _Parent::StorageType storage_type;	
	typedef bitutil<block_type, element_type> bu;
	static const element_type bits_per_block = sizeof(block_type) * 8;
private:
	element_type bit_num;
	const storage_type* storage;
	typename storage_type::const_iterator blk_iter;
	block_type current_block;

public:

	explicit dynamic_bitset_iterator(const storage_type* _storage)
		: storage(_storage)
	{
		blk_iter = storage->cbegin();
		if(storage->empty()) return;
		current_block = *blk_iter;
		advance();
	}

	dynamic_bitset_iterator(storage_type* _storage, element_type _bit_num)
		: storage(_storage), bit_num(_bit_num)
	{
		blk_iter = storage->cbegin() + _bit_num/bits_per_block;
		current_block = *blk_iter;
		assert(bt(current_block, _bit_num%bits_per_block));
		current_block &= ~static_cast<block_type>(0) << (_bit_num%bits_per_block + 1);
	}

	dynamic_bitset_iterator(const iterator& org)
		: storage(org.storage), bit_num(org.bit_num), current_block(org.current_block), blk_iter(org.blk_iter)
	{
	}

	element_type operator*() const 
	{
		assert(blk_iter != storage->end());
		return bit_num;
	}

	void advance()
	{
		assert(blk_iter != storage->end());
		element_type bit_idx;
		do {
			if(bu::bsf(current_block, &bit_idx))
			{
				bu::bc(&current_block, bit_idx);
				auto blk_idx = std::distance(storage->begin(), blk_iter);
				element_type blk_bit_begin = static_cast<element_type>(blk_idx) * bits_per_block;
				bit_num = bit_idx + blk_bit_begin;
				break;
			}
			++blk_iter;
			if(blk_iter == storage->end()) break;
			current_block = *blk_iter;
		} while(true);
	}

	bool is_end() const
	{
		return blk_iter == storage->end();
	}

	iterator& operator++()
	{
		advance();
		return *this;
	}

	bool operator==(const dynamic_bitset_iterator<_Parent>& rhs) const
	{
		const dynamic_bitset_iterator<_Parent>& lhs = *this;
		auto b1 = lhs.blk_iter == storage->end();
		auto b2 = rhs.blk_iter == storage->end();
		if(b1 != b2) return false;
		return lhs.bit_num == rhs.bit_num;
	}
};

template<typename ElementType, typename Block, typename Allocator>
class dynamic_bitset 
{
public:
	typedef ElementType element_type;
	typedef Block block_type;
	typedef dynamic_bitset<ElementType, Block, Allocator> dynamic_bitset_type;
	typedef dynamic_bitset_iterator<dynamic_bitset_type> iterator;
	typedef bitutil<block_type, element_type> bu;
	static const element_type bits_per_block = sizeof(Block) * 8;

private:
	typedef std::vector<Block, Allocator> StorageType;
	StorageType storage;
	element_type bits;

	friend class dynamic_bitset_iterator<dynamic_bitset_type>;

	size_t blocks_need(element_type bits)
	{
		return bits == 0 ? 0 : (bits - 1) / bits_per_block + 1;
	}

public:

	dynamic_bitset(element_type _bits)
		: storage(blocks_need(_bits)), bits(_bits)
	{
	}

	dynamic_bitset(element_type _bits, Allocator allocator)
		: storage(blocks_need(_bits), allocator), bits(_bits)
	{
	}

	dynamic_bitset()
		: dynamic_bitset(0)
	{
	}

	void add(element_type n)
	{
		bu::bs(&storage[n/bits_per_block], n%bits_per_block);
	}

	void remove(element_type n)
	{
		bu::bc(&storage[n/bits_per_block], n%bits_per_block);
	}

	bool contains(element_type n) const
	{
		return bu::bt(storage[n/bits_per_block], n%bits_per_block);
	}

	void complement()
	{
		for(auto& i : storage) i = ~i;
		block_type mask = (block_type(1) << (bits % bits_per_block)) - 1;
		storage.back() &= mask;
	}

	bool test_and_add(element_type n)
	{
		return bu::bts(&storage[n/bits_per_block], n%bits_per_block);
	}

	bool test_and_remove(element_type n)
	{
		return bu::btc(&storage[n/bits_per_block], n%bits_per_block);
	}

	void clear() 
	{
		for(auto& i : storage) i = 0;
	}

	element_type find_first() const
	{
		element_type bit_blk=0;
		element_type p;
		for(const auto& i : storage)
		{
			if(bu::bsf(i, &p))
			{
				return p + bit_blk;
			}
			bit_blk += bits_per_block;
		}
	}

	element_type find_next(element_type n)
	{
		element_type bit_idx = n % bits_per_block;
		element_type block_idx = n / bits_per_block;
		block_type mask = ~static_cast<Block>(0) << (bit_idx + 1);
				
		block_type b = storage[block_idx] & mask;
		element_type p;
		if(bsf(b, &p))
		{
			return p + block_idx * bits_per_block;
		}
		while(block_idx++ < storage.size())
		{
			b = storage[block_idx];
			if(bu::bsf(b, &p)) return p + block_idx * bits_per_block;
		}
		return max_count();
	}

	element_type max_count() const
	{
		// 2^n-1
		element_type p = (1 << (sizeof(element_type)*8)) - 1;
		return p;
	}

	void union_with(const dynamic_bitset_type& bt) 
	{
		auto i = bt.storage.begin();
		auto j = storage.begin();
		while(i!=bt.storage.end())
		{
			*j++ |= *i++;
		}
	}

	void intersect_with(const dynamic_bitset_type& bt) 
	{
		auto i = bt.storage.begin();
		auto j = storage.begin();
		while(i!=bt.storage.end())
		{
			*j++ &= *i++;
		}
	}

	void difference_with(const dynamic_bitset_type& bt)
	{
		auto i = bt.storage.begin();
		auto j = storage.begin();
		while(i!=bt.storage.end())
		{
			*j++ &= ~*i++;
		}
	}

	bool none() const
	{
		for(auto i : storage)
		{
			if(i) return false;
		}
		return true;
	}

	bool any() const 
	{
		for(auto i : storage)
		{
			if(i) return true;
		}
		return false;
	}

	element_type count() const
	{
		element_type c=0;
		for(auto i : storage)
		{
			c += bu::popcnt(i);
		}
		return c;
	}

	element_type element_in_position(element_type cnt) const
	{
		assert(cnt < count());
		element_type c = 0;
		auto i = storage.begin();
		if (cnt >= bits_per_block)
		while(true)
		{
			auto p = bu::popcnt(*i);
			if(p > cnt) break;
			cnt -= p;
			c += bits_per_block;
			i++;
		}
		block_type b = *i;
		element_type bit;
		do
		{			
			bu::bsf(b, &bit);
			bu::bc(&b, bit);
		} while(cnt--);
		return c + bit;
	}

	bool operator==(const dynamic_bitset_type& rhs) const
	{
		auto j=storage.begin();
		auto i=rhs.storage.begin();
		while(i != rhs.storage.end())
		{
			if(*i++ != *j++) return false;
		}
		return true;
	}

	iterator begin() const
	{
		return iterator(&storage);
	}

	iterator end() const
	{
		return iterator(&storage, storage->end());
	}

	size_t hash_value() const
	{
		return boost::hash_value(storage);
	}
};
