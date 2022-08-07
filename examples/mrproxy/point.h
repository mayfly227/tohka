#ifndef TOHKA_EXAMPLES_MRPROXY_POINT_H
#define TOHKA_EXAMPLES_MRPROXY_POINT_H
#include <fstream>
#include <iostream>
#include <string>

#include "context.h"
#include "json.hpp"
using namespace nlohmann;
using namespace std;

OutHandlerPrt_t OutCreate(const string& id, const TcpEventPrt_t& other,
                          NetAddress name, InHandler* in);
vector<InHandlerPrt_t> InCreate();

#endif