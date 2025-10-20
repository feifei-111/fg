#include "fg_scene/scene.h"
#include <numeric>
#include "fg_utils/utils.h"

namespace fg_scene {

/* ============================ Mesh ============================ */
struct VertexAttr {
    enum AttrType {
        kVertex = 0,
        kTextureCoord1D = 1,
        kTextureCoord2D = 2,
        kTextureCoord3D = 3,  // 最多支持到 3D coord
        kNormal = 4,
        kTangent = 5,    // u 向切线
        kBitangent = 6,  // v 向切线
        kColor = 7
        // 关于 bone 和 animation 暂时没搞
    };
    VertexAttr();
    VertexAttr(AttrType type, unsigned int idx, unsigned int size)
        : type(type), idx(idx), size(size) {}
    AttrType type;
    unsigned int idx;  // 多个 color texture 存在时的 idx
    unsigned int size;

    const std::string DebugStr() const {
        std::ostringstream ss;
        ss << "type:" << static_cast<unsigned int>(type) << ", idx:" << idx
           << ", size:" << size;
        return ss.str();
    }
};

// 参考 game/third_party/assimp/include/assimp/mesh.h
Mesh::Mesh(aiMesh* ai_mesh) {
    primitive_types_ = ai_mesh->mPrimitiveTypes;
    material_idx_ = ai_mesh->mMaterialIndex;
    num_faces_ = ai_mesh->mNumFaces;
    num_vertices_ = ai_mesh->mNumVertices;

    // step1. 初始化 vertex attr meta 信息
    std::vector<VertexAttr> attr_meta;
    unsigned int data_width = 0;

    // Vertex
    attr_meta.emplace_back(VertexAttr::kVertex, 0, 3);
    data_width += 3;

    // Normal
    if (ai_mesh->mNormals) {
        attr_meta.emplace_back(VertexAttr::kNormal, 0, 3);
        data_width += 3;
    }

    // texture
    num_textures_ = 0;
    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
        if (ai_mesh->mTextureCoords[i] && ai_mesh->mNumUVComponents[i] > 0 &&
            ai_mesh->mNumUVComponents[i] < 4) {
            attr_meta.emplace_back(
              static_cast<VertexAttr::AttrType>(ai_mesh->mNumUVComponents[i]),
              i,
              ai_mesh->mNumUVComponents[i]);
            data_width += ai_mesh->mNumUVComponents[i];
            num_textures_++;
        }
    }

    // tangents
    if (ai_mesh->mTangents) {
        attr_meta.emplace_back(VertexAttr::kTangent, 0, 3);
        data_width += 3;
    }

    // bitangent
    if (ai_mesh->mBitangents) {
        attr_meta.emplace_back(VertexAttr::kBitangent, 0, 3);
        data_width += 3;
    }

