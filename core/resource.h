#ifndef CORE_RESOURCE_H_
#define CORE_RESOURCE_H_

#include <unordered_map>
#include <memory>
#include <string>
#include <any>
#include <fstream>
#include <format>
#include <filesystem>
#include <boost/leaf.hpp>
#include <boost/url.hpp>

namespace core {

// TODO: change std classes to exception free alternatives
class ResourceManager {
 public:
  ResourceManager(const std::filesystem::path& root_path)
      : root_path_{root_path} {}

  template<typename T>
  boost::leaf::result<std::shared_ptr<T>> Get(boost::url_view url) {
    if (auto it = resources_cache_.find(url);
        it != resources_cache_.end()) {
      if (!it->second.expired()) {
        std::shared_ptr<T> obj;
        std::shared_ptr<std::any> any_obj;
        any_obj = it->second.lock();
        T* value_ptr = std::any_cast<T>(any_obj.get());
        obj = std::shared_ptr<T>(any_obj, value_ptr);
        if (obj) return obj;
      }
      
    }
    BOOST_LEAF_AUTO(new_obj, Add<T>(url));
    return new_obj;
  }

 private:
  template<typename T>
  boost::leaf::result<std::shared_ptr<T>> Add(boost::url_view url) {
    std::string path = url.path();
    if (!path.empty() && path[0] == '/') path.erase(0, 1);

    std::ifstream ifs(root_path_ / path, std::ios::binary);
    if (!ifs.is_open()) {
      return boost::leaf::new_error(
          std::format("Could not get a resource \"{}\"", url.data()));
    }

    std::shared_ptr<std::any> obj = std::make_shared<std::any>();
    obj->emplace<T>();
    ifs >> *std::any_cast<T>(obj.get());  // TODO: check bytes amount
    resources_cache_.emplace(url, obj);
    return std::shared_ptr<T>(obj, std::any_cast<T>(obj.get()));
  }

  std::unordered_map<boost::url, std::weak_ptr<std::any>> resources_cache_{};
  std::filesystem::path root_path_;
};



}

#endif