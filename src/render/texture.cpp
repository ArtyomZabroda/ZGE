#include <texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

zge::Texture::Texture(ID3D11Device* device, const char* path, TextureType type)
    : type_{type} {
  int width, height, nr_channels;
  unsigned char* data =
      stbi_load(path, &width, &height, &nr_channels, STBI_rgb_alpha);

  D3D11_TEXTURE2D_DESC tex_desc{.Width = (unsigned int)width,
                                .Height = (unsigned int)height,
                                .MipLevels = 1,
                                .ArraySize = 1,
                                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                                .SampleDesc = {.Count = 1, .Quality = 0},
                                .Usage = D3D11_USAGE_DEFAULT,
                                .BindFlags = D3D11_BIND_SHADER_RESOURCE,
                                .CPUAccessFlags = 0,
                                .MiscFlags = 0};

  D3D11_SUBRESOURCE_DATA subresource_data{
      .pSysMem = data, .SysMemPitch = (unsigned int)width * 4};

  device->CreateTexture2D(&tex_desc, &subresource_data,
                           texture_.GetAddressOf());

  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{
      .Format = tex_desc.Format,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D = {.MostDetailedMip = 0, .MipLevels = 1}};

  device->CreateShaderResourceView(texture_.Get(), &srv_desc, &srv_);

  stbi_image_free(data);
}

void zge::Texture::SetSampler(unsigned int slot, ID3D11DeviceContext* context) {
  context->PSSetShaderResources(0, 1, srv_.GetAddressOf());
}
