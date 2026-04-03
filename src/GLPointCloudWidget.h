#pragma once

#include <QOpenGLWidget>
#include <memory>

class PointCloud;
using PointCloudPtr = std::shared_ptr<PointCloud>;

/**
 * @brief OpenGL点云渲染组件
 *
 * 阶段一：留空，仅作为占位
 * 阶段二：实现完整的点云3D渲染
 *
 * 预计学习内容：
 * - QOpenGLWidget 使用
 * - OpenGL 3.3 Core Profile 初始化
 * - GLSL 顶点/片段着色器
 * - VBO/VAO 缓冲区管理
 * - 模型-视图-投影矩阵
 * - 鼠标交互（旋转/平移/缩放）
 */
class GLPointCloudWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GLPointCloudWidget(QWidget* parent = nullptr);
    ~GLPointCloudWidget();

    // 设置要显示的点云
    void setPointCloud(const PointCloudPtr& cloud);

    // 渲染参数
    void setPointSize(float size);
    void setBackgroundColor(const QColor& color);

    // 相机控制
    void resetCamera();
    void setFrontView();
    void setTopView();
    void setSideView();

protected:
    // OpenGL 生命周期
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // 鼠标交互（用于相机控制）
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    PointCloudPtr m_cloud;  // 当前显示的点云
    float m_pointSize = 2.0f;
    QColor m_backgroundColor = QColor(50, 50, 50);

    // 阶段二再实现以下内容：
    // - OpenGL缓冲区对象
    // - 着色器程序
    // - 相机参数（位置、目标、上向量）
    // - 交互状态（鼠标位置、旋转角度等）
};