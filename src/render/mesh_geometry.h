#ifndef RENDER_MESH_GEOMETRY_H_
#define RENDER_MESH_GEOMETRY_H_

#include <d3d11.h>
#include <vector>
#include <wrl/client.h>
#include <glm/glm.hpp>
#include <log.h>

namespace zge {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coord;
};

struct Subset {
  unsigned int id;
  unsigned int vertex_start;
  unsigned int vertex_count;
  unsigned int face_start;
  unsigned int face_count;
};

class MeshGeometry {
 public:
  MeshGeometry(ID3D11Device* device,
               const std::vector<Vertex>& vertices,
               const std::vector<unsigned int>& indices,
               const std::vector<Subset>& subsets);
  void Draw(ID3D11DeviceContext* dc, unsigned int subset_id) const;
 private: 
  Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
  Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;

  unsigned int vertex_stride_ = sizeof(Vertex);
  std::vector<Subset> subset_table_;
};

}  // namespace zge

#endif