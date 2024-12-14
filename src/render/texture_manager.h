#ifndef RENDER_TEXTURE_MANAGER_H_
#define RENDER_TEXTURE_MANAGER_H_

#include <map>
#include <d3d11.h>
#include <wrl/client.h>

namespace zge {

using TextureId = unsigned int;

class TextureManager {
 public:
  TextureManager(ID3D11Device* device);
  TextureId GetTexture(const char* texture_path);
 private:
  ID3D11Device* device_;
};

}

#endif