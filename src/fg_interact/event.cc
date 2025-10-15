#ifdef _WIN32
#include<Windows.h>
#endif

#include<iostream>
#include<unordered_map>

#include"fg_interact/event.h"
#include"fg_utils/utils.h"



namespace fg_interact{


static std::unordered_map<size_t, WindowRegInfo> GlobalWindowRegMap;

void RegisterWindow(const WindowRegInfo& reg_info){
    GlobalWindowRegMap.emplace(reg_info.window_id, reg_info);
}

void UnregisterWindow(size_t window_id){
    GlobalWindowRegMap.erase(window_id);    
}


struct EventList{
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
        VLOG(7) << "PushEvent: " << event->type  << ", " << event->window_id << ", " <<event->time_stamp;
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

static EventList GlobalEventList;

bool PushEvent(Event* event){
    return GlobalEventList.PushEvent(event);
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


bool PollEvent(Event* event){

    if (GlobalEventList.Empty()){
        // 除了耗时上限，这里还可以加一些关于处理的 msg 数量上线
        // 应该至少还增加了一个 event
        // end_time = GetTime() + 1;
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && GlobalEventList.Count() < GlobalEventList.Capacity) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            // if check(msg.time, end_time) {
            //    break;
            //}
        }

        VLOG(7) << "End Dispatch";
        
    }

    if(GlobalEventList.PopHeadEvent(event)){
        // std::cout << event->type  << ", " << event->window_id << ", " <<event->time_stamp << std::endl;
        CHECK(GlobalWindowRegMap.find(event->window_id) != GlobalWindowRegMap.end()) << "window id " << event->window_id << " not registered !!!";
        UpdateState(event, GlobalWindowRegMap[event->window_id].state);
        return true;
    }else{
        return false;
    }
}
#endif

}