    // color
    num_colors_ = 0;
    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; i++) {
        if (ai_mesh->mColors[i]) {
            attr_meta.emplace_back(VertexAttr::kColor, i, 4);
            data_width += 4;
            num_colors_++;
        }
    }

    if (VLOG_IS_ON(6)) {
        VLOG(6) << "attr_meta";
        for (const VertexAttr& va : attr_meta) {
            VLOG(6) << "    " << va.DebugStr();
        }
    }

    if (ai_mesh->mTextureCoordsNames) {
        for (unsigned int i = 0; i < num_textures_; i++) {
            texture_coords_names_.emplace_back(
              ai_mesh->mTextureCoordsNames[i]->C_Str());
        }
    }

    // step2. 填充 vbo buffer
    // 我们用一个 vector 记录数据，因为 ai_mesh 中的不同数据放在不同的地方，而
    // opengl 需要放到一起 此外，assimp load 的数据就是 float，所以不用考虑 vbo
    // dtype 了 如果有混合类型，只能用 vector<char> + reinterpret_cast 逐个 byte
    // 赋值了
    std::vector<float> vbo_buf;

    for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
        for (const VertexAttr& attr : attr_meta) {
            switch (attr.type) {
                case VertexAttr::kVertex:
                    vbo_buf.push_back(ai_mesh->mVertices[i].x);
                    vbo_buf.push_back(ai_mesh->mVertices[i].y);
                    vbo_buf.push_back(ai_mesh->mVertices[i].z);
                    break;
                case VertexAttr::kNormal:
                    vbo_buf.push_back(ai_mesh->mNormals[i].x);
                    vbo_buf.push_back(ai_mesh->mNormals[i].y);
                    vbo_buf.push_back(ai_mesh->mNormals[i].z);
                    break;
                case VertexAttr::kTangent:
                    vbo_buf.push_back(ai_mesh->mTangents[i].x);
                    vbo_buf.push_back(ai_mesh->mTangents[i].y);
                    vbo_buf.push_back(ai_mesh->mTangents[i].z);
                    break;
                case VertexAttr::kBitangent:
                    vbo_buf.push_back(ai_mesh->mBitangents[i].x);
                    vbo_buf.push_back(ai_mesh->mBitangents[i].y);
                    vbo_buf.push_back(ai_mesh->mBitangents[i].z);
                    break;
                case VertexAttr::kColor:
                    vbo_buf.push_back(ai_mesh->mColors[attr.idx][i].r);
                    vbo_buf.push_back(ai_mesh->mColors[attr.idx][i].g);
                    vbo_buf.push_back(ai_mesh->mColors[attr.idx][i].b);
                    vbo_buf.push_back(ai_mesh->mColors[attr.idx][i].a);
                    break;
                case VertexAttr::kTextureCoord1D:
                    vbo_buf.push_back(ai_mesh->mTextureCoords[attr.idx][i].x);
                    break;
                case VertexAttr::kTextureCoord2D:
                    vbo_buf.push_back(ai_mesh->mTextureCoords[attr.idx][i].x);
                    vbo_buf.push_back(ai_mesh->mTextureCoords[attr.idx][i].y);
                    break;
                case VertexAttr::kTextureCoord3D:
                    vbo_buf.push_back(ai_mesh->mTextureCoords[attr.idx][i].x);
                    vbo_buf.push_back(ai_mesh->mTextureCoords[attr.idx][i].y);
                    vbo_buf.push_back(ai_mesh->mTextureCoords[attr.idx][i].z);
                    break;
            }
        }
    }

    // step3. 填充 ebo buffer
    // 注意了，这里如果是有多种不同的
    // faces，那么就需要分开才行，我们就默认这里就用三角形的，这个可以在 import
    // 的时候指定参数保证
    std::vector<unsigned int> ebo_buf;
    for (unsigned int i = 0; i < num_faces_; i++) {
        const aiFace& face = ai_mesh->mFaces[i];
        CHECK(face.mNumIndices == 3) << "face must be triangle";
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            ebo_buf.push_back(face.mIndices[j]);
        }
    }

    // step4. 初始化 vao, vbo, ebo
    CHECK(data_width * num_vertices_ == vbo_buf.size()) << "load vbo error";
    CHECK(3 * num_faces_ == ebo_buf.size()) << "load ebo error";

    if (VLOG_IS_ON(8)) {
        std::ostringstream ss;
        unsigned int idx = 0;
        VLOG(8) << "num vertex" << num_vertices_ << " data_width " << data_width
                << " vbo_buf.size() " << vbo_buf.size();
        VLOG(8) << "vbo buf";
        for (int i = 0; i < num_vertices_; i++) {
            ss = std::ostringstream();
            // ss << "line " << i << ":  ";
            for (int j = 0; j < data_width; j++) {
                ss << vbo_buf[idx++] << " ";
            }
            VLOG(8) << ss.str();
        }

        VLOG(8) << "ebo buf";
        for (int i = 0; i < num_faces_; i++) {
            ss = std::ostringstream();
            for (int j = 0; j < 3; j++) {
                ss << ebo_buf[i * 3 + j] << " ";
            }
            VLOG(8) << ss.str();
        }
    }

    vao_.Init();
    vbo_.Init(vbo_buf.data(), data_width, num_vertices_, GL_STATIC_DRAW);
    ebo_.Init(
      ebo_buf.data(), ebo_buf.size() * sizeof(unsigned int), GL_STATIC_DRAW);

    vao_.Bind();
    vbo_.Bind();
    ebo_.Bind();

    for (const VertexAttr& attr : attr_meta) {
        vbo_.SetAttr(attr.size, 0);
    }

    vao_.UnBind();
    vbo_.UnBind();
    ebo_.UnBind();
}

void Mesh::Delete() {
    vao_.Delete();
    vbo_.Delete();
    ebo_.Delete();
}

