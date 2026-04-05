# SimplePointCloudViewer 项目说明

这是一个用来学习 C++ 和 Qt 的小项目。
我希望通过实现一个简洁的点云查看器，逐步熟悉 Qt 桌面程序开发、OpenGL 渲染、点云文件解析，以及后续 CloudCompare 插件开发的基本流程。

## 项目定位

- 项目名称：`SimplePointCloudViewer`
- 项目性质：C++ / Qt / 点云可视化学习项目
- 当前目标：完成一个独立运行的 Qt 点云查看器
- 后续目标：在理解 CloudCompare 架构后，将功能逐步迁移为插件

## 项目计划

### 阶段一：独立 Qt 点云查看器

#### 功能列表

1. 文件操作
   - 读取 PCD、PLY、TXT 格式的点云文件
   - 显示文件基本信息，如点数、包围盒
2. 3D 可视化
   - 使用 Qt6 的 `QOpenGLWidget` 渲染点云
   - 支持鼠标交互：旋转、平移、缩放
   - 实现基础相机控制
3. 点云处理
   - 实现简单的体素下采样
   - 显示坐标轴
   - 支持背景色切换
4. Qt 界面
   - 主窗口、菜单栏、工具栏
   - 侧边栏显示点云属性
   - 状态栏显示运行信息

#### 技术要点

- Qt6 Widgets：`QMainWindow`、`QOpenGLWidget`
- OpenGL 3.3 Core Profile 点云渲染
- 信号槽机制与界面交互
- 文件 I/O 和错误处理

### 阶段二：CloudCompare 插件改造

#### 学习内容

1. CloudCompare 插件架构
   - 插件接口：`ccPluginInterface`、`ccGLPluginInterface`
   - 与 CloudCompare 主程序的交互方式
   - 相关数据结构：`ccPointCloud`、`ccHObject` 等
2. 代码改造方向
   - 将独立程序逐步封装为插件
   - 使用 CloudCompare 的渲染引擎替代自定义 OpenGL
   - 将功能入口集成到 CloudCompare 菜单或工具栏

## 推荐项目结构

```text
SimplePointCloudViewer/
├── CMakeLists.txt
├── README.md
├── NOTES.md
├── src/
│   ├── main.cpp
│   ├── MainWindow.cpp/h
│   ├── GLPointCloudWidget.cpp/h
│   ├── PointCloud.cpp/h
│   ├── PointCloudIO.cpp/h
│   └── PointCloudProcessor.cpp/h
├── plugin/
│   ├── CCPlugin.cpp/h
│   └── CCPluginWidget.cpp/h
└── resources/
    ├── shaders/
    │   ├── vertex.glsl
    │   └── fragment.glsl
    └── icons/
```

## 重点学习模块

| 文件 | 学习内容 |
| --- | --- |
| `PointCloud.h` | 点云数据结构定义，例如位置、颜色、法向量 |
| `PointCloudIO.cpp` | PCD / PLY / TXT 文件格式解析 |
| `GLPointCloudWidget.cpp` | OpenGL 点云渲染、相机控制、交互逻辑 |
| `MainWindow.cpp` | Qt 主窗口、菜单栏、工具栏、信号槽连接 |
| `PointCloudProcessor.cpp` | 体素网格下采样等基础处理算法 |
| `CCPlugin.cpp` | CloudCompare 插件接口实现 |

## 希望通过这个项目掌握的内容

### Qt 基础

- Qt 项目的 CMake 配置
- `QMainWindow` 布局管理
- `QAction`、`QToolBar`、`QMenu` 的使用
- 信号槽机制
- 资源文件管理

### Qt 3D 图形

- `QOpenGLWidget` 的使用方法
- OpenGL 着色器编写
- VBO / VAO 管理
- 模型、视图、投影矩阵
- 鼠标交互与相机控制

### 点云处理

- 点云文件格式解析
- 点云数据结构设计
- 体素下采样算法
- 包围盒与坐标变换

### CloudCompare 插件开发

- CloudCompare 插件接口规范
- 与 CloudCompare 数据结构的集成方式
- 插件编译、加载和调试流程

## 后续可扩展方向

1. 增加更多点云格式支持：LAS、E57、XYZ
2. 增加更多处理功能：统计滤波、平面分割、法线估计
3. 优化界面体验：QSS、美化主题、交互细节
4. 优化性能：LOD、八叉树、分块加载

## 参考资料

- CloudCompare: <https://github.com/CloudCompare/CloudCompare>
- Qt6 官方文档: <https://doc.qt.io/qt-6/>
- PCL 文档: <https://pointclouds.org/documentation/>

