#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <string_view>
#include <vector>

namespace string_util {

//
// str_split
//

/**
 * source:    char* + len
 * delimiter: char* + len
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result>
str_split(const _Elem *str, size_t str_len, const _Elem *delimiter, size_t delimiter_len, size_t max = 0) {
  std::vector<_Result> r;
  if (delimiter_len == (_Elem)'\0')
    return r;
  const _Elem *p = str;
  for (size_t count = 0; p <= str + str_len && (max == 0 || count < max); ++count) {
    const _Elem *f = (max > 0 && count == max - 1)
                         ? str + str_len
                         : std::search(p, str + str_len, delimiter, delimiter + delimiter_len);
    r.push_back(_Result(p, f - p));
    p = f + delimiter_len;
  }
  return r;
}

/**
 * source:    char* + len
 * delimiter: null-terminated
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const _Elem *str, size_t str_len, const _Elem *delimiter, size_t max = 0) {
  return str_split(str, str_len, delimiter, std::basic_string_view<_Elem, _Traits>(delimiter).length(), max);
}

/**
 * source:    char* + len
 * delimiter: std::string
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const _Elem *str,
                                      size_t str_len,
                                      const std::basic_string<_Elem, _Traits, _Alloc> &delimiter,
                                      size_t max = 0) {
  return str_split(str, str_len, delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    char* + len
 * delimiter: std::string_view
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result>
str_split(const _Elem *str, size_t str_len, const std::basic_string_view<_Elem, _Traits> &delimiter, size_t max = 0) {
  return str_split(str, str_len, delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    char* + len
 * delimiter: char
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const _Elem *str, size_t str_len, const _Elem delimiter, size_t max = 0) {
  return str_split(str, str_len, &delimiter, 1, max);
}

/**
 * source:    null-terminated
 * delimiter: char* + len
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const _Elem *str, const _Elem *delimiter, size_t delimiter_len, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter, delimiter_len, max);
}

/**
 * source:    null-terminated
 * delimiter: null-terminated
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const _Elem *str, const _Elem *delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter,
                   std::basic_string_view<_Elem, _Traits>(delimiter).length(), max);
}

/**
 * source:    null-terminated
 * delimiter: std::string
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result>
str_split(const _Elem *str, const std::basic_string<_Elem, _Traits, _Alloc> &delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter.c_str(), delimiter.length(),
                   max);
}

/**
 * source:    null-terminated
 * delimiter: std::string_view
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result>
str_split(const _Elem *str, const std::basic_string_view<_Elem, _Traits> &delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter.c_str(), delimiter.length(),
                   max);
}

/**
 * source:    null-terminated
 * delimiter: char
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const _Elem *str, const _Elem delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), &delimiter, 1, max);
}

/**
 * source:    std::string
 * delimiter: char*+len
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                      const _Elem *delimiter,
                                      size_t delimiter_len,
                                      size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter, delimiter_len, max);
}

/**
 * source:    std::string
 * delimiter: null-terminated
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result>
str_split(const std::basic_string<_Elem, _Traits, _Alloc> &str, const _Elem *delimiter, size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter, std::basic_string_view<_Elem, _Traits>(delimiter).length(),
                   max);
}

/**
 * source:    std::string
 * delimiter: std::string
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                      const std::basic_string<_Elem, _Traits, _Alloc> &delimiter,
                                      size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    std::string
 * delimiter: std::string_view
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                      const std::basic_string_view<_Elem, _Traits> &delimiter,
                                      size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    std::string
 * delimiter: char
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result>
str_split(const std::basic_string<_Elem, _Traits, _Alloc> &str, const _Elem delimiter, size_t max = 0) {
  return str_split(str.c_str(), str.length(), &delimiter, 1, max);
}

/**
 * source:    std::string_view
 * delimiter: char*+len
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const std::basic_string_view<_Elem, _Traits> &str,
                                      const _Elem *delimiter,
                                      size_t delimiter_len,
                                      size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter, delimiter_len, max);
}

/**
 * source:    std::string_view
 * delimiter: null-terminated
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result>
str_split(const std::basic_string_view<_Elem, _Traits> &str, const _Elem *delimiter, size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter, std::basic_string_view<_Elem, _Traits>(delimiter).length(),
                   max);
}

/**
 * source:    std::string_view
 * delimiter: std::string
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const std::basic_string_view<_Elem, _Traits> &str,
                                      const std::basic_string<_Elem, _Traits, _Alloc> &delimiter,
                                      size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    std::string_view
 * delimiter: std::string_view
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Result = std::basic_string_view<_Elem, _Traits>>
inline std::vector<_Result> str_split(const std::basic_string_view<_Elem, _Traits> &str,
                                      const std::basic_string_view<_Elem, _Traits> &delimiter,
                                      size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter.data(), delimiter.length(), max);
}

//
// str_split_copy
//

/**
 * source:    char* + len
 * delimiter: char* + len
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, size_t str_len, const _Elem *delimiter, size_t delimiter_len, size_t max = 0) {
  return str_split(str, str_len, delimiter, delimiter_len, max);
}

/**
 * source:    char* + len
 * delimiter: null-terminated
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, size_t str_len, const _Elem *delimiter, size_t max = 0) {
  return str_split(str, str_len, delimiter, std::basic_string_view<_Elem, _Traits>(delimiter).length(), max);
}

/**
 * source:    char* + len
 * delimiter: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>> str_split_copy(
    const _Elem *str, size_t str_len, const std::basic_string<_Elem, _Traits, _Alloc> &delimiter, size_t max = 0) {
  return str_split(str, str_len, delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    char* + len
 * delimiter: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>> str_split_copy(
    const _Elem *str, size_t str_len, const std::basic_string_view<_Elem, _Traits> &delimiter, size_t max = 0) {
  return str_split(str, str_len, delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    char* + len
 * delimiter: char
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, size_t str_len, const _Elem delimiter, size_t max = 0) {
  return str_split(str, str_len, &delimiter, 1, max);
}

/**
 * source:    null-terminated
 * delimiter: char* + len
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, const _Elem *delimiter, size_t delimiter_len, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter, delimiter_len, max);
}

/**
 * source:    null-terminated
 * delimiter: null-terminated
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, const _Elem *delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter,
                   std::basic_string_view<_Elem, _Traits>(delimiter).length(), max);
}

/**
 * source:    null-terminated
 * delimiter: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, const std::basic_string<_Elem, _Traits, _Alloc> &delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter.c_str(), delimiter.length(),
                   max);
}

/**
 * source:    null-terminated
 * delimiter: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, const std::basic_string_view<_Elem, _Traits> &delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), delimiter.c_str(), delimiter.length(),
                   max);
}

/**
 * source:    null-terminated
 * delimiter: char
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const _Elem *str, const _Elem delimiter, size_t max = 0) {
  return str_split(str, std::basic_string_view<_Elem, _Traits>(str).length(), &delimiter, 1, max);
}

/**
 * source:    std::string
 * delimiter: char*+len
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string<_Elem, _Traits, _Alloc> &str,
               const _Elem *delimiter,
               size_t delimiter_len,
               size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter, delimiter_len, max);
}

/**
 * source:    std::string
 * delimiter: null-terminated
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string<_Elem, _Traits, _Alloc> &str, const _Elem *delimiter, size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter, std::basic_string_view<_Elem, _Traits>(delimiter).length(),
                   max);
}

/**
 * source:    std::string
 * delimiter: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string<_Elem, _Traits, _Alloc> &str,
               const std::basic_string<_Elem, _Traits, _Alloc> &delimiter,
               size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    std::string
 * delimiter: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string<_Elem, _Traits, _Alloc> &str,
               const std::basic_string_view<_Elem, _Traits> &delimiter,
               size_t max = 0) {
  return str_split(str.c_str(), str.length(), delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    std::string
 * delimiter: char
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string<_Elem, _Traits, _Alloc> &str, const _Elem delimiter, size_t max = 0) {
  return str_split(str.c_str(), str.length(), &delimiter, 1, max);
}

/**
 * source:    std::string_view
 * delimiter: char*+len
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>> str_split_copy(
    const std::basic_string_view<_Elem, _Traits> &str, const _Elem *delimiter, size_t delimiter_len, size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter, delimiter_len, max);
}

/**
 * source:    std::string_view
 * delimiter: null-terminated
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string_view<_Elem, _Traits> &str, const _Elem *delimiter, size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter, std::basic_string_view<_Elem, _Traits>(delimiter).length(),
                   max);
}

/**
 * source:    std::string_view
 * delimiter: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string_view<_Elem, _Traits> &str,
               const std::basic_string<_Elem, _Traits, _Alloc> &delimiter,
               size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter.c_str(), delimiter.length(), max);
}

/**
 * source:    std::string_view
 * delimiter: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::vector<std::basic_string<_Elem, _Traits, _Alloc>>
str_split_copy(const std::basic_string_view<_Elem, _Traits> &str,
               const std::basic_string_view<_Elem, _Traits> &delimiter,
               size_t max = 0) {
  return str_split(str.data(), str.length(), delimiter.data(), delimiter.length(), max);
}

//
// str_join
//

/**
 * list:      std::string_view
 * delimiter: char*+len
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Source = std::basic_string_view<_Elem, _Traits>>
inline std::basic_string<_Elem, _Traits, _Alloc>
str_join(const std::vector<_Source> &str_list, const _Elem *delimiter, size_t delimiter_len) {
  std::basic_string<_Elem, _Traits, _Alloc> ret;
  size_t total_len = 0;
  for (typename std::vector<_Source>::const_iterator it = str_list.begin(); it != str_list.end(); ++it) {
    if (it != str_list.begin())
      total_len += delimiter_len;
    total_len += it->length();
  }
  ret.reserve(total_len);
  for (typename std::vector<_Source>::const_iterator it = str_list.begin(); it != str_list.end(); ++it) {
    if (it != str_list.begin())
      ret.append(delimiter, delimiter_len);
    ret.append(it->data(), it->c_str() + it->length());
  }
  return ret;
}

/**
 * list:      std::string_view
 * delimiter: null-terminated
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Source = std::basic_string_view<_Elem, _Traits>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_join(const std::vector<_Source> &str_list,
                                                          const _Elem *delimiter) {
  return str_join(str_list, delimiter, std::basic_string_view<_Elem, _Traits>(delimiter).length());
}

/**
 * list:      std::string_view
 * delimiter: std::string
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Source = std::basic_string_view<_Elem, _Traits>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_join(const std::vector<_Source> &str_list,
                                                          const std::basic_string<_Elem, _Traits, _Alloc> &delimiter) {
  return str_join(str_list, delimiter.c_str(), delimiter.length());
}

/**
 * list:      std::string_view
 * delimiter: std::string_view
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Source = std::basic_string_view<_Elem, _Traits>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_join(const std::vector<_Source> &str_list,
                                                          const std::basic_string_view<_Elem, _Traits> &delimiter) {
  return str_join(str_list, delimiter.data(), delimiter.length());
}

/**
 * list:      std::string_view
 * delimiter: char
 */
template <class _Elem,
          class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>,
          class _Source = std::basic_string_view<_Elem, _Traits>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_join(const std::vector<_Source> &str_list, const _Elem delimiter) {
  return str_join(str_list, &delimiter, 1);
}

//
// str_replace_inplace
//

/**
 * source:  char*+len
 * find:    char*+len
 * replace: char*+len
 */
template <class _Elem>
inline _Elem *str_replace_inplace(_Elem *str,
                                  size_t str_len,
                                  const _Elem *find,
                                  size_t find_len,
                                  const _Elem *replace,
                                  size_t replace_len,
                                  size_t max = 0) {
  assert(find_len >= replace_len);
  if (find_len == 0)
    return str;
  _Elem *dest = str;
  const _Elem *src = str;
  for (size_t count = 0; src < str + str_len && (max == 0 || count < max); ++count) {
    const _Elem *f = std::search(src, (const _Elem *)str + str_len, find, find + find_len);
    if (find_len != replace_len)
      memcpy(dest, src, sizeof(_Elem) * (f - src));
    dest += (f - src);
    src = f;
    if (f < str + str_len) {
      memcpy(dest, replace, sizeof(_Elem) * replace_len);
      dest += replace_len;
      src += find_len;
    }
  }
  if (src < str + str_len) {
    if (find_len != replace_len)
      memcpy(dest, src, sizeof(_Elem) * (str + str_len - src));
    dest += (str + str_len - src);
  }
  *dest = (_Elem)'\0';
  return str;
}

/**
 * source:  char*+len
 * find:    null-terminated
 * replace: null-terminated
 */
template <class _Elem>
inline _Elem *str_replace_inplace(_Elem *str, size_t str_len, const _Elem *find, const _Elem *replace, size_t max = 0) {
  return str_replace_inplace(str, str_len, find, std::basic_string_view<_Elem>(find).length(), replace,
                             std::basic_string_view<_Elem>(replace).length(), max);
}

/**
 * source:  char*+len
 * find:    std::string
 * replace: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline _Elem *str_replace_inplace(_Elem *str,
                                  size_t str_len,
                                  const std::basic_string<_Elem, _Traits, _Alloc> &find,
                                  const std::basic_string<_Elem, _Traits, _Alloc> &replace,
                                  size_t max = 0) {
  return str_replace_inplace(str, str_len, find.c_str(), find.length(), replace.c_str(), replace.length(), max);
}

/**
 * source:  char*+len
 * find:    std::string_view
 * replace: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline _Elem *str_replace_inplace(_Elem *str,
                                  size_t str_len,
                                  const std::basic_string_view<_Elem, _Traits> &find,
                                  const std::basic_string_view<_Elem, _Traits> &replace,
                                  size_t max = 0) {
  return str_replace_inplace(str, str_len, find.data(), find.length(), replace.data(), replace.length(), max);
}

/**
 * source:  char*+len
 * find:    char
 * replace: char
 */
template <class _Elem>
inline _Elem *str_replace_inplace(_Elem *str, size_t str_len, const _Elem find, const _Elem replace, size_t max = 0) {
  return str_replace_inplace(str, str_len, &find, 1, &replace, 1, max);
}

/**
 * source:  std::string
 * find:    char*+len
 * replace: char*+len
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> &str_replace_inplace(std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                                      const _Elem *find,
                                                                      size_t find_len,
                                                                      const _Elem *replace,
                                                                      size_t replace_len,
                                                                      size_t max = 0) {
  assert(find_len >= replace_len);
  str_replace_inplace(&str[0], str.length(), find, find_len, replace, replace_len, max);
  str.resize(std::basic_string_view<_Elem, _Traits>(str.c_str()).length());
  return str;
}

/**
 * source:  std::string
 * find:    null-terminated
 * replace: null-terminated
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> &str_replace_inplace(std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                                      const _Elem *find,
                                                                      const _Elem *replace,
                                                                      size_t max = 0) {
  return str_replace_inplace(str, find, std::basic_string_view<_Elem, _Traits>(find).length(), replace,
                             std::basic_string_view<_Elem, _Traits>(replace).length(), max);
}

/**
 * source:  std::string
 * find:    std::string
 * replace: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> &
str_replace_inplace(std::basic_string<_Elem, _Traits, _Alloc> &str,
                    const std::basic_string<_Elem, _Traits, _Alloc> &find,
                    const std::basic_string<_Elem, _Traits, _Alloc> &replace,
                    size_t max = 0) {
  return str_replace_inplace(str, find.c_str(), find.length(), replace.c_str(), replace.length(), max);
}

/**
 * source:  std::string
 * find:    std::string_view
 * replace: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> &
str_replace_inplace(std::basic_string<_Elem, _Traits, _Alloc> &str,
                    const std::basic_string_view<_Elem, _Traits> &find,
                    const std::basic_string_view<_Elem, _Traits> &replace,
                    size_t max = 0) {
  return str_replace_inplace(str, find.data(), find.length(), replace.data(), replace.length(), max);
}

/**
 * source:  std::string
 * find:    char
 * replace: char
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> &str_replace_inplace(std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                                      const _Elem find,
                                                                      const _Elem replace,
                                                                      size_t max = 0) {
  return str_replace_inplace(str, &find, 1, &replace, 1, max);
}

//
// str_replace
//

/**
 * source:  char*+len
 * find:    char*+len
 * replace: char*+len
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const _Elem *str,
                                                             size_t str_len,
                                                             const _Elem *find,
                                                             size_t find_len,
                                                             const _Elem *replace,
                                                             size_t replace_len,
                                                             size_t max = 0) {
  std::basic_string<_Elem, _Traits, _Alloc> ret;
  if (find_len <= replace_len) {
    ret.assign(str, str_len);
    str_replace_inplace(&ret[0], str_len, find, find_len, replace, replace_len, max);
    ret.resize(std::basic_string_view<_Elem, _Traits>(ret.c_str()).length());
    return ret;
  } else {
    ret = str_join(str_split(str, str_len, find, find_len, max), replace, replace_len);
  }
  return ret;
}

/**
 * source:  char*+len
 * find:    null-terminated
 * replace: null-terminated
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc>
str_replace(const _Elem *str, size_t str_len, const _Elem *find, const _Elem *replace, size_t max = 0) {
  return str_replace(str, str_len, find, std::basic_string_view<_Elem, _Traits>(find).length(), replace,
                     std::basic_string_view<_Elem, _Traits>(replace).length(), max);
}

/**
 * source:  char*+len
 * find:    std::string
 * replace: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const _Elem *str,
                                                             size_t str_len,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &find,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &replace,
                                                             size_t max = 0) {
  return str_replace(str, str_len, find.c_str(), find.length(), replace.c_str(), replace.length(), max);
}

/**
 * source:  char*+len
 * find:    std::string_view
 * replace: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const _Elem *str,
                                                             size_t str_len,
                                                             const std::basic_string_view<_Elem, _Traits> &find,
                                                             const std::basic_string_view<_Elem, _Traits> &replace,
                                                             size_t max = 0) {
  return str_replace(str, str_len, find.data(), find.length(), replace.data(), replace.length(), max);
}

/**
 * source:  char*+len
 * find:    char
 * replace: char
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc>
str_replace(const _Elem *str, size_t str_len, const _Elem find, const _Elem replace, size_t max = 0) {
  return str_replace(str, str_len, &find, 1, &replace, 1, max);
}

/**
 * source:  null-terminated
 * find:    char*+len
 * replace: char*+len
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(
    const _Elem *str, const _Elem *find, size_t find_len, const _Elem *replace, size_t replace_len, size_t max = 0) {
  return str_replace(str, std::basic_string_view<_Elem, _Traits>(str).length(), find, find_len, replace, replace_len,
                     max);
}

/**
 * source:  null-terminated
 * find:    null-terminated
 * replace: null-terminated
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc>
str_replace(const _Elem *str, const _Elem *find, const _Elem *replace, size_t max = 0) {
  return str_replace(str, std::basic_string_view<_Elem, _Traits>(str).length(), find,
                     std::basic_string_view<_Elem, _Traits>(find).length(), replace,
                     std::basic_string_view<_Elem, _Traits>(replace).length(), max);
}

/**
 * source:  null-terminated
 * find:    std::string
 * replace: std::string
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const _Elem *str,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &find,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &replace,
                                                             size_t max = 0) {
  return str_replace(str, std::basic_string_view<_Elem, _Traits>(str).length(), find.data(), find.length(),
                     replace.data(), replace.length(), max);
}

/**
 * source:  null-terminated
 * find:    std::string_view
 * replace: std::string_view
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const _Elem *str,
                                                             const std::basic_string_view<_Elem, _Traits> &find,
                                                             const std::basic_string_view<_Elem, _Traits> &replace,
                                                             size_t max = 0) {
  return str_replace(str, std::basic_string_view<_Elem, _Traits>(str).length(), find.data(), find.length(),
                     replace.data(), replace.length(), max);
}

/**
 * source:  null-terminated
 * find:    char
 * replace: char
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc>
str_replace(const _Elem *str, const _Elem find, const _Elem replace, size_t max = 0) {
  return str_replace(str, std::basic_string_view<_Elem, _Traits>(str).length(), &find, 1, &replace, 1, max);
}

/**
 * source:  std::string
 * find:    char*+len
 * replace: char*+len
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                             const _Elem *find,
                                                             size_t find_len,
                                                             const _Elem *replace,
                                                             size_t replace_len,
                                                             size_t max = 0) {
  return str_replace(str.c_str(), str.length(), find, find_len, replace, replace_len, max);
}

/**
 * source:  std::string
 * find:    null-terminated
 * replace: null-terminated
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                             const _Elem *find,
                                                             const _Elem *replace,
                                                             size_t max = 0) {
  return str_replace(str, find, std::basic_string_view<_Elem, _Traits>(find).length(), replace,
                     std::basic_string_view<_Elem, _Traits>(replace).length(), max);
}

/**
 * source:  std::string
 * find:    std::string
 * replace: std::string
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &find,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &replace,
                                                             size_t max = 0) {
  return str_replace(str, find.c_str(), find.length(), replace.c_str(), replace.length(), max);
}

/**
 * source:  std::string
 * find:    std::string_view
 * replace: std::string_view
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                             const std::basic_string_view<_Elem, _Traits> &find,
                                                             const std::basic_string_view<_Elem, _Traits> &replace,
                                                             size_t max = 0) {
  return str_replace(str, find.c_str(), find.length(), replace.c_str(), replace.length(), max);
}

/**
 * source:  std::string
 * find:    char
 * replace: char
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string<_Elem, _Traits, _Alloc> &str,
                                                             const _Elem find,
                                                             const _Elem replace,
                                                             size_t max = 0) {
  return str_replace(str, &find, 1, &replace, 1, max);
}

/**
 * source:  std::string_view
 * find:    char*+len
 * replace: char*+len
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string_view<_Elem, _Traits> &str,
                                                             const _Elem *find,
                                                             size_t find_len,
                                                             const _Elem *replace,
                                                             size_t replace_len,
                                                             size_t max = 0) {
  return str_replace(str.c_str(), str.length(), find, find_len, replace, replace_len, max);
}

/**
 * source:  std::string_view
 * find:    null-terminated
 * replace: null-terminated
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string_view<_Elem, _Traits> &str,
                                                             const _Elem *find,
                                                             const _Elem *replace,
                                                             size_t max = 0) {
  return str_replace(str, find, std::basic_string_view<_Elem, _Traits>(find).length(), replace,
                     std::basic_string_view<_Elem, _Traits>(replace).length(), max);
}

/**
 * source:  std::string_view
 * find:    std::string
 * replace: std::string
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string_view<_Elem, _Traits> &str,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &find,
                                                             const std::basic_string<_Elem, _Traits, _Alloc> &replace,
                                                             size_t max = 0) {
  return str_replace(str, find.c_str(), find.length(), replace.c_str(), replace.length(), max);
}

/**
 * source:  std::string_view
 * find:    std::string_view
 * replace: std::string_view
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc> str_replace(const std::basic_string_view<_Elem, _Traits> &str,
                                                             const std::basic_string_view<_Elem, _Traits> &find,
                                                             const std::basic_string_view<_Elem, _Traits> &replace,
                                                             size_t max = 0) {
  return str_replace(str.data(), str.length(), find.data(), find.length(), replace.data(), replace.length(), max);
}

/**
 * source:  std::string_view
 * find:    char
 * replace: char
 */
template <class _Elem, class _Traits, class _Alloc>
inline std::basic_string<_Elem, _Traits, _Alloc>
str_replace(const std::basic_string_view<_Elem, _Traits> &str, const _Elem find, const _Elem replace, size_t max = 0) {
  return str_replace(str.data(), str.length(), &find, 1, &replace, 1, max);
}

} // namespace string_util
