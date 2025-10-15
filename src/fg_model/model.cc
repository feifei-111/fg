#include <vector>
#include <sstream>

#include "fg_model/model.h"


namespace fg_model{

struct VertexAttr{
    enum AttrType{
        kNone,
        kTextureCoord1D = 1,
        kTextureCoord2D = 2,
        kTextureCoord3D = 3,    // 最多支持到 3D coord
        kVertex,
        kNormal,
        kTangent, // u 向切线
        kBitangent, // v 向切线
        kColor
        // 关于 bone 和 animation 暂时没搞
    };
    VertexAttr();
    VertexAttr(AttrType type, unsigned int idx, unsigned int size):type(type),idx(idx),size(size){}
    AttrType type;
    unsigned int idx;   // 多个 color texture 存在时的 idx
    unsigned int size;
};


const std::string& GetMaterialTypeStr(unsigned int mat_type){
    switch(mat_type){
        case 1 : return "Diffuse";
        case 2 : return "Specular";
        case 3 : return "Ambient";
        case 4 : return "Emissive";
        case 5 : return "Height";
        case 6 : return "Normals";
        case 7 : return "Shininess";
        case 8 : return "Opacity";
        case 9 : return "Displacement";
        case 10 : return "LightMap";
        case 11 : return "Reflection";
        default: return std::string();
    }
}

// 参考 game/third_party/assimp/include/assimp/mesh.h
Mesh::Mesh(aiMesh* ai_mesh) {
    primitive_types_ = ai_mesh->mPrimitiveTypes;
    material_idx_ = ai_mesh->mMaterialIndex;
    num_faces_ = ai_mesh->mNumVertices;

    // step1. 初始化 vertex attr meta 信息
    std::vector<VertexAttr> attr_meta;

    attr_meta.emplace_back(VertexAttr::kVertex, 0, 3);
    if (ai_mesh->mNormals){
        attr_meta.emplace_back(VertexAttr::kNormal, 0, 3);
    }
    if (ai_mesh->mTangents){
        attr_meta.emplace_back(VertexAttr::kTangent, 0, 3);
    }
    if (ai_mesh->mBitangents){
        attr_meta.emplace_back(VertexAttr::kBitangent, 0, 3);
    }
    num_colors_ = AI_MAX_NUMBER_OF_COLOR_SETS;
    for (unsigned int i=0; i<AI_MAX_NUMBER_OF_COLOR_SETS; i++){
        if (ai_mesh->mColors[i]){
            attr_meta.emplace_back(VertexAttr::kColor, i, 4);
        }else {
            num_colors_ = i;
            break;
        }
    }
    num_textures_ = AI_MAX_NUMBER_OF_TEXTURECOORDS;
    for (unsigned int i=0; i<AI_MAX_NUMBER_OF_TEXTURECOORDS; i++){
        if (ai_mesh->mTextureCoords[i] && ai_mesh->mNumUVComponents[i] > 0 && ai_mesh->mNumUVComponents[i] < 4){
            attr_meta.emplace_back(
                static_cast<VertexAttr::AttrType>(ai_mesh->mNumUVComponents[i]), 
                i, ai_mesh->mNumUVComponents[i]
            );
        }else {
            num_textures_ = i;
            break;
        }
    }
    for (unsigned int i=0; i<num_textures_; i++){
        texture_coords_namse_.emplace_back(
            ai_mesh->mTextureCoordsNames[i]->C_Str()
        );
    }

    // step2. 填充 vbo buffer
    // 我们用一个 vector 记录数据，因为 ai_mesh 中的不同数据放在不同的地方，而 opengl 需要放到一起
    // 此外，assimp load 的数据就是 float，所以不用考虑 vbo dtype 了
    // 如果有混合类型，只能用 vector<char> + reinterpret_cast 逐个 byte 赋值了
    std::vector<float> vbo_buf;

    for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
        for (const VertexAttr& attr : attr_meta){
            switch(attr.type){
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
    // 注意了，这里如果是有多种不同的 faces，那么就需要分开才行，我们就默认这里就用三角形的，这个可以在 import 的时候指定参数保证
    std::vector<unsigned int> ebo_buf;
    for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
        const aiFace& face = ai_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            ebo_buf.push_back(face.mIndices[j]);
        }
    }
    num_faces_ = ai_mesh->mNumFaces;

    // step4. 初始化 vao, vbo, ebo
    vbo_.Init(
        vbo_buf.data(), 
        vbo_buf.size() / ai_mesh->mNumVertices, 
        ai_mesh->mNumVertices,
        GL_STATIC_DRAW
    );

    ebo_.Init(ebo_buf.data(), ebo_buf.size() * sizeof(unsigned int), GL_STATIC_DRAW);

    vao_.Bind();
    vbo_.Bind();
    ebo_.Bind();

    for (const VertexAttr& attr : attr_meta){
        vbo_.SetAttr(attr.size, 0);
    }

    vao_.UnBind();
}

bool Model::Load(const std::string &path){
    path_ = path;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        return false;
    }
    ProcessNode(scene->mRootNode, scene);
    for(unsigned int i=0; i<scene->mNumMaterials; i++){
        ProcessMaterial(scene->mMaterials[i], scene);
    }
    return true;
}

