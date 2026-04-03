#include "PointCloud.h"
#include <QtMath>
#include <QStringList>

PointCloud::PointCloud()
    : m_minBounds(0, 0, 0)
    , m_maxBounds(0, 0, 0)
{
}

PointCloud::~PointCloud() = default;

void PointCloud::clear()
{
    m_positions.clear();
    m_colors.clear();
    m_hasBounds = false;
    m_fileName.clear();
}

void PointCloud::addPoint(const QVector3D& position)
{
    m_positions.append(position);
}

void PointCloud::addPoint(const QVector3D& position, const QVector3D& color)
{
    m_positions.append(position);
    m_colors.append(color);
}

void PointCloud::computeBoundingBox()
{
    if (m_positions.isEmpty()) {
        m_hasBounds = false;
        return;
    }

    m_minBounds = m_positions[0];
    m_maxBounds = m_positions[0];

    for (const auto& p : m_positions) {
        m_minBounds.setX(qMin(m_minBounds.x(), p.x()));
        m_minBounds.setY(qMin(m_minBounds.y(), p.y()));
        m_minBounds.setZ(qMin(m_minBounds.z(), p.z()));

        m_maxBounds.setX(qMax(m_maxBounds.x(), p.x()));
        m_maxBounds.setY(qMax(m_maxBounds.y(), p.y()));
        m_maxBounds.setZ(qMax(m_maxBounds.z(), p.z()));
    }

    m_hasBounds = true;
}

QVector3D PointCloud::center() const
{
    if (!m_hasBounds) return QVector3D(0, 0, 0);
    return (m_minBounds + m_maxBounds) * 0.5f;
}

float PointCloud::diagonal() const
{
    if (!m_hasBounds) return 0.0f;
    return (m_maxBounds - m_minBounds).length();
}

QString PointCloud::getInfoString() const
{
    QStringList info;
    info << QString("文件名: %1").arg(m_fileName.isEmpty() ? "未命名" : m_fileName);
    info << QString("点数: %1").arg(pointCount());
    info << QString("有颜色: %1").arg(hasColors() ? "是" : "否");

    if (m_hasBounds) {
        info << QString("\n包围盒:");
        info << QString("  X: [%1, %2]").arg(m_minBounds.x()).arg(m_maxBounds.x());
        info << QString("  Y: [%1, %2]").arg(m_minBounds.y()).arg(m_maxBounds.y());
        info << QString("  Z: [%1, %2]").arg(m_minBounds.z()).arg(m_maxBounds.z());
        info << QString("对角线长度: %1").arg(diagonal());
    }

    return info.join("\n");
}
