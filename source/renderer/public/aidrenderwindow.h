#pragma once

#include <vector>

#include <QWidget>
#include <QUrl>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QMargins>
#include <qfileinfo.h>
#include <QPainter>
#include <QPolygonF>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/qorbitcameracontroller.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QTexture>
#include <Qt3DCore/QBoundingVolume>
#include <QTextureMaterial>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QDebugOverlay>

#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QPickingSettings>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QShaderData>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QStencilOperation>

#include <QVector3D>
#include <QMatrix4x4>

#include <QRectF>

#include <QRandomGenerator>
#include <QDateTime>

#include "aidobjmodel.h"
#include "aidcamera.h"
#include "aidoutlinematerial.h"

#include "Plane3D.h"
#include "Line3D.h"

// QOL includes
#include <QElapsedTimer>
#include "qt3dgizmo.h"



namespace AID {
  struct CModel
  {
    QVector<QVector3D> positions;
    QVector<ushort> indices;
    QVector<QVector3D> normals;
  };

  struct BoundingBox3D {
    QVector3D minPt;
    QVector3D maxPt;
  };

  struct Face {
    QVector3D p1, p2, p3, p4;
  };

  struct Rect2D {
    float width;
    float height;
  };
  
  class RenderWindow : public QWidget {
        Q_OBJECT

    public:
        explicit RenderWindow(QWidget* parent = nullptr);
        ~RenderWindow();
        //void createMesh(const ObjModel& model);
        void deleteSelectedEntity();
        void mousePressEvent(QMouseEvent* event);
        
        Qt3DCore::QEntity* findFirstEntity(Qt3DCore::QEntity* rootEntity);
        void printVertices(Qt3DCore::QNodeId s);
        void findMeshes(Qt3DCore::QNode* node, QList<Qt3DRender::QMesh*>& meshes);
        QVector3D centerGeometry(Qt3DRender::QGeometryRenderer* geometryRenderer);
        void Call2DWindow();
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override;
        void updateTreeListWidget();


    signals:
        //void meshLoaded(const QString& filePath);  // Notify that the mesh was loaded
        void centerCalculated();

    public slots:
        void onEntityClicked(Qt3DRender::QPickEvent* event);
        void loadAndSetMesh(const QString& url, const bool isAsset);  // Slot to load and set the mesh
        void createMesh(const ObjModel& model);
        void setCameraView();
        void onCameraLinearSpeedChanged(float linearSpeed);
        void onCameraLookSpeedChanged(float lookSpeed);
        void onCameraAccelerationChanged(float acceleration);
        void onCameraDecelerationChanged(float deceleration);

        void onLightColorChanged(const QColor& color);

        void onLightIntensityChanged(double intensity);

        void onLightPositionChanged(const QVector3D& position);
        //void onSceneLoaded(Qt3DRender::QSceneLoader::Status status);


    private:
        Qt3DExtras::Qt3DWindow* view;
        
        Qt3DRender::QCamera* cameraEntity;

        AID::AIDCamera* camController;

        Qt3DCore::QEntity* rootEntity;

        QVector<Qt3DCore::QEntity*> entityList;

        int diffuseRed;
        int diffuseGreen;
        int diffuseBlue;
        int diffuseAlpha;
        std::vector<QColor> Qcolors;
        int colorValue;

        Qt3DRender::QEffect* phongEffect;

        float customLightIntensity;
        QVector3D customLightPostition;

        Qt3DRender::QParameter* lightIntensityParam1;
        Qt3DRender::QParameter* lightPositionParam1;

        Qt3DRender::QParameter* lightIntensityParam2;
        Qt3DRender::QParameter* lightPositionParam2;

        int wavefrontObjCount;
        QVariant savedAmbientColor;
        QVariant savedDiffuseColor;
        Qt3DGizmo* gizmo;
        Qt3DRender::QLayer* gizmoLayer;
        Qt3DRender::QLayer* objectsLayer;

        QVector3D absoluteCenterForObj;
        
        Qt3DCore::QEntity* selectedEntity;
        Qt3DRender::QMaterial* selectedMaterial; // Change it to proper material type
        Qt3DCore::QTransform* selectedTransform;
        QColor savedAmbientColorObj;
        QColor savedDiffuseColorObj;
        Qt3DCore::QEntity* lastSelectedEntity;
        AIDOutlineMaterial* outlineMaterial;
        
        Qt3DCore::QEntity* selectedEntityObj;
        Qt3DExtras::QPhongMaterial* selectedMaterialObj;
        Qt3DCore::QTransform* selectedTransformObj;

        QList<Qt3DCore::QTransform*> transformList;

        Qt3DCore::QEntity* lightEntity1;
        Qt3DCore::QEntity* lightEntity2;
        Qt3DRender::QPointLight* pointLight1;
        Qt3DRender::QPointLight* pointLight2;
        Qt3DCore::QTransform* lightTransform1;
        Qt3DCore::QTransform* lightTransform2;

        int object_id;
        bool dragStarted;
        QVector<CModel> calculatePositionsInRealWorld(Qt3DCore::QEntity* pickedEntity);
        QVector<QVector<CLine3D>> generateBoundariesForWallAndItsSubmodels(QVector<QVector3D> positions, QVector<ushort> indices, QVector<QVector3D> normals, bool isWall, Plane3D plane, BoundingBox3D box);
        QVector<QVector<CLine3D>> GenerateBoundary(const QVector<QVector3D>& vList);
        QVector<QVector<CLine3D>> GenerateClosedLineSegments(const QVector<CLine3D>& linesList);
        bool IsAdjacent(const CLine3D& pTempLine1, const CLine3D& pTempLine2);
        QVector<CLine3D> JoinLineSegments(QVector<CLine3D> segments);
        QVector<CLine3D> GenerateLineSegments(const QVector<QVector3D>& vList);
        void CallParentEntity(Qt3DCore::QEntity** currentEntity);
        QVector<QPointF> findTilesInPolygon(QVector<QPointF> polygon, const Rect2D& tile);
        bool isPointInPolygon(QPointF p, QVector<QPointF>& polygon);
        bool onSegment(QPointF p, QPointF q, QPointF r);
        int orientation(QPointF p, QPointF q, QPointF r);
        QVector<Face> GenerateCubeFaces(const QVector3D& min, const QVector3D& max);
        };


} // namespace AID




