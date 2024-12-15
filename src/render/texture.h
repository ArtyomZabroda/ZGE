#ifndef RENDER_TEXTURE_H_
#define RENDER_TEXTURE_H_

#include <d3d11.h>
#include <wrl/client.h>

namespace zge {

class Texture {
 public:
  enum class TextureType { kDiffuse, kSpecular };

  Texture(ID3D11Device* device, const char* path, TextureType type);
  TextureType Type() const { return type_; };
  void SetSampler(unsigned int slot, ID3D11DeviceContext* context);
 private:
  Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
  TextureType type_;
};

}



#endif