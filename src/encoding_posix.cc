#include <cassert>
#include <stack>
#include <xl/encoding>

namespace xl {

namespace encoding {

//
// UTF-8 Definition:
//
// UTF-32 Value         UTF-8 Value
// (UCS-2)
// 00000000 - 0000007F: 0xxxxxxx (1 byte)
// 00000080 - 000007FF: 110xxxxx 10xxxxxx (2 bytes)
// 00000800 - 0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx (3 bytes)
// (UCS-4)
// 00010000 - 001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (4 bytes)
// 00200000 - 03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (5 bytes)
// 04000000 - 7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (6 bytes)
//
// UTF-16 Definition:
//
// UTF-32 Value         UTF-16 Value
// (UCS-2)
// 00000000 - 0000FFFF: xxxxxxxx xxxxxxxx (1 word)
// (UCS-4)
// 00000000 - 0010FFFF: 110110xx xxxxxxxx 110111xx xxxxxxxx (2 words)
//
// The placeholder "x" should be filled by the original bits of the Unicode value by order
//

std::wstring utf8_to_utf16(const char *utf8, size_t length) {
  std::wstring utf16;
  for (size_t i = 0; length == -1 ? utf8[i] != '\0' : i < length;) {
    int bytes = 0;
    // count how many leading 1
    for (char c = utf8[i]; c < 0; c <<= 1) {
      ++bytes;
    }
    unsigned int u32_value = 0;
    if (bytes == 0) {
      bytes = 1;
      u32_value = utf8[i];
    } else {
      char leading_byte_mask = 0;
      for (int i = 0; i < 7 - bytes; ++i) {
        leading_byte_mask = (leading_byte_mask << 1) | 1;
      }
      u32_value = utf8[i] & leading_byte_mask;
      for (size_t j = i + 1; j < i + bytes; ++j) {
        if (length == -1 ? utf8[j] == '\0' : j >= length) {
          assert(false && "invalid utf8 string");
          return utf16;
        }
        u32_value = (u32_value << 6) | (utf8[j] & 0x3f);
      }
    }
    if (u32_value <= 0xffff) {
      utf16.append(1, (wchar_t)u32_value);
    } else {
      u32_value -= 0x10000;
      // 1101 10xx xxxx xxxx
      utf16.append(1, (wchar_t)(0xd800 | ((u32_value >> 10) & 0x03ff)));
      // 1101 11xx xxxx xxxx
      utf16.append(1, (wchar_t)(0xdc00 | (u32_value & 0x03ff)));
    }
    i += bytes;
  }
  return utf16;
}

std::wstring utf8_to_utf16(const char *utf8) {
  return utf8_to_utf16(utf8, -1);
}

std::wstring utf8_to_utf16(const std::string &utf8) {
  return utf8_to_utf16(utf8.c_str(), utf8.length());
}

std::string utf16_to_utf8(const wchar_t *utf16, size_t length) {
  std::string utf8;
  std::stack<char> stk;
  for (size_t i = 0; length == -1 ? utf16[i] != L'\0' : i < length;) {
    int words = 1;
    // 1101 1?xx xxx xxxxx, check leading 5 bits
    if ((utf16[i] & 0xf800) == 0xd800) {
      ++words;
    }
    unsigned int u32_value = 0;
    if (words == 1) {
      u32_value = utf16[i];
    } else {
      // 1101 10xx xxxx xxxx
      u32_value = utf16[i] & 0x03ff;
      if (length == -1 ? utf16[i + 1] == L'\0' : i + 1 >= length) {
        assert(false && "invalid utf16 string");
        return utf8;
      }
      u32_value = (u32_value << 10) | (utf16[i + 1] & 0x03ff);
      u32_value += 0x10000;
    }
    if (u32_value <= 0x7f) {
      utf8.append(1, (char)u32_value);
    } else {
      char mask = '\x80'; // 1000 0000 => 1..1 0..0
      while (u32_value > 0) {
        stk.push(u32_value & 0x3f);
        u32_value >>= 6;
        mask >>= 1;
      }
      if ((stk.top() & mask) != 0) {
        // if bit of the first byte is all used, need another byte
        stk.push(0);
      } else {
        // mask begins with 1000 0000, already has an 1, so drop 1 here
        mask <<= 1;
      }
      utf8.append(1, stk.top() | mask);
      stk.pop();
      while (!stk.empty()) {
        utf8.append(1, stk.top() | 0x80);
        stk.pop();
      }
    }
    i += words;
  }
  return utf8;
}

std::string utf16_to_utf8(const wchar_t *utf16) {
  return utf16_to_utf8(utf16, -1);
}

std::string utf16_to_utf8(const std::wstring &utf16) {
  return utf16_to_utf8(utf16.c_str(), utf16.length());
}

} // namespace encoding

} // namespace xl
