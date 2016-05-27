#pragma once

#include <iostream>
#include <vector>
using namespace std;

template<class K, class V>
struct HashTableNode
{
	HashTableNode(K key, V value)
		:_key(key)
		,_value(value)
		,_next(NULL)
	{}
	K _key;
	V _value;
	HashTableNode<K, V> *_next;
};

template<class K, class V >
class HashTableBucket
{
	typedef HashTableNode<K, V> Node;
public:
	HashTableBucket();

	HashTableBucket(const HashTableBucket<K, V> &t);
	HashTableBucket<K, V>& operator=(HashTableBucket<K, V> t);
	~HashTableBucket();
	bool Insert(const K& key, const V& value);
	Node *Find(const K& key);
	bool Remove(const K& key);
	void PrintTables();

protected:
	size_t _HashFunc(const K &key);
	size_t _GetNextPrime(size_t size);
	void _CheckExpand();

protected:
	vector<Node*> _tables;
	size_t _size;
};

template<class K, class V>
inline size_t HashTableBucket<K, V>::_GetNextPrime(size_t size)
{
	// 使用素数表对齐做哈希表的容量，降低哈希冲突
	const int _PrimeSize = 28;
	static const unsigned long _PrimeList[_PrimeSize] =
	{
		53ul,         97ul,         193ul,       389ul,       769ul,
		1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
		49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
		1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
		50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
		1610612741ul, 3221225473ul, 4294967291ul
	};
	for (size_t i = 0; i < _PrimeSize; ++i)
	{
		if (_PrimeList[i] > size)
		{
			return _PrimeList[i];
		}
	}
	return size;
}

template<class K, class V>
inline void HashTableBucket<K, V>::_CheckExpand()
{
	if (_size >= (0.9 * _tables.size()))
	{
		_tables.resize(_GetNextPrime(_tables.size()));
	}
}

template<class K, class V>
inline HashTableBucket<K, V>::HashTableBucket()
	:_size(0)
{
	_tables.resize(_GetNextPrime(_tables.size()));
}

template<class K, class V>
inline HashTableBucket<K, V>::HashTableBucket(const HashTableBucket<K, V>& t)
	:_size(0)
{
	_tables.resize(t.size());
	for (size_t i = 0; i < t._size; ++i)
	{
		Node *cur = t._tables[i];
		while (cur)
		{
			this->Insert(cur->_key, cur->_value);
			cur = cur->_next;
		}
	}
}


template<class K, class V>
inline bool HashTableBucket<K, V>::Insert(const K & key, const V & value)
{
	_CheckExpand();
	int index = key % _tables.size();
	Node *NewNode = new Node(key, value);
	if (_tables[index] == NULL)
	{
		_tables[index] = NewNode;
	}
	else
	{
		NewNode->_next = _tables[index];
		_tables[index] = NewNode;
	}
	++_size;
	return true;
}


template<class K, class V>
inline HashTableBucket<K, V>& HashTableBucket<K, V>::operator=(HashTableBucket<K, V> t)
{
	_tables.swap(t);
	swap(_size, t._size);
}

template<class K, class V>
inline HashTableBucket<K, V>::~HashTableBucket()
{
	for (size_t i = 0; i < _tables.size(); ++i)
	{
		Node *cur = _tables[i];
		while (cur)
		{
			Node *del = cur;
			cur = cur->_next;
			delete del;
		}
		_tables[i] = NULL;
	}
}
