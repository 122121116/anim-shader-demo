#include "mesh.h"
#include "shader.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// 模型加载类
// 使用 Assimp 库加载 3D 模型文件，并将其转换为 Mesh 对象集合
class Model 
{
    public:
        /*  函数   */
        // 构造函数：加载指定路径的模型
        Model(const char *path)
        {
            loadModel(path);
        }

        // 绘制模型：遍历所有 Mesh 并绘制
        void Draw(Shader &shader);   
    private:
        /*  模型数据  */
        std::vector<Mesh> meshes; // 模型包含的网格列表

        /*  函数   */
        // 加载模型文件的主入口
        void loadModel(const std::string &path);
        
        // 递归处理 Assimp 节点树
        void processNode(aiNode *node, const aiScene *scene);
        
        // 将 Assimp 的 mesh 数据转换为自定义 Mesh 类
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        
        // 加载材质纹理
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, const aiScene* scene);
};
