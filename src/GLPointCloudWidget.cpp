#include "GLPointCloudWidget.h"
#include "PointCloud.h"
#include <QPainter>
#include <QMouseEvent>

GLPointCloudWidget::GLPointCloudWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    // 阶段一：仅设置背景色
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);  // 抗锯齿
    setFormat(format);

    setBackgroundColor(QColor(240, 240, 240));
}

GLPointCloudWidget::~GLPointCloudWidget() = default;

void GLPointCloudWidget::setPointCloud(const PointCloudPtr& cloud)
{
    m_cloud = cloud;
    // 阶段二：此处应更新OpenGL缓冲区
    update();
}

void GLPointCloudWidget::setPointSize(float size)
{
    m_pointSize = size;
    update();
}

void GLPointCloudWidget::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
    update();
}

void GLPointCloudWidget::resetCamera()
{
    // 阶段二：重置相机位置
}

void GLPointCloudWidget::setFrontView()
{
    // 阶段二：设置正视图
}

void GLPointCloudWidget::setTopView()
{
    // 阶段二：设置俯视图
}

void GLPointCloudWidget::setSideView()
{
    // 阶段二：设置侧视图
}

// ========== OpenGL 生命周期（阶段一：简单绘制占位） ==========

void GLPointCloudWidget::initializeGL()
{
    // 阶段二：初始化OpenGL资源
    // - 编译着色器
    // - 创建VBO/VAO
    // - 设置OpenGL状态
}

void GLPointCloudWidget::resizeGL(int w, int h)
{
    // 阶段二：更新投影矩阵
    Q_UNUSED(w)
    Q_UNUSED(h)
}

void GLPointCloudWidget::paintGL()
{
    // 阶段一：简单绘制背景和提示文字
    QPainter painter(this);
    painter.fillRect(rect(), m_backgroundColor);

    if (!m_cloud || m_cloud->isEmpty()) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter,
            "OpenGL 渲染区\n\n"
            "阶段一：文件IO和UI界面\n"
            "阶段二：实现OpenGL点云渲染\n\n"
            "（请等待阶段二实现）");
    } else {
        // 阶段二：真正的OpenGL点云渲染
        painter.setPen(Qt::darkGray);
        painter.drawText(rect(), Qt::AlignCenter,
            QString("点云已加载：%1 点\n\n"
                   "OpenGL渲染将在阶段二实现").arg(m_cloud->pointCount()));

        // 绘制一个简单的指示：显示点云有数据
        painter.setPen(QPen(Qt::darkGreen, 3));
        int cx = width() / 2;
        int cy = height() / 2;
        painter.drawEllipse(cx - 50, cy - 50, 100, 100);
        painter.drawLine(cx - 70, cy, cx + 70, cy);
        painter.drawLine(cx, cy - 70, cx, cy + 70);
    }
}

// ========== 鼠标交互（阶段一：空实现，阶段二补充） ==========

void GLPointCloudWidget::mousePressEvent(QMouseEvent* event)
{
    // 阶段二：开始相机操作
    QOpenGLWidget::mousePressEvent(event);
}

void GLPointCloudWidget::mouseMoveEvent(QMouseEvent* event)
{
    // 阶段二：更新相机
    QOpenGLWidget::mouseMoveEvent(event);
}

void GLPointCloudWidget::mouseReleaseEvent(QMouseEvent* event)
{
    // 阶段二：结束相机操作
    QOpenGLWidget::mouseReleaseEvent(event);
}

void GLPointCloudWidget::wheelEvent(QWheelEvent* event)
{
    // 阶段二：缩放
    QOpenGLWidget::wheelEvent(event);
}