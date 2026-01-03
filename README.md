# 图形学大作业项目说明

本项目基于Glad + GLFW + OpenGL3 渲染，并使用 Dear ImGui 作为调试与 UI。第三方库统一放置在 `thirdparty` 目录中，工程使用 CMake 构建。


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
- `src/`：项目源码，入口为 [main.cpp]
- `include/`：项目头文件（可按需新增）
- `resource/`：资源文件（模型、纹理、**着色器**等）
- `thirdparty/`：第三方库源代码（精简版本）
  - `glfw/`：GLFW 源码（只用于 Windows + OpenGL）
  - `imgui/`：Dear ImGui 源码（只保留 `glfw` 与 `opengl3` 后端）
  - `assimp/`：Assimp 源码（仅保留 OBJ 导入能力，其他格式已裁剪）stb_image.h也配置到assimp的include目录中
- `build/`：CMake 生成的构建输出目录
- [CMakeLists.txt](file:///f:/图形学大作业/CMakeLists.txt)：顶层构建配置

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

## 参考文献


