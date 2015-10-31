
#include "type.hpp"

namespace cxx
{

namespace
{

// C standard ints
// The type 'uint8_t'
Uint8_t uint8_;

// The type 'uint16_t'
Uint16_t uint16_;

// The type 'uint32_t'
Uint32_t uint32_;

// The type 'uint64_t'
Uint64_t uint64_;

// The type 'uint128_t'
Uint128_t uint128_;

// The type 'uint24_t'
Uint24_t uint24_;

// The type 'uint48_t'
Uint48_t uint48_;

Int_type int_;

}

Type* get_uint8_type()
{
  return &uint8_;
}

Type* get_uint16_type()
{
  return &uint16_;
}


Type* get_uint32_type()
{
  return &uint32_;
}


Type* get_uint64_type()
{
  return &uint64_;
}


Type* get_uint128_type()
{
  return &uint128_;
}


Type* get_uint24_type()
{
  return &uint24_;
}


Type* get_uint48_type()
{
  return &uint48_;
}


// Fundametal Types
Type* get_int_type()
{
  return &int_;
}


// FIXME: Implement check for declared but not defined classes.
bool
is_incomplete_object_type(Type* t) {
  return is<Incomplete_array_type>(t);
}

bool
is_incomplete_type(Type* t) {
  return is_incomplete_object_type(t) or is<Void_type>(t);
}

} // namespace cxx
