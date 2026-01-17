#include "mesh.h"
#include "shader.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Model 
{
    public:
        /*  函数   */
        Model(const char *path)
        {
            loadModel(path);
        }

        void Draw(Shader &shader);   
    private:
        /*  模型数据  */
        std::vector<Mesh> meshes;

        /*  函数   */
        void loadModel(const std::string &path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, const aiScene* scene);
};
