#include <include/selfupdate/selfupdate.h>
#include <memory>
#define RAPIDJSON_HAS_STDSTRING 1
#include <cstdio>
#include <direct.h>
#include <rapidjson/document.h>
#include <sstream>
#include <utility/crypto.h>
#include <utility/http_client.h>
#include <utility/system_util.h>

namespace selfupdate {

namespace {

enum selfupdate_error_code {
  SUE_OK = 0,
  SUE_PackageInfoFormatError,
  SUE_PackageSizeError,
  SUE_PackageVerifyError,

  SUE_Count,
};

struct SelfUpdateErrorMessage {
#ifdef _DEBUG
  selfupdate_error_code code;
#endif
  const char *message = nullptr;
};

#ifdef _DEBUG
#define _(code, message) code, message
#else
#define _(code, message) message
#endif
static const SelfUpdateErrorMessage selfupdate_error_message_[] = {
    {_(SUE_OK, "OK")},
    {_(SUE_PackageInfoFormatError, "package info format error")},
    {_(SUE_PackageSizeError, "package size error")},
    {_(SUE_PackageVerifyError, "package verify error")},
};
#undef _

class selfupdate_error_category : public std::error_category {
  const char *name() const noexcept override {
    return "selfupdate_error";
  }

  std::string message(int _Errval) const override {
    if (_Errval >= 0 && _Errval < SUE_Count)
      return selfupdate_error_message_[_Errval].message;
    return "unknown error";
  }
};

std::error_category &selfupdate_category() {
  static selfupdate_error_category instance;
  return instance;
}

std::error_code make_selfupdate_error(selfupdate_error_code error) {
  return std::error_code(error, selfupdate_category());
}

static std::string app_name_ = "selfupdate";
static std::string user_agent_ = "";

const unsigned QUERY_TIMEOUT = 10000;

const char *PACKAGEINFO_PACKAGE_NAME = "package_name";
const char *PACKAGEINFO_PACKAGE_VERSION = "package_version";
const char *PACKAGEINFO_PACKAGE_URL = "package_url";
const char *PACKAGEINFO_PACKAGE_SIZE = "package_size";
const char *PACKAGEINFO_PACKAGE_FORMAT = "package_format";
const char *PACKAGEINFO_PACKAGE_HASH = "package_hash";
const char *PACKAGEINFO_PACKAGE_HASH_ALGO_MD5 = "md5";
const char *PACKAGEINFO_PACKAGE_HASH_ALGO_SHA1 = "sha1";
const char *PACKAGEINFO_PACKAGE_HASH_ALGO_SHA224 = "sha224";
const char *PACKAGEINFO_PACKAGE_HASH_ALGO_SHA256 = "sha256";
const char *PACKAGEINFO_PACKAGE_HASH_ALGO_SHA384 = "sha384";
const char *PACKAGEINFO_PACKAGE_HASH_ALGO_SHA512 = "sha512";
const char *PACKAGEINFO_UPDATE_TITLE = "update_title";
const char *PACKAGEINFO_UPDATE_DESCRIPTION = "update_description";

} // namespace

void Initialize(const std::string &app_name, const std::string &user_agent) {
  app_name_ = app_name;
  user_agent_ = user_agent;
}

std::error_code Query(const std::string &query_url, const std::string &query_body, PackageInfo &package_info) {
  HttpClient http_client(user_agent_);
  unsigned status = 0;
  std::string response;
  std::error_code ec;
  if (query_body.empty())
    ec = http_client.Get(query_url, {}, &status, nullptr, &response, QUERY_TIMEOUT);
  else
    ec = http_client.Post(query_url, {}, query_body, &status, nullptr, &response, QUERY_TIMEOUT);
  if (ec || status != 200)
    return ec;

  rapidjson::Document doc;
  doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag | rapidjson::kParseNanAndInfFlag>(
      response);
  if (doc.HasParseError())
    return make_selfupdate_error(SUE_PackageInfoFormatError);

  if (!doc.HasMember(PACKAGEINFO_PACKAGE_NAME) || !doc[PACKAGEINFO_PACKAGE_NAME].IsString() ||
      !doc.HasMember(PACKAGEINFO_PACKAGE_VERSION) || !doc[PACKAGEINFO_PACKAGE_VERSION].IsString() ||
      !doc.HasMember(PACKAGEINFO_PACKAGE_URL) || !doc[PACKAGEINFO_PACKAGE_URL].IsString() ||
      !doc.HasMember(PACKAGEINFO_PACKAGE_SIZE) || !doc[PACKAGEINFO_PACKAGE_SIZE].IsUint64() ||
      !doc.HasMember(PACKAGEINFO_PACKAGE_FORMAT) || !doc[PACKAGEINFO_PACKAGE_FORMAT].IsString() ||
      !doc.HasMember(PACKAGEINFO_PACKAGE_HASH) || !doc[PACKAGEINFO_PACKAGE_HASH].IsObject() ||
      !doc.HasMember(PACKAGEINFO_UPDATE_TITLE) || !doc[PACKAGEINFO_UPDATE_TITLE].IsString() ||
      !doc.HasMember(PACKAGEINFO_UPDATE_DESCRIPTION) || !doc[PACKAGEINFO_UPDATE_DESCRIPTION].IsString())
    return make_selfupdate_error(SUE_PackageInfoFormatError);

  package_info.package_name.assign(doc[PACKAGEINFO_PACKAGE_NAME].GetString(),
                                   doc[PACKAGEINFO_PACKAGE_NAME].GetStringLength());
  package_info.package_version.assign(doc[PACKAGEINFO_PACKAGE_VERSION].GetString(),
                                      doc[PACKAGEINFO_PACKAGE_VERSION].GetStringLength());
  package_info.package_url.assign(doc[PACKAGEINFO_PACKAGE_URL].GetString(),
                                  doc[PACKAGEINFO_PACKAGE_URL].GetStringLength());
  package_info.package_size = doc[PACKAGEINFO_PACKAGE_SIZE].GetUint64();
  package_info.package_format.assign(doc[PACKAGEINFO_PACKAGE_FORMAT].GetString(),
                                     doc[PACKAGEINFO_PACKAGE_FORMAT].GetStringLength());
  for (auto it = doc[PACKAGEINFO_PACKAGE_HASH].MemberBegin(); it != doc[PACKAGEINFO_PACKAGE_HASH].MemberEnd(); ++it) {
    if (!it->name.IsString() || !it->value.IsString()) {
      return make_selfupdate_error(SUE_PackageInfoFormatError);
    } else if (_stricmp(it->name.GetString(), PACKAGEINFO_PACKAGE_HASH_ALGO_MD5) == 0 ||
               _stricmp(it->name.GetString(), PACKAGEINFO_PACKAGE_HASH_ALGO_SHA1) == 0 ||
               _stricmp(it->name.GetString(), PACKAGEINFO_PACKAGE_HASH_ALGO_SHA224) == 0 ||
               _stricmp(it->name.GetString(), PACKAGEINFO_PACKAGE_HASH_ALGO_SHA256) == 0 ||
               _stricmp(it->name.GetString(), PACKAGEINFO_PACKAGE_HASH_ALGO_SHA384) == 0 ||
               _stricmp(it->name.GetString(), PACKAGEINFO_PACKAGE_HASH_ALGO_SHA512) == 0) {
      package_info.package_hash.insert(std::make_pair(it->name.GetString(), it->value.GetString()));
    } else {
      return make_selfupdate_error(SUE_PackageInfoFormatError);
    }
  }
  package_info.update_title.assign(doc[PACKAGEINFO_UPDATE_TITLE].GetString(),
                                   doc[PACKAGEINFO_UPDATE_TITLE].GetStringLength());
  package_info.update_description.assign(doc[PACKAGEINFO_UPDATE_DESCRIPTION].GetString(),
                                         doc[PACKAGEINFO_UPDATE_DESCRIPTION].GetStringLength());
  return {};
}

namespace {

long long ReadInteger(const std::string &file) {
  FILE *f = fopen(file.c_str(), "r");
  if (f == nullptr)
    return -1;
  unsigned long long v = 0;
  fscanf(f, "%lld", &v);
  fclose(f);
  return v;
}
void WriteInteger(const std::string &file, long long v) {
  FILE *f = fopen(file.c_str(), "w");
  if (f == nullptr)
    return;
  fprintf(f, "%lld", v);
  fclose(f);
}

const char *PACKAGE_NAME_VERSION_SEP = "-";
const char *FILE_NAME_EXT_SEP = ".";
const char *DOWNLOADING_FILE_SUFFIX = ".downloading";

bool VerifyPackage(const std::string &package_file, const std::map<std::string, std::string> &hash) {
  for (const auto &item : hash) {
    if (item.first == PACKAGEINFO_PACKAGE_HASH_ALGO_MD5) {
      if (crypto::MD5(package_file.c_str()) != item.second)
        return false;
    }
    if (item.first == PACKAGEINFO_PACKAGE_HASH_ALGO_SHA1) {
      if (crypto::SHA1(package_file.c_str()) != item.second)
        return false;
    }
    if (item.first == PACKAGEINFO_PACKAGE_HASH_ALGO_SHA224) {
      if (crypto::SHA224(package_file.c_str()) != item.second)
        return false;
    }
    if (item.first == PACKAGEINFO_PACKAGE_HASH_ALGO_SHA256) {
      if (crypto::SHA256(package_file.c_str()) != item.second)
        return false;
    }
    if (item.first == PACKAGEINFO_PACKAGE_HASH_ALGO_SHA384) {
      if (crypto::SHA384(package_file.c_str()) != item.second)
        return false;
    }
    if (item.first == PACKAGEINFO_PACKAGE_HASH_ALGO_SHA512) {
      if (crypto::SHA512(package_file.c_str()) != item.second)
        return false;
    }
  }
  return true;
}

} // namespace

std::error_code Download(const PackageInfo &package_info, DownloadProgressMonitor download_progress_monitor) {
  std::string cache_dir = system_util::GetTempDirPath() + app_name_;
  _mkdir(cache_dir.c_str());
  std::string package_file = cache_dir + system_util::GetPathSep() + package_info.package_name +
                             PACKAGE_NAME_VERSION_SEP + package_info.package_version + FILE_NAME_EXT_SEP +
                             package_info.package_format;
  std::string package_downloading_file = package_file + DOWNLOADING_FILE_SUFFIX;
  long long downloaded_size = ReadInteger(package_downloading_file);

  FILE *f = fopen(package_file.c_str(), "ab");
  _fseeki64(f, 0, SEEK_END);
  long long offset = _ftelli64(f);
  if (offset == package_info.package_size && downloaded_size < 0 &&
      VerifyPackage(package_file, package_info.package_hash)) {
    return {};
  }

  if (downloaded_size > 0 && offset >= downloaded_size) {
    _fseeki64(f, downloaded_size, SEEK_SET);
  } else {
    _fseeki64(f, downloaded_size, SEEK_SET);
    downloaded_size = 0;
  }
  HttpClient http_client(user_agent_);
  unsigned status = 0;
  HttpClient::ResponseHeader response_header;
  std::error_code ec = http_client.Head(package_info.package_url, {}, &status, &response_header);
  if (ec)
    return ec;

  long long total_size = package_info.package_size;
  auto it = response_header.find("Content-Length");
  if (it != response_header.end()) {
    std::string content_length = it->second;
    total_size = atoll(content_length.c_str());
  }

  if (total_size != package_info.package_size)
    return make_selfupdate_error(SUE_PackageSizeError);

  std::stringstream range_expr;
  range_expr << "bytes=";
  range_expr << downloaded_size;
  range_expr << "-";
  HttpClient::RequestHeader request_header = {
      {"Range", range_expr.str()}
  };
  ec =
      http_client.Get(package_info.package_url, request_header, &status, nullptr, [&](const void *data, size_t length) {
        fwrite(data, 1, length, f);
        fflush(f);
        downloaded_size += length;
        WriteInteger(package_downloading_file, downloaded_size);
        if (download_progress_monitor != nullptr)
          download_progress_monitor(downloaded_size, total_size);
      });
  fclose(f);
  if (downloaded_size == total_size) {
    _unlink(package_downloading_file.c_str());
    if (!VerifyPackage(package_file, package_info.package_hash)) {
      _unlink(package_file.c_str());
      return make_selfupdate_error(SUE_PackageVerifyError);
    }
  }
  return ec;
}

std::error_code Install(const PackageInfo &PackageInfo) {
  return {};
}

} // namespace selfupdate
