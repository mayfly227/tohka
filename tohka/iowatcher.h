//
// Created by li on 2022/2/24.
//

#ifndef TOHKA_TOHKA_IOWATCHER_H
#define TOHKA_TOHKA_IOWATCHER_H

#include <map>
#include <vector>

#include "noncopyable.h"
#include "timepoint.h"
#include "tohka.h"
namespace tohka {
class IoEvent;
class IoWatcher : noncopyable {
 public:
  virtual ~IoWatcher() = default;
  // call poll
  virtual TimePoint PollEvents(int timeout, EventList* event_list) = 0;
  // Register and Update io_event to io_events_map
  virtual void RegisterEvent(IoEvent* io_event) = 0;
  virtual void UnRegisterEvent(IoEvent* io_event) = 0;

 protected:
  // map(fd->io_event)
  using IoEventsMap = std::map<int, IoEvent*>;
  IoEventsMap io_events_map;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_IOWATCHER_H
