#ifndef RENDER_RENDERER_H_
#define RENDER_RENDERER_H_

#include <window.h>
#include <extent.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <render_mesh.h>

namespace zge {

class Renderer {
 public:
  Renderer(Window* window);
  void DrawFrame();
  void DrawMesh(const RenderMesh& mesh, 
                ID3D11VertexShader* vs,
                ID3D11PixelShader* ps);
 private:
  Window* window_;
  Microsoft::WRL::ComPtr<ID3D11Device> device_;
  Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context_;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer_;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> pixel_shader_;

  D3D11_VIEWPORT screen_viewport_;

  void OnResized(Extent2D new_extent);
};


}

#endif