#include "fg_utils/log.h"

#ifdef USE_GLOG
namespace fg_utils{

struct GlogInit {
    GlogInit() {
        google::InitGoogleLogging("fg");
        VLOG(1) << "GLOG Inited!";
    }

    ~GlogInit() {
        google::ShutdownGoogleLogging();
    }
};

static GlogInit glog_init;

}
#endif