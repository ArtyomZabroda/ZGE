#ifndef CORE_LOG_H_
#define CORE_LOG_H_

#define GLOG_USE_GLOG_EXPORT
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

namespace zge {

void InitLogging(const char* app_path);

}

#endif