bool Model::ProcessNode(aiNode* node, const aiScene* scene){
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        mesh_.emplace_back(mesh);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++){
        ProcessNode(node->mChildren[i], scene);
    }
}

// 参考 game/third_party/assimp/include/assimp/material.h
bool Model::ProcessMaterial(aiMaterial* material, const aiScene* scene){
    // 这里 assimp 的 material 会将 texture 根据功能分类对 texture 进行划分
    // 对应的贴图参考上面头文件中的 enum aiTextureType 

    // 1 ~ 11 是 legacy material texture，也就是 diffuse specular 之类，还有法线贴图等
    // 12 ~ 17 是 PBR material，现代游戏基本都是用 PBR 模型
    // 剩下还有一些包括 PBR material modifiers 作为 PBR 模型的扩展
    // 还有一些特定场景的纹理，比如纺织物
    // 还有关于 MAYA 的纹理形式（MAYA 和 blender 都是用来建模的）

    // 这里我们就先看 legacy 的部分，PBR 的后面加

    material_.emplace_back();
    Material* this_mat = &(material_.back());

    for(unsigned int mat_type_val=1; mat_type_val<=11; mat_type_val++){
        aiTextureType mat_type = static_cast<aiTextureType>(mat_type_val);

        for(unsigned int i = 0; i < material->GetTextureCount(mat_type); i++){
            aiString str;
            material->GetTexture(mat_type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            std::string texture_path = str.C_Str();

            if (texture_.find(texture_path) == texture_.end()){
                texture_.emplace(texture_path, texture_path);
            }
            this_mat->texture_paths_[i].emplace_back(texture_path);
        }
    }
    return true;
}

void Model::DrawMesh(unsigned int mesh_idx, const fg_gl::ShaderProgram& program){
    Mesh* mesh = &mesh_[mesh_idx];

    std::unordered_map<unsigned int, unsigned int> counter_map;
    unsigned int texture_unit_cnt = 0;

    for(const auto& pair_data : material_[mesh->GetMaterialIdx()].texture_paths_){
        unsigned int mat_type = pair_data.first;
        for (const std::string& path : pair_data.second){
            texture_[path].BindUnit(texture_unit_cnt++);
            std::stringstream ss;
            ss << "Tex" << GetMaterialTypeStr(mat_type) << counter_map[mat_type];
            counter_map[mat_type] += 1;
            program.UniformInt(ss.str(), texture_[path].CurrentUnit());
        }
    }
    
    mesh->BindVao();
    // 我们就默认会 draw triangle 否则太麻烦了
    glDrawElements(GL_TRIANGLES, mesh->GetNumFaces(), GL_UNSIGNED_INT, 0);
    
    mesh->UnBindVao();
    glActiveTexture(GL_TEXTURE0);
}

void Model::Draw(const fg_gl::ShaderProgram& program){
    for (unsigned int i=0; i<mesh_.size(); i++){
        DrawMesh(i, program);
    }
}

}