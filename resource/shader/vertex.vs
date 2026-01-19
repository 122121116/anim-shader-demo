#version 330 core
// 顶点属性输入
layout(location = 0) in vec3 aPos;       // 顶点位置
layout(location = 1) in vec3 aNormal;    // 顶点法线
layout(location = 2) in vec2 aTexCoords; // 纹理坐标

// 全局 uniform 变量
uniform mat4 model;         // 模型矩阵
uniform mat4 view;          // 视图矩阵
uniform mat4 projection;    // 投影矩阵
uniform float outlineWidth; // 描边宽度 (通常在 0.02-0.05 之间)
uniform vec3 viewPos;       // 相机/观察者位置

// 输出到片元着色器的变量
out vec3 FragPos;   // 世界空间中的片元位置
out vec3 Normal;    // 世界空间中的法线
out vec2 TexCoords; // 纹理坐标
out float vIsOutline; // 标记当前片元是否属于描边

void main() {
    // 1. 计算世界空间位置和法线
    vec4 worldPos = model * vec4(aPos, 1.0);
    // 法线矩阵：模型矩阵逆转置的左上 3x3 部分，用于正确变换非均匀缩放下的法线
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 worldNormal = normalize(normalMatrix * aNormal);
    
    // 2. 计算视线方向（世界空间）
    vec3 viewDir = normalize(viewPos - worldPos.xyz);
    
    // 3. 简单的描边逻辑：顶点沿法线膨胀
    // 计算视线与法线的点积
    // ndotv > 0 表示面朝向观察者（正面），ndotv < 0 表示背向观察者（背面）
    // 注意：这里使用的是简单的背面剔除思想的变体来做描边
    // 实际上通常的做法是绘制两遍：第一遍正常绘制，第二遍剔除正面并沿法线膨胀绘制背面作为描边
    // 这个着色器尝试在一个 Pass 中做一些处理（通过 vIsOutline 标记）
    float ndotv = dot(worldNormal, viewDir);
    
    vec4 finalPos = worldPos;
    vIsOutline = 0.0;
    
    // 如果法线朝向观察者（这里逻辑可能与常规描边相反，取决于具体需求）
    // 通常描边是在背面顶点上进行的
    if (ndotv > 0.0) {
        // 沿法线方向挤出顶点位置
        finalPos.xyz += worldNormal * outlineWidth;
        vIsOutline = 1.0;
    }
    
    // 传递数据给片元着色器
    FragPos = finalPos.xyz;
    Normal = worldNormal;
    TexCoords = aTexCoords;
    
    // 计算裁剪空间坐标
    gl_Position = projection * view * finalPos;
}
