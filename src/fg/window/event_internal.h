#include <fg/window/event.h>

namespace fg::window::event {
    bool PushEvent(Event* event);
    Event* PrepareNextEvent(unsigned int window_id);
    bool PrepareEventDone();

    void SetFetchEvent(void(*hook)());
}