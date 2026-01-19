#include "model.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstring>

// 绘制模型
// 遍历模型中包含的所有网格并逐个绘制
void Model::Draw(Shader &shader)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

// 加载模型文件
// 使用 Assimp 库读取模型文件，并处理可能出现的错误
void Model::loadModel(const std::string &path)
{
    Assimp::Importer import;
    // ReadFile 选项说明：
    // aiProcess_Triangulate: 将非三角形面（如四边形）转换为三角形
    // aiProcess_FlipUVs: 翻转纹理坐标的 y 轴（OpenGL 的纹理坐标原点在左下角，而大部分图像格式在左上角）
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);    

    // 检查场景是否加载成功
    // !scene: 场景指针为空
    // AI_SCENE_FLAGS_INCOMPLETE: 标志位显示加载不完整
    // !scene->mRootNode: 根节点为空
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    // 从根节点开始递归处理场景图
    processNode(scene->mRootNode, scene);
}

// 递归处理节点
// node: 当前处理的 Assimp 节点
// scene: Assimp 场景对象，包含所有网格和材质数据
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 处理当前节点引用的所有网格
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // 节点中只存储了网格的索引，实际的网格数据在 scene->mMeshes 中
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));         
    }
    // 递归处理所有子节点
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

// 将 Assimp 的 mesh 数据转换为我们自定义的 Mesh 类
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // 统计纹理坐标集的数量
    int uvSets = 0;
    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i) {
        if (mesh->mTextureCoords[i]) ++uvSets;
    }

    // 遍历网格的每个顶点
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        
        // 1. 处理位置
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        
        // 2. 处理法线
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        } else {
            vertex.Normal = glm::vec3(0.0f);
        }

        // 3. 处理纹理坐标
        // Assimp 允许一个顶点有多个纹理坐标（最多8个），我们只关心第一组（索引0）
        if(mesh->mTextureCoords[0]) 
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        
        vertices.push_back(vertex);
    }
    
    // 处理索引（面数据）
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // 获取面对应的所有顶点索引
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    // 处理材质
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        
        // 加载材质中的纹理（目前只处理了内嵌纹理的逻辑）
        std::vector<Texture> colorMaps = loadMaterialTextures(material, scene);
        
        textures.insert(textures.end(), colorMaps.begin(), colorMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

// 加载材质纹理
// 目前主要处理 GLTF 等格式中的内嵌纹理
std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, const aiScene* scene)
{
    std::vector<Texture> out;
    aiString texPathBase;
    aiString texPathDiff;
    
    // 尝试获取基础颜色纹理（PBR 工作流）或漫反射纹理（传统工作流）
    bool hasBase = (AI_SUCCESS == mat->GetTexture(aiTextureType_BASE_COLOR, 0, &texPathBase));
    bool hasDiff = (AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPathDiff));
    
    const char* query = nullptr;
    if (hasBase) {
        query = texPathBase.C_Str();
    } else if (hasDiff) {
        query = texPathDiff.C_Str();
    }
    
    if (!query || !scene) {
        return out;
    }

    // 检查是否为内嵌纹理（以 * 开头，或者路径指向 scene->mTextures 中的索引）
    const aiTexture* at = scene->GetEmbeddedTexture(query);
   
    Texture t{};
    t.type = "texture_diffuse";
    GLuint tex = 0;
    
    if (at) {
        // 如果是内嵌纹理
        if (at->mHeight == 0) {
            // 压缩格式（如 png/jpg 数据的二进制流）
            int w=0,h=0,ch=0;
            stbi_set_flip_vertically_on_load(false); // 加载模型纹理时通常不需要翻转
            const unsigned char* mem = reinterpret_cast<const unsigned char*>(at->pcData);
            unsigned char* data = stbi_load_from_memory(mem, (int)at->mWidth, &w, &h, &ch, 4);
            if (data) {   
                glGenTextures(1, &tex);
                glBindTexture(GL_TEXTURE_2D, tex);
                
                // 设置纹理参数
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                // 上传纹理数据
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(data);
            }
        } 
        // 这里的逻辑可以扩展处理未压缩的 raw 数据 (at->mHeight > 0)
    } 
    
    t.id = tex;
    if (t.id != 0) {
        out.push_back(t);
    }
    return out;
}
