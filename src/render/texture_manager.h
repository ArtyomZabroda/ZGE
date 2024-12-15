#ifndef RENDER_TEXTURE_MANAGER_H_
#define RENDER_TEXTURE_MANAGER_H_

#include <map>
#include <texture.h>
#include <d3d11.h>
#include <wrl/client.h>

namespace zge {

class TextureManager {
 public:
  TextureManager(ID3D11Device* device);
  Texture GetTexture(const char* texture_path, Texture::TextureType type);
 private:
  ID3D11Device* device_;
  std::map<const char*, Texture> textures_;
};

}

#endif