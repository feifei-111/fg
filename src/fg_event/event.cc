#ifdef _WIN32
#include<Windows.h>
#endif

#include<iostream>
#include"fg_event/event.h"



namespace fg_event{

struct EventListClass{
    static constexpr unsigned int capacity_ = 256;

    Event events_[capacity_];
    unsigned int head_ = 0;
    unsigned int tail_ = 0;
    unsigned int count_ = 0;

    int Count(){
        return count_;
    }

    // SDL 里面的 event 也是拷贝的，反正先写了再说
    bool PushEvent(Event* event){
        if (count_ == capacity_){
            return false;
        }
        events_[tail_] = *event;
        tail_ = (tail_ + 1) % capacity_;
        count_ += 1;
        return true;
    }

    bool PopHeadEvent(Event* event){
        if (count_ == 0){
            return false;
        }
        *event = events_[head_];
        head_ = (head_ + 1) % capacity_;
        count_ -= 1;
        return true;
    }

    bool Empty(){
        return count_ == 0;
    }
};

static EventListClass EventList;

bool PushEvent(Event* event){
    return EventList.PushEvent(event);
}

#ifdef _WIN32
bool PollEvent(Event* event){


    if (EventList.Empty()){
        // 除了耗时上限，这里还可以加一些关于处理的 msg 数量上线
        // 应该至少还增加了一个 event
        // end_time = GetTime() + 1;
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && EventList.Count() < 32) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            // if check(msg.time, end_time) {
            //    break;
            //}
        }

        std::cout << "end dispatch" << std::endl;
        
    }

    return EventList.PopHeadEvent(event);
}
#endif

}