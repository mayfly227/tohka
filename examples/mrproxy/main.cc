//
// Created by li on 2022/4/22.
//
#include <filesystem>
#include <string>

#include "point.h"
#include "tohka/ioloop.h"
#include "unistd.h"
using namespace tohka;
using namespace std;
namespace fs = filesystem;

json global_json;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("usage: mrproxy yourconfig.json\n");
    return 0;
  }
  char buf[128];
  getcwd(buf, 128);
  printf("current %s\n", buf);
  string name = string(argv[1]);
  fprintf(stdout, "loading config file %s\n", name.c_str());
  fs::path p(name);
  if (fs::exists(p)) {
    std::ifstream i(name);
    i >> global_json;

    for (const auto& in_handler : InCreate()) {
      in_handler->StartServer();
    }

  } else {
    fprintf(stderr, "no such file %s!\n", name.c_str());
  }
  return 0;
}
