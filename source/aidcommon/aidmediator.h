#pragma once

#include <QObject>
#include <Qt3DCore/QEntity>

struct ObjModel;

class Mediator : public QObject
{
    Q_OBJECT
public:
    static Mediator& getInstance()
    {
        static Mediator instance;
        return instance;
    }

signals:
    void addConvertedObjToScene(const QString& url, const bool isAsset);
    void createMeshFromData(const ObjModel& model);
    void setCameraFromData();
    void meshLoaded(const QString& filePath);
    void addMeshToScene(const QString& meshFilePath, const bool isAsset);
    void parseXml(const QString& importfilename, const bool isAsset);
    //void readFile(const QString& importname);
    void readFile(const QString& importname, const bool isAsset);
    void childrenUpdated(const QStringList& childrenIDs);
    void entitySelected(Qt3DCore::QEntity* entity);
    void transformChanged();
    void hidePropertiesWindow();

    void cameraLinearSpeedChanged(float linearSpeed);
    void cameraLookSpeedChanged(float lookSpeed);
    void cameraAccelerationChanged(float acceleration);
    void cameraDecelerationChanged(float deceleration);

    void cameraSettingsChanged(float linearSpeed, float lookSpeed, float acceleration, float deceleration); 

    void lightColorChanged(const QColor& color);
    void lightIntensityChanged(double intensity);
    void lightPositionChanged(const QVector3D& position);

private:
    Mediator() = default;
    Mediator(const Mediator&) = delete;
    void operator=(const Mediator&) = delete;


};
