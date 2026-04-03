#include <QApplication>
#include "MainWindow.h"

/**
 * @brief SimplePointCloudViewer 主入口
 *
 * Qt + CloudCompare 学习项目 - 阶段一
 *
 * 学习目标：
 * 1. Qt6 Widgets 基础（QMainWindow、QDockWidget、信号槽）
 * 2. CMake 项目配置
 * 3. 点云文件解析（PCD、PLY、TXT）
 * 4. 界面交互设计
 *
 * 编译步骤：
 *   mkdir build && cd build
 *   cmake ..
 *   cmake --build .
 */
int main(int argc, char* argv[])
{
    // 启用高分屏支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    app.setApplicationName("SimplePointCloudViewer");
    app.setOrganizationName("LearningProject");

    MainWindow window;
    window.show();

    return app.exec();
}