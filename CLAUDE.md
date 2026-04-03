# CLAUDE.md - SimplePointCloudViewer

## 项目简介

用于学习 Qt6 和 CloudCompare 的点云查看器项目。

- **语言**: C++17
- **框架**: Qt6 (Widgets, OpenGL)
- **构建**: CMake
- **平台**: Windows (MSVC2022), Linux, macOS

---

## 快速开始

### 构建步骤

```bash
cd SimplePointCloudViewer
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="F:/env/Qt/6.11.0/msvc2022_64"
cmake --build . --config Release
```

### 运行

```bash
cd Release
# 临时设置Qt路径
$env:PATH = "F:/env/Qt/6.11.0/msvc2022_64/bin;$env:PATH"
./SimplePointCloudViewer.exe
```

---

## 项目结构

| 路径 | 说明 |
|------|------|
| `src/MainWindow.cpp` | 主窗口：菜单、工具栏、侧边栏 |
| `src/PointCloud.cpp` | 点云数据结构（位置、颜色、包围盒） |
| `src/PointCloudIO.cpp` | 文件IO：PCD、PLY、TXT格式 |
| `src/GLPointCloudWidget.cpp` | **阶段二**：OpenGL点云渲染 |
| `resources/shaders/` | **阶段二**：GLSL着色器 |

---

## 开发阶段

### 阶段一：Qt基础 + 文件IO ✅
- QMainWindow 完整界面
- PCD/PLY/TXT 文件读写
- 侧边栏属性显示
- 信号槽交互

### 阶段二：OpenGL 3D渲染
- QOpenGLWidget 实现
- 顶点着色器 + 片段着色器
- 轨道相机交互

### 阶段三：CC插件
- CloudCompare 插件接口
- 使用CC渲染引擎

---

## 常见问题

### 缺少DLL
```powershell
# 复制Qt DLL到Release目录
cp F:/env/Qt/6.11.0/msvc2022_64/bin/Qt6Core.dll .
cp F:/env/Qt/6.11.0/msvc2022_64/bin/Qt6Gui.dll .
cp F:/env/Qt/6.11.0/msvc2022_64/bin/Qt6Widgets.dll .
cp F:/env/Qt/6.11.0/msvc2022_64/bin/Qt6OpenGL.dll .
cp F:/env/Qt/6.11.0/msvc2022_64/bin/Qt6OpenGLWidgets.dll .
mkdir platforms
cp F:/env/Qt/6.11.0/msvc2022_64/plugins/platforms/qwindows.dll ./platforms/
```

### CMake找不到Qt
确保路径正确（使用正斜杠）：
```bash
cmake .. -DCMAKE_PREFIX_PATH="F:/env/Qt/6.11.0/msvc2022_64"
```

---

## 学习笔记

见 `NOTES.md`

---

## 支持格式

| 格式 | 读 | 写 | 颜色 |
|------|----|----|------|
| PCD (ASCII) | ✅ | ✅ | ✅ |
| PLY (ASCII) | ✅ | ✅ | ✅ |
| TXT/XYZ | ✅ | ✅ | ✅ |

---

## 测试数据

创建 `test.txt`：
```
# x y z r g b
0 0 0 255 0 0
1 0 0 255 0 0
0 1 0 0 255 0
0 0 1 0 0 255
```

---

## 参考

- Qt6: https://doc.qt.io/qt-6/
- CloudCompare: https://github.com/CloudCompare/CloudCompare
