
#ifndef TYPE_HPP
#define TYPE_HPP

#include "ast.hpp"

namespace cxx
{

// Narrow character types
constexpr Node_kind char_type                   = make_type_node(1100);
constexpr Node_kind signed_char_type            = make_type_node(1101);
constexpr Node_kind unsigned_char_type          = make_type_node(1102);
// Wide character types
constexpr Node_kind wchar_type                  = make_type_node(1103);
constexpr Node_kind char16_type                 = make_type_node(1104);
constexpr Node_kind char32_type                 = make_type_node(1105);
// Signed integer types
constexpr Node_kind short_int_type              = make_type_node(1106);
constexpr Node_kind int_type                    = make_type_node(1107);
constexpr Node_kind long_int_type               = make_type_node(1108);
constexpr Node_kind long_long_int_type          = make_type_node(1109);
// Unsigned integer type
constexpr Node_kind unsigned_short_int_type     = make_type_node(1110);
constexpr Node_kind unsigned_int_type           = make_type_node(1111);
constexpr Node_kind unsigned_long_int_type      = make_type_node(1112);
constexpr Node_kind unsigned_long_long_int_type = make_type_node(1113);
// Other integer types
constexpr Node_kind bool_type                   = make_type_node(1114);
// Floating point types
constexpr Node_kind float_type                  = make_type_node(1115);
constexpr Node_kind double_type                 = make_type_node(1116);
constexpr Node_kind long_double_type            = make_type_node(1117);
// Void type
constexpr Node_kind void_type                   = make_type_node(1118);
// Null pointer type
constexpr Node_kind nullptr_type                = make_type_node(1119);
// Miscellaneous types
constexpr Node_kind default_type                = make_type_node(1120);
constexpr Node_kind unknown_type                = make_type_node(1121);
// Compound types
constexpr Node_kind pointer_type                = make_type_node(1123); // T*
constexpr Node_kind lvalue_reference_type       = make_type_node(1124); // T&
constexpr Node_kind rvalue_reference_type       = make_type_node(1125); // T&&
constexpr Node_kind member_pointer_type         = make_type_node(1104); // T (C::*)
constexpr Node_kind array_type                  = make_type_node(1109); // T[N]
constexpr Node_kind incomplete_array_type       = make_type_node(1110); // T[]
constexpr Node_kind function_type               = make_type_node(1111); // Tr (T1, T2, ..., Tn)
constexpr Node_kind class_type                  = make_type_node(1112); // [class|struct] {...}
constexpr Node_kind union_type                  = make_type_node(1114); // union {...}
constexpr Node_kind enum_type                   = make_type_node(1115); // enum {...}
constexpr Node_kind typename_type               = make_type_node(1117); // type template parameter
constexpr Node_kind auto_type                   = make_type_node(1118); // auto
constexpr Node_kind decltype_auto_type          = make_type_node(1119); // decltype(auto)
constexpr Node_kind decltype_type               = make_type_node(1120); // decltype(expr)
constexpr Node_kind base_type                   = make_type_node(1130); // base class
constexpr Node_kind namespace_type              = make_type_node(1140); // namespace {...}
constexpr Node_kind id_type                     = make_type_node(1141); // unresolved type
// C standard integers
constexpr Node_kind uint8_type                  = make_type_node(1142);
constexpr Node_kind uint16_type                 = make_type_node(1143);
constexpr Node_kind uint32_type                 = make_type_node(1144);
constexpr Node_kind uint64_type                 = make_type_node(1145);
constexpr Node_kind uint128_type                = make_type_node(1146);
// special non standard ints that will be needed for packets
constexpr Node_kind uint24_type                 = make_type_node(1147);
constexpr Node_kind uint48_type                 = make_type_node(1148);


// -------------------------------------------------------------------------- //
// Types                                                           [basic.types]

// -------------------------------------------------------------------------- //
// Fundamental types                                         [basic.fundamental]

// The basic type class is a helper for generating definitions 
// of fundamental types.
template<Node_kind K>
  struct Basic_type : Type, Kind_of<K> {
    Basic_type()
      : Type(K) { }
    Basic_type(Decl_spec s)
      : Type(K, s) { }
  };

// The type 'char'.
struct Char_type : Basic_type<char_type> { };

// The type 'signed char'.
struct Signed_char_type : Basic_type<signed_char_type> { };

// The type 'unsigned char'
struct Unsigned_char_type : Basic_type<unsigned_char_type> { };

// The type 'wchar_t'.
struct Wchar_type : Basic_type<wchar_type> { };

// The type 'char16_t'.
struct Char16_type : Basic_type<char16_type> { };

// The type 'char32_t'.
struct Char32_type : Basic_type<char32_type> { };

// The type 'short int'.
struct Short_int_type : Basic_type<short_int_type> { };

// The type 'int'.
struct Int_type : Basic_type<int_type> { };

// The type 'long int'.
struct Long_int_type : Basic_type<long_int_type> { };

// The type 'long long int'.
struct Long_long_int_type : Basic_type<long_long_int_type> { };

// The type 'unsigned short int'.
struct Unsigned_short_int_type : Basic_type<unsigned_short_int_type> { };

// The type 'unsigned int'.
struct Unsigned_int_type : Basic_type<unsigned_int_type> { };

// The type 'unsigned long int'.
struct Unsigned_long_int_type : Basic_type<unsigned_long_int_type> { };

// The type 'unsigned long long int'.
struct Unsigned_long_long_int_type : Basic_type<unsigned_long_long_int_type> { };

// The type 'uint8_t'
struct Uint8_t : Basic_type<uint8_type> { };

// The type 'uint16_t'
struct Uint16_t : Basic_type<uint16_type> { };

// The type 'uint32_t'
struct Uint32_t : Basic_type<uint32_type> { };

// The type 'uint64_t'
struct Uint64_t : Basic_type<uint64_type> { };

// The type 'uint128_t'
struct Uint128_t : Basic_type<uint128_type> { };

// The type 'uint24_t'
struct Uint24_t : Basic_type<uint24_type> { };

// The type 'uint48_t'
struct Uint48_t : Basic_type<uint48_type> { };

// The type 'bool'.
struct Bool_type : Basic_type<bool_type> { };

// The type 'void'.
struct Void_type : Basic_type<void_type> { };

// The type 'nullptr_t'.
struct Nullptr_type : Basic_type<nullptr_type> { };

// The type 'float'.
struct Float_type : Basic_type<float_type> { };

// The type 'double'.
struct Double_type : Basic_type<double_type> { };

// The type 'long double'.
struct Long_double_type : Basic_type<long_double_type> { };

// The type of the 'default' expression. This is not a standard
// C++ type, but used in this representation.
struct Default_type : Basic_type<default_type> { };

// Certain expressions in C++ have no type, e.g., a braced 
// initializer list.
struct Unknown_type : Basic_type<unknown_type> { };

bool is_incompletely_defined_object_type(const Type*);
bool is_incomplete(const Type*);

// -------------------------------------------------------------------------- //
// Placeholders                                                  [dcl.spec.auto]
//
// The type specifiers include placeholder types auto, decltype(auto), 
// and decltype(e).

// The auto type specifier.
struct Auto_type : Type, Kind_of<auto_type> {
  Auto_type()
    : Type(Kind) { }
  Auto_type(Decl_spec s)
    : Type(Kind, s) { }
};

// The decltype(auto) specifier.
struct Decltype_auto_type : Type, Kind_of<decltype_auto_type> {
  Decltype_auto_type()
    : Type(Kind) { }
  Decltype_auto_type(Decl_spec s)
    : Type(Kind, s) { }
};

// The declared type of an expression 'decltype(e)'.
struct Decltype_type : Type, Kind_of<decltype_type> {
  Decltype_type(Expr* e)
    : Type(Kind), first(e) { }
  Decltype_type(Decl_spec s, Expr* e)
    : Type(Kind, s), first(e) { }
  
