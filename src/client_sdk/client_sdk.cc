#include <include/selfupdate/selfupdate.h>
#include <memory>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <utility/http_client.h>

namespace selfupdate {

const char *SELFUPDATE_USER_AGENT = "selfupdate";
const unsigned QUERY_TIMEOUT      = 10000;

const char *PACKAGEINFO_TITLE          = "title";
const char *PACKAGEINFO_DESCRIPTION    = "description";
const char *PACKAGEINFO_SERVER_VERSION = "server_version";
const char *PACKAGEINFO_PACKAGE_URL    = "package_url";
const char *PACKAGEINFO_PACKAGE_FORMAT = "package_format";

std::error_code Query(const std::string &query_url, const std::string &query_body, PackageInfo &package_info) {
  HttpClient http_client(SELFUPDATE_USER_AGENT);
  unsigned status = 0;
  std::string response;
  std::error_code ec;
  if (query_body.empty())
    ec = http_client.Get(query_url, {}, &status, nullptr, &response, QUERY_TIMEOUT);
  else
    ec = http_client.Post(query_url, {}, query_body, &status, nullptr, &response, QUERY_TIMEOUT);
  if (ec)
    return ec;

  rapidjson::Document doc;
  doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag | rapidjson::kParseNanAndInfFlag>(
      response);
  if (doc.HasParseError())
    return std::make_error_code(std::errc::bad_message);

  if (!doc.HasMember(PACKAGEINFO_TITLE) || !doc[PACKAGEINFO_TITLE].IsString() ||
      !doc.HasMember(PACKAGEINFO_DESCRIPTION) || !doc[PACKAGEINFO_DESCRIPTION].IsString() ||
      !doc.HasMember(PACKAGEINFO_SERVER_VERSION) || !doc[PACKAGEINFO_SERVER_VERSION].IsString() ||
      !doc.HasMember(PACKAGEINFO_PACKAGE_URL) || !doc[PACKAGEINFO_PACKAGE_URL].IsString() ||
      !doc.HasMember(PACKAGEINFO_PACKAGE_FORMAT) || !doc[PACKAGEINFO_PACKAGE_FORMAT].IsString())
    return std::make_error_code(std::errc::bad_message);

  package_info.title.assign(doc[PACKAGEINFO_TITLE].GetString(), doc[PACKAGEINFO_TITLE].GetStringLength());
  package_info.description.assign(doc[PACKAGEINFO_DESCRIPTION].GetString(),
                                  doc[PACKAGEINFO_DESCRIPTION].GetStringLength());
  package_info.server_version.assign(doc[PACKAGEINFO_SERVER_VERSION].GetString(),
                                     doc[PACKAGEINFO_SERVER_VERSION].GetStringLength());
  package_info.package_url.assign(doc[PACKAGEINFO_PACKAGE_URL].GetString(),
                                  doc[PACKAGEINFO_PACKAGE_URL].GetStringLength());
  package_info.package_format.assign(doc[PACKAGEINFO_PACKAGE_FORMAT].GetString(),
                                     doc[PACKAGEINFO_PACKAGE_FORMAT].GetStringLength());
  return {};
}

void Download(const PackageInfo &package_url, DownloadProgressMonitor monitor) {
}

void Install(const PackageInfo &PackageInfo) {
}

} // namespace selfupdate
