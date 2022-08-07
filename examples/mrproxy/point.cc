#include "point.h"
// 结点
#include <vector>

#include "freedom.h"
#include "run_in.h"
#include "run_out.h"
#include "socks_in.h"

using namespace nlohmann;
using namespace std;
extern json global_json;

OutHandlerPrt_t OutCreate(const string& id, const TcpEventPrt_t& other,
                          NetAddress dest, InHandler* in) {
  const auto& out_conf = global_json["out"];
  if (out_conf["protocol"] == "run") {
    return make_shared<RunOut>(id, other, dest, in, out_conf);
  }
  if (out_conf["protocol"] == "freedom") {
    return make_shared<FreeDom>(id, other, dest, in);
  }
  return nullptr;
}

vector<InHandlerPrt_t> InCreate() {
  vector<InHandlerPrt_t> v;
  for (const auto& in_conf : global_json["in"]) {
    if (in_conf["protocol"] == "socks5") {
      cout << in_conf["listen"] << endl;
      auto in = make_shared<SocksIn>(in_conf);
      v.push_back(in);
    }
    if (in_conf["protocol"] == "run") {
      auto in = make_shared<RunIn>(in_conf);
      v.push_back(in);
    }
    if (in_conf["protocol"] == "http") {
      // TODO
    }
  }
  return v;
}