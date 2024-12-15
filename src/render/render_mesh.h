#ifndef RENDER_RENDER_MESH_H_
#define RENDER_RENDER_MESH_H_

#include <vector>
#include <glm/glm.hpp>
#include <wrl/client.h>
#include <d3d11.h>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <mesh_geometry.h>
#include <optional>
#include <texture_manager.h>

namespace zge {

class Renderer;

struct Material {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec4 reflect;
};

class RenderMesh {
 public:
  friend Renderer;
  static std::optional<RenderMesh> Create(ID3D11Device* device, const char* path, TextureManager& texture_mgr);
 private:
  RenderMesh() {};
  void ProcessNode(aiNode* node,
                   const aiScene* scene,
                   const char* directory,
                   TextureManager& texture_mgr);
  void ProcessMesh(aiMesh* mesh,
                   const aiScene* scene,
                   const char* directory,
                   TextureManager& texture_mgr);
  std::vector<Texture> LoadMaterialTextures(aiMaterial* mat,
                                            aiTextureType type,
                                            const char* directory,
                                            TextureManager& texture_mgr);

  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<Subset> subsets_;

  std::vector<Texture> textures_;

  std::optional<MeshGeometry> geometry;

};

}

#endif