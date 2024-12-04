#include <render_mesh.h>

zge::RenderMesh::RenderMesh(ID3D11Device* device,
                            const std::vector<Vertex>& vertices) {
  D3D11_BUFFER_DESC vbd{
      .ByteWidth = (UINT)(sizeof(Vertex) * vertices.size()),
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_VERTEX_BUFFER,
      .CPUAccessFlags = 0,
      .MiscFlags = 0,
      .StructureByteStride = 0,
  };
  D3D11_SUBRESOURCE_DATA v_init_data {.pSysMem = vertices.data()};

  CHECK(SUCCEEDED(device->CreateBuffer(&vbd, &v_init_data, &vertex_buffer_)))
      << "Failed to create a vertex buffer";
}
