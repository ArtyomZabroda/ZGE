#ifndef RENDER_RENDERER_H_
#define RENDER_RENDERER_H_

#include <window.h>
#include <extent.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <render_mesh.h>
#include <vector>
#include <render_camera.h>

namespace zge {

struct PerObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

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
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
  Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
  Microsoft::WRL::ComPtr<ID3D11Buffer> per_object_cbuffer;

  D3D11_VIEWPORT screen_viewport_;

  void OnResized(Extent2D new_extent);

  Vertex vertices[3]{
      {glm::vec3(-0.5f, -0.5f, 0.0f)},
      {glm::vec3(0.0f, 0.5f, 0.0f)},
      {glm::vec3(0.5f, -0.5f, 0.0f)}
  };

  RenderCamera camera_;
};


}

#endif