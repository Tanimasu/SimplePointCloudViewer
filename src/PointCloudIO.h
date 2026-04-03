#pragma once

#include "PointCloud.h"
#include <QString>
#include <QObject>

/**
 * @brief 点云文件读写类
 *
 * 支持格式：
 * - PCD (Point Cloud Data)
 * - PLY (Polygon File Format)
 * - TXT (简单文本格式: x y z [r g b])
 */
class PointCloudIO : public QObject
{
    Q_OBJECT

public:
    explicit PointCloudIO(QObject* parent = nullptr);
    ~PointCloudIO();

    // 从文件加载点云
    PointCloudPtr load(const QString& filePath, QString* errorMsg = nullptr);

    // 保存点云到文件
    bool save(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg = nullptr);

    // 支持的文件格式过滤字符串（用于QFileDialog）
    static QString getOpenFileFilter();
    static QString getSaveFileFilter();

signals:
    // 读取进度信号（用于进度条）
    void progressChanged(int percent);
    void loadFinished(PointCloudPtr cloud);
    void loadFailed(const QString& error);

private:
    // 各格式解析器
    PointCloudPtr loadPCD(const QString& filePath, QString* errorMsg);
    PointCloudPtr loadPLY(const QString& filePath, QString* errorMsg);
    PointCloudPtr loadTXT(const QString& filePath, QString* errorMsg);

    bool savePCD(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg);
    bool savePLY(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg);
    bool saveTXT(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg);

    // 获取文件扩展名
    static QString getExtension(const QString& filePath);
};