/* ============================ Material ============================ */
// 对应 assimp 里面定义的 material 类型
static const std::string GetMaterialTypeStr(unsigned int mat_type) {
    switch (mat_type) {
        case 1:
            return "Diffuse";
        case 2:
            return "Specular";
        case 3:
            return "Ambient";
        case 4:
            return "Emissive";
        case 5:
            return "Height";
        case 6:
            return "Normals";
        case 7:
            return "Shininess";
        case 8:
            return "Opacity";
        case 9:
            return "Displacement";
        case 10:
            return "LightMap";
        case 11:
            return "Reflection";
        default:
            return "";
    }
}

// 参考 game/third_party/assimp/include/assimp/material.h
Material::Material(aiMaterial* material, Scene* scene) {
    for (unsigned int mat_type_val = 1; mat_type_val <= 11; mat_type_val++) {
        aiTextureType mat_type = static_cast<aiTextureType>(mat_type_val);

        for (unsigned int i = 0; i < material->GetTextureCount(mat_type); i++) {
            aiString str;
            material->GetTexture(mat_type, i, &str);
            // check if texture was loaded before and if so, continue to next
            // iteration: skip loading a new texture
            std::string texture_path = scene->GetDirectory() + str.C_Str();

            if (scene->GetTextures()->find(texture_path) ==
                scene->GetTextures()->end()) {
                scene->GetTextures()->emplace(texture_path, texture_path);
            }
            texture_paths_[mat_type_val].emplace_back(texture_path);
        }
    }
    VLOG(6) << DebugStr();
}

const std::string Material::DebugStr() const {
    std::ostringstream ss;
    ss << "Material:\n";
    for (const auto& pair : texture_paths_) {
        ss << "    " << GetMaterialTypeStr(pair.first) << ": ";
        for (const auto& path : pair.second) {
            ss << path << ", ";
        }
        ss << "\n";
    }
    return ss.str();
}

/* ============================ SceneNode ============================ */
SceneNode::SceneNode(aiNode* node, SceneNode* parent) {
    name_ = node->mName.C_Str();
    parent_ = parent;
    for (int i = 0; i < node->mNumMeshes; i++) {
        mesh_idxes_.emplace_back(node->mMeshes[i]);
    }
    for (int i = 0; i < node->mNumChildren; i++) {
        children_.emplace_back(node->mChildren[i], this);
    }
}

const std::string SceneNode::DebugStr() const {
    if (mesh_idxes_.size() == 0) return name_;
    std::ostringstream ss;
    ss << name_ << ": [" << mesh_idxes_[0];
    for (int i = 1; i < mesh_idxes_.size(); i++) {
        ss << "," << mesh_idxes_[i];
    }
    ss << "]";
    return ss.str();
}

SceneNode* SceneNode::FindNode(const std::string& node_name) {
    if (name_ == node_name)
        return this;
    else {
        for (int i = 0; i < children_.size(); i++) {
            SceneNode* result = children_[i].FindNode(node_name);
            if (result) return result;
        }
    }
    return nullptr;
}

static const std::string PrefixMarkToStr(int mark, bool transform) {
    if (transform) {
        switch (mark) {
            case 0:
            case 1:
                return " |    ";
            case 2:
            case 3:
                return "      ";
        }
    } else {
        switch (mark) {
            case 0:
                return " |--- ";
            case 1:
                return " |    ";
            case 2:
                return " +--- ";
            case 3:
                return "      ";
        }
    }
}

static void TreeDebugStrImpl(const SceneNode* node,
                             std::ostringstream* log,
                             std::vector<int>* prefix) {
    for (int i = 0; i < prefix->size(); i++) {
        *log << PrefixMarkToStr((*prefix)[i], i != (prefix->size() - 1));
    }
    *log << node->DebugStr() << "\n";

    auto children = node->GetChildren();
    for (int i = 0; i < children.size(); i++) {
        if (i == children.size() - 1) {
            prefix->emplace_back(2);
        } else {
            prefix->emplace_back(0);
        }
        TreeDebugStrImpl(&children[i], log, prefix);
        prefix->pop_back();
    }
}

const std::string SceneNode::TreeDebugStr() const {
    std::ostringstream log;
    std::vector<int> prefix;
    log << "NodeTree\n";
    TreeDebugStrImpl(this, &log, &prefix);
    return log.str();
}

