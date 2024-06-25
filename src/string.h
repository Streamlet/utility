#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

#if __cplusplus >= 201703L
#include <string_view>
#endif

namespace xl {

namespace string {

size_t length(const char *s) {
  return strlen(s);
}

size_t length(const wchar_t *s) {
  return wcslen(s);
}

} // namespace string

template <typename CharType>
class basic_string_ref {
public:
  basic_string_ref() : data_(nullptr), length_(0) {
  }
  basic_string_ref(const CharType *data, size_t length) : data_(data), length_(length) {
  }
  basic_string_ref(const CharType *data) : data_(data), length_(string::length(data)) {
  }
  const CharType *c_str() const {
    return data_;
  }
  size_t length() const {
    return length_;
  }
  bool operator==(const basic_string_ref &that) const {
    return this->length_ == that.length_ && memcmp(this->data_, that.data_, length_) == 0;
  }
  bool operator!=(const basic_string_ref &that) const {
    return this->length_ != that.length_ || memcmp(this->data_, that.data_, length_) != 0;
  }
  operator std::basic_string<CharType>() const {
    return std::basic_string<CharType>(data_, length_);
  }
#if __cplusplus >= 201703L
  operator std::basic_string_view<CharType>() const {
    return std::basic_string_view<CharType>(data_, length_);
  }
#endif
private:
  const CharType *data_;
  size_t length_;
};

using string_ref = basic_string_ref<char>;
using wstring_ref = basic_string_ref<wchar_t>;

namespace string {
////////////////////////////////////////////////////////////////////////////////
// Split

/**
 * @brief Split string
 *
 * @param string string to be splitted
 * @param string_length -1 indicates str is null-terminated
 * @param separator string to be used to split by
 * @param separator_length -1 indicates separator is null-terminated
 * @param max max parts to return, 0 indicates unlimited
 */
template <typename CharType, typename ResultType>
inline std::vector<ResultType> split_t(const CharType *string,
                                       const CharType *separator,
                                       size_t max = 0,
                                       size_t string_length = -1,
                                       size_t separator_length = -1) {
  if (string_length == -1) {
    string_length = length(string);
  }
  if (separator_length == -1) {
    separator_length = length(separator);
  }
  if (string_length == 0 || separator_length == 0) {
    return {ResultType(string, string_length)};
  }
  std::vector<ResultType> string_list;
  const CharType *p = string;
  for (size_t count = 0; p <= string + string_length && (max == 0 || count < max); ++count) {
    const CharType *s = (max > 0 && count >= max - 1)
                            ? string + string_length
                            : std::search(p, string + string_length, separator, separator + separator_length);
    string_list.push_back(ResultType(p, s - p));
    p = s + separator_length;
  }
  return string_list;
}

template <typename CharType, typename ResultType>
inline std::vector<ResultType>
split_t(const CharType *string, CharType separator, size_t max = 0, size_t string_length = -1) {
  return split_t<CharType, ResultType>(string, &separator, max, string_length, 1);
}

template <typename CharType, typename ResultType>
inline std::vector<ResultType> split_t(const CharType *string,
                                       const std::basic_string<CharType> &separator,
                                       size_t max = 0,
                                       size_t string_length = -1) {
  return split_t<CharType, ResultType>(string, separator.c_str(), max, string_length, separator.length());
}

template <typename CharType, typename ResultType>
inline std::vector<ResultType> split_t(const std::basic_string<CharType> &string,
                                       const CharType *separator,
                                       size_t max = 0,
                                       size_t separator_length = -1) {
  return split_t<CharType, ResultType>(string.c_str(), separator, max, string.length(), separator_length);
}

template <typename CharType, typename ResultType>
inline std::vector<ResultType> split_t(const std::basic_string<CharType> &string, CharType separator, size_t max = 0) {
  return split_t<CharType, ResultType>(string.c_str(), &separator, max, string.length(), 1);
}

template <typename CharType, typename ResultType>
inline std::vector<ResultType>
split_t(const std::basic_string<CharType> &string, const std::basic_string<CharType> separator, size_t max = 0) {
  return split_t<CharType, ResultType>(string.c_str(), separator.c_str(), max, string.length(), separator.length());
}

template <typename CharType>
inline std::vector<basic_string_ref<CharType>> split_ref(const CharType *string,
                                                         const CharType *separator,
                                                         size_t max = 0,
                                                         size_t string_length = -1,
                                                         size_t separator_length = -1) {
  return split_t<CharType, basic_string_ref<CharType>>(string, separator, max, string_length, separator_length);
}

template <typename CharType>
inline std::vector<basic_string_ref<CharType>>
split_ref(const CharType *string, const CharType separator, size_t max = 0, size_t string_length = -1) {
  return split_t<CharType, basic_string_ref<CharType>>(string, separator, max, string_length);
}

template <typename CharType>
inline std::vector<basic_string_ref<CharType>> split_ref(const CharType *string,
                                                         const std::basic_string<CharType> &separator,
                                                         size_t max = 0,
                                                         size_t string_length = -1) {
  return split_t<CharType, basic_string_ref<CharType>>(string, separator, max, string_length);
}

template <typename CharType>
inline std::vector<basic_string_ref<CharType>> split_ref(const std::basic_string<CharType> &string,
                                                         const CharType *separator,
                                                         size_t max = 0,
                                                         size_t separator_length = -1) {
  return split_t<CharType, basic_string_ref<CharType>>(string, separator, max, separator_length);
}

template <typename CharType>
inline std::vector<basic_string_ref<CharType>>
split_ref(const std::basic_string<CharType> &string, const CharType separator, size_t max = 0) {
  return split_t<CharType, basic_string_ref<CharType>>(string, &separator, max);
}

template <typename CharType>
inline std::vector<basic_string_ref<CharType>>
split_ref(const std::basic_string<CharType> &string, const std::basic_string<CharType> separator, size_t max = 0) {
  return split_t<CharType, basic_string_ref<CharType>>(string, separator, max);
}

template <typename CharType>
inline std::vector<std::basic_string<CharType>> split(const CharType *string,
                                                      const CharType *separator,
                                                      size_t max = 0,
                                                      size_t string_length = -1,
                                                      size_t separator_length = -1) {
  return split_t<CharType, std::basic_string<CharType>>(string, separator, max, string_length, separator_length);
}

template <typename CharType>
inline std::vector<std::basic_string<CharType>>
split(const CharType *string, const CharType separator, size_t max = 0, size_t string_length = -1) {
  return split_t<CharType, std::basic_string<CharType>>(string, separator, max, string_length);
}

template <typename CharType>
inline std::vector<std::basic_string<CharType>>
split(const CharType *string, const std::basic_string<CharType> &separator, size_t max = 0, size_t string_length = -1) {
  return split_t<CharType, std::basic_string<CharType>>(string, separator, max, string_length);
}

template <typename CharType>
inline std::vector<std::basic_string<CharType>> split(const std::basic_string<CharType> &string,
                                                      const CharType *separator,
                                                      size_t max = 0,
                                                      size_t separator_length = -1) {
  return split_t<CharType, std::basic_string<CharType>>(string, separator, max, separator_length);
}

template <typename CharType>
inline std::vector<std::basic_string<CharType>>
split(const std::basic_string<CharType> &string, const CharType separator, size_t max = 0) {
  return split_t<CharType, std::basic_string<CharType>>(string, &separator, max);
}

template <typename CharType>
inline std::vector<std::basic_string<CharType>>
split(const std::basic_string<CharType> &string, const std::basic_string<CharType> separator, size_t max = 0) {
  return split_t<CharType, std::basic_string<CharType>>(string, separator, max);
}

////////////////////////////////////////////////////////////////////////////////
// Join

/**
 * @brief Join string list
 *
 * @param string_list string list to join
 * @param separator string to join by
 * @param separator_length  -1 indicates separator is null-terminated
 * @return std::basic_string<CharType>
 */
template <typename CharType, typename ElementType>
inline std::basic_string<CharType>
join(const std::vector<ElementType> &string_list, const CharType *separator, size_t separator_length = -1) {
  if (separator_length == -1) {
    separator_length = length(separator);
  }
  std::basic_string<CharType> string;
  size_t total_length = 0;
  for (typename std::vector<ElementType>::const_iterator it = string_list.begin(); it != string_list.end(); ++it) {
    if (it != string_list.begin()) {
      total_length += separator_length;
    }
    total_length += it->length();
  }
  string.reserve(total_length);
  for (typename std::vector<ElementType>::const_iterator it = string_list.begin(); it != string_list.end(); ++it) {
    if (it != string_list.begin()) {
      string.append(separator, separator_length);
    }
    string.append(it->c_str(), it->c_str() + it->length());
  }
  return string;
}

template <typename CharType, typename ElementType>
inline std::basic_string<CharType> join(const std::vector<ElementType> &string_list, const CharType separator) {
  return join(string_list, &separator, 1);
}

template <typename CharType, typename ElementType>
inline std::basic_string<CharType> join(const std::vector<ElementType> &string_list,
                                        const std::basic_string<CharType> &separator) {
  return join(string_list, separator.c_str(), separator.length());
}

////////////////////////////////////////////////////////////////////////////////
// Replace

template <class CharType>
inline std::basic_string<CharType> replace(const CharType *string,
                                           const CharType *search,
                                           const CharType *replace,
                                           size_t max = 0,
                                           size_t string_length = -1,
                                           size_t search_length = -1,
                                           size_t replace_length = -1) {
  if (string_length == -1) {
    string_length = length(string);
  }
  if (search_length == -1) {
    search_length = length(search);
  }
  if (replace_length == -1) {
    replace_length = length(replace);
  }
  if (string_length == 0 || search_length == 0 || replace_length == 0) {
    return std::basic_string<CharType>(string, string_length);
  }
  std::vector<basic_string_ref<CharType>> string_list;
  size_t total_length = 0;
  const CharType *p = string;
  for (size_t count = 0; p <= string + string_length && (max == 0 || count <= max); ++count) {
    const CharType *s = (max > 0 && count >= max)
                            ? string + string_length
                            : std::search(p, string + string_length, search, search + search_length);
    string_list.push_back(basic_string_ref<CharType>(p, s - p));
    total_length += s - p;
    p = s + search_length;
  }
  std::basic_string<CharType> replaced;
  replaced.reserve(total_length);
  for (typename std::vector<basic_string_ref<CharType>>::const_iterator it = string_list.begin();
       it != string_list.end(); ++it) {
    if (it != string_list.begin()) {
      replaced.append(replace, replace_length);
    }
    replaced.append(it->c_str(), it->c_str() + it->length());
  }
  return replaced;
}

template <class CharType>
inline std::basic_string<CharType> replace(
    const CharType *string, const CharType search, const CharType replace, size_t max = 0, size_t string_length = -1) {
  return replace(string, &search, &replace, max, string_length, 1, 1)
}

template <class CharType>
inline std::basic_string<CharType> replace(const CharType *string,
                                           const std::basic_string<CharType> &search,
                                           const std::basic_string<CharType> &replace,
                                           size_t max = 0,
                                           size_t string_length = -1) {
  return replace(string, search.c_str(), replace.c_str(), max, string_length, search.length(), replace.length());
}

template <class CharType>
inline std::basic_string<CharType> replace(const std::basic_string<CharType> &string,
                                           const CharType *search,
                                           const CharType *replace,
                                           size_t max = 0,
                                           size_t search_length = -1,
                                           size_t replace_length = -1) {
  return replace(string, search, replace, max, string.length(), search_length, replace_length);
}

template <class CharType>
inline std::basic_string<CharType>
replace(const std::basic_string<CharType> &string, CharType search, CharType replace, size_t max = 0) {
  return replace(string, &search, &replace, max, string.length(), 1, 1);
}

template <class CharType>
inline std::basic_string<CharType> replace(const std::basic_string<CharType> &string,
                                           const std::basic_string<CharType> &search,
                                           const std::basic_string<CharType> &replace,
                                           size_t max = 0) {
  return replace(string, search, replace, max, string.length(), search.length(), replace.length());
}

} // namespace string

} // namespace xl
