#ifndef RENDER_RENDERER_H_
#define RENDER_RENDERER_H_

#include <window.h>
#include <extent.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <render_mesh.h>
#include <vector>
#include <render_camera.h>
#include <entt/entity/registry.hpp>
#include <debug_layer.h>
#include <lights.h>
#include <texture_manager.h>

namespace zge {

struct PerObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
  Material mat;
};

struct PerFrame {
  DirectionalLight dir_light;
  glm::vec3 eye_pos;
  float pad;
};

class Renderer {
 public:
  Renderer(Window* window, entt::registry* registry, entt::entity camera_entity, DebugLayer* debug_layer);
  void DrawFrame();
  void DrawMesh(const RenderMesh& mesh, 
                ID3D11VertexShader* vs,
                ID3D11PixelShader* ps);
  ID3D11Device* Device() { return device_.Get(); }
  ID3D11DeviceContext* ImmediateContext() { return immediate_context_.Get(); }
  RenderCamera& Camera() { return camera_; }
 private:
  Window* window_;
  entt::registry* registry_;
  DebugLayer* debug_layer_;
  Microsoft::WRL::ComPtr<ID3D11Device> device_;
  Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context_;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer_;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
  Microsoft::WRL::ComPtr<ID3D11Buffer> per_object_cbuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> per_frame_cbuffer_;

  D3D11_VIEWPORT screen_viewport_;

  void OnResized(Extent2D new_extent);

  std::vector<Vertex> vertices {
      {glm::vec3(0.5f, 0.5f, 0.0f)},
      {glm::vec3(0.5f, -0.5f, 0.0f)},
      {glm::vec3(-0.5f, -0.5f, 0.0f)},
      {glm::vec3(-0.5f, 0.5f, 0.0f)}
  };

  std::vector<unsigned int> indices{
      0, 1, 3,  // first triangle
      1, 2, 3   // second triangle
  };
  RenderMesh mesh;

  entt::entity camera_entity_;
  RenderCamera camera_;

  std::optional<TextureManager> texture_manager_;
};


}

#endif