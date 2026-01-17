# 图形学大作业项目说明

本项目基于 Glad + GLFW + OpenGL3 渲染，并使用 Dear ImGui 作为调试与 UI。第三方库统一放置在 `thirdparty` 目录中，工程使用 CMake 构建。


## 主要需要实现的内容：
- 自定义法线：避免普通平滑法线的不规则阴影。可以通过**法线贴图**解决。所以需要实现发现贴图导入和应用模块。如果模型自带法线就不使用法线贴图。
- **ILM贴图**：实现对模型的ILM贴图渲染。（力大砖飞，同时也意味着要下载和使用有ILM的模型）
- **描边算法**：背面膨胀算法，需要实现对模型的描边渲染。优化该算法问题（待定）
- 基础光照模型：包括环境光、漫反射光、镜面反射光。
- **ui（widget）**：使用imgui实现相机控制以及参数调整，包括模型导入、法线贴图导入、环境光、漫反射光、镜面反射光的参数调整。
- 场景搭建（目前导入个模型和墙壁就行，后续再说）
- 时间够的话可以添加动画功能
- 其他渲染必要组件
尽量只修改自己负责部分的代码，避免影响到其他部分的功能。创建本地分支进行开发，合并时只合并自己负责的部分。测试文件放在`test/`目录中，需要在本地配置test.exe（修改本地makelist，上传代码时不要传修改后的cmakelist）


## 项目结构
- `src/`：项目源码，入口为 `main.cpp`
- `include/`：项目头文件（可按需新增）
- `resource/`：资源文件（模型、纹理、**着色器**等）
- `thirdparty/`：第三方库源代码（精简版本）
  - `glfw/`：GLFW 源码（只用于 Windows + OpenGL）
  - `imgui/`：Dear ImGui 源码（只保留 `glfw` 与 `opengl3` 后端）
  - `assimp/`：Assimp 源码（仅保留 OBJ 导入能力，其他格式已裁剪）stb_image.h也配置到assimp的include目录中
- `build/`：CMake 生成的构建输出目录
- `CMakeLists.txt`：顶层构建配置

典型目录示意：

```
root/
├─ src/
│  └─ main.cpp
├─ include/
├─ resource/
├─ thirdparty/
│  ├─ glfw/
│  ├─ imgui/
│  │  └─ backends/ (imgui_impl_glfw, imgui_impl_opengl3)
│  └─ assimp/
│     ├─ code/Common + AssetLib/Obj
│     └─ include/assimp
├─ build/
└─ CMakeLists.txt
```

## 环境要求
- Windows 10/11
- Visual Studio 2022（含 C++ 开发组件）或任意 MSVC/Clang 工具链
- CMake ≥ 3.16
- Git（用于获取第三方依赖）

## 第三方库与精简说明
- GLFW：用于创建窗口与管理 OpenGL 上下文。项目仅面向 Windows，非 Windows/Unix 后端源码已删除。
- Dear ImGui：用于 UI 与调试，保留 `imgui_impl_glfw` 与 `imgui_impl_opengl3` 后端。
- Assimp：用于模型导入，当前仅启用 OBJ 格式（`AssetLib/Obj`），其它格式与示例、测试、文档等已移除以减小体积。
  - 如需更多格式（如 FBX、glTF），可在 CMake 中开启对应导入器并恢复相关目录。

## 配置与构建
1. Virtual Studio方案，直接打开项目，编译后选GraphicsHomework.exe运行

2. cmake
- 在项目根目录生成构建目录并配置：
   ```
   mkdir build
   cd build
   cmake ..
   ```
- 编译：
   ```
   cmake --build .
   ```
   - 成功后可执行文件位于 `build/Debug/GraphicsHomework.exe`（或对应配置）

顶层构建的关键点：
- 自动包含 `thirdparty/imgui` 源文件及后端
- 使用 `add_subdirectory(thirdparty/glfw)` 构建 GLFW
- 集成 Assimp，并关闭不必要的组件，只保留 OBJ 导入
- 在 Windows 上链接 `opengl32` 与 `gdi32`

## 运行
- 直接运行生成的可执行文件：
  ```
  GraphicsHomework.exe
  ```
- 程序将创建一个 OpenGL3 上下文的窗口，并展示 ImGui 示例界面。

## 关键模块与用法
- 顶点/材质/纹理结构体（`include/attribute.h`）
  - Vertex: position[3], normal[3], uv0[2], uv1[2], uv2[2]
  - MaterialDesc: color[3], map_uuid
  - TextureDesc: image_uuid, image_bytes, width, height, srgb, generate_mipmaps
- 模型加载（`include/load.h`, `src/load.cpp`）
  - Load::loadFromFile(path): 加载模型，填充顶点、材质与 GL 纹理
  - vertices(): 返回所有顶点
  - materials(): 返回所有材质
  - texturesGL(): 返回创建好的 OpenGL 纹理句柄数组
- 缓冲绑定（`include/buffer.h`, `src/graghics/buffer.cpp`）
  - MeshBuffer: 管理 VAO/VBO/EBO，setData(vertices, indices) 上传数据并设置属性布局（位置/法线/uv0/uv1/uv2）
  - draw(): 使用 glDrawElements 绘制

## 测试
- 加载测试（`src/test/loadtest.cpp`）
  - `int test_load_model()`：打印顶点、材质与 GL 纹理数量，用于验证加载是否成功
- 缓冲填充测试（`src/test/bufferfill_test.cpp`）
  - `int test_buffer_fill()`：自建隐藏窗口初始化 OpenGL，上传一个三角形的顶点/索引，打印 VBO/EBO 实际字节大小与期望值，用于验证缓冲填充正确性
  - 在 `main.cpp` 最开始调用 `test_buffer_fill()`，避免与主窗口的 GLFW 生命周期冲突

## UV 通道与贴图约定
- uv0（TEXCOORD_0）：基础颜色/法线等主贴图
- uv1（TEXCOORD_1）：贴花/部件专用贴图
- uv2（TEXCOORD_2）：AO/Lightmap 等光照相关贴图
- GL 纹理上传
  - BaseColor/Albedo 建议使用 sRGB 内部格式
  - AO/法线使用线性格式
  - 按 generate_mipmaps 决定是否生成 mipmap

## 注意
- 若在同一进程内多次初始化/销毁 GLFW，请确保调用顺序不会破坏已有窗口与上下文
- 外部贴图路径需在 `resource/` 下有效；内嵌贴图由 Assimp 从 glb 中解析并上传

## 参考文献

