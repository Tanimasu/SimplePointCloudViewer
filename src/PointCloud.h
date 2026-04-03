#pragma once

#include <QVector3D>
#include <QVector>
#include <QString>
#include <memory>

/**
 * @brief 点云数据结构
 *
 * 存储点的位置、颜色信息
 * 后续可扩展法线、强度等属性
 */
class PointCloud
{
public:
    PointCloud();
    ~PointCloud();

    // 清空数据
    void clear();

    // 添加点
    void addPoint(const QVector3D& position);
    void addPoint(const QVector3D& position, const QVector3D& color);

    // 数据访问
    const QVector<QVector3D>& positions() const { return m_positions; }
    const QVector<QVector3D>& colors() const { return m_colors; }

    // 基本信息
    size_t pointCount() const { return m_positions.size(); }
    bool hasColors() const { return !m_colors.isEmpty(); }
    bool isEmpty() const { return m_positions.isEmpty(); }

    // 包围盒
    void computeBoundingBox();
    QVector3D minBounds() const { return m_minBounds; }
    QVector3D maxBounds() const { return m_maxBounds; }
    QVector3D center() const;
    float diagonal() const;

    // 文件信息
    void setFileName(const QString& name) { m_fileName = name; }
    QString fileName() const { return m_fileName; }

    // 数据统计信息（用于UI显示）
    QString getInfoString() const;

private:
    QVector<QVector3D> m_positions;  // 点位置
    QVector<QVector3D> m_colors;   // 点颜色（可选）

    // 包围盒
    QVector3D m_minBounds;
    QVector3D m_maxBounds;
    bool m_hasBounds = false;

    QString m_fileName;  // 来源文件名
};

using PointCloudPtr = std::shared_ptr<PointCloud>;