  Expr* expr() const { return first; }

  Expr* first;
};


// -------------------------------------------------------------------------- //
// Compound types                                          [basic.type.compound]

// A pointer type has the form 'T*' where 'T' is a pointee type.
struct Pointer_type : Type, Kind_of<pointer_type> {
  Pointer_type(Type* t)
    : Type(Kind), first(t) { }
  Pointer_type(Decl_spec s, Type* t)
    : Type(Kind, s), first(t) { }

  Type* pointee() const { return first; }

  Type* first;
};

// An lvalue reference type has the form 'T&' where 'T' is
// the referent type.
struct Lvalue_reference_type : Type, Kind_of<lvalue_reference_type> {
  Lvalue_reference_type(Type* t)
    : Type(Kind), first(t) { }
  Lvalue_reference_type(Decl_spec s, Type* t)
    : Type(Kind, s), first(t) { }

  Type* referent() const { return first; }

  Type* first;
};

// An rvalue reference type has the form 'T&&' where 'T' is
// the referent type.
struct Rvalue_reference_type : Type, Kind_of<rvalue_reference_type> {
  Rvalue_reference_type(Type* t)
    : Type(Kind), first(t) { }
  Rvalue_reference_type(Decl_spec s, Type* t)
    : Type(Kind, s), first(t) { }

  Type* referent() const { return first; }

  Type* first;
};

// An array of known bound type has the form 'T[n]' where 'T'
// is an object type and 'n' is an integral constant expression
// whose value is greater than or equal to 0.
struct Array_type : Type, Kind_of<array_type> {
  Array_type(Type* t, Expr* n)
    : Type(Kind), first(t), second(n) { }
  Array_type(Decl_spec s, Type* t, Expr* n)
    : Type(Kind, s), first(t), second(n) { }

