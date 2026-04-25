#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <fg/macros.h>
#include "fg/graphics/opengl/gl.h"

namespace fg::graphics::gl_scene {

// 用来解除对 assimp 头文件的依赖
namespace internal {
struct AssimpMesh;
struct AssimpMaterial;
struct AssimpSceneNode;
}  // namespace internal

struct FG_API GLScene;

struct FG_API GLMesh {
    // 从 aiMesh 构造 GLMesh（处理基础属性）
    GLMesh(const internal::AssimpMesh& assimp_mesh);
    unsigned int GetMaterialIdx() const { return material_idx_; }
    unsigned int GetNumFaces() const { return num_faces_; }
    unsigned int GetNumVertices() const { return num_vertices_; }
    void BindVao() { vao_.Bind(); }
    void UnBindVao() { vao_.UnBind(); }
    void Delete();

private:
    opengl::VAO vao_;
    opengl::VBO vbo_;
    opengl::EBO ebo_;

    unsigned int material_idx_;
    std::vector<std::string> texture_coords_names_;

    unsigned num_colors_;
    unsigned num_textures_;
    unsigned num_faces_;
    unsigned num_vertices_;

    // mesh 类型，比如 triangles, polygons, lines
    // 这是一个 bitwise mix flag，意味着可能出现的 primitive type
    // 见 game/third_party/assimp/include/assimp/mesh.h
    // 的 aiPrimitiveType enum
    unsigned int primitive_types_;
};

// Assimp 的 material 用 string 记录 texture path
// 并且 material 中的 texture 会分配一个类型
struct FG_API GLMaterial {
    GLMaterial(const internal::AssimpMaterial& assimp_material, GLScene* scene);
    const std::string ToString() const;
    const std::unordered_map<unsigned int, std::vector<std::string>>&
    GetTexturePathes() {
        return texture_paths_;
    }

private:
    std::unordered_map<unsigned int, std::vector<std::string>> texture_paths_;
};

// 用 shared ptr weak ptr 写这个 node 太折磨了
// 我们这里 vector 不存指针，直接存 obj，会自动析构
// chilren_，这里也不用自己写析构，就这样吧
struct FG_API GLSceneNode {
    GLSceneNode(const internal::AssimpSceneNode& assimp_node,
                GLSceneNode* parent);
    GLSceneNode* FindNode(const std::string& node_name);
    const std::string& GetName() const { return name_; }
    const std::vector<GLSceneNode>& GetChildren() const { return children_; }
    const std::vector<unsigned int>& GetMeshIdxs() const { return mesh_idxes_; }
    const std::string ToString() const;
    const std::string TreeString() const;

private:
    std::string name_;
    std::vector<unsigned int> mesh_idxes_;

    GLSceneNode* parent_;
    std::vector<GLSceneNode> children_;
    // glm::mat4 transformation_;
};

struct FG_API GLScene {
    bool Load(const std::string& path);
    void DrawMesh(unsigned int mesh_idx, const opengl::ShaderProgram& program);
    void DrawNode(const std::string& node_name,
                  const opengl::ShaderProgram& program,
                  bool recursively = true);
    void DrawNode(GLSceneNode* node,
                  const opengl::ShaderProgram& program,
                  bool recursively = true);
    void DrawRoot(const opengl::ShaderProgram& program);
    GLSceneNode* FindNode(const std::string& node_name);
    const std::string& GetDirectory() { return directory_; }
    const std::string& GetPath() { return path_; }
    std::unordered_map<std::string, opengl::Texture2D>* GetTextures() {
        return &textures_;
    }
    ~GLScene();

private:
    std::string path_;
    std::string directory_;

    std::shared_ptr<GLSceneNode> root_;
    std::vector<GLMesh> meshes_;
    // 有的纹理是直接放在模型文件中的，叫做嵌入纹理，存在 embed_textures_
    // 而用另一位文件，比如 .jpg 之类的叫做外部纹理，存在 materials_ 中
    // 这里为了防止重复 load 不同 material 共用的纹理，增加一个 texture_
    std::vector<GLMaterial> materials_;
    std::unordered_map<std::string, opengl::Texture2D> textures_;
    std::vector<opengl::Texture2D> embed_textures_;
};

}  // namespace fg::graphics::gl_scene
