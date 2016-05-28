#pragma once

#include <iostream>
#include <vector>
using namespace std;

struct __HashFuncString
{
	size_t operator()(const string &key)
	{
		size_t hash = 0;
		for (size_t i = 0; i < key.size(); ++i)
		{
			hash += key[i];
		}
		return hash;
	}
};

struct __HashFuncInt
{
	size_t operator()(const int &key)
	{
		return key;
	}
};


//利用函数的重载，可以实现对int string类型 用不同的hash算法！！
template <class K>
struct __DefaultHashFunc
{
	size_t operator()(const string &key)
	{
		return HashFuncString(key);
	}

	size_t operator()(const int &key)
	{
		return HashFuncInt(key);
	}

	__HashFuncString HashFuncString;
	__HashFuncInt HashFuncInt;
};

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

template<class K, class V, class HashFunc = __DefaultHashFunc<K> >
class HashTableBucket
{
	typedef HashTableNode<K, V> Node;
public:
	HashTableBucket();

	HashTableBucket(const HashTableBucket<K, V, HashFunc> &t);
	HashTableBucket<K, V, HashFunc>& operator=(HashTableBucket<K, V, HashFunc> t);
	~HashTableBucket();
	bool Insert(const K& key, const V& value);
	HashTableNode<K, V> *Find(const K& key);
	bool Remove(const K& key);


	friend ostream& operator<< (ostream &os, HashTableBucket<K, V, HashFunc> &t)
	{
		t.PrintTables();
		return os;
	}
protected:
	size_t _GetNextPrime(size_t size);
	void _CheckExpand();
	void PrintTables();

protected:
	vector<Node*> _tables;
	size_t _size;
};

template<class K, class V, class HashFunc>
inline size_t HashTableBucket<K, V, HashFunc>::_GetNextPrime(size_t size)
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

template<class K, class V, class HashFunc>
inline void HashTableBucket<K, V, HashFunc>::_CheckExpand()
{
	if (_size >= (0.9 * _tables.size()))
	{
		_tables.resize(_GetNextPrime(_tables.size()));
	}
}

template<class K, class V, class HashFunc>
inline HashTableBucket<K, V, HashFunc>::HashTableBucket()
	:_size(0)
{
	_tables.resize(_GetNextPrime(_tables.size()));
}

template<class K, class V, class HashFunc>
inline HashTableBucket<K, V, HashFunc>::HashTableBucket(const HashTableBucket<K, V, HashFunc>& t)
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


template<class K, class V, class HashFunc>
inline bool HashTableBucket<K, V, HashFunc>::Insert(const K & key, const V & value)
{
	_CheckExpand();
	int index = HashFunc()(key) % _tables.size();
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

template<class K, class V, class HashFunc>
inline HashTableNode<K, V>* HashTableBucket<K, V, HashFunc>::Find(const K & key)
{
	int index = HashFunc()(key) % _tables.size();
	Node *cur = _tables[index];
	while (cur)
	{
		if (cur->_key == key)
		{
			return cur;
		}
		cur = cur->_next;
	}
}

template<class K, class V, class HashFunc>
inline bool HashTableBucket<K, V, HashFunc>::Remove(const K & key)
{
	int index = HashFunc()(key) % _tables.size();
	Node *cur = _tables[index];
	if (cur == NULL)
	{
		return false;
	}
	if (cur->_key == key)
	{
		_tables[index] = cur->_next;
		delete cur;
		cur = NULL;
	}
	while (cur && cur->_next)
	{
		if (cur->_next->_key == key)
		{
			Node *del = cur->_next;
			cur->_next = del->_next;
			delete del;
			del = NULL;
			_size--;
			return true;
		}
		cur = cur->_next;
	}
	return false;
}

template<class K, class V, class HashFunc>
inline void HashTableBucket<K, V, HashFunc>::PrintTables()
{
	for (size_t i = 0; i < _tables.size(); ++i)
	{
		Node *cur = _tables[i];
		cout << "第" << i << "行： ";
		while (cur)
		{
			cout << cur->_key << " :" << cur->_value << " -->";
			cur = cur->_next;
		}
		cout << "NULL" << endl;
	}
}


template<class K, class V, class HashFunc>
inline HashTableBucket<K, V, HashFunc>& HashTableBucket<K, V, HashFunc>::operator=(HashTableBucket<K, V, HashFunc> t)
{
	_tables.swap(t);
	swap(_size, t._size);
}

template<class K, class V, class HashFunc>
inline HashTableBucket<K, V, HashFunc>::~HashTableBucket()
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
