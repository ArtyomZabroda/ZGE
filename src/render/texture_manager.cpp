#include <texture_manager.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


zge::TextureManager::TextureManager(ID3D11Device* device) 
  : device_{device} {}

zge::TextureId zge::TextureManager::GetTexture(const char* texture_path) {
  int width, height, nr_channels;
  unsigned char* data =
      stbi_load(texture_path, &width, &height, &nr_channels, STBI_rgb_alpha);

  D3D11_TEXTURE2D_DESC tex_desc{
    .Width = (unsigned int)width,
    .Height = (unsigned int)height,
    .MipLevels = 1,
    .ArraySize = 1,
    .Format = DXGI_FORMAT_R8G8B8A8_UNORM, 
    .SampleDesc = {.Count = 1, .Quality = 0},
    .Usage = D3D11_USAGE_DEFAULT,
    .BindFlags = D3D11_BIND_SHADER_RESOURCE,
    .CPUAccessFlags = 0,
    .MiscFlags = 0
  };

  D3D11_SUBRESOURCE_DATA subresource_data{.pSysMem = data,
  .SysMemPitch = (unsigned int)width * 4};

  Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_srv;

  device_->CreateTexture2D(&tex_desc, &subresource_data, texture.GetAddressOf());

  D3D11_SHADER_RESOURCE_VIEW_DESC sh_v_desc{
      .Format = tex_desc.Format,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D = {.MostDetailedMip = 0, .MipLevels = 1}};

  device_->CreateShaderResourceView(texture.Get(), &sh_v_desc, &texture_srv);

  stbi_image_free(data);
  return 0;
}