# Qt + CloudCompare 学习笔记

## 项目：SimplePointCloudViewer

### 学习路径

#### 阶段一：Qt基础 + 文件IO ✅

**已掌握的知识点：**

##### 1. Qt项目结构
```
项目/
├── CMakeLists.txt          # 构建配置
├── src/
│   ├── main.cpp             # 入口
│   ├── MainWindow.h/cpp     # 主窗口
│   └── ...
└── resources/               # 资源文件
```

##### 2. CMake配置要点
```cmake
# 启用Qt的元对象编译器
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt组件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
```

##### 3. QMainWindow布局
- **MenuBar**（菜单栏）：`menuBar()->addMenu()`
- **ToolBar**（工具栏）：`addToolBar()`
- **CentralWidget**（中央区）：`setCentralWidget()`
- **DockWidget**（侧边栏）：`QDockWidget` + `addDockWidget()`
- **StatusBar**（状态栏）：`statusBar()`

##### 4. 信号槽（Signal/Slot）
```cpp
// 定义信号
class Worker : public QObject {
    Q_OBJECT
signals:
    void progressChanged(int percent);
    void finished();
};

// 连接信号和槽
connect(worker, &Worker::progressChanged, this, [this](int p) {
    progressBar->setValue(p);
});

// 发送信号
emit progressChanged(50);
```

##### 5. 点云文件格式

**PCD格式：**
```
VERSION 0.7
FIELDS x y z rgb
SIZE 4 4 4 4
TYPE F F F F
COUNT 1 1 1 1
WIDTH 1000
HEIGHT 1
POINTS 1000
DATA ascii
0.0 0.0 0.0 [packed_rgb]
...
```

**PLY格式：**
```
ply
format ascii 1.0
element vertex 1000
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
end_header
0.0 0.0 0.0 255 0 0
...
```

##### 6. 关键Qt类使用

| 类 | 用途 |
|----|------|
| `QVector3D` | 3D向量（位置/颜色） |
| `QVector<T>` | 动态数组（存储点云） |
| `QString` | 字符串处理 |
| `QFile` | 文件IO |
| `QTextStream` | 文本流读写 |
| `QSettings` | 配置持久化 |
| `QDockWidget` | 可停靠面板 |
| `QProgressBar` | 进度显示 |

##### 7. 事件处理
```cpp
// 拖拽事件
void dragEnterEvent(QDragEnterEvent* event) override;
void dropEvent(QDropEvent* event) override;

// 鼠标事件
void mousePressEvent(QMouseEvent* event) override;
void mouseMoveEvent(QMouseEvent* event) override;
void wheelEvent(QWheelEvent* event) override;
```

---

#### 阶段二：OpenGL 3D渲染（待学习）

**预期知识点：**

##### 1. OpenGL基础
- QOpenGLWidget 生命周期：initializeGL → resizeGL → paintGL
- VBO/VAO 顶点缓冲区
- GLSL 着色器（顶点着色器 + 片段着色器）

##### 2. MVP矩阵
```
Model（模型矩阵）：点云位置/旋转/缩放
View（视图矩阵）：相机位置和朝向
Projection（投影矩阵）：透视/正交投影
```

##### 3. 相机交互
- 轨道相机（Orbit Camera）
- 鼠标拖拽旋转、滚轮缩放、右键平移

---

#### 阶段三：CloudCompare插件（待学习）

**预期知识点：**
- CC插件接口（ccPluginInterface）
- CC数据结构（ccPointCloud）
- CC渲染引擎集成

---

### 踩坑记录

| 问题 | 原因 | 解决 |
|------|------|------|
| 运行无窗口 | 缺少Qt DLL | 复制DLL到程序目录 |
| CMake找不到Qt | 路径未指定 | `-DCMAKE_PREFIX_PATH="F:/Qt/6.x.x/..."` |
| 信号槽不触发 | 忘记`emit` | 检查是否写了`emit signalName()` |
| PCD颜色显示异常 | RGB打包方式 | 使用union转换float到uint32 |

---

### 参考资源

- Qt6官方文档：https://doc.qt.io/qt-6/
- PCL点云库：http://pointclouds.org/
- CloudCompare：https://github.com/CloudCompare/CloudCompare
- OpenGL教程：https://learnopengl.com/

---

### 下一步计划

1. [ ] 实现GLPointCloudWidget的OpenGL渲染
2. [ ] 添加轨道相机控制
3. [ ] 支持更多点云格式（LAS、E57）
4. [ ] 封装为CloudCompare插件
