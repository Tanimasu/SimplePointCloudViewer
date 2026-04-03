#include "MainWindow.h"
#include "PointCloudIO.h"
#include "GLPointCloudWidget.h"

#include <QtWidgets>
#include <QSettings>
#include <QMimeData>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_io(new PointCloudIO(this))
{
    setWindowTitle("SimplePointCloudViewer - Qt+点云学习项目");
    setMinimumSize(1024, 768);
    resize(1280, 900);

    // 接受拖放
    setAcceptDrops(true);

    // 连接IO信号
    connect(m_io, &PointCloudIO::loadFinished, this, &MainWindow::onPointCloudLoaded);
    connect(m_io, &PointCloudIO::loadFailed, this, &MainWindow::onLoadFailed);
    connect(m_io, &PointCloudIO::progressChanged, this, &MainWindow::onProgressChanged);

    setupUI();
    loadSettings();
    updateSidebar();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    setupMenu();
    setupToolbar();
    setupSidebar();
    setupCentralWidget();
    setupStatusBar();
}

void MainWindow::setupMenu()
{
    // 文件菜单
    m_fileMenu = menuBar()->addMenu("文件(&F)");

    m_openAction = m_fileMenu->addAction(
        QIcon::fromTheme("document-open", QIcon(":/icons/open.png")),
        "打开(&O)...", this, &MainWindow::onOpenFile,
        QKeySequence::Open);

    m_saveAction = m_fileMenu->addAction(
        QIcon::fromTheme("document-save", QIcon(":/icons/save.png")),
        "保存(&S)...", this, &MainWindow::onSaveFile,
        QKeySequence::Save);
    m_saveAction->setEnabled(false);

    m_fileMenu->addSeparator();

    // 最近文件子菜单
    m_recentMenu = m_fileMenu->addMenu("最近打开(&R)");
    for (int i = 0; i < MaxRecentFiles; ++i) {
        auto* action = new QAction(this);
        action->setVisible(false);
        connect(action, &QAction::triggered, this, &MainWindow::onRecentFile);
        m_recentActions.append(action);
        m_recentMenu->addAction(action);
    }
    m_recentMenu->addSeparator();
    m_recentMenu->addAction("清空历史", this, [this]() {
        m_recentFiles.clear();
        loadRecentFiles();
    });

    m_fileMenu->addSeparator();

    m_clearAction = m_fileMenu->addAction(
        QIcon::fromTheme("edit-clear", QIcon(":/icons/clear.png")),
        "清空(&C)", this, &MainWindow::onClear);

    m_fileMenu->addSeparator();

    m_exitAction = m_fileMenu->addAction(
        "退出(&X)", this, &QWidget::close,
        QKeySequence::Quit);

    // 视图菜单
    m_viewMenu = menuBar()->addMenu("视图(&V)");
    m_toggleSidebarAction = m_viewMenu->addAction(
        "侧边栏(&S)", this, &MainWindow::onToggleSidebar,
        QKeySequence("F4"));
    m_toggleSidebarAction->setCheckable(true);
    m_toggleSidebarAction->setChecked(true);

    // 帮助菜单
    m_helpMenu = menuBar()->addMenu("帮助(&H)");
    m_aboutAction = m_helpMenu->addAction("关于(&A)", this, &MainWindow::onAbout);
    m_aboutQtAction = m_helpMenu->addAction("关于Qt", this, &MainWindow::onAboutQt);
}

void MainWindow::setupToolbar()
{
    m_mainToolBar = addToolBar("主工具栏");
    m_mainToolBar->setMovable(false);

    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_clearAction);
}

void MainWindow::setupSidebar()
{
    m_sidebarDock = new QDockWidget("点云信息", this);
    m_sidebarDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_sidebarDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    // 信息面板内容
    QWidget* sidebarContent = new QWidget(m_sidebarDock);
    QVBoxLayout* layout = new QVBoxLayout(sidebarContent);

    m_infoTextEdit = new QTextEdit(sidebarContent);
    m_infoTextEdit->setReadOnly(true);
    m_infoTextEdit->setPlaceholderText("尚未加载点云数据\n\n请使用菜单栏 [文件 -> 打开] 加载点云文件");

    layout->addWidget(m_infoTextEdit);
    layout->addStretch();

    m_sidebarDock->setWidget(sidebarContent);
    addDockWidget(Qt::LeftDockWidgetArea, m_sidebarDock);

    // 侧边栏关闭时同步菜单状态
    connect(m_sidebarDock, &QDockWidget::visibilityChanged, this, [this](bool visible) {
        m_toggleSidebarAction->setChecked(visible);
    });
}

void MainWindow::setupCentralWidget()
{
    // 阶段一：使用占位提示，后续替换为 GLPointCloudWidget
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    m_centralPlaceholder = new QLabel(central);
    m_centralPlaceholder->setAlignment(Qt::AlignCenter);
    m_centralPlaceholder->setText(
        "<h2>SimplePointCloudViewer</h2>"
        "<p>Qt + CloudCompare 学习项目</p>"
        "<p style='color: gray;'>阶段一：文件IO和UI界面</p>"
        "<p style='color: gray;'>阶段二：OpenGL 3D渲染（待实现）</p>"
        "<br>"
        "<p>支持的格式：PCD、PLY、TXT/XYZ</p>"
        "<p>拖拽文件到窗口即可打开</p>"
    );
    m_centralPlaceholder->setStyleSheet("background-color: #f0f0f0;");

    layout->addWidget(m_centralPlaceholder);
    setCentralWidget(central);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("就绪", this);
    statusBar()->addWidget(m_statusLabel, 1);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMaximumWidth(200);
    m_progressBar->setVisible(false);
    statusBar()->addPermanentWidget(m_progressBar);
}

