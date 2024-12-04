#ifndef RENDER_RENDER_MESH_H_
#define RENDER_RENDER_MESH_H_

#include <vector>
#include <glm/glm.hpp>
#include <wrl/client.h>
#include <d3d11.h>
#include <log.h>

namespace zge {

struct Vertex {
  glm::vec3 position;
};

class Renderer;

class RenderMesh {
 public:
  friend Renderer;
  RenderMesh(ID3D11Device* device, const std::vector<Vertex>& vertices);
 private:
  Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
};

}

#endif