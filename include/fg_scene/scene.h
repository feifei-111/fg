#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include "fg_gl/gl.h"
#include "fg_macros.h"


namespace fg_scene{

struct FG_API Scene;

struct FG_API Mesh {
    unsigned int VAO, VBO, EBO;
    // 从 aiMesh 构造 Mesh（处理基础属性）
    Mesh(aiMesh* aiMesh);
    unsigned int GetMaterialIdx() const {return material_idx_;}
    unsigned int GetNumFaces() const {return num_faces_;}
    unsigned int GetNumVertices() const {return num_vertices_;}
    void BindVao(){vao_.Bind();}
    void UnBindVao(){vao_.UnBind();}
    void Delete();

private:
    fg_gl::VAO vao_;
    fg_gl::VBO vbo_;
    fg_gl::EBO ebo_;
    
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
struct FG_API Material{
    Material(aiMaterial* material, Scene* scene);
    const std::string DebugStr() const;
    const std::unordered_map<unsigned int, std::vector<std::string>>& GetTexturePathes(){
        return texture_paths_;
    }
private:
    std::unordered_map<unsigned int, std::vector<std::string>> texture_paths_;
};

// 用 shared ptr weak ptr 写这个 node 太折磨了
// 我们这里 vector 不存指针，直接存 obj，会自动析构 chilren_，这里也不用自己写析构，就这样吧
struct FG_API SceneNode{
    SceneNode(aiNode* node, SceneNode* parent=nullptr);
    SceneNode* FindNode(const std::string& node_name);
    const std::string& GetName() const {return name_;}
    const std::vector<SceneNode>& GetChildren() const {return children_;}
    const std::vector<unsigned int>& GetMeshIdxs() const {return mesh_idxes_;}
    const std::string DebugStr() const;
    const std::string TreeDebugStr() const;
private:
    std::string name_;
    std::vector<unsigned int> mesh_idxes_;

    SceneNode* parent_;
    std::vector<SceneNode> children_;
    // glm::mat4 transformation_;
};

struct Scene{
    bool Load(const std::string &path);
    void DrawMesh(unsigned int mesh_idx, const fg_gl::ShaderProgram& program);
    void DrawNode(const std::string& node_name, const fg_gl::ShaderProgram& program, bool recursively=true);
    void DrawNode(SceneNode* node, const fg_gl::ShaderProgram& program, bool recursively=true);
    void DrawRoot(const fg_gl::ShaderProgram& program);
    SceneNode* FindNode(const std::string& node_name);
    const std::string& GetDirectory(){return directory_;}
    const std::string& GetPath(){return path_;}
    std::unordered_map<std::string, fg_gl::Texture2D>* GetTextures(){return &textures_;}
    ~Scene();

private:
    std::string path_;
    std::string directory_;

    std::shared_ptr<SceneNode> root_;
    std::vector<Mesh> meshes_;
    // 有的纹理是直接放在模型文件中的，叫做嵌入纹理，存在 embed_textures_
    // 而用另一位文件，比如 .jpg 之类的叫做外部纹理，存在 materials_ 中
    // 这里为了防止重复 load 不同 material 共用的纹理，增加一个 texture_
    std::vector<Material> materials_;
    std::unordered_map<std::string, fg_gl::Texture2D> textures_;
    std::vector<fg_gl::Texture2D> embed_textures_;
};

}