// ========== 槽函数 ==========

void MainWindow::onOpenFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "打开点云文件",
        QString(),
        PointCloudIO::getOpenFileFilter());

    if (!filePath.isEmpty()) {
        loadFile(filePath);
    }
}

void MainWindow::onSaveFile()
{
    if (!m_currentCloud || m_currentCloud->isEmpty()) {
        QMessageBox::warning(this, "保存", "没有可保存的点云数据");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "保存点云文件",
        QString(),
        PointCloudIO::getSaveFileFilter());

    if (filePath.isEmpty()) return;

    QString errorMsg;
    if (m_io->save(m_currentCloud, filePath, &errorMsg)) {
        m_statusLabel->setText(QString("已保存到: %1").arg(filePath));
    } else {
        QMessageBox::critical(this, "保存失败", errorMsg);
    }
}

void MainWindow::onClear()
{
    m_currentCloud.reset();
    m_saveAction->setEnabled(false);
    updateSidebar();
    m_statusLabel->setText("已清空");
}

void MainWindow::onRecentFile()
{
    auto* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString filePath = action->data().toString();
        if (QFile::exists(filePath)) {
            loadFile(filePath);
        } else {
            QMessageBox::warning(this, "文件不存在", filePath);
            m_recentFiles.removeAll(filePath);
            loadRecentFiles();
        }
    }
}

void MainWindow::onToggleSidebar()
{
    if (m_sidebarDock->isVisible()) {
        m_sidebarDock->hide();
    } else {
        m_sidebarDock->show();
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "关于",
        "<h2>SimplePointCloudViewer</h2>"
        "<p>一个学习 Qt 和 CloudCompare 的简洁点云查看器项目。</p>"
        "<p>支持 PCD、PLY、TXT/XYZ 格式的点云文件。</p>"
        "<p>版本: 1.0 (阶段一)</p>"
        "<hr>"
        "<p>学习要点:</p>"
        "<ul>"
        "<li>Qt6 Widgets 界面开发</li>"
        "<li>CMake 项目构建</li>"
        "<li>点云文件格式解析</li>"
        "<li>OpenGL 3D 渲染 (阶段二)</li>"
        "</ul>");
}

void MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onPointCloudLoaded(PointCloudPtr cloud)
{
    m_currentCloud = cloud;
    m_saveAction->setEnabled(true);
    m_progressBar->setVisible(false);

    updateSidebar();
    updateRecentFiles(cloud->fileName());

    m_statusLabel->setText(QString("加载完成: %1 点").arg(cloud->pointCount()));
}

void MainWindow::onLoadFailed(const QString& error)
{
    m_progressBar->setVisible(false);
    QMessageBox::critical(this, "加载失败", error);
    m_statusLabel->setText("加载失败");
}

void MainWindow::onProgressChanged(int percent)
{
    m_progressBar->setVisible(true);
    m_progressBar->setValue(percent);
}

// ========== 辅助函数 ==========

void MainWindow::loadFile(const QString& filePath)
{
    m_statusLabel->setText(QString("正在加载: %1...").arg(filePath));
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);

    // 实际加载（这里可以改为多线程）
    QString errorMsg;
    auto cloud = m_io->load(filePath, &errorMsg);

    if (!cloud) {
        // 加载失败信号会触发onLoadFailed
    }
}

void MainWindow::updateSidebar()
{
    if (m_currentCloud && !m_currentCloud->isEmpty()) {
        m_infoTextEdit->setPlainText(m_currentCloud->getInfoString());
    } else {
        m_infoTextEdit->clear();
        m_infoTextEdit->setPlaceholderText(
            "尚未加载点云数据\n\n"
            "请使用菜单栏 [文件 -> 打开] 加载点云文件\n\n"
            "支持格式: PCD、PLY、TXT/XYZ");
    }
}

void MainWindow::updateRecentFiles(const QString& filePath)
{
    QFileInfo info(filePath);
    QString fullPath = info.absoluteFilePath();

    m_recentFiles.removeAll(fullPath);
    m_recentFiles.prepend(fullPath);

    while (m_recentFiles.size() > MaxRecentFiles) {
        m_recentFiles.removeLast();
    }

    loadRecentFiles();
    saveSettings();
}

void MainWindow::loadRecentFiles()
{
    for (int i = 0; i < MaxRecentFiles; ++i) {
        if (i < m_recentFiles.size()) {
            QString text = QString("&%1 %2").arg(i + 1).arg(m_recentFiles[i]);
            m_recentActions[i]->setText(text);
            m_recentActions[i]->setData(m_recentFiles[i]);
            m_recentActions[i]->setVisible(true);
        } else {
            m_recentActions[i]->setVisible(false);
        }
    }

    bool hasRecent = !m_recentFiles.isEmpty();
    m_recentMenu->setEnabled(hasRecent);
}

void MainWindow::saveSettings()
{
    QSettings settings("SimplePointCloudViewer", "config");
    settings.setValue("recentFiles", m_recentFiles);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::loadSettings()
{
    QSettings settings("SimplePointCloudViewer", "config");
    m_recentFiles = settings.value("recentFiles").toStringList();
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    loadRecentFiles();
}

// ========== 事件处理 ==========

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            if (!filePath.isEmpty()) {
                loadFile(filePath);
            }
        }
    }
}
