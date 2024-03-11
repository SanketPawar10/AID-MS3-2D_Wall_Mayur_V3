#pragma once

#include <QCoreApplication>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>

#include <QDebug>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <QTemporaryFile>
#include <QResource>
#include <qthread.h>

#include "aidobjmodel.h"


namespace AID {

    class Parser : public QWidget {
        Q_OBJECT

    public:
        explicit Parser(QWidget* parent = nullptr);
        ~Parser();

        QString create3dObj(const ObjModel& model, const QString& fileName, int sequenceNumber);
        
        QVector3D calculateBoundingBoxCenter(const QPair<QVector3D, QVector3D>& boundingBox);
        QPair<QVector3D, QVector3D> calculateBoundingBox(const QVector<ObjModel>& objModels);

        ObjMaterial parseMaterialElement(QXmlStreamReader& xmlReader, const uint version);
        QVector3D parseColor(const QString& colorString);
        void parseExtendElement(QXmlStreamReader& xmlReader, ObjMaterial& material);
        QMap<QString, ObjMaterial> parseMaterials(const QString& fileName, const Object3D& properties);
        void writeMTLFile(const QMap<QString, ObjMaterial>& materials, const QString& outFileName);
        
        void readXmlIntoModels(const QString& fileName, const Object3D& properties, const QMap<QString, ObjMaterial>& materials, const bool isAsset);
        QString writeObjFile(const ObjModel& model, const QString& baseName, int sequenceNumber);
        
        Object3D parseObject3dFile(const QString& fileName);
        
        
    public slots:
        void parseXmlToObj(const QString& url, const bool isAsset); // Slot to parse xml to obj
        void fileReader(const QString& importname, const bool isAsset);

    private:
        
    };
    

    

} // namespace AID
