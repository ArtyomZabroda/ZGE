#include "renderer.h"

#include <cassert>
#include <log.h>
#include <winerror.h>
#include <fstream>
#include <utils.h>
#include <transform.h>
#include <mesh_instance.h>
#include <parent.h>
#include <lights.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

zge::Renderer::Renderer(Window* window,
                        entt::registry* registry,
                        entt::entity camera_entity,
                        DebugLayer* debug_layer)
    : window_{window},
      registry_{registry},
      camera_{glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
      camera_entity_{camera_entity},
      debug_layer_{debug_layer} {
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

  std::vector<std::byte> vert_shader_bytes, pixel_shader_bytes;
  CHECK(GetBytesFromFile("vert.vso", vert_shader_bytes))
      << "Failed to read the vertex shader file";
  CHECK(GetBytesFromFile("pixel.pso", pixel_shader_bytes))
      << "Failed to read the pixel shader file";

  CHECK(SUCCEEDED(device_->CreateVertexShader(vert_shader_bytes.data(),
                              vert_shader_bytes.size(), nullptr,
                                              &vertex_shader_)))
      << "Failed to create a vertex shader";

  CHECK(SUCCEEDED(device_->CreatePixelShader(pixel_shader_bytes.data(),
                             pixel_shader_bytes.size(), nullptr,
                                             &pixel_shader_)))
      << "Failed to create a pixel shader";

  D3D11_INPUT_ELEMENT_DESC vertex_desc[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal),
       D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, tex_coord),
       D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  device_->CreateInputLayout(
      vertex_desc, sizeof(vertex_desc) / sizeof(D3D11_INPUT_ELEMENT_DESC),
                             vert_shader_bytes.data(),
                             vert_shader_bytes.size(), &input_layout_);

  D3D11_BUFFER_DESC cbd{.ByteWidth = sizeof(PerObject),
                        .Usage = D3D11_USAGE_DYNAMIC,
                        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
                        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
                        .MiscFlags = 0,
                        .StructureByteStride = 0};

  CHECK(SUCCEEDED(device_->CreateBuffer(&cbd, nullptr, &per_object_cbuffer)))
      << "Failed to create a per object constant buffer";

  D3D11_BUFFER_DESC cpfbd{.ByteWidth = sizeof(PerFrame),
                        .Usage = D3D11_USAGE_DYNAMIC,
                        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
                        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
                        .MiscFlags = 0,
                        .StructureByteStride = 0};

  CHECK(SUCCEEDED(device_->CreateBuffer(&cpfbd, nullptr, &per_frame_cbuffer_)))
      << "Failed to create a per frame constant buffer";

  mesh = RenderMesh::Create(device_.Get(), "assets/meshes/backpack/backpack.obj").value();

  debug_layer_->RenderInit(device_.Get(), immediate_context_.Get());

  texture_manager_ = TextureManager(device_.Get());
  texture_manager_->GetTexture("assets/meshes/backpack/diffuse.jpg");

  LOG(INFO) << "Renderer has been created successfully";
}

void zge::Renderer::DrawFrame() {
  float color[4]{0.2, 0.3, 0.3, 1};
  immediate_context_->ClearRenderTargetView(render_target_view_.Get(), color);
  immediate_context_->ClearDepthStencilView(
      depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  immediate_context_->IASetInputLayout(input_layout_.Get());
  immediate_context_->IASetPrimitiveTopology(
      D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  immediate_context_->VSSetShader(vertex_shader_.Get(), nullptr, 0);
  immediate_context_->PSSetShader(pixel_shader_.Get(), nullptr, 0);

  for (auto&& [entity, transform, mesh_instance] :
       registry_->view<zge::Transform, zge::MeshInstance>().each()) {
    zge::Parent* parent;
    glm::mat4 model;
    model = glm::mat4(1);
    model = glm::translate(model, transform.position);
    model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
    model = glm::scale(model, transform.scale);

    glm::mat4 camera_model =
        CalculateGlobalModelTransform(*registry_, camera_entity_);

    camera_.SetPosition(glm::vec3(camera_model[3]));
    glm::vec3 rotation_r;
    glm::extractEulerAngleYXZ(camera_model, rotation_r.y, rotation_r.x, rotation_r.z);
    glm::vec3 rot_euler = glm::degrees(rotation_r);
    camera_.SetRotation(rot_euler);

    PerObject pb;
    pb.model = model;
    pb.view = camera_.View();
    pb.proj = camera_.Proj();
    pb.mat = Material{
      .ambient = glm::vec4(0.48f, 0.77f, 0.46f, 1.0f),
      .diffuse = glm::vec4(0.48f, 0.77f, 0.46f, 1.0f),
      .specular = glm::vec4(0.2f, 0.2f, 0.2f, 16.0f)
    };

    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    immediate_context_->Map(per_object_cbuffer.Get(), 0,
                            D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    memcpy(mapped_resource.pData, &pb, sizeof(pb));
    immediate_context_->Unmap(per_object_cbuffer.Get(), 0);
    immediate_context_->VSSetConstantBuffers(0, 1,
                                             per_object_cbuffer.GetAddressOf());
    immediate_context_->PSSetConstantBuffers(0, 1,
                                             per_object_cbuffer.GetAddressOf());
    DrawMesh(mesh, vertex_shader_.Get(), pixel_shader_.Get());
  }

  PerFrame pf;
  pf.eye_pos = camera_.Position();
  for (auto&& [entity, dir_light1] :
       registry_->view<zge::DirectionalLight>().each()) {

    pf.dir_light = dir_light1;

    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    immediate_context_->Map(per_frame_cbuffer_.Get(), 0,
                            D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    memcpy(mapped_resource.pData, &pf, sizeof(pf));
    immediate_context_->Unmap(per_frame_cbuffer_.Get(), 0);
    immediate_context_->PSSetConstantBuffers(1, 1,
                                             per_frame_cbuffer_.GetAddressOf());
  }

  debug_layer_->Render();

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

  for (unsigned int subset{}; subset < mesh.subsets_.size(); ++subset) {
    mesh.geometry->Draw(immediate_context_.Get(), subset);
  }
}