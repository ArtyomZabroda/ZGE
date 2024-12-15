#include <render_mesh.h>
#include <log.h>
#include <filesystem>

std::optional<zge::RenderMesh> zge::RenderMesh::Create(
    ID3D11Device* device,
    const char* path,
    TextureManager& texture_mgr) {
  RenderMesh mesh;
  Assimp::Importer importer;
  const aiScene* scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    LOG(ERROR) << importer.GetErrorString();
    return std::nullopt;
  }
  mesh.ProcessNode(scene->mRootNode, scene,
                   std::filesystem::path(path).parent_path().string().c_str(), texture_mgr);
  mesh.geometry = MeshGeometry(device, mesh.vertices_, mesh.indices_, mesh.subsets_);
  return mesh;
}

void zge::RenderMesh::ProcessNode(aiNode* node,
                                  const aiScene* scene,
                                  const char* directory,
                                  TextureManager& texture_mgr) {
  for (unsigned int i{}; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    ProcessMesh(mesh, scene, directory, texture_mgr);
  }
  for (unsigned int i{}; i < node->mNumChildren; ++i) {
    ProcessNode(node->mChildren[i], scene, directory, texture_mgr);
  }
}

void zge::RenderMesh::ProcessMesh(aiMesh* mesh,
                                  const aiScene* scene,
                                  const char* directory,
                                  TextureManager& texture_mgr) {
  Subset subset{.vertex_start = (unsigned int)vertices_.size(),
                .vertex_count = mesh->mNumVertices,
                .face_start = (unsigned int)indices_.size() / 3,
                .face_count = mesh->mNumFaces};
  subsets_.push_back(subset);
  for (unsigned int i{}; i < mesh->mNumVertices; ++i) {
    Vertex vertex;
    vertex.position.x = mesh->mVertices[i].x;
    vertex.position.y = mesh->mVertices[i].y;
    vertex.position.z = mesh->mVertices[i].z;
    vertex.normal.x = mesh->mNormals[i].x;
    vertex.normal.y = mesh->mNormals[i].y;
    vertex.normal.z = mesh->mNormals[i].z;
    if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.tex_coord = vec;
    } else
      vertex.tex_coord = glm::vec2(0.0f, 0.0f);  
    vertices_.push_back(vertex);
  }
  for (unsigned int i{}; i < mesh->mNumFaces; ++i) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j{}; j < face.mNumIndices; ++j) {
      indices_.push_back(face.mIndices[j]);
    }
  }
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuse_maps =
        LoadMaterialTextures(mat, aiTextureType_DIFFUSE, directory, texture_mgr);
    std::vector<Texture> specular_maps =
        LoadMaterialTextures(mat, aiTextureType_SPECULAR, directory, texture_mgr);
    textures_.insert(textures_.end(), diffuse_maps.begin(), diffuse_maps.end());
    textures_.insert(textures_.end(), specular_maps.begin(), specular_maps.end());
  }
}

std::vector<zge::Texture> zge::RenderMesh::LoadMaterialTextures(aiMaterial* mat,
    aiTextureType type,
    const char* directory,
    TextureManager& texture_mgr) {
  std::vector<zge::Texture> textures;
  zge::Texture::TextureType zge_texture_type;
  switch (type) { 
   case aiTextureType_DIFFUSE:
    zge_texture_type = zge::Texture::TextureType::kDiffuse;
    break;
   case aiTextureType_SPECULAR:
    zge_texture_type = zge::Texture::TextureType::kSpecular;
    break;
   default:
     LOG(ERROR) << "Could not load material";
     return textures;
  }

  for (unsigned int i{}; i < mat->GetTextureCount(type); ++i) {
    aiString str;
    mat->GetTexture(type, i, &str);
    textures.push_back(texture_mgr.GetTexture(
        (std::filesystem::path(directory) / str.C_Str()).string().c_str(),
        zge_texture_type));
  }
  return textures;
}
