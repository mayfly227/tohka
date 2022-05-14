#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "tohka/iobuf.h"
#include "tohka/ioloop.h"
#include "tohka/netaddress.h"
#include "tohka/tcpclient.h"
#include "tohka/tohka.h"
#include "tohka/util/log.h"

using namespace tohka;

static long readbytes = 0;
static long readcount = 0;

class Session {
 public:
  Session(IoLoop* loop, const NetAddress& peer, std::string name, int send_size)
      : client_(loop, peer, std::move(name)), send_size_(send_size) {
    client_.SetOnConnection(
        [this](const TcpEventPrt_t& conn) { this->OnConnection(conn); });
    client_.SetOnMessage([this](const TcpEventPrt_t& conn, IoBuf* buf) {
      this->OnMessage(conn, buf);
    });
    data = new char[send_size_];
    memset(data, 0, send_size_);
  }
  ~Session() { delete data; }
  void OnConnection(const TcpEventPrt_t& conn) {
    if (conn->Connected()) {
      conn->Send(data, send_size_);
      readbytes += send_size_;
    }
  }
  void OnMessage(const TcpEventPrt_t& conn, IoBuf* buf) {
    readbytes += buf->GetReadableSize();
    readcount +=1;
    conn->Send(buf);
  }
  void Run() { client_.Connect(); }
  void Stop() { client_.Disconnect(); }

 private:
  TcpClient client_;
  int send_size_;
  char* data;
};

void OnTimeOut(std::vector<Session*> total_) {
  for (auto& t : total_) {
    t->Stop();
  }
  IoLoop::GetLoop()->Quit();
}

int main(int argc, char* argv[]) {
  IoLoop loop;
  log_set_level(LOG_NONE);
  std::vector<Session*> total_sess;


  int connections = 100;
  int send_size = 1024;
  int timeout = 10;

  NetAddress addr(6666);
  for (int i = 0; i < connections; i++) {
    Session* s = new Session(&loop, addr, std::to_string(i), send_size);
    total_sess.push_back(s);
  }
  loop.CallLater(timeout * 1000, [=] { OnTimeOut(total_sess); });
  for (auto c : total_sess) {
    c->Run();
  }
  loop.RunForever();
  for (auto c : total_sess) {
    delete c;
  }

  int toal_mbs = readbytes / 1024 / 1024 / timeout;
  printf("1 threads and %d connections, send 1024 bytes each time\n",
         connections);
  printf("total readcount=%ld readbytes=%ld\n", readcount, readbytes);
  printf("throughput = %d MB/s\n", toal_mbs);
  return 0;
}