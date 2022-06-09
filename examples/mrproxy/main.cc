//
// Created by li on 2022/4/22.
//
#include "socks_in.h"
#include "tohka/ioloop.h"
using namespace tohka;

int main(int argc, char* argv[]) {
  //  Point* point = newPoint();
  //  point->in->Create(point)->StartServer();
  auto in = socks_in();
  //  auto out = FreeDom();
  in.StartServer();
  return 0;
}