// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_ENVIRONMENT_HPP
#define BEAKER_ENVIRONMENT_HPP

#include <cassert>
#include <unordered_map>
#include <vector>


// The environment maintains all active bindings at
// a certain point in the program.
template<typename S, typename T>
class Environment : std::unordered_map<S, T>
{
private:
  using Map = std::unordered_map<S, T>;
public:
  using Name = S;
  using Value = T;
  using Binding = typename Map::value_type;

  Binding const& bind(S const&, T const&);
  Binding const& get(S const&) const;
  Binding const* lookup(S const&) const;
};


// Create a new name binding for the given entity. Behavior 
// is undefined if a the symbol is already bound in this 
// environment.
template<typename S, typename T>
inline auto
Environment<S, T>::bind(S const& sym, T const& ent) -> Binding const& 
{
  assert(!this->count(sym));
  auto ins = this->emplace(sym, ent);
  return *ins.first;
}


// Returns the entity bound to the given name. Behavior is
// undefined if there is no binding for the symbol.
template<typename S, typename T>
inline auto
Environment<S, T>::get(S const& sym) const -> Binding const&
{
  assert(this->count(sym));
  auto iter = this->find(sym);
  return *iter;
}


// Returns the entity bound to the given symbol.
template<typename S, typename T>
inline auto
Environment<S, T>::lookup(S const& sym) const -> Binding const*
{
  auto iter = this->find(sym);
  if (iter == this->end())
    return nullptr;
  else
    return &*iter;
}


// The stack maintains the nested binding environments at
// certain point in the program. Symbol lookup is processed
// in the innermost environment, and works outward.
template<typename E>
struct Stack : std::vector<E>
{
  using Environment = E;
  using Name = typename E::Name;
  using Value = typename E::Value;
  using Binding = typename E::Binding;
  
  template<typename... Args>
  void push(Args&&...);
  
  void pop();

  E&       top()       { return this->back(); }
  E const& top() const { return this->back(); }

  E&       bottom()       { return this->front(); }
  E const& bottom() const { return this->front(); }

  Binding const* lookup(Name const&) const;
};


template<typename E>
template<typename... Args>
inline void 
Stack<E>::push(Args&&... args)
{ 
  this->push_back(E(std::forward<Args>(args)...)); 
}


template<typename E>
inline void 
Stack<E>::pop()
{ 
  this->pop_back(); 
}


template<typename E>
auto
Stack<E>::lookup(Name const& n) const -> Binding const*
{
  for (auto iter = this->rbegin(); iter != this->rend(); ++iter) {
    if (Binding const* bind = iter->lookup(n))
      return bind;
  }
  return nullptr;
}


#endif
