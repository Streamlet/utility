// MIT License
//
// Copyright (c) 2022 Streamlet (streamlet@outlook.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <algorithm>
#include <sstream>
#include <xl/file>
#include <xl/ini>

namespace xl {

bool ini::load(const TCHAR *path) {
  std::string content = file::read_text_auto(path);
  if (content.empty()) {
    return false;
  }
  path_ = path;
  return parse(content);
}

bool ini::save(const TCHAR *path) const {
  std::string content = dump();
  return file::write(path_.c_str(), content);
}

namespace {

/**
 * file: section+
 * section: section_header? line*
 * section_header: [section_name] comment?
 * line: kv? comment?
 * kv: key? (=value)?
 * section_name: [^\[\]]+
 * key: [^=;]+
 * value: [^;]+
 */

void pass_blank(const std::string &content, size_t &pos, bool back = false) {
  if ((!back && pos >= content.length()) || (back && pos == 0)) {
    return;
  }
  while (true) {
    switch (content[pos]) {
    case ' ':
    case '\t':
      back ? --pos : ++pos;
      break;
    default:
      return;
    }
  }
}

bool match_key(const std::string &content, size_t &pos, std::string &key) {
  pass_blank(content, pos);
  size_t begin = pos;
  bool stop = false;
  if (pos < content.length() && content[pos] == '[') {
    return false;
  }
  while (pos < content.length() && !stop) {
    switch (content[pos]) {
    case '\r':
    case '\n':
    case '=':
    case ';':
      stop = true;
      break;
    default:
      ++pos;
      break;
    }
  }
  size_t end = pos;
  if (end != begin && end != 0) {
    --end;
    pass_blank(content, end, true);
    ++end;
  }
  key = content.substr(begin, end - begin);
  return true;
}

bool match_value(const std::string &content, size_t &pos, std::string &value) {
  pass_blank(content, pos);
  size_t begin = pos;
  bool stop = false;
  while (pos < content.length() && !stop) {
    switch (content[pos]) {
    case '\r':
    case '\n':
    case ';':
      stop = true;
      break;
    default:
      ++pos;
      break;
    }
  }
  size_t end = pos;
  if (end != begin && end != 0) {
    --end;
    pass_blank(content, end, true);
    ++end;
  }
  value = content.substr(begin, end - begin);
  return true;
}

bool match_comment(const std::string &content, size_t &pos, std::string &comment) {
  pass_blank(content, pos);
  if (pos >= content.length() || content[pos] != ';') {
    return true;
  }
  ++pos;
  size_t begin = pos;
  while (pos < content.length() && (content[pos] != '\r' && content[pos] != '\n')) {
    ++pos;
  }
  comment = content.substr(begin, pos - begin);
  return true;
}

bool match_line_ending(const std::string &content, size_t &pos) {
  while (content[pos] == '\r' && pos < content.length()) {
    ++pos;
  }
  if (content[pos] != '\n') {
    return false;
  }
  ++pos;
  return true;
}

bool match_line(const std::string &content, size_t &pos, ini::ini_line &line) {
  if (!match_key(content, pos, line.key)) {
    return false;
  }
  if (pos < content.length() && content[pos] == '=') {
    ++pos;
    if (!match_value(content, pos, line.value)) {
      return false;
    }
  }
  if (!match_comment(content, pos, line.comment)) {
    return false;
  }
  if (!match_line_ending(content, pos)) {
    return false;
  }
  return true;
}

bool match_section_name(const std::string &content, size_t &pos, std::string &name) {
  pass_blank(content, pos);
  size_t begin = pos;
  while (pos < content.length() && content[pos] != ']') {
    ++pos;
  }
  if (pos >= content.length()) {
    return false;
  }
  size_t end = pos;
  if (end != begin && end != 0) {
    --end;
    pass_blank(content, end, true);
    ++end;
  }
  name = content.substr(begin, end - begin);
  return true;
}

bool match_section_header(const std::string &content, size_t &pos, std::string &name, std::string &comment) {
  pass_blank(content, pos);
  if (pos < content.length() && content[pos] != '[') {
    return true;
  }
  ++pos;
  if (!match_section_name(content, pos, name)) {
    return false;
  }
  if (pos >= content.length() || content[pos] != ']') {
    return false;
  }
  ++pos;
  if (!match_comment(content, pos, comment)) {
    return false;
  }
  if (!match_line_ending(content, pos)) {
    return false;
  }
  return true;
}

void section_add_line(ini::ini_section &section, ini::ini_line line) {
  auto it = section.line_map.find(line.key);
  if (it != section.line_map.end()) {
    section.lines.erase(it->second);
  }
  section.lines.push_back(std::move(line));
  section.line_map[section.lines.back().key] = std::prev(section.lines.end());
}

bool match_section(const std::string &content, size_t &pos, ini::ini_section &section) {
  if (!match_section_header(content, pos, section.name, section.comment)) {
    return false;
  }
  while (true) {
    ini::ini_line line;
    if (!match_line(content, pos, line)) {
      break;
    }
    if (!line.key.empty() || !line.value.empty() || !line.comment.empty()) {
      section_add_line(section, std::move(line));
    }
  }
  return true;
}

bool match_file(const std::string &content, size_t &pos, ini::ini_data &data) {
  while (pos < content.size()) {
    ini::ini_section section;
    if (!match_section(content, pos, section)) {
      return false;
    }
    auto it = data.section_map.find(section.name);
    if (it != data.section_map.end()) {
      if (!section.comment.empty()) {
        it->second->comment = section.comment;
        for (auto &line : section.lines) {
          section_add_line(*it->second, std::move(line));
        }
      }
    } else {
      data.sections.push_back(std::move(section));
      data.section_map[data.sections.back().name] = std::prev(data.sections.end());
    }
  }
  return true;
}

} // namespace

bool ini::parse(const std::string &content) {
  ini::ini_data data;
  size_t pos = 0;
  if (!match_file(content, pos, data)) {
    return false;
  }
  data_ = std::move(data);
  return true;
}

namespace {

size_t calc_aligment(const std::list<ini::ini_line> &lines, std::list<ini::ini_line>::const_iterator &it) {
  size_t alignment = 0;
  while (it != lines.end()) {
    if ((it->key.empty() && it->value.empty()) || it->comment.empty()) {
      ++it;
      break;
    }
    alignment = std::max(alignment, it->key.length() + 3 + it->value.length());
    ++it;
  }
  return alignment;
}

std::string dump_alignment(size_t used_size, size_t alignment) {
  return std::string((alignment > 0 ? alignment - used_size : 0) + (used_size > 0 ? 1 : 0), ' ');
}

std::string dump_comment(const std::string &comment, size_t used_size, size_t alignment = 0) {
  if (comment.empty()) {
    return "";
  }
  std::stringstream ss;
  ss << dump_alignment(used_size, alignment) << ';' << comment;
  return ss.str();
}

std::string dump_line(const ini::ini_line &line, size_t alignment = 0) {
  std::stringstream ss;
  if (!line.key.empty() || !line.value.empty()) {
    ss << line.key << " = " << line.value
       << dump_comment(line.comment, line.key.length() + 3 + line.value.length(), alignment);
  } else if (!line.comment.empty()) {
    ss << dump_comment(line.comment, 0, 0);
  }
  ss << "\r\n";
  return ss.str();
}

std::string dump_section_header(const std::string &name, const std::string &comment, size_t alignment = 0) {
  std::stringstream ss;
  if (!name.empty() || !comment.empty()) {
    ss << '[' << name << ']' << dump_comment(comment, name.length() + 2, alignment) << "\r\n";
  }
  return ss.str();
}

std::string dump_section(const ini::ini_section &section) {
  std::stringstream ss;
  auto it_alignment = section.lines.begin();
  size_t alignment = calc_aligment(section.lines, it_alignment);
  alignment = std::max(alignment, section.name.length() + 2);
  ss << dump_section_header(section.name, section.comment, alignment);
  for (auto it = section.lines.begin(); it != section.lines.end(); ++it) {
    if (it == it_alignment) {
      alignment = calc_aligment(section.lines, it_alignment);
    }
    ss << dump_line(*it, alignment);
  }
  return ss.str();
}

std::string dump_file(const ini::ini_data &data) {
  std::stringstream ss;
  for (const auto &section : data.sections) {
    ss << dump_section(section) << "\r\n";
  }
  return ss.str();
}

} // namespace

std::string ini::dump() const {
  return dump_file(data_);
}

const ini::ini_data &ini::data() const {
  return data_;
}

std::vector<std::string> ini::enum_sections() const {
  std::vector<std::string> section_names;
  section_names.reserve(data_.sections.size());
  for (const auto &section : data_.sections) {
    section_names.push_back(section.name);
  }
  return section_names;
}

bool ini::has_section(const std::string &section) const {
  return data_.section_map.find(section) != data_.section_map.end();
}

bool ini::add_section(const std::string &section, const std::string &comment, bool save) {
  auto it = data_.section_map.find(section);
  if (it == data_.section_map.end()) {
    data_.sections.push_back({section, comment, {}, {}});
    data_.section_map[section] = std::prev(data_.sections.end());
  } else {
    if (it->second->comment == comment) {
      return false;
    }
    it->second->comment = comment;
  }
  if (save && !path_.empty()) {
    this->save(path_.c_str());
  }
  return true;
}

bool ini::remove_section(const std::string &section, bool save) {
  auto it = data_.section_map.find(section);
  if (it == data_.section_map.end()) {
    return false;
  }
  data_.sections.erase(it->second);
  data_.section_map.erase(it);
  if (save && !path_.empty()) {
    this->save(path_.c_str());
  }
  return true;
}

std::vector<std::string> ini::enum_keys(const std::string &section) const {
  auto it = data_.section_map.find(section);
  if (it == data_.section_map.end()) {
    return {};
  }
  std::vector<std::string> keys;
  for (const auto &line : it->second->lines) {
    keys.push_back(line.key);
  }
  return keys;
}

bool ini::has_key(const std::string &section, const std::string &key) const {
  auto it_section = data_.section_map.find(section);
  if (it_section == data_.section_map.end()) {
    return false;
  }
  const auto &line_map = it_section->second->line_map;
  auto it_key = line_map.find(key);
  if (it_key == line_map.end()) {
    return false;
  }
  return true;
}

std::string ini::get_value(const std::string &section, const std::string &key) const {
  auto it_section = data_.section_map.find(section);
  if (it_section == data_.section_map.end()) {
    return "";
  }
  const auto &line_map = it_section->second->line_map;
  auto it_key = line_map.find(key);
  if (it_key == line_map.end()) {
    return "";
  }
  return it_key->second->value;
}

bool ini::set_value(const std::string &section,
                    const std::string &key,
                    const std::string &value,
                    const std::string &comment,
                    bool save) {
  auto it_section = data_.section_map.find(section);
  if (it_section == data_.section_map.end()) {
    data_.sections.push_back({section, comment, {}, {}});
    data_.section_map[section] = std::prev(data_.sections.end());
    it_section = data_.section_map.find(section);
  }
  auto &ini_section = *it_section->second;
  auto it_key = ini_section.line_map.find(key);
  if (it_key == ini_section.line_map.end()) {
    ini_section.lines.push_back({key, value, comment});
    ini_section.line_map[key] = std::prev(ini_section.lines.end());
  } else {
    if (it_key->second->value == value && it_key->second->comment == comment) {
      return false;
    }
    it_key->second->value = value;
    it_key->second->comment = comment;
  }
  if (save && !path_.empty()) {
    this->save(path_.c_str());
  }
  return true;
}

bool ini::remove_value(const std::string &section, const std::string &key, bool save) {
  auto it_section = data_.section_map.find(section);
  if (it_section == data_.section_map.end()) {
    return false;
  }
  auto &ini_section = *it_section->second;
  auto it_key = ini_section.line_map.find(key);
  if (it_key == ini_section.line_map.end()) {
    return false;
  }
  ini_section.lines.erase(it_key->second);
  ini_section.line_map.erase(it_key);
  if (save && !path_.empty()) {
    this->save(path_.c_str());
  }
  return false;
}

} // namespace xl
