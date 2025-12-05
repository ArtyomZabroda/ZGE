#include <gtest/gtest.h>
#include "resource.h"

TEST(ResourceTest, GettingResource) {
  core::ResourceManager resource_manager(std::filesystem::temp_directory_path());
  std::ofstream ofs{std::filesystem::temp_directory_path() / "helloworld.bin",
                    std::ios::binary};
  ofs << 7;
  ofs.close();
  boost::leaf::result<std::shared_ptr<int>> helloworld_data =
      resource_manager.Get<int>("res:/helloworld.bin");
  EXPECT_TRUE(helloworld_data.has_value());
  EXPECT_NE(helloworld_data.value(), nullptr);
  EXPECT_EQ(*helloworld_data.value().get(), 7);
}


TEST(ResourceTest, ResourceCaching) {
  core::ResourceManager resource_manager(
      std::filesystem::temp_directory_path());
  std::ofstream ofs{std::filesystem::temp_directory_path() / "helloworld.bin",
                    std::ios::binary};
  ofs << 7;
  ofs.close();
  boost::leaf::result<std::shared_ptr<int>> helloworld_data =
      resource_manager.Get<int>("res:/helloworld.bin");
  boost::leaf::result<std::shared_ptr<int>> helloworld_data_cached =
      resource_manager.Get<int>("res:/helloworld.bin");
  EXPECT_TRUE(helloworld_data.has_value());
  EXPECT_EQ(helloworld_data.value().get(), helloworld_data_cached.value().get());
}

TEST(ResourceTest, ResourcesGetDeleted) {
  core::ResourceManager resource_manager(
      std::filesystem::temp_directory_path());
  std::ofstream ofs{std::filesystem::temp_directory_path() / "helloworld.bin",
                    std::ios::binary};
  ofs << 7;
  ofs.close();
  boost::leaf::result<std::shared_ptr<int>> helloworld_data =
      resource_manager.Get<int>("res:/helloworld.bin");
  EXPECT_TRUE(helloworld_data.has_value());

  std::weak_ptr weak = helloworld_data.value();

  boost::leaf::result<std::shared_ptr<int>> helloworld_data_cached =
      resource_manager.Get<int>("res:/helloworld.bin");
  int* intptr = helloworld_data.value().get();
  EXPECT_EQ(helloworld_data_cached.value().use_count(), 2);
  helloworld_data.value().reset();
  EXPECT_EQ(helloworld_data_cached.value().use_count(), 1);
  helloworld_data_cached.value().reset();
  EXPECT_TRUE(weak.expired());

}