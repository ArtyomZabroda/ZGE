#include "renderer.h"

#include <cassert>
#include <log.h>
#include <winerror.h>
#include <fstream>

zge::Renderer::Renderer(Window* window) : window_{window} {
  LOG(INFO) << "Creating a renderer";
  DCHECK(window != nullptr);

  unsigned int flags = NULL;

#ifndef NDEBUG
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  Extent2D extent = window->Extent();

  DXGI_SWAP_CHAIN_DESC sc_desc{
      .BufferDesc = {.Width = static_cast<UINT>(extent.width),
                     .Height = static_cast<UINT>(extent.height),
                     .RefreshRate = {.Numerator = 60, .Denominator = 1},
                     .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                     .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
                     .Scaling = DXGI_MODE_SCALING_UNSPECIFIED},
      .SampleDesc = {.Count = 1, .Quality = 0},
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = 1,
      .OutputWindow = window->GetNativeHandle(),
      .Windowed = true,
      .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
      .Flags = 0
  };

  D3D_FEATURE_LEVEL feature_level;
  CHECK(SUCCEEDED(D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0,
      D3D11_SDK_VERSION, &sc_desc, &swap_chain_, &device_, &feature_level,
      &immediate_context_)))
      << "Renderer intialization has failed";

  OnResized(extent);

  window->Resized().Connect(
      [this](Extent2D new_extent) { OnResized(new_extent); });

  LOG(INFO) << "Renderer has been created successfully";
}

void zge::Renderer::DrawFrame() {
  float color[4]{0.2, 0.3, 0.3, 1};
  immediate_context_->ClearRenderTargetView(render_target_view_.Get(), color);
  immediate_context_->ClearDepthStencilView(
      depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  if (FAILED(swap_chain_->Present(0, 0))) {
    LOG(ERROR) << "Could not present a frame";
  }
}

void zge::Renderer::OnResized(Extent2D new_extent) {
  LOG(INFO) << "Starting to resize the renderer to Extent2D(" << new_extent.width
            << ", " << new_extent.height << ')';
  if (render_target_view_ != nullptr) render_target_view_->Release();
  if (depth_stencil_view_ != nullptr) depth_stencil_view_->Release();
  if (depth_stencil_buffer_ != nullptr) depth_stencil_buffer_->Release();

  CHECK(SUCCEEDED(swap_chain_->ResizeBuffers(1, (unsigned int)new_extent.width,
                                             (unsigned int)new_extent.height,
                                             DXGI_FORMAT_R8G8B8A8_UNORM, NULL)))
      << "Failed to resize swap chain buffers";

  Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
  CHECK(SUCCEEDED(
      swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer)))
      << "Failed to get back buffer";
  CHECK(SUCCEEDED(device_->CreateRenderTargetView(back_buffer.Get(), 0,
                                                  render_target_view_.GetAddressOf())))
      << "Failed to create a render target view";

  D3D11_TEXTURE2D_DESC depth_stencil_desc{
      .Width = new_extent.width,
      .Height = new_extent.height,
      .MipLevels = 1,
      .ArraySize = 1,
      .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
      .SampleDesc = {.Count = 1, .Quality = 0},
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_DEPTH_STENCIL,
      .CPUAccessFlags = 0,
      .MiscFlags = 0};

  CHECK(SUCCEEDED(
      device_->CreateTexture2D(&depth_stencil_desc, 0, depth_stencil_buffer_.GetAddressOf())))
      << "Could not create a depth stencil buffer";

  CHECK(SUCCEEDED(device_->CreateDepthStencilView(depth_stencil_buffer_.Get(), 0, depth_stencil_view_.GetAddressOf())))
      << "Could not create a depth stencil view";

  immediate_context_->OMSetRenderTargets(1, render_target_view_.GetAddressOf(),
                                         depth_stencil_view_.Get());

  D3D11_VIEWPORT screen_viewport{
    .TopLeftX = 0,
    .TopLeftY = 0,
    .Width = (float)new_extent.width,
    .Height = (float)new_extent.height,
    .MinDepth = 0.0f,
    .MaxDepth = 1.0f
  };
  screen_viewport_ = screen_viewport;

  immediate_context_->RSSetViewports(1, &screen_viewport_);
  LOG(INFO) << "Renderer has been resized";
}

void zge::Renderer::DrawMesh(const zge::RenderMesh& mesh,
                             ID3D11VertexShader* vs,
                             ID3D11PixelShader* ps) {
  
}