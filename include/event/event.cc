#include<stdio.h>
#include<Windows.h>
#include"event/event.h"


namespace fei_window{


struct EventListClass{
    static constexpr capacity_ = 512;

    Event[capacity_] events_;
    unsigned int head_ = 0;
    unsigned int tail_ = 0;
    unsigned int count_ = 0;

    int Count(){
        return count_;
    }

    // SDL 里面的 event 也是拷贝的，反正先写了再说
    bool PushEvent(Event* event){
        if (count_ == capacity_){
            MessageBoxW(NULL, L"EventList full when push", L"Error", MB_ICONERROR);
            return false;
        }
        events_[tail_] = *event;
        tail_ = (tail_ + 1) % capacity_;
        count_ += 1
        return true;
    }

    bool PopHeadEvent(Event* event){
        if (count_ == 0){
            MessageBoxW(NULL, L"EventList empty when pop", L"Error", MB_ICONERROR);
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

bool PollEvent(Event* event){

    if (event_list.Empty()){
        // 除了耗时上限，这里还可以加一些关于处理的 msg 数量上线
        // 应该至少还增加了一个 event
        // end_time = GetTime() + 1;
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            // if check(msg.time, end_time) {
            //    break;
            //}
        }
    }

    return event_list.PopHeadEvent(event);
}

}