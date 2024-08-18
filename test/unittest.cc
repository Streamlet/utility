#include <gtest/gtest.h>
#include <xl/file>
#include <xl/http>
#include <xl/native_string>
#include <xl/process>
#include <xl/scope_exit>

int _tmain(int argc, TCHAR *argv[]) {
  auto workdir = xl::path::dirname(xl::process::executable_path().c_str());
  int pid = xl::process::start(_T("python"), {_T("http_echo.py")}, workdir);
  XL_ON_BLOCK_EXIT(xl::process::kill, pid);
  printf("Starting local HTTP server...\n");
  while (true) {
    int status = xl::http::get("http://localhost:8080/", nullptr);
    if (status == xl::http::StatusOK) {
      break;
    }
    xl::process::sleep(100);
  }

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
