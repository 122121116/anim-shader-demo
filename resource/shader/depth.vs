#version 330 core

// ---------------------------------------------------------
// 阴影深度贴图生成的顶点着色器
// ---------------------------------------------------------

layout(location = 0) in vec3 aPos; // 顶点位置

uniform mat4 model;            // 模型矩阵
uniform mat4 lightSpaceMatrix; // 光空间矩阵 (这通常用于平行光阴影，但本例是点光源万向阴影)
                               // 注意：对于万向阴影(Cubemap Shadow)，通常不需要 lightSpaceMatrix。
                               // 这里的 lightSpaceMatrix 可能是用于几何着色器之前的变换，或者如果是简单的深度渲染
                               // 实际上点光源阴影通常在几何着色器中处理 6 个面。
                               // 如果没有几何着色器，可能是在 CPU 端多次绘制 6 个面，或者这里只是个通用的深度 Pass。
                               // 
                               // 结合 pixel shader 来看，这里似乎没有用到几何着色器？
                               // 检查 depth_frag.vs 发现它只计算线性深度。
                               // 如果是生成 Cubemap 阴影，通常需要几何着色器将三角形发射到 6 个面。
                               // 或者应用程序端对每个面做一次绘制调用。

out vec4 FragPos; // 输出世界空间位置

void main() {
    // 计算世界空间位置
    FragPos = model * vec4(aPos, 1.0);
    
    // 如果是传统的 Shadow Map，这里会是 gl_Position = lightSpaceMatrix * FragPos;
    // 但如果是有 Geometry Shader 或者特殊的设置，可能不同。
    // 在本例的代码中：gl_Position = lightSpaceMatrix * FragPos;
    // 这看起来像是用于定向光阴影或者单面阴影。
    // 但 depth_frag.vs 计算的是 distance / farPlane，这是点光源阴影的特征。
    // 这种组合比较特殊，可能是通过 geometry shader 或者是多次 pass 渲染。
    gl_Position = lightSpaceMatrix * FragPos;
}