  Type* type() const { return first; }
  Expr* bound() const { return second; }

  Type* first;
  Expr* second;
};

// An array of unknown bound type has the form 'T[]' where
// 'T' is an object type.
struct Incomplete_array_type : Type, Kind_of<incomplete_array_type> {
  Incomplete_array_type(Type* t)
    : Type(Kind), first(t) { }
  Incomplete_array_type(Decl_spec s, Type* t)
    : Type(Kind, s), first(t) { }

  Type* type() const { return first; }

  Type* first;
};

// A function type has the form 'T(T1, T2, ..., Tn)' where
// 'T' is the return type and each 'Ti' is a parameter type.
struct Function_type : Type, Kind_of<function_type> {
  Function_type(Type* r, Type_seq* p)
    : Type(Kind), first(r), second(p) { }
  Function_type(Decl_spec s, Type* r, Type_seq* p)
    : Type(Kind, s), first(r), second(p) { }

  Type* result() const { return first; }
  Type_seq* parms() const { return second; }

  Type* first;
  Type_seq* second;
};


// Represents the type of a class or struct of the form 
// 'S n : b { m }' where 's' is the class or struct type specifier,
// 'n' is the name of the class, 'b' is a sequence of base types, 
// and 'm' is a sequence of member declaration.
struct Class_type : Type, Kind_of<class_type> {
  Class_type(Name* n, Type_seq b, Decl_seq m)
    : Type(Kind, class_type), first(n), second(b), third(m) { }

  // TODO: The declaration specifiers must include either 
  // struct_spec or class_spec, but not both.
  Class_type(Decl_spec s, Name* n, Type_seq b, Decl_seq m)
    : Type(Kind, s), first(n), second(b), third(m) { }

  Name* name() const { return first; }
  Type_seq bases() const { return second; }
  Decl_seq members() const { return third; }

  Name* first;
  Type_seq second;
  Decl_seq third;
};

// Represents a union type has the form 'union n { m }' where 'n'
// is the union name and 'm' is a sequence of union members.
struct Union_type : Type, Kind_of<union_type> {
  Union_type(Name* n, Decl_seq* m)
    : Type(Kind, union_spec), first(n), second(m) { }

  // TODO: Verify that the declaration specifier.
  Union_type(Decl_spec s, Name* n, Decl_seq* m)
    : Type(Kind, s | union_spec), first(n), second(m) { }

  Name* name() const { return first; }
  Decl_seq* members() const { return second; }

  Name* first;
  Decl_seq* second;
};

// Represents an enumeration type has the form 'enum n : u { e }'
// where 'n' is its name, 'u' is the underlying type, and 'e'
// is a sequence of enumerators.
//
// When the underlying type is not explicitly given, it is the
// same as 'int'.
struct Enum_type : Type, Kind_of<enum_type> {
  Enum_type(Name* n, Type* u, Decl_seq* e)
    : Type(Kind), first(n), second(u), third(e) { }
  Enum_type(Decl_spec s, Name* n, Type* u, Decl_seq* e)
    : Type(Kind, s | enum_spec), first(n), second(u), third(e) { }

  Name* name() const { return first; }
  Type* base() const { return second; }
  Decl_seq* enums() const { return third; }

  Name* first;
  Type* second;
  Decl_seq* third;
};

// A pointer to a non-static member has the form 'T C::*' where
// 'T' is type of the member and 'C' is the enclosing class.
struct Member_pointer_type : Type, Kind_of<member_pointer_type> {
  Member_pointer_type(Type* t, Type* c)
    : Type(Kind), first(t), second(c) { }
  Member_pointer_type(Decl_spec s, Type* t, Type* c)
    : Type(Kind, s), first(t), second(c) { }

  Type* pointee() const { return first; }
  Type* scope() const { return second; }

  Type* first;
  Type* second;
};

// ------------------------------------------------------------------- //
//            Type construction

// TODO: Implement the rest of these

Type* get_uint8_type();
Type* get_uint16_type();
Type* get_uint32_type();
Type* get_uint64_type();
Type* get_uint128_type();
// special integer lengths
Type* get_uint24_type();
Type* get_uint48_type();

// Fundamental types
Type* get_int_type();
Type* get_void_type();
Type* get_bool_type();
Type* get_char_type();


// // The type of a type template parameter.
// struct Typename_type : Type {
//   Typename_type(Name*);
//   Name* name;
// };

// // A namespace.
// struct Namespace_type : Type {
//   Namespace_type(Name*, Seq<Decl>*);
//   Name* name;
//   Seq<Decl>* decls;
// };

} // namespace cxx

#endif
