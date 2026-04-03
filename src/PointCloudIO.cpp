#include "PointCloudIO.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QRegularExpression>
#include <QtMath>

PointCloudIO::PointCloudIO(QObject* parent)
    : QObject(parent)
{
}

PointCloudIO::~PointCloudIO() = default;

PointCloudPtr PointCloudIO::load(const QString& filePath, QString* errorMsg)
{
    QString ext = getExtension(filePath).toLower();

    if (ext == "pcd") {
        return loadPCD(filePath, errorMsg);
    } else if (ext == "ply") {
        return loadPLY(filePath, errorMsg);
    } else if (ext == "txt" || ext == "xyz") {
        return loadTXT(filePath, errorMsg);
    }

    if (errorMsg) {
        *errorMsg = QString("不支持的文件格式: %1").arg(ext);
    }
    return nullptr;
}

bool PointCloudIO::save(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg)
{
    if (!cloud || cloud->isEmpty()) {
        if (errorMsg) *errorMsg = "点云为空";
        return false;
    }

    QString ext = getExtension(filePath).toLower();

    if (ext == "pcd") {
        return savePCD(cloud, filePath, errorMsg);
    } else if (ext == "ply") {
        return savePLY(cloud, filePath, errorMsg);
    } else if (ext == "txt" || ext == "xyz") {
        return saveTXT(cloud, filePath, errorMsg);
    }

    if (errorMsg) {
        *errorMsg = QString("不支持的文件格式: %1").arg(ext);
    }
    return false;
}

QString PointCloudIO::getOpenFileFilter()
{
    return "点云文件 (*.pcd *.ply *.txt *.xyz);;PCD文件 (*.pcd);;PLY文件 (*.ply);;文本文件 (*.txt *.xyz);;所有文件 (*)";
}

QString PointCloudIO::getSaveFileFilter()
{
    return "PCD文件 (*.pcd);;PLY文件 (*.ply);;文本文件 (*.txt)";
}

QString PointCloudIO::getExtension(const QString& filePath)
{
    QFileInfo info(filePath);
    return info.suffix();
}

// ========== PCD格式 ==========
PointCloudPtr PointCloudIO::loadPCD(const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = "无法打开文件: " + filePath;
        return nullptr;
    }

    QTextStream stream(&file);
    auto cloud = std::make_shared<PointCloud>();
    cloud->setFileName(QFileInfo(filePath).fileName());

    // 解析PCD头
    bool isBinary = false;
    int pointCount = 0;
    int xIdx = -1, yIdx = -1, zIdx = -1;
    int rgbIdx = -1;
    int fieldCount = 0;
    bool inHeader = true;
    QStringList fieldNames;

    while (!stream.atEnd() && inHeader) {
        QString line = stream.readLine().trimmed();
        QStringList parts = line.split(QRegularExpression("\\s+"));

        if (parts.isEmpty()) continue;

        if (parts[0] == "VERSION" || parts[0] == "FIELDS" ||
            parts[0] == "SIZE" || parts[0] == "TYPE" || parts[0] == "COUNT") {
            if (parts[0] == "FIELDS") {
                for (int i = 1; i < parts.size(); ++i) {
                    fieldNames.append(parts[i]);
                }
                // 找到xyz和rgb的位置
                for (int i = 0; i < fieldNames.size(); ++i) {
                    if (fieldNames[i] == "x") xIdx = i;
                    else if (fieldNames[i] == "y") yIdx = i;
                    else if (fieldNames[i] == "z") zIdx = i;
                    else if (fieldNames[i] == "rgb" || fieldNames[i] == "rgba") rgbIdx = i;
                }
            }
        } else if (parts[0] == "POINTS") {
            pointCount = parts[1].toInt();
        } else if (parts[0] == "DATA") {
            isBinary = (parts[1] == "binary" || parts[1] == "binary_compressed");
            inHeader = false;
        }
    }

    if (xIdx < 0 || yIdx < 0 || zIdx < 0) {
        if (errorMsg) *errorMsg = "PCD文件缺少xyz字段";
        return nullptr;
    }

    fieldCount = fieldNames.size();

    // 读取数据（只支持ASCII）
    if (isBinary) {
        if (errorMsg) *errorMsg = "暂不支持二进制PCD格式";
        return nullptr;
    }

    int loaded = 0;
    while (!stream.atEnd() && loaded < pointCount) {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList values = line.split(QRegularExpression("\\s+"));
        if (values.size() < 3) continue;

        float x = values[xIdx].toFloat();
        float y = values[yIdx].toFloat();
        float z = values[zIdx].toFloat();

        if (rgbIdx >= 0 && values.size() > rgbIdx) {
            // RGB是一个打包的float，需要解码
            union { float f; quint32 i; } rgb;
            rgb.f = values[rgbIdx].toFloat();
            quint8 r = (rgb.i >> 16) & 0xFF;
            quint8 g = (rgb.i >> 8) & 0xFF;
            quint8 b = rgb.i & 0xFF;
            cloud->addPoint(QVector3D(x, y, z), QVector3D(r/255.0f, g/255.0f, b/255.0f));
        } else {
            cloud->addPoint(QVector3D(x, y, z));
        }

        loaded++;
        if (loaded % 1000 == 0) {
            emit progressChanged(qMin(100, loaded * 100 / pointCount));
        }
    }

    cloud->computeBoundingBox();
    emit progressChanged(100);
    emit loadFinished(cloud);
    return cloud;
}

