//
// Created by li on 2022/2/24.
//

#include "iowatcher.h"

#include "platform.h"
#include "poll.h"
using namespace tohka;

IoWatcher* IoWatcher::ChooseIoWatcher() {
#if defined(OS_UNIX) || defined(OS_WIN)
  return new Poll();
#endif
#if defined(OS_LINUX)

#endif
}