#include <texture_manager.h>



zge::TextureManager::TextureManager(ID3D11Device* device) 
  : device_{device} {}

zge::Texture zge::TextureManager::GetTexture(const char* texture_path, Texture::TextureType type) {
  auto it = textures_.find(texture_path);
  if (it != textures_.end()) {
    return it->second;
  }
  return textures_.emplace(texture_path, Texture(device_, texture_path, type))
      .first->second;
  ;
}
