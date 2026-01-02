# 图形学大作业项目说明

本项目基于 GLFW + OpenGL3 渲染，并使用 Dear ImGui 作为调试与 UI。第三方库统一放置在 `thirdparty` 目录中，工程使用 CMake 构建。

## 项目结构
- `src/`：项目源码，入口为 [main.cpp]
- `include/`：项目头文件（可按需新增）
- `resource/`：资源文件（模型、纹理等）
- `thirdparty/`：第三方库源代码（精简版本）
  - `glfw/`：GLFW 源码（只用于 Windows + OpenGL）
  - `imgui/`：Dear ImGui 源码（只保留 `glfw` 与 `opengl3` 后端）
  - `assimp/`：Assimp 源码（仅保留 OBJ 导入能力，其他格式已裁剪）
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
1. 在项目根目录生成构建目录并配置：
   ```
   mkdir build
   cd build
   cmake ..
   ```
2. 编译：
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
  ./build/Debug/GraphicsHomework.exe
  ```
- 程序将创建一个 OpenGL3 上下文的窗口，并展示 ImGui 示例界面。

## 参考文件


