# SimplePointCloudViewer

一个用于学习 **Qt6** 和 **CloudCompare** 的点云查看器项目。

## 项目目标

通过构建一个简洁的点云可视化工具，系统学习：
1. **Qt基础**：信号槽、界面布局、多线程
2. **点云处理**：文件格式解析、数据结构
3. **3D图形**：OpenGL渲染、相机控制
4. **CC插件开发**：CloudCompare插件架构

## 当前阶段：阶段一

### 已实现功能
- ✅ PCD、PLY、TXT/XYZ 格式文件读写
- ✅ Qt主窗口、菜单栏、工具栏、状态栏
- ✅ 侧边栏（QDockWidget）显示点云信息
- ✅ 最近打开文件列表
- ✅ 拖拽文件打开
- ✅ 进度条显示

### 待实现（阶段二）
- [ ] OpenGL 3D点云渲染
- [ ] 鼠标交互（旋转/平移/缩放）
- [ ] 相机视图切换

## 项目结构

```
SimplePointCloudViewer/
├── CMakeLists.txt          # 构建配置
├── README.md               # 本文件
├── src/
│   ├── main.cpp            # 程序入口
│   ├── MainWindow.cpp/h    # 主窗口（菜单、工具栏、侧边栏）
│   ├── PointCloud.cpp/h    # 点云数据结构
│   ├── PointCloudIO.cpp/h  # 文件读写（PCD/PLY/TXT）
│   └── GLPointCloudWidget.cpp/h  # OpenGL渲染（阶段二实现）
└── resources/
    └── shaders/            # GLSL着色器（阶段二）
```

## 编译运行

### 要求
- Qt6（Core、Gui、Widgets、OpenGL、OpenGLWidgets）
- CMake >= 3.16
- C++17 编译器

### Windows (MinGW/MSVC)
```bash
# 配置Qt环境变量后
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/msvc2019_64
cmake --build . --config Release
```

### 运行
```bash
./SimplePointCloudViewer
```

## 支持的点云格式

| 格式 | 读取 | 保存 | 说明 |
|------|------|------|------|
| PCD  | ✅ | ✅ | ASCII格式，支持RGB |
| PLY  | ✅ | ✅ | ASCII格式，支持颜色 |
| TXT  | ✅ | ✅ | `x y z [r g b]` |

## 学习要点

### Qt相关
- `QMainWindow` 布局管理
- `QDockWidget` 侧边栏
- `QAction` + `QMenu` + `QToolBar` 菜单系统
- 信号槽机制（`connect`、`emit`、`signals`、`slots`）
- `QSettings` 持久化配置
- 拖放事件处理

### 点云相关
- 点云数据结构（位置、颜色、包围盒）
- PCD文件格式解析（头+数据）
- PLY文件格式解析
- 简单的文件格式推断

## 下一步

阶段二将实现OpenGL点云渲染，学习：
- `QOpenGLWidget` 使用
- GLSL 着色器编写
- VBO/VAO 顶点缓冲管理
- MVP 矩阵变换
- 轨道相机（Orbit Camera）交互

## 参考

- [Qt6官方文档](https://doc.qt.io/qt-6/)
- [CloudCompare GitHub](https://github.com/CloudCompare/CloudCompare)
- [PCD文件格式](http://pointclouds.org/documentation/tutorials/pcd_file_format.php)