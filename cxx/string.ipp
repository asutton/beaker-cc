
namespace cxx
{


// Equality comparison
// Returns true when two strings refer to the same object.

inline bool
operator==(String a, String b) { return a.ptr() == b.ptr(); }

inline bool
operator!=(String a, String b) { return a.ptr() != b.ptr(); }

// Ordering
// Returns true when a points to an object with a lesser address
// than b. This operation does not define a lexicographical order.

inline bool
operator<(String a, String b) { return a.ptr() < b.ptr(); }

inline bool
operator>(String a, String b) { return a.ptr() > b.ptr(); }

inline bool
operator<=(String a, String b) { return a.ptr() <= b.ptr(); }

inline bool
operator>=(String a, String b) { return a.ptr() >= b.ptr(); }

// Formatting
template<typename C> 
  inline fmt::StrFormatSpec<C> 
  pad(const String& s, unsigned w, C f) {
    return fmt::StrFormatSpec<C>(s.data(), w, f);
  }

// Streaming
template<typename C, typename T>
  inline std::basic_ostream<C, T>&
  operator<<(std::basic_ostream<C, T>& os, String s) { return os << s.str(); }

} // namespace cxx

namespace std {

inline std::size_t 
hash<cxx::String>::operator()(cxx::String str) const {
  hash<const void*> h;
  return h(str.ptr());
}

} // namespace std
