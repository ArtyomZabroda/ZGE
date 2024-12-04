#include <log.h>

void zge::InitLogging(const char* app_path) {
  google::InitGoogleLogging(app_path);
  FLAGS_stderrthreshold = google::GLOG_INFO;
}