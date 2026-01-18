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
- Assimp：用于模型导入，当前示例使用 `resource/model/mi.glb`（glTF 二进制格式），只保留项目需要的核心代码与依赖。
  - 如需支持更多格式或功能，可在 CMake 中调整 Assimp 相关配置。

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
- 程序将创建一个 OpenGL3 上下文的窗口，并加载 `resource/model/mi.glb` 模型与 ImGui 控制面板。

## 关键模块
- `include/shader.h`, `src/graghics/shader.cpp`
  - 封装 OpenGL 着色器编译与 uniform 设置（`setVec3`, `setFloat` 等）。
- `include/mesh.h`, `src/graghics/mesh.cpp`
  - 封装模型网格（顶点、索引、纹理）与 VAO/VBO/EBO 绑定及绘制。
- `include/model.h`, `src/graghics/model.cpp`
  - 使用 Assimp 加载模型（当前示例为 glb），并递归构建多个 `Mesh`。
- `include/ui.h`, `src/widget/ui.cpp`
  - 管理相机、模型、光源和立方体的 UI 状态。
  - 使用 ImGui 绘制控制面板（相机参数、模型变换、光照参数、Cube 参数等）。
- `include/cube.h`, `src/tool/cube.cpp`
  - 根据长宽高与颜色动态生成立方体顶点和索引，并使用单独的着色器进行渲染。
- `src/main.cpp`
  - 程序入口：初始化 OpenGL、ImGui、加载模型与着色器，主循环中更新 UIState、设置矩阵与光照参数并绘制场景。

## 交互与控件说明
### 基本操作
- `P`：开启/关闭 ImGui 控制面板，同时切换鼠标捕获。
- 鼠标移动：在未暂停输入时控制相机视角（Yaw/Pitch）。
- `W/S/A/D`：前后左右移动相机。
- `Space` / `Left Ctrl`：沿世界 Up 方向上升 / 下降。

### ImGui 面板（ui_draw）
- Camera
  - `Camera Position`：相机世界坐标。
  - `Yaw` / `Pitch`：相机欧拉角。
  - `Speed`：相机移动速度。
  - `FOV`：透视投影视角。
- Model
  - `Model Pos` / `Model Rot` / `Model Scale`：控制主模型的平移、旋转与缩放。
- Light
  - `Direcional Light Direction`：方向光方向。
  - `Direcional Light Color`：方向光颜色。
- Cube
  - `Cube Pos` / `Cube Rot` / `Cube Scale`：控制立方体在世界空间中的变换。
  - `Cube Color`：立方体颜色。
  - `Create Cube` 按钮：开关立方体的显示与渲染。
- 其他
  - `outline width`：控制描边膨胀宽度（影响主模型轮廓效果）。

### 关于 Cube 按钮与相机
- 当按下 `Create Cube` 按钮后，会根据当前 UI 中的尺寸与颜色参数创建一个立方体，并与场景共享同一套 `view`/`projection` 矩阵。
- 移动相机（鼠标 + 键盘）时，主模型和立方体都会随视角变化正常运动，不会出现模型“贴死在屏幕上”的现象。

## 注意
- 若在同一进程内多次初始化/销毁 GLFW，请确保调用顺序不会破坏已有窗口与上下文。
- 资源文件（模型、着色器）路径需在 `resource/` 目录下有效。
