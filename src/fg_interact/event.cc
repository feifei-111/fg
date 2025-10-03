#ifdef _WIN32
#include<Windows.h>
#endif

#include<iostream>
#include"fg_interact/event.h"



namespace fg_interact{

struct EventListClass{
    static constexpr unsigned int Capacity = 256;

    Event events_[Capacity];
    unsigned int head_ = 0;
    unsigned int tail_ = 0;
    unsigned int count_ = 0;

    int Count(){
        return count_;
    }

    // SDL 里面的 event 也是拷贝的，反正先写了再说
    bool PushEvent(Event* event){
        if (count_ == Capacity){
            return false;
        }
        events_[tail_] = *event;
        tail_ = (tail_ + 1) % Capacity;
        count_ += 1;
        return true;
    }

    bool PopHeadEvent(Event* event){
        if (count_ == 0){
            return false;
        }
        *event = events_[head_];
        head_ = (head_ + 1) % Capacity;
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

static void UpdateState(Event* event, WindowState* state){
    switch(event->type){
        case EventType::kMouseMoveEvent:
            state->mouse_x = event->data.mouse_move_event.x;
            state->mouse_y = event->data.mouse_move_event.y;
            break;
        case EventType::kMouseClickEvent:
            state->mouse_x = event->data.mouse_click_event.x;
            state->mouse_y = event->data.mouse_click_event.y;
            switch(event->data.mouse_click_event.move){
                case ButtonMove::DOWN:
                    state->button_map[size_t(event->data.mouse_click_event.button)] = true;
                    break;
                case ButtonMove::UP:
                    state->button_map[size_t(event->data.mouse_click_event.button)] = false;
                    break;
            }
            break;
        case EventType::kMouseWheelEvent:
            state->mouse_x = event->data.mouse_wheel_event.x;
            state->mouse_y = event->data.mouse_wheel_event.y;
            break;
        case EventType::kKeyboardEvent:
            switch(event->data.keyboard_event.move){
                case ButtonMove::DOWN:
                    state->button_map[size_t(event->data.keyboard_event.button)] = true;
                    break;
                case ButtonMove::UP:
                    state->button_map[size_t(event->data.keyboard_event.button)] = false;
                    break;
            }
    }
}

#ifdef _WIN32


bool PollEvent(Event* event, WindowState* state){

    if (EventList.Empty()){
        // 除了耗时上限，这里还可以加一些关于处理的 msg 数量上线
        // 应该至少还增加了一个 event
        // end_time = GetTime() + 1;
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && EventList.Count() < EventList.Capacity) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            // if check(msg.time, end_time) {
            //    break;
            //}
        }

        // std::cout << "end dispatch" << std::endl;
        
    }

    if(EventList.PopHeadEvent(event)){
        UpdateState(event, state);
        return true;
    }else{
        return false;
    }
}
#endif

}