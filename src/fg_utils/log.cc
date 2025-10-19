#include "fg_utils/log.h"
#include <iomanip>

namespace fg_utils{

void MyPrefixFormatter(std::ostream& s, const google::LogMessage& m, void* /*data*/) {
    s <<  "[FG]["

    // << google::GetLogSeverityName(m.severity())[0]
    // << std::setw(4) << 1900 + m.time().year()
    // << std::setw(2) << 1 + m.time().month()
    // << std::setw(2) << m.time().day()
    // << ' '

    // << std::setw(2) << m.time().hour() << ':'
    // << std::setw(2) << m.time().min()  << ':'
    // << std::setw(2) << m.time().sec() << "."
    // << std::setw(6) << m.time().usec()
    // << ' '

    // << std::setfill(' ') << std::setw(5)
    // << m.thread_id() << std::setfill('0')
    // << ' '
    << m.basename() << ':' << m.line() << "]";
}

struct GlogInit {
    GlogInit() {
        google::InitGoogleLogging("fg");
        FLAGS_logtostderr = 1;
        FLAGS_log_prefix = false;
        // FLAGS_log_prefix = true;
        // google::InstallPrefixFormatter(&MyPrefixFormatter);

        VLOG(1) << "GLOG Inited!";
    }

    ~GlogInit() {
        google::ShutdownGoogleLogging();
    }
};

static GlogInit glog_init;

}