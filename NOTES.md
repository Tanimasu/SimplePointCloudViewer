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
