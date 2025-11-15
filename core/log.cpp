#include "log.h"
void core::SingleThreadedLogger::Log(SeverityLevel severity, std::string_view msg) {
  BOOST_LOG_SEV(logger_, severity) << msg;
}
void core::SingleThreadedLogger::LogTrace(std::string_view msg) {
  Log(SeverityLevel::kTrace, msg);
}

void core::SingleThreadedLogger::LogDebug(std::string_view msg) {
  Log(SeverityLevel::kDebug, msg);
}

void core::SingleThreadedLogger::LogInfo(std::string_view msg) {
  Log(SeverityLevel::kInfo, msg);
}

void core::SingleThreadedLogger::LogWarning(std::string_view msg) {
  Log(SeverityLevel::kWarning, msg);
}

void core::SingleThreadedLogger::LogError(std::string_view msg) {
  Log(SeverityLevel::kError, msg);
}

void core::SingleThreadedLogger::LogFatal(std::string_view msg) {
  Log(SeverityLevel::kFatal, msg);
}
