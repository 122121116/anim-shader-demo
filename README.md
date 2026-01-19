# OpenGL 动画与着色器演示项目 (Anim Shader Demo)

本项目是一个基于现代 OpenGL (Core Profile) 的图形学演示程序，旨在展示高级光照、阴影渲染、模型加载及交互式控制功能。项目使用 C++ 编写，集成了 Dear ImGui 用于实时调试和参数调整。

## ✨ 主要功能 (Features)

### 1. 渲染与光照
*   **基础光照模型**：实现了 Blinn-Phong 光照模型，支持环境光、漫反射和镜面反射。
*   **高级阴影渲染**：
    *   **万向阴影映射 (Omnidirectional Shadow Mapping)**：支持点光源产生的全方位阴影。
    *   **PCF 软阴影 (Percentage-Closer Filtering)**：通过 20 次采样和随机旋转偏移，实现了高质量的软阴影边缘，有效消除了锯齿。
    *   **高分辨率阴影贴图**：使用 2048x2048 分辨率的深度立方体贴图，保证阴影细节。
*   **描边效果 (Outline)**：
    *   基于顶点法线膨胀（Vertex Extrusion）的背面描边算法。
    *   支持动态调整描边宽度。

### 2. 模型与资源
*   **模型加载**：集成 **Assimp** 库，支持 glTF (`.glb`) 等多种通用 3D 模型格式加载。
*   **纹理支持**：支持漫反射纹理映射，对于无纹理模型支持纯色渲染。
*   **自定义几何体**：内置 Cube 类，支持动态创建、变换（平移、旋转、缩放）和颜色调整。

### 3. 交互与 UI
*   **Dear ImGui 集成**：提供全功能的控制面板。
*   **实时参数调整**：
    *   **光照控制**：实时修改光源位置、颜色。
    *   **模型变换**：控制主模型的旋转、缩放和位置。
    *   **相机控制**：支持场景漫游（通过 UI 或键鼠）。
    *   **物体管理**：动态添加/删除场景中的立方体，并独立控制其属性。

## 🛠️ 技术栈 (Tech Stack)

*   **语言**：C++17
*   **图形 API**：OpenGL 3.3 Core Profile
*   **窗口管理**：GLFW
*   **OpenGL 加载器**：GLAD
*   **数学库**：GLM (OpenGL Mathematics)
*   **GUI 库**：Dear ImGui
*   **模型加载**：Assimp (Open Asset Import Library)
*   **构建系统**：CMake

## 📦 目录结构 (Project Structure)

```
root/
├── include/            # 头文件
│   ├── shader.h        # 着色器管理类
│   ├── mesh.h          # 网格数据类
│   ├── model.h         # 模型加载类
│   ├── light.h         # 光源与阴影管理
│   ├── cube.h          # 立方体类
│   └── ui.h            # UI 状态与逻辑
├── resource/           # 资源文件
│   ├── model/          # 3D 模型文件 (.glb)
│   └── shader/         # GLSL 着色器源码
│       ├── pixel.vs/fs         # 主渲染着色器
│       ├── pixel_cube.vs/fs    # 立方体着色器
│       ├── depth.vs/fs         # 阴影深度图生成着色器
│       └── vertex.vs           # 顶点着色器（含描边逻辑）
├── src/                # 源代码
│   ├── graphics/       # 渲染相关实现 (Shader, Mesh, Model, Light)
│   ├── tool/           # 工具类 (Cube)
│   ├── widget/         # UI 实现
│   └── main.cpp        # 程序入口
├── thirdparty/         # 第三方库 (GLFW, ImGui, Assimp, GLM 等)
├── CMakeLists.txt      # CMake 构建配置
└── README.md           # 项目说明文档
```

## 🚀 构建与运行 (Build & Run)

### 环境要求
*   Windows 10/11
*   Visual Studio 2019/2022 (MSVC) 或其他 C++ 编译器
*   CMake 3.16+
*   Git

### 方式一：使用 Visual Studio 直接打开
1.  启动 Visual Studio。
2.  选择 "打开本地文件夹" 并指向项目根目录。
3.  Visual Studio 会自动检测 `CMakeLists.txt` 并进行配置。
4.  在顶部启动项选择 `GraphicsHomework.exe`。
5.  点击运行（绿色播放按钮）。

### 方式二：使用命令行 (CMake)
1.  在项目根目录打开终端。
2.  创建构建目录并配置：
    ```bash
    mkdir build
    cd build
    cmake ..
    ```
3.  编译项目：
    ```bash
    cmake --build . --config Release
    ```
4.  运行程序：
    *   生成的可执行文件通常位于 `build/Release/GraphicsHomework.exe` (或 `Debug` 目录)。
    *   **注意**：程序运行时会自动将 `resource` 目录复制到可执行文件同级目录，确保资源能被正确加载。

## 🎮 操作说明 (Controls)

程序启动后，你会看到主渲染窗口和 ImGui 控制面板。按P键可以暂停画面。

*   **View 面板**：
    *   `Model Transform`：控制主模型的旋转 (Rotate)、缩放 (Scale)。
    *   `Light`：调整点光源的位置 (Position) 和颜色 (Color)。
    *   `Outline`：调整描边宽度 (Width)。
*   **Scene 面板**：
    *   `Cubes` 列表：显示当前场景中的立方体。
    *   `Add Cube`：在场景中添加一个新的立方体。
    *   选中列表中的立方体后，可以调整其 `Position` (位置), `Rotation` (旋转), `Scale` (缩放) 和 `Visible` (可见性)。

## 📝 实现细节 (Implementation Details)

### 阴影系统
使用了**全向阴影贴图 (Omnidirectional Shadow Maps)** 技术。
1.  **深度 Pass**：首先从光源视角向 6 个方向（立方体贴图的 6 个面）渲染场景深度，生成深度立方体贴图 (Depth Cubemap)。
2.  **渲染 Pass**：在主渲染阶段，计算片元到光源的距离，并与深度贴图中的值进行比较。
3.  **PCF 优化**：为了解决阴影锯齿，实现了 Percentage-Closer Filtering。在着色器中，沿光线方向及周围随机采样 20 次，取平均值作为阴影因子，从而产生柔和的阴影边缘。

### 描边系统
使用了**顶点法线外扩**技术。
1.  在顶点着色器中，判断当前是否处于描边绘制阶段（通过 `vIsOutline` 变量）。
2.  如果是描边阶段，将顶点沿法线方向向外移动一定距离（由 `outlineWidth` 控制）。
3.  在片元着色器中，将这些膨胀后的背面渲染为纯黑色，从而在原模型周围形成黑色轮廓。

### 资源管理
*   实现了 `Mesh` 和 `Cube` 类的**RAII**（资源获取即初始化）管理。
*   添加了移动构造函数和析构函数，确保 OpenGL 对象（VAO, VBO, EBO）在对象生命周期结束时自动释放，防止显存泄漏和 ImGui 崩溃。
