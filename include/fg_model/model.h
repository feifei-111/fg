#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include "fg_gl/gl.h"


namespace fg_model{

struct Mesh {
    // 从 aiMesh 构造 Mesh（处理基础属性）
    Mesh(aiMesh* aiMesh);
    unsigned int GetMaterialIdx() const {
        return material_idx_;
    }
    unsigned int GetNumFaces() const {
        return num_faces_;
    }
    void BindVao(){
        vao_.Bind();
    }
    void UnBindVao(){
        vao_.UnBind();
    }

private:
    fg_gl::VAO vao_;
    fg_gl::VBO vbo_;
    fg_gl::EBO ebo_;
    
    unsigned num_colors_;
    unsigned num_textures_;
    unsigned num_faces_;

    // mesh 类型，比如 triangles, polygons, lines
    // 这是一个 bitwise mix flag，意味着可能出现的 primitive type
    // 见 game/third_party/assimp/include/assimp/mesh.h
    // 的 aiPrimitiveType enum
    unsigned int primitive_types_;
    unsigned int material_idx_;
    std::vector<std::string> texture_coords_namse_;
};

// Assimp 的 material 用 string 记录 texture path
// 并且 material 中的 texture 会分配一个类型
struct Material{
    std::unordered_map<unsigned int, std::vector<std::string>> texture_paths_;
};

struct Model{
    std::vector<Mesh> mesh_;
    std::unordered_map<std::string, fg_gl::Texture2D> texture_;
    std::vector<Material> material_;
    std::string path_;

    bool Load(const std::string &path);
    bool ProcessNode(aiNode* node, const aiScene* scene);
    bool ProcessMaterial(aiMaterial* material, const aiScene* scene);

    void DrawMesh(unsigned int mesh_idx, const fg_gl::ShaderProgram& program);
    void Draw(const fg_gl::ShaderProgram& program);
};

}