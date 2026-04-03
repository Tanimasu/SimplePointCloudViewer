#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QSharedPointer>
#include "PointCloud.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QToolBar;
class QLabel;
class QProgressBar;
class QTextEdit;
class QFileDialog;
QT_END_NAMESPACE

class PointCloudIO;

/**
 * @brief 主窗口类
 *
 * 包含：
 * - 菜单栏：文件、视图、帮助
 * - 工具栏：打开、保存、清空
 * - 侧边栏：点云属性显示（QDockWidget）
 * - 中央区：OpenGL渲染占位（先用QWidget提示）
 * - 状态栏：进度条、信息提示
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    // 文件操作
    void onOpenFile();
    void onSaveFile();
    void onClear();
    void onRecentFile();

    // 视图操作
    void onToggleSidebar();

    // 帮助
    void onAbout();
    void onAboutQt();

    // 点云处理
    void onPointCloudLoaded(PointCloudPtr cloud);
    void onLoadFailed(const QString& error);
    void onProgressChanged(int percent);

private:
    void setupUI();
    void setupMenu();
    void setupToolbar();
    void setupSidebar();
    void setupStatusBar();
    void setupCentralWidget();

    void loadFile(const QString& filePath);
    void updateSidebar();
    void updateRecentFiles(const QString& filePath);
    void loadRecentFiles();
    void saveSettings();
    void loadSettings();

    // UI组件
    QDockWidget* m_sidebarDock = nullptr;
    QTextEdit* m_infoTextEdit = nullptr;     // 侧边栏信息显示
    QLabel* m_centralPlaceholder = nullptr; // 中央占位提示
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_statusLabel = nullptr;

    // 菜单
    QMenu* m_fileMenu = nullptr;
    QMenu* m_recentMenu = nullptr;
    QMenu* m_viewMenu = nullptr;
    QMenu* m_helpMenu = nullptr;

    // 工具栏
    QToolBar* m_mainToolBar = nullptr;

    // 动作
    QAction* m_openAction = nullptr;
    QAction* m_saveAction = nullptr;
    QAction* m_clearAction = nullptr;
    QAction* m_toggleSidebarAction = nullptr;
    QAction* m_exitAction = nullptr;
    QAction* m_aboutAction = nullptr;
    QAction* m_aboutQtAction = nullptr;
    QList<QAction*> m_recentActions;

    // 数据
    PointCloudPtr m_currentCloud;
    PointCloudIO* m_io = nullptr;
    QStringList m_recentFiles;
    static constexpr int MaxRecentFiles = 10;
};