/* ============================ Scene ============================ */
bool Scene::Load(const std::string& path) {
    VLOG(6) << "Load model: " << path;
    path_ = path;
    directory_ = path.substr(0, path.find_last_of('/')) + "/";

    Assimp::Importer importer;
    const aiScene* scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs
                        // | aiProcess_GenSmoothNormals
                        // | aiProcess_CalcTangentSpace
      );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        VLOG(6) << "assimp ReadFile failed";
        return false;
    }
    VLOG(6) << "assimp ReadFile success";

    VLOG(6) << "begin parse node";
    root_ = std::make_shared<SceneNode>(scene->mRootNode);
    VLOG(6) << root_->TreeDebugStr();
    VLOG(6) << "end parse node";

    VLOG(6) << "begin parse mesh";
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        VLOG(6) << "parse mesh " << i;
        meshes_.emplace_back(scene->mMeshes[i]);
    }
    VLOG(6) << "end parse mesh";

    VLOG(6) << "begin parse material";
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        materials_.emplace_back(scene->mMaterials[i], this);
    }
    VLOG(6) << "end parse material";

    VLOG(5) << "Scene Load End:"
            << "\n    Num mesh: " << meshes_.size()
            << "\n    Num materials: " << materials_.size()
            << "\n    Num texture: " << textures_.size()
            << "\n    Path: " << path_ << "\n    Directory: " << directory_;
    return true;
}

void Scene::DrawMesh(unsigned int mesh_idx,
                     const fg_gl::ShaderProgram& program) {
    // VLOG(6) << "DrawMesh " << mesh_idx;
    Mesh* mesh = &meshes_[mesh_idx];

    std::unordered_map<unsigned int, unsigned int> counter_map;
    unsigned int texture_unit_cnt = 0;

    VLOG(7) << "DrawMesh With: " << mesh->GetNumVertices() << " vertices, "
            << mesh->GetNumFaces() << " faces \n"
            << materials_[mesh->GetMaterialIdx()].DebugStr();

    for (const auto& pair_data :
         materials_[mesh->GetMaterialIdx()].GetTexturePathes()) {
        unsigned int mat_type = pair_data.first;
        for (const std::string& path : pair_data.second) {
            textures_[path].BindUnit(texture_unit_cnt++);
            std::stringstream ss;
            ss << "Tex" << GetMaterialTypeStr(mat_type)
               << counter_map[mat_type];
            counter_map[mat_type] += 1;
            VLOG(6) << "Bind " << path << " as " << ss.str() << " to unit "
                    << textures_[path].CurrentUnit();
            program.UniformInt(ss.str(), textures_[path].CurrentUnit());
        }
    }

    fg_gl::Texture2D::SimpleSetParameter();

    mesh->BindVao();
    // 我们就默认会 draw triangle 否则太麻烦了
    glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, 0);

    mesh->UnBindVao();
    glActiveTexture(GL_TEXTURE0);
}

SceneNode* Scene::FindNode(const std::string& node_name) {
    return root_->FindNode(node_name);
}

static void DrawSceneNode(SceneNode* node,
                          Scene* scene,
                          bool recursively,
                          const fg_gl::ShaderProgram& program) {
    auto mesh_idxs = node->GetMeshIdxs();
    for (unsigned int idx : mesh_idxs) {
        scene->DrawMesh(idx, program);
    }
    if (recursively) {
        auto children = node->GetChildren();
        for (int i = 0; i < children.size(); i++) {
            DrawSceneNode(&children[i], scene, recursively, program);
        }
    }
}

void Scene::DrawNode(const std::string& node_name,
                     const fg_gl::ShaderProgram& program,
                     bool recursively) {
    SceneNode* node = FindNode(node_name);
    CHECK(node) << "DrawNode " << node_name << " but " << node_name
                << " not exist!!";
    DrawSceneNode(node, this, recursively, program);
}

void Scene::DrawNode(SceneNode* node,
                     const fg_gl::ShaderProgram& program,
                     bool recursively) {
    SceneNode* found_node = FindNode(node->GetName());
    CHECK(found_node == node)
      << "DrawNode with pointer, but found node is not correct";
    DrawSceneNode(node, this, recursively, program);
}

void Scene::DrawRoot(const fg_gl::ShaderProgram& program) {
    DrawNode(root_.get(), program);
}

Scene::~Scene() {
    for (auto& mesh : meshes_) {
        mesh.Delete();
    }
    for (auto& pair : textures_) {
        pair.second.Delete();
    }
}

}  // namespace fg_scene
