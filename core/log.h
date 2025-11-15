#ifndef LOG_H_
#define LOG_H_

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <string_view>

namespace core {

enum class SeverityLevel {
  kTrace,
  kDebug,
  kInfo,
  kWarning,
  kError,
  kFatal
};

class ILogger {
 public:
  virtual ~ILogger() = default;
  virtual void Log(SeverityLevel severity, std::string_view msg) = 0;
  virtual void LogTrace(std::string_view msg) = 0;
  virtual void LogDebug(std::string_view msg) = 0;
  virtual void LogInfo(std::string_view msg) = 0;
  virtual void LogWarning(std::string_view msg) = 0;
  virtual void LogError(std::string_view msg) = 0;
  virtual void LogFatal(std::string_view msg) = 0;
};

class SingleThreadedLogger : public ILogger {
 public:
  void Log(SeverityLevel severity, std::string_view msg) override;
  void LogTrace(std::string_view msg) override;
  void LogDebug(std::string_view msg) override;
  void LogInfo(std::string_view msg) override;
  void LogWarning(std::string_view msg) override;
  void LogError(std::string_view msg) override;
  void LogFatal(std::string_view msg) override;
 private:
  boost::log::sources::severity_logger<SeverityLevel> logger_;
};

}

#endif