bool PointCloudIO::savePCD(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = "无法创建文件";
        return false;
    }

    QTextStream stream(&file);

    // 写入头
    stream << "# .PCD v0.7 - Point Cloud Data file format\n";
    stream << "VERSION 0.7\n";

    if (cloud->hasColors()) {
        stream << "FIELDS x y z rgb\n";
        stream << "SIZE 4 4 4 4\n";
        stream << "TYPE F F F F\n";
        stream << "COUNT 1 1 1 1\n";
    } else {
        stream << "FIELDS x y z\n";
        stream << "SIZE 4 4 4\n";
        stream << "TYPE F F F\n";
        stream << "COUNT 1 1 1\n";
    }

    stream << "WIDTH " << cloud->pointCount() << "\n";
    stream << "HEIGHT 1\n";
    stream << "POINTS " << cloud->pointCount() << "\n";
    stream << "DATA ascii\n";

    // 写入数据
    const auto& positions = cloud->positions();
    const auto& colors = cloud->colors();

    for (size_t i = 0; i < cloud->pointCount(); ++i) {
        const auto& p = positions[i];
        stream << p.x() << " " << p.y() << " " << p.z();

        if (cloud->hasColors()) {
            // 打包RGB为float
            const auto& c = colors[i];
            quint32 r = qBound(0, int(c.x() * 255), 255);
            quint32 g = qBound(0, int(c.y() * 255), 255);
            quint32 b = qBound(0, int(c.z() * 255), 255);
            quint32 rgb = (r << 16) | (g << 8) | b;
            union { float f; quint32 i; } packed;
            packed.i = rgb;
            stream << " " << packed.f;
        }
        stream << "\n";
    }

    return true;
}

// ========== PLY格式 ==========
PointCloudPtr PointCloudIO::loadPLY(const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = "无法打开文件: " + filePath;
        return nullptr;
    }

    QTextStream stream(&file);
    auto cloud = std::make_shared<PointCloud>();
    cloud->setFileName(QFileInfo(filePath).fileName());

    // 解析PLY头
    bool isBinary = false;
    int vertexCount = 0;
    bool hasX = false, hasY = false, hasZ = false;
    bool hasRed = false, hasGreen = false, hasBlue = false;
    bool inHeader = true;

    while (!stream.atEnd() && inHeader) {
        QString line = stream.readLine().trimmed();
        QStringList parts = line.split(QRegularExpression("\\s+"));

        if (parts.isEmpty()) continue;

        if (parts[0] == "ply" || parts[0] == "comment") {
            continue;
        } else if (parts[0] == "format") {
            isBinary = (parts[1] != "ascii");
        } else if (parts[0] == "element" && parts[1] == "vertex") {
            vertexCount = parts[2].toInt();
        } else if (parts[0] == "property") {
            if (parts[2] == "x") hasX = true;
            else if (parts[2] == "y") hasY = true;
            else if (parts[2] == "z") hasZ = true;
            else if (parts[2] == "red" || parts[2] == "r") hasRed = true;
            else if (parts[2] == "green" || parts[2] == "g") hasGreen = true;
            else if (parts[2] == "blue" || parts[2] == "b") hasBlue = true;
        } else if (parts[0] == "end_header") {
            inHeader = false;
        }
    }

    if (!hasX || !hasY || !hasZ) {
        if (errorMsg) *errorMsg = "PLY文件缺少坐标信息";
        return nullptr;
    }

    if (isBinary) {
        if (errorMsg) *errorMsg = "暂不支持二进制PLY格式";
        return nullptr;
    }

    bool hasColor = hasRed && hasGreen && hasBlue;

    int loaded = 0;
    while (!stream.atEnd() && loaded < vertexCount) {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList values = line.split(QRegularExpression("\\s+"));
        if (values.size() < 3) continue;

        float x = values[0].toFloat();
        float y = values[1].toFloat();
        float z = values[2].toFloat();

        if (hasColor && values.size() >= 6) {
            float r = values[3].toFloat() / 255.0f;
            float g = values[4].toFloat() / 255.0f;
            float b = values[5].toFloat() / 255.0f;
            cloud->addPoint(QVector3D(x, y, z), QVector3D(r, g, b));
        } else {
            cloud->addPoint(QVector3D(x, y, z));
        }

        loaded++;
        if (loaded % 1000 == 0) {
            emit progressChanged(qMin(100, loaded * 100 / vertexCount));
        }
    }

    cloud->computeBoundingBox();
    emit progressChanged(100);
    emit loadFinished(cloud);
    return cloud;
}

bool PointCloudIO::savePLY(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = "无法创建文件";
        return false;
    }

    QTextStream stream(&file);

    // 写入头
    stream << "ply\n";
    stream << "format ascii 1.0\n";
    stream << "comment Created by SimplePointCloudViewer\n";
    stream << "element vertex " << cloud->pointCount() << "\n";
    stream << "property float x\n";
    stream << "property float y\n";
    stream << "property float z\n";

    if (cloud->hasColors()) {
        stream << "property uchar red\n";
        stream << "property uchar green\n";
        stream << "property uchar blue\n";
    }

    stream << "end_header\n";

    // 写入数据
    const auto& positions = cloud->positions();
    const auto& colors = cloud->colors();

    for (size_t i = 0; i < cloud->pointCount(); ++i) {
        const auto& p = positions[i];
        stream << p.x() << " " << p.y() << " " << p.z();

        if (cloud->hasColors()) {
            const auto& c = colors[i];
            int r = qBound(0, int(c.x() * 255), 255);
            int g = qBound(0, int(c.y() * 255), 255);
            int b = qBound(0, int(c.z() * 255), 255);
            stream << " " << r << " " << g << " " << b;
        }
        stream << "\n";
    }

    return true;
}

// ========== TXT格式 ==========
PointCloudPtr PointCloudIO::loadTXT(const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = "无法打开文件: " + filePath;
        return nullptr;
    }

    QTextStream stream(&file);
    auto cloud = std::make_shared<PointCloud>();
    cloud->setFileName(QFileInfo(filePath).fileName());

    // 先读取第一行判断格式
    QString firstLine = stream.readLine().trimmed();
    stream.seek(0);

    bool hasColor = false;
    if (!firstLine.isEmpty()) {
        QStringList parts = firstLine.split(QRegularExpression("\\s+"));
        hasColor = (parts.size() >= 6);  // x y z r g b
    }

    int totalLines = 0;
    while (!stream.atEnd()) {
        stream.readLine();
        totalLines++;
    }
    stream.seek(0);

    int loaded = 0;
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;

        QStringList values = line.split(QRegularExpression("\\s+"));
        if (values.size() < 3) continue;

        float x = values[0].toFloat();
        float y = values[1].toFloat();
        float z = values[2].toFloat();

        if (hasColor && values.size() >= 6) {
            float r = values[3].toFloat();
            float g = values[4].toFloat();
            float b = values[5].toFloat();
            // 判断是否是0-255范围
            if (r > 1.0f || g > 1.0f || b > 1.0f) {
                r /= 255.0f;
                g /= 255.0f;
                b /= 255.0f;
            }
            cloud->addPoint(QVector3D(x, y, z), QVector3D(r, g, b));
        } else {
            cloud->addPoint(QVector3D(x, y, z));
        }

        loaded++;
        if (loaded % 1000 == 0) {
            emit progressChanged(qMin(100, loaded * 100 / totalLines));
        }
    }

    cloud->computeBoundingBox();
    emit progressChanged(100);
    emit loadFinished(cloud);
    return cloud;
}

bool PointCloudIO::saveTXT(const PointCloudPtr& cloud, const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = "无法创建文件";
        return false;
    }

    QTextStream stream(&file);
    stream << "# SimplePointCloudViewer export\n";
    stream << "# Format: x y z";
    if (cloud->hasColors()) stream << " r g b";
    stream << "\n";

    const auto& positions = cloud->positions();
    const auto& colors = cloud->colors();

    for (size_t i = 0; i < cloud->pointCount(); ++i) {
        const auto& p = positions[i];
        stream << p.x() << " " << p.y() << " " << p.z();

        if (cloud->hasColors()) {
            const auto& c = colors[i];
            int r = qBound(0, int(c.x() * 255), 255);
            int g = qBound(0, int(c.y() * 255), 255);
            int b = qBound(0, int(c.z() * 255), 255);
            stream << " " << r << " " << g << " " << b;
        }
        stream << "\n";
    }

    return true;
}