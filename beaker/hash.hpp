// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_HASH_HPP
#define BEAKER_HASH_HPP

#include <beaker/prelude.hpp>
#include <beaker/equal.hpp>

#include <boost/functional/hash.hpp>

#include <unordered_set>
#include <unordered_map>


// FIXME: This is broken. The use of boost::hash_combine
// does not call the appropriate overloads for pointers
// to AST objects, so equivalent trees produces non-equal
// hashes.


std::size_t hash_value(Type const*);


// A hash function object.
template<typename T>
struct Hash_fn
{
  std::size_t operator()(T const* t) const { return hash_value(t); }
};


template<typename K, typename V>
using Hash_map = std::unordered_map<K, V, Hash_fn<K>, Equal_fn<K>>;


template<typename T>
using Hash_set = std::unordered_set<T, Hash_fn<T>, Equal_fn<T>>;


#endif
