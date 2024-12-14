#include "mesh_geometry.h"

zge::MeshGeometry::MeshGeometry(ID3D11Device* device,
                                const std::vector<Vertex>& vertices,
                                const std::vector<unsigned int>& indices,
                                const std::vector<Subset>& subsets)
    : subset_table_{subsets} {
  D3D11_BUFFER_DESC vbd{
      .ByteWidth = (UINT)(sizeof(Vertex) * vertices.size()),
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_VERTEX_BUFFER,
      .CPUAccessFlags = 0,
      .MiscFlags = 0,
      .StructureByteStride = 0,
  };
  D3D11_SUBRESOURCE_DATA v_init_data{.pSysMem = vertices.data()};

  CHECK(SUCCEEDED(device->CreateBuffer(&vbd, &v_init_data, &vertex_buffer_)))
      << "Failed to create a vertex buffer";

  D3D11_BUFFER_DESC ibd{
      .ByteWidth = (UINT)(sizeof(unsigned int) * indices.size()),
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_INDEX_BUFFER,
      .CPUAccessFlags = 0,
      .MiscFlags = 0,
      .StructureByteStride = 0};

  D3D11_SUBRESOURCE_DATA i_init_data{.pSysMem = indices.data()};
  CHECK(SUCCEEDED(device->CreateBuffer(&ibd, &i_init_data, &index_buffer_)))
      << "Failed to create an index buffer";
}

void zge::MeshGeometry::Draw(ID3D11DeviceContext* dc, unsigned int subset_id) const {
  unsigned int offset = 0;
  dc->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &vertex_stride_,
                         &offset);
  dc->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

  dc->DrawIndexed(subset_table_[subset_id].face_count * 3,
                  subset_table_[subset_id].face_start * 3,
                  subset_table_[subset_id].vertex_start);
}