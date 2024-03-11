#include "aidrenderwindow.h"
#include "aidmediator.h"
//#include "twicereflectingmaterial.h"
#include "aid2dViewDialog.h"
#include <math.h>


namespace AID {

    RenderWindow::RenderWindow(QWidget* parent)
        : QWidget(parent), view(new Qt3DExtras::Qt3DWindow()), rootEntity(new Qt3DCore::QEntity()), gizmo(new Qt3DGizmo())
    {
       
        QWidget* container = QWidget::createWindowContainer(view, this);
        
        int diffuseRed = 0;
        int diffuseGreen = 0;
        int diffuseBlue = 0;
        int diffuseAlpha = 0;
        wavefrontObjCount = 0;
        absoluteCenterForObj = QVector3D(0.0f, 0.0f, 0.0f);

        Qcolors = {
        Qt::red,
        Qt::green,
        Qt::blue,        
        Qt::cyan,        
        Qt::magenta,        
        Qt::yellow,        
        Qt::gray,        
        Qt::lightGray
        };
        colorValue = 0;
        
        selectedEntity = nullptr;
        selectedEntityObj = nullptr;
        
        selectedMaterial = nullptr;
        selectedTransform = nullptr;

        selectedTransformObj = nullptr;
        selectedMaterialObj = nullptr;

        lastSelectedEntity = nullptr;
        outlineMaterial = new AIDOutlineMaterial();

        dragStarted = false;

        view->camera()->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 100000.0f);
        view->camera()->setPosition(QVector3D(0, 0, 100.0f));
        view->camera()->setUpVector(QVector3D(0, 1, 0));
        view->camera()->setViewCenter(QVector3D(0, 0, 0));
        view->camera()->setObjectName("Camera");
        
        this->setContextMenuPolicy(Qt::ActionsContextMenu);
/*
        // For camera controls
        Qt3DExtras::QOrbitCameraController* camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
        //Qt3DExtras::QFirstPersonCameraController* camController = new Qt3DExtras::QFirstPersonCameraController(rootEntity);
        camController->setCamera(view->camera());
        camController->setAcceleration(20.0);
        camController->setLinearSpeed(50.0);
 */       
        camController = new AID::AIDCamera(rootEntity);
        camController->setCamera(view->camera());
        camController->setAcceleration(20.0);
        camController->setLinearSpeed(50.0);
        camController->setDeceleration(20.0);
        camController->setLookSpeed(20);
        camController->setObjectName("Camera controller");

        emit Mediator::getInstance().cameraSettingsChanged(camController->linearSpeed(), camController->lookSpeed(), camController->acceleration(), camController->deceleration());

        connect(camController, &AID::AIDCamera::linearSpeedChanged, this, [this]() {
            emit Mediator::getInstance().cameraSettingsChanged(camController->linearSpeed(), camController->lookSpeed(), camController->acceleration(), camController->deceleration());
            });
        connect(camController, &AID::AIDCamera::lookSpeedChanged, this, [this]() {
            emit Mediator::getInstance().cameraSettingsChanged(camController->linearSpeed(), camController->lookSpeed(), camController->acceleration(), camController->deceleration());
            });
        connect(camController, &AID::AIDCamera::accelerationChanged, this, [this]() {
            emit Mediator::getInstance().cameraSettingsChanged(camController->linearSpeed(), camController->lookSpeed(), camController->acceleration(), camController->deceleration());
            });
        connect(camController, &AID::AIDCamera::decelerationChanged, this, [this]() {
            emit Mediator::getInstance().cameraSettingsChanged(camController->linearSpeed(), camController->lookSpeed(), camController->acceleration(), camController->deceleration());
            });

        gizmo->setParent(rootEntity);
        gizmo->setWindowSize(view->size());
        gizmo->setCamera(view->camera());
        gizmo->setScale(100);
        gizmo->setScaleToCameraDistance(true);
        gizmoLayer = new Qt3DRender::QLayer;
        gizmoLayer->setRecursive(true);
        gizmo->addComponent(gizmoLayer);
        gizmo->setHideMouseWhileTransforming(false);

        objectsLayer = new Qt3DRender::QLayer;
        objectsLayer->setRecursive(true);
  
        // Working code
        Qt3DRender::QRenderSurfaceSelector* surfaceSelector = new Qt3DRender::QRenderSurfaceSelector;
        surfaceSelector->setSurface(view);
        
        Qt3DRender::QViewport* viewport = new Qt3DRender::QViewport(surfaceSelector);
        viewport->setNormalizedRect(QRectF(0, 0, 1.0, 1.0));
        
        Qt3DRender::QCameraSelector* cameraSelector = new Qt3DRender::QCameraSelector(viewport);
        cameraSelector->setCamera(view->camera());
        
        Qt3DRender::QRenderStateSet* renderStateSet = new Qt3DRender::QRenderStateSet(cameraSelector);
        
        Qt3DRender::QDepthTest* depthTest = new Qt3DRender::QDepthTest;
        depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
        renderStateSet->addRenderState(depthTest);

        Qt3DRender::QCullFace* cullFace = new Qt3DRender::QCullFace(renderStateSet);
        cullFace->setMode(Qt3DRender::QCullFace::NoCulling);
        renderStateSet->addRenderState(cullFace);

        Qt3DRender::QClearBuffers* clearBuffers = new Qt3DRender::QClearBuffers(renderStateSet);
        clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
        clearBuffers->setClearColor(Qt::gray);
        Qt3DRender::QNoDraw* noDraw = new Qt3DRender::QNoDraw(clearBuffers);

        Qt3DRender::QLayerFilter* layerFilter1 = new Qt3DRender::QLayerFilter(renderStateSet);
        layerFilter1->addLayer(objectsLayer);

        Qt3DRender::QClearBuffers* clearBuffers2 = new Qt3DRender::QClearBuffers(renderStateSet);
        clearBuffers2->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
        Qt3DRender::QNoDraw* noDraw2 = new Qt3DRender::QNoDraw(clearBuffers2);

        Qt3DRender::QLayerFilter* layerFilter2 = new Qt3DRender::QLayerFilter(renderStateSet);
        layerFilter2->addLayer(gizmoLayer);
        
        // Create a new layer for the outline entities
        Qt3DRender::QLayer* outlineLayer = new Qt3DRender::QLayer();
        
        // Framegraph node will become the child of the parent, so there is a chain to follow here, you can set
        // the top node as activeFrameGraph, always keep debugOverlay as last node
        // Check Render views in debug overlay
        
         Qt3DRender::QDebugOverlay* debugOverlay = new Qt3DRender::QDebugOverlay(layerFilter2);
        
        // Frustrum culling can be added later
        view->setActiveFrameGraph(surfaceSelector);
        
        Qt3DRender::QPickingSettings::PickMethod trianglePick = Qt3DRender::QPickingSettings::PickMethod::TrianglePicking;
        view->renderSettings()->pickingSettings()->setPickMethod(trianglePick);
        view->renderSettings()->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::NearestPriorityPick);

        Qt3DRender::QPickingSettings::FaceOrientationPickingMode facePick = Qt3DRender::QPickingSettings::FaceOrientationPickingMode::FrontAndBackFace;
        view->renderSettings()->pickingSettings()->setFaceOrientationPickingMode(facePick);
        
        view->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::Always);

        // Material - Let's leave it here
        Qt3DRender::QMaterial* material = new Qt3DExtras::QPhongMaterial(rootEntity);

        // First Light Entity
        lightEntity1 = new Qt3DCore::QEntity(rootEntity);
        pointLight1 = new Qt3DRender::QPointLight(lightEntity1);
        pointLight1->setColor("white");
        pointLight1->setIntensity(1.0);

        lightTransform1 = new Qt3DCore::QTransform;
        lightTransform1->setTranslation(QVector3D(-325.0f, 100.0f, 134.0f));

        lightEntity1->addComponent(pointLight1);
        lightEntity1->addComponent(lightTransform1);
        lightEntity1->setObjectName("Point Light 1");

        lightIntensityParam1 = new Qt3DRender::QParameter();
        lightIntensityParam1->setName("lightIntensity1");
        lightIntensityParam1->setValue(0.8f); // Set initial value

        lightPositionParam1 = new Qt3DRender::QParameter();
        lightPositionParam1->setName("lightPosition1");
        lightPositionParam1->setValue(QVector3D(-325.0f, 100.0f, 134.0f));


        // Second Light Entity
        lightEntity2 = new Qt3DCore::QEntity(rootEntity);
        pointLight2 = new Qt3DRender::QPointLight(lightEntity2);
        pointLight2->setColor("white");
        pointLight2->setIntensity(1.0);

        lightTransform2 = new Qt3DCore::QTransform;
        lightTransform2->setTranslation(QVector3D(-325.0f, 100.0f, -134.0f)); // Adjust the position for the second light

        lightEntity2->addComponent(pointLight2);
        lightEntity2->addComponent(lightTransform2);
        lightEntity2->setObjectName("Point Light 2");

        lightIntensityParam2 = new Qt3DRender::QParameter();
        lightIntensityParam2->setName("lightIntensity2");
        lightIntensityParam2->setValue(0.8f); // Set initial value

        lightPositionParam2 = new Qt3DRender::QParameter();
        lightPositionParam2->setName("lightPosition2");
        lightPositionParam2->setValue(QVector3D(-325.0f, 100.0f,-134.0f)); // Adjust the position for the second light

/*
        connect(pointLight, &Qt3DRender::QPointLight::colorChanged, this, [this]() {
            emit Mediator::getInstance().lightColorChanged(pointLight->color());
            });
        connect(pointLight, &Qt3DRender::QPointLight::intensityChanged, this, [this]() {
            emit Mediator::getInstance().lightIntensityChanged(pointLight->intensity());
            });
        connect(lightTransform, &Qt3DCore::QTransform::translationChanged, this, [this]() {
            emit Mediator::getInstance().lightPositionChanged(lightTransform->translation());
            });
 */
        connect(&Mediator::getInstance(), &Mediator::lightColorChanged, this, &RenderWindow::onLightColorChanged);
        connect(&Mediator::getInstance(), &Mediator::lightIntensityChanged, this, &RenderWindow::onLightIntensityChanged);
        connect(&Mediator::getInstance(), &Mediator::lightPositionChanged, this, &RenderWindow::onLightPositionChanged);


        // Try commenting this if you are stuck
        QObject::connect(view, &Qt3DExtras::Qt3DWindow::widthChanged,
            gizmo, &Qt3DGizmo::setWindowWidth);
        QObject::connect(view, &Qt3DExtras::Qt3DWindow::heightChanged,
            gizmo, &Qt3DGizmo::setWindowHeight);
        
/*
        QObject::connect(view, &Qt3DExtras::Qt3DWindow::widthChanged, this,
            [this](int newWidth) {
                emit widthChanged(newWidth);
            });

        QObject::connect(view, &Qt3DExtras::Qt3DWindow::heightChanged, this,
            [this](int newHeight) {
                emit heightChanged(newHeight);
            });
*/
        connect(&Mediator::getInstance(), &Mediator::addMeshToScene, this, &RenderWindow::loadAndSetMesh);
        connect(&Mediator::getInstance(), &Mediator::addConvertedObjToScene, this, &RenderWindow::loadAndSetMesh);
        connect(&Mediator::getInstance(), &Mediator::createMeshFromData, this, &RenderWindow::createMesh);
        connect(&Mediator::getInstance(), &Mediator::setCameraFromData, this, &RenderWindow::setCameraView);

        connect(&Mediator::getInstance(), &Mediator::cameraLinearSpeedChanged, this, &RenderWindow::onCameraLinearSpeedChanged);
        connect(&Mediator::getInstance(), &Mediator::cameraLookSpeedChanged, this, &RenderWindow::onCameraLookSpeedChanged);
        connect(&Mediator::getInstance(), &Mediator::cameraAccelerationChanged, this, &RenderWindow::onCameraAccelerationChanged);
        connect(&Mediator::getInstance(), &Mediator::cameraDecelerationChanged, this, &RenderWindow::onCameraDecelerationChanged);

        Qt3DCore::QTransform* dummyTransform = new Qt3DCore::QTransform();
        transformList.append(dummyTransform);

        //Qt3DRender::QEffect* effect = outlineMaterial->effect();
        //Qt3DRender::QTechnique* technique = effect->techniques().first();
        //Qt3DRender::QRenderPass* renderPass = technique->renderPasses().first();
        //Qt3DRender::QShaderProgram* shaderProgram = renderPass->shaderProgram();

        //Qt3DRender::QParameter* outlineScaleParam = new Qt3DRender::QParameter(QStringLiteral("outlineScale"), 1.0f);
        //Qt3DRender::QParameter* outlineColorParam = new Qt3DRender::QParameter(QStringLiteral("outlineColor"), QColor(Qt::red));

        //effect->addParameter(outlineScaleParam);
        //effect->addParameter(outlineColorParam);

        Qt3DRender::QShaderProgram* PhongshaderProgram = new Qt3DRender::QShaderProgram();

        PhongshaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/default.vert"))));
        PhongshaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/test.frag"))));

        Qt3DRender::QRenderPass* renderPass = new Qt3DRender::QRenderPass();
        renderPass->setShaderProgram(PhongshaderProgram);

        // without this filter the default forward renderer would not render this
        Qt3DRender::QFilterKey* filterKey = new Qt3DRender::QFilterKey;
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue("forward");

        Qt3DRender::QTechnique* technique = new Qt3DRender::QTechnique();
        technique->addRenderPass(renderPass);
        technique->addFilterKey(filterKey);
        technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
        technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
        technique->graphicsApiFilter()->setMajorVersion(3);
        technique->graphicsApiFilter()->setMinorVersion(3);

        phongEffect = new Qt3DRender::QEffect();
        phongEffect->addTechnique(technique);

        object_id = 0;

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(container);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
        //qDebug() << "layout->totalSizeHint()" << layout->totalSizeHint();

        view->installEventFilter(this);
        view->setRootEntity(rootEntity);
        updateTreeListWidget();
        //qDebug() << "view->size();" << view->size();
    }

    RenderWindow::~RenderWindow()
    {
    }

    QVector<Face> RenderWindow::GenerateCubeFaces(const QVector3D& min, const QVector3D& max) {
      QVector<Face> faces;
      // front face
      faces.push_back({ {min.x(), min.y(), min.z()}, {max.x(), min.y(), min.z()}, {max.x(), max.y(), min.z()}, {min.x(), max.y(), min.z()} });
      // back face
      faces.push_back({ {max.x(), min.y(), max.z()}, {min.x(), min.y(), max.z()}, {min.x(), max.y(), max.z()}, {max.x(), max.y(), max.z()} });
      // left face
      faces.push_back({ {min.x(), min.y(), max.z()}, {min.x(), min.y(), min.z()}, {min.x(), max.y(), min.z()}, {min.x(), max.y(), max.z()} });
      // right face
      faces.push_back({ {max.x(), min.y(), min.z()}, {max.x(), min.y(), max.z()}, {max.x(), max.y(), max.z()}, {max.x(), max.y(), min.z()} });
      // top face
      faces.push_back({ {min.x(), max.y(), min.z()}, {max.x(), max.y(), min.z()}, {max.x(), max.y(), max.z()}, {min.x(), max.y(), max.z()} });
      // bottom face
      faces.push_back({ {min.x(), min.y(), max.z()}, {max.x(), min.y(), max.z()}, {max.x(), min.y(), min.z()}, {min.x(), min.y(), min.z()} });
      return faces;
    }

    BoundingBox3D getBoundingBox3D(const QVector<QVector3D>& points) {
      if (points.empty()) {
        // Handle empty input
        return { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
      }

      // Initialize min and max points
      QVector3D min_point = points[0];
      QVector3D max_point = points[0];

      // Update min and max coordinates
      for (const auto& p : points) {

        min_point.setX(std::min(min_point.x(), p.x()));
        min_point.setY(std::min(min_point.y(), p.y()));
        min_point.setZ(std::min(min_point.z(), p.z()));
        max_point.setX(std::max(max_point.x(), p.x()));
        max_point.setY(std::max(max_point.y(), p.y()));
        max_point.setZ(std::max(max_point.z(), p.z()));
      }

      return { min_point, max_point };
    }

    QVector<CModel> RenderWindow::calculatePositionsInRealWorld(Qt3DCore::QEntity* pickedEntity)
    {
      QVector<CModel> modelsList;

      if (!pickedEntity)
        return modelsList;

      QList<Qt3DRender::QGeometryRenderer*> geometryRendererList =
        pickedEntity->findChildren<Qt3DRender::QGeometryRenderer*>();

      if (geometryRendererList.isEmpty())
        return modelsList;
      
      for (Qt3DRender::QGeometryRenderer* geometryRenderer : geometryRendererList)
      {
        CModel model;

        if (!geometryRenderer)
          return modelsList;

        Qt3DCore::QGeometry* geometry = geometryRenderer->geometry();

        if (!geometry)
          return modelsList;

        for (Qt3DCore::QAttribute* attribute : geometry->attributes()) {
          QByteArray bufferData = attribute->buffer()->data();

          if (attribute->name() == Qt3DCore::QAttribute::defaultPositionAttributeName() && attribute->count() > 0) {
            const float* rawPositionData = reinterpret_cast<const float*>(bufferData.constData());
            const int count = attribute->count();
            model.positions.reserve(count);
            for (int i = 0; i < count; ++i) {
              QVector3D vertex(
                rawPositionData[i * 3],
                rawPositionData[i * 3 + 1],
                rawPositionData[i * 3 + 2]
              );
              model.positions.push_back( vertex);
            }
          }
          else if (attribute->name() == Qt3DCore::QAttribute::defaultJointIndicesAttributeName() && attribute->count() > 0) {
            const ushort* rawTexCoordData = reinterpret_cast<const ushort*>(bufferData.constData());
            const int count = attribute->count();
            model.indices.reserve(count);
            //qDebug() << attribute->name();
            for (int i = 0; i < count; ++i) {
              model.indices.push_back(rawTexCoordData[i]);
              //qDebug() << rawTexCoordData[i];
            }
          }
          else if (attribute->name() == Qt3DCore::QAttribute::defaultNormalAttributeName() && attribute->count() > 0) {
            const float* rawNormalData = reinterpret_cast<const float*>(bufferData.constData());
            const int count = attribute->count();
            model.normals.reserve(count);
            //qDebug() << attribute->name();
            for (int i = 0; i < count; ++i) {
              QVector3D normal(
                rawNormalData[i * 3],
                rawNormalData[i * 3 + 1],
                rawNormalData[i * 3 + 2]
              );
              model.normals.push_back(normal);
            }
          }
        }  modelsList.push_back(model);

      }
      return modelsList;
    }

    void RenderWindow::createMesh(const ObjModel& model)
    {
        Qt3DCore::QEntity* modelrootEntity = nullptr;
        Qt3DCore::QEntity* customMeshEntity = nullptr;

        Qt3DRender::QObjectPicker* picker = new Qt3DRender::QObjectPicker;
        Qt3DCore::QTransform* transform = new Qt3DCore::QTransform;

        //QVector3D translate;
        //translate.setX(QRandomGenerator::global()->bounded(-10, 10));

        // if vertex is 10,10,10 and boundingbox center is at 10,10,10, the model will be moved negative of center
        // so it will be moved 10,10,10 in opposite direction, thereby putting it at center
        // it's actually 0 - model.center
        transform->setTranslation(-model.center);
        QVector3D modelOffset = model.center;
        
               
        qDebug() << "\n***" << "Model center is at:" << model.center << "***";
        qDebug() << "***" << "Model moved by:" << -model.center << " from center to position at 0, 0, 0" << "***";
        qDebug() << "***" << "Gizmo location will be at:" << -model.center << "***\n";

        qDebug() << "\n        ***New model loading***\n";
        
        if (model.count == 0 && model.isAsset)
        {
            modelrootEntity = new Qt3DCore::QEntity(rootEntity);
            entityList.append(modelrootEntity);
            customMeshEntity = new Qt3DCore::QEntity(modelrootEntity);
            qDebug() << "Loaded model is asset:" << model.isAsset;
            qDebug() << "model count:" << model.count;
            qDebug() << "New model root created and added to entityList. It's count is:" << entityList.count();
            //modelrootEntity->addComponent(transform);
            //modelrootEntity->addComponent(picker);
            //modelrootEntity->addComponent(objectsLayer);

            ////customMeshEntity->addComponent(torusPicker);
            //modelrootEntity->setObjectName(model.name);
        }
        else if (model.count > 0 && model.isAsset)
        {
            if (!entityList.isEmpty())
            {
                modelrootEntity = entityList.last();
                customMeshEntity = new Qt3DCore::QEntity(modelrootEntity);
                qDebug() << "Loaded model is asset:" << model.isAsset;
                qDebug() << "model count:" << model.count;
                qDebug() << "Entity list is not empty and modelrootEntity is retrived from last entry of entityList";

            }
            else
            {
                qDebug() << "Entity list is empty. Falling back to root entity";
                modelrootEntity = new Qt3DCore::QEntity(rootEntity);
                customMeshEntity = new Qt3DCore::QEntity(modelrootEntity);
                /*modelrootEntity->addComponent(transform);
                modelrootEntity->addComponent(picker);
                modelrootEntity->addComponent(objectsLayer);*/
                qDebug() << "Loaded model is asset:" << model.isAsset;
                qDebug() << "model count:" << model.count;
                qDebug() << "Creating new modelrootEntity with rootEntity as parent";
            }
        }
        else if (!model.isAsset)
        {
            modelrootEntity = new Qt3DCore::QEntity(rootEntity);
            customMeshEntity = new Qt3DCore::QEntity(modelrootEntity);
           /* modelrootEntity->addComponent(transform);
            modelrootEntity->addComponent(picker);
            modelrootEntity->addComponent(objectsLayer);*/
            qDebug() << "Loaded model is asset:" << model.isAsset;
            qDebug() << "model count:" << model.count;
            qDebug() << "Creating new modelrootEntity with rootEntity as parent";
        }
        else
        {
            qDebug() << "Unknown model parent behaviour, check createMesh function";
            qDebug() << "Loaded model is asset:" << model.isAsset;
            qDebug() << "model count:" << model.count;
        }
        Qt3DExtras::QTextureMaterial* material = new Qt3DExtras::QTextureMaterial;
      

        Qt3DCore::QTransform* gizmoTransform = new Qt3DCore::QTransform;
        gizmoTransform->setTranslation(model.localCenter);

        //Qt3DRender::QGeometryRenderer* customMeshRenderer = new Qt3DRender::QGeometryRenderer;
        QVector<QPair<Qt3DRender::QGeometryRenderer*, Qt3DRender::QMaterial*>>  geometryRendererVector;

        Qt3DCore::QGeometry* customGeometry = new Qt3DCore::QGeometry(customMeshEntity);

        Qt3DCore::QBuffer* vertexDataBuffer = new Qt3DCore::QBuffer(customGeometry);
        Qt3DCore::QBuffer* faceDataBuffer = new Qt3DCore::QBuffer(customGeometry);


        bool isTexEmpty = model.texCoords.empty();
        bool isNormalEmpty = model.normals.empty();
        bool isMatEmpty = model.matGroups.empty();
        
        QByteArray vertexBufferData;
        int vertexSize = model.vertices.size();
        //qDebug() << "Vertex Size: " << vertexSize;
        vertexBufferData.resize(vertexSize * 3 * sizeof(float));
        vertexDataBuffer->setData(vertexBufferData);
        
        const QVector<QVector3D> vertices = model.vertices;

        float* rawVertexArray = reinterpret_cast<float*>(vertexBufferData.data());
        int idx = 0;

        for (const QVector3D& v : vertices) {
            rawVertexArray[idx++] = v.x();
            rawVertexArray[idx++] = v.y();
            rawVertexArray[idx++] = v.z();
            //qDebug() << v.x() << v.y() << v.z();
        }

        //qDebug() << "rawVertexArray[7]" << rawVertexArray[7];
        //qDebug() << "Vertex id after importing this geometry: " << idx;

        // Indices (12)
        QByteArray faceBufferData;
        int faceSize = model.faces.size();
        //qDebug() << "Face Size: " << faceSize;
        faceBufferData.resize(faceSize * 3 * sizeof(ushort));
        ushort* rawIndexArray = reinterpret_cast<ushort*>(faceBufferData.data());
        int fdx = 0;

        const QVector<QVector3D> faces = model.faces;
        
        for (const QVector3D& f : faces) {
            rawIndexArray[fdx++] = f.x();
            rawIndexArray[fdx++] = f.y();
            rawIndexArray[fdx++] = f.z();
            //qDebug() << f.x() << f.y() << f.z();
        }
        //qDebug() << "rawIndexArray[7]" << rawIndexArray[7];
        //qDebug() << "Index id after importing this geometry: " << fdx;
        
        QByteArray texCoordBufferData;
        int texCoordSize;
        float* rawTexCoordArray;
        Qt3DCore::QBuffer* texCoordDataBuffer;
        const QVector<QPointF> texCoords = model.texCoords;
        
        if (!isTexEmpty)
        {
            texCoordSize = model.texCoords.size();
            texCoordBufferData.resize(texCoordSize * 2 * sizeof(float)); // Assuming 2D texture coordinates

            rawTexCoordArray = reinterpret_cast<float*>(texCoordBufferData.data());
            int tdx = 0;

            for (const QPointF& tc : texCoords) {
                rawTexCoordArray[tdx++] = tc.x();
                rawTexCoordArray[tdx++] = tc.y();
            }

            texCoordDataBuffer = new Qt3DCore::QBuffer(customGeometry);
            texCoordDataBuffer->setData(texCoordBufferData);
        }
        
        QVector<QVector3D> vertexNormals(vertices.size());

        QMap<int, QVector<QVector3D>> vertexToFaceNormalsMap;

        for (int i = 0; i < faces.size(); ++i) {
            QVector3D v1 = vertices[faces[i].x()];
            QVector3D v2 = vertices[faces[i].y()];
            QVector3D v3 = vertices[faces[i].z()];

            QVector3D normal = QVector3D::crossProduct(v2 - v1, v3 - v1).normalized();

            vertexToFaceNormalsMap[faces[i].x()].push_back(normal);
            vertexToFaceNormalsMap[faces[i].y()].push_back(normal);
            vertexToFaceNormalsMap[faces[i].z()].push_back(normal);
        }

        for (int i = 0; i < vertices.size(); ++i) {
            QVector3D sum = QVector3D(0, 0, 0);

            for (const QVector3D& normal : vertexToFaceNormalsMap[i]) {
                sum += normal;
            }

            vertexNormals[i] = sum.normalized();
        }
        
        Qt3DCore::QBuffer* normalDataBuffer = new Qt3DCore::QBuffer(customGeometry);
        QByteArray normalBufferData;
        normalBufferData.resize(vertices.size() * 3 * sizeof(float));

        float* rawNormalArray = reinterpret_cast<float*>(normalBufferData.data());
        for (int i = 0; i < vertices.size(); ++i) {
            rawNormalArray[i * 3 + 0] = vertexNormals[i].x();
            rawNormalArray[i * 3 + 1] = vertexNormals[i].y();
            rawNormalArray[i * 3 + 2] = vertexNormals[i].z();
        }

        Qt3DCore::QBuffer* tangentDataBuffer;
        QByteArray tangentBufferData;
        float* rawTangentArray;

        if (!isTexEmpty)
        {
            QVector<QVector3D> vertexTangents(vertices.size(), QVector3D(0, 0, 0));
            QVector<QVector3D> vertexBitangents(vertices.size(), QVector3D(0, 0, 0));
            QMap<int, QVector<QPair<QVector3D, QVector3D>>> vertexToTangentBitangentMap;

            // Calculating tangents and bitangents (Do we need this? idk)
            for (int i = 0; i < faces.size(); ++i) {
                QVector3D v0 = vertices[faces[i].x()];
                QVector3D v1 = vertices[faces[i].y()];
                QVector3D v2 = vertices[faces[i].z()];

                QVector2D uv0 = QVector2D(texCoords[faces[i].x()]);
                QVector2D uv1 = QVector2D(texCoords[faces[i].y()]);
                QVector2D uv2 = QVector2D(texCoords[faces[i].z()]);

                QVector3D deltaPos1 = v1 - v0;
                QVector3D deltaPos2 = v2 - v0;

                QVector2D deltaUV1 = uv1 - uv0;
                QVector2D deltaUV2 = uv2 - uv0;

                float r = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());
                QVector3D tangent = (deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y()) * r;
                QVector3D bitangent = (deltaPos2 * deltaUV1.x() - deltaPos1 * deltaUV2.x()) * r;

                vertexToTangentBitangentMap[faces[i].x()].push_back(qMakePair(tangent, bitangent));
                vertexToTangentBitangentMap[faces[i].y()].push_back(qMakePair(tangent, bitangent));
                vertexToTangentBitangentMap[faces[i].z()].push_back(qMakePair(tangent, bitangent));
            }

            // Averaging tangents and bitangents for smooth shading, but
            for (int i = 0; i < vertices.size(); ++i) {
                QVector3D finalTangent(0, 0, 0);
                QVector3D finalBitangent(0, 0, 0);
                for (const auto& pair : vertexToTangentBitangentMap[i]) {
                    finalTangent += pair.first;
                    finalBitangent += pair.second;
                }
                finalTangent.normalize();
                finalBitangent.normalize();
                vertexTangents[i] = finalTangent;
                // vertexBitangents[i] = finalBitangent; // Uncomment if you decide to use bitangents
            }

            tangentDataBuffer = new Qt3DCore::QBuffer(customGeometry);
            tangentBufferData.resize(vertexTangents.size() * 3 * sizeof(float)); // x, y, z for each tangent

            rawTangentArray = reinterpret_cast<float*>(tangentBufferData.data());
            for (int i = 0; i < vertexTangents.size(); ++i) {
                rawTangentArray[i * 3 + 0] = vertexTangents[i].x();
                rawTangentArray[i * 3 + 1] = vertexTangents[i].y();
                rawTangentArray[i * 3 + 2] = vertexTangents[i].z();
            }
            tangentDataBuffer->setData(tangentBufferData);
        }

        normalDataBuffer->setData(normalBufferData);
        vertexDataBuffer->setData(vertexBufferData);
        faceDataBuffer->setData(faceBufferData);
        
        Qt3DCore::QAttribute* positionAttribute = new Qt3DCore::QAttribute();
        positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        positionAttribute->setBuffer(vertexDataBuffer);
        positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setByteOffset(0);
        positionAttribute->setByteStride(3 * sizeof(float));
        positionAttribute->setCount(vertexSize);
        positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
        //qDebug() << "Position attribute count" << positionAttribute->count();
        
        Qt3DCore::QAttribute* indexAttribute = new Qt3DCore::QAttribute();
        indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
        indexAttribute->setBuffer(faceDataBuffer);
        indexAttribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedShort);
        indexAttribute->setVertexSize(1);
        indexAttribute->setByteOffset(0);
        indexAttribute->setByteStride(0);
        indexAttribute->setCount(faceSize * 3);
        indexAttribute->setName(Qt3DCore::QAttribute::defaultJointIndicesAttributeName());
        //qDebug() << "Index attribute count" << indexAttribute->count();

        Qt3DCore::QAttribute* texCoordAttribute;
        if (!isTexEmpty)
        {
            texCoordAttribute = new Qt3DCore::QAttribute();
            texCoordAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
            texCoordAttribute->setBuffer(texCoordDataBuffer);
            texCoordAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
            texCoordAttribute->setVertexSize(2); // 2 components for UV coordinates
            texCoordAttribute->setByteOffset(0);
            texCoordAttribute->setByteStride(2 * sizeof(float));
            texCoordAttribute->setCount(texCoordSize);
            texCoordAttribute->setName(Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName());
        }
        
        Qt3DCore::QAttribute* normalAttribute = new Qt3DCore::QAttribute();
        normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        normalAttribute->setBuffer(normalDataBuffer);
        normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        normalAttribute->setVertexSize(3); // 3 components for normal
        normalAttribute->setByteOffset(0);
        normalAttribute->setByteStride(3 * sizeof(float));
        normalAttribute->setCount(vertices.size());
        normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());

        Qt3DCore::QAttribute* tangentAttribute;
        // Create tangent attribute
        if (!isTexEmpty)
        {
            tangentAttribute = new Qt3DCore::QAttribute();
            tangentAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
            tangentAttribute->setBuffer(tangentDataBuffer);
            tangentAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
            tangentAttribute->setVertexSize(3); // 3 components for tangent
            tangentAttribute->setByteOffset(0);
            tangentAttribute->setByteStride(3 * sizeof(float));
            tangentAttribute->setCount(vertices.size());
            tangentAttribute->setName(Qt3DCore::QAttribute::defaultTangentAttributeName()); // Define tangent name attribute

        }
       
        customGeometry->addAttribute(positionAttribute);        
        customGeometry->addAttribute(indexAttribute);
        customGeometry->addAttribute(normalAttribute);

        if (!isTexEmpty)
        {
            customGeometry->addAttribute(texCoordAttribute);
            customGeometry->addAttribute(tangentAttribute);
        }
       
        QString texturePath = "D:/dev/AID_MS3/resources/WOOD01.JPG";
        Qt3DRender::QTextureLoader* textureLoader = new Qt3DRender::QTextureLoader(material);
        textureLoader->setSource(QUrl::fromLocalFile(texturePath));

        material->setTexture(textureLoader);

        // Temporary material in the place of texture - legacy
        Qt3DExtras::QPhongMaterial* tempMaterial = new Qt3DExtras::QPhongMaterial;
        //Qt3DRender::QMaterial* tempCustomMaterial = new TwiceReflectingMaterial;
       
        //createdMaterial->dumpObjectTree();
        //qDebug() << createdMaterial->property("ka").value<QColor>();
        Qt3DCore::QGeometryView* customMeshGeometryView = new Qt3DCore::QGeometryView;
        customMeshGeometryView->setGeometry(customGeometry);
        customMeshGeometryView->setPrimitiveType(Qt3DCore::QGeometryView::Triangles);
        customMeshGeometryView->setInstanceCount(1);
        customMeshGeometryView->setIndexOffset(0);
        customMeshGeometryView->setFirstInstance(0);

        qDebug() << "Materials count" << model.matGroupsCount;

        int materialIndexOffset = 0;
        
        for (auto& mat_grp : model.matGroups)
        {
            qDebug() << "\nMaterial Group\n" << "Name: " << mat_grp.first << "indiceCount's induces: " << mat_grp.second;
            int indiceCount = mat_grp.second.count();
            Qt3DRender::QGeometryRenderer* customMeshRenderer = new Qt3DRender::QGeometryRenderer();
            qDebug() << "New geometry renderer created";
            customMeshRenderer->setInstanceCount(1);
            qDebug() << "Index offset and first instance: " << materialIndexOffset;
            customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
            customMeshRenderer->setIndexBufferByteOffset(materialIndexOffset * 3 * sizeof(ushort));
            customMeshRenderer->setGeometry(customGeometry);
            //customMeshRenderer->setView(customMeshGeometryView);
            customMeshRenderer->setVertexCount(3 * indiceCount); // previously indiceCount was faceSize
            qDebug() << "vertex count: 3x" << indiceCount;

            ObjMaterial localMaterialData;

            for (auto mat : model.materials)
            {
                if (mat.first == mat_grp.first)
                {
                    qDebug() << "Model material name" << mat.first << mat.second.name << mat.second.ka;
                    localMaterialData = mat.second;
                }

            }

            materialIndexOffset += mat_grp.second.count();
            qDebug() << "incremented materialIndexOffset" << materialIndexOffset << "\n";

            Qt3DRender::QMaterial* createdMaterial = new Qt3DRender::QMaterial();
            createdMaterial->setEffect(phongEffect);

            if (!localMaterialData.ka.isNull())
            {
                QColor ka = QColor::fromRgbF(localMaterialData.ka.x(), localMaterialData.ka.y(), localMaterialData.ka.z(), 1.0f);
                //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("ka"), ka));
                createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("ka"), ka));

            }
            if (!localMaterialData.kd.isNull())
            {
                QColor kd = QColor::fromRgbF(localMaterialData.kd.x(), localMaterialData.kd.y(), localMaterialData.kd.z(), 1.0f);
                createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("kd"), kd));

            }
            if (!localMaterialData.ks.isNull())
            {
                QColor ks = QColor::fromRgbF(localMaterialData.ks.x(), localMaterialData.ks.y(), localMaterialData.ks.z(), 1.0f);
                createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("ks"), ks));
            }
            /*if (!localMaterialData.ke.isNull())
            {
                QColor ke = QColor::fromRgbF(localMaterialData.ke.x(), localMaterialData.ke.y(), localMaterialData.ke.z(), 1.0f);
                createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("ke"), ke));
            }*/
            
            createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("useTextureMap"), localMaterialData.textureMap.isEmpty() ? false : true));
            
            if (!(localMaterialData.textureMap.isEmpty()))
            {
                Qt3DRender::QTextureImage* image = new Qt3DRender::QTextureImage;
                QString textureMapLocation = model.assetPath + localMaterialData.textureMap;
                qDebug() << textureMapLocation << "added as texture";
                image->setSource(QUrl::fromLocalFile(QString(textureMapLocation)));

                // Texture2D
                Qt3DRender::QTexture2D* texture2D = new Qt3DRender::QTexture2D;
                texture2D->setGenerateMipMaps(true);
                texture2D->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
                texture2D->setMinificationFilter(Qt3DRender::QTexture2D::Linear);
                texture2D->addTextureImage(image);

                createdMaterial->addParameter(new Qt3DRender::QParameter("textureMap", texture2D));
            }

            createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("useNormalMap"), localMaterialData.normalMap.isEmpty() ? false : true));

            if (!(localMaterialData.normalMap.isEmpty()))
            {
                Qt3DRender::QTextureImage* image = new Qt3DRender::QTextureImage;
                QString normalMapLocation = model.assetPath + localMaterialData.normalMap;
                qDebug() << normalMapLocation << "added as normal map";
                image->setSource(QUrl::fromLocalFile(QString(normalMapLocation)));

                // Texture2D
                Qt3DRender::QTexture2D* texture2D = new Qt3DRender::QTexture2D;
                texture2D->setGenerateMipMaps(false);
                texture2D->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
                texture2D->setMinificationFilter(Qt3DRender::QTexture2D::Linear);

                texture2D->addTextureImage(image);
                
                createdMaterial->addParameter(new Qt3DRender::QParameter("normalMap", texture2D));                
            }
            
            createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("useAmbientOcclusionMap"), localMaterialData.ambientOcclusionMap.isEmpty() ? false : true));

            if (!(localMaterialData.ambientOcclusionMap.isEmpty()))
            {
                Qt3DRender::QTextureImage* image = new Qt3DRender::QTextureImage;
                QString ambientOcclusionMapLocation = model.assetPath + localMaterialData.ambientOcclusionMap;
                qDebug() << ambientOcclusionMapLocation << "added as ambient occlusion map";
                image->setSource(QUrl::fromLocalFile(QString(ambientOcclusionMapLocation)));

                // Texture2D
                Qt3DRender::QTexture2D* texture2D = new Qt3DRender::QTexture2D;
                texture2D->setGenerateMipMaps(false);
                texture2D->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
                texture2D->setMinificationFilter(Qt3DRender::QTexture2D::Linear);

                texture2D->addTextureImage(image);

                createdMaterial->addParameter(new Qt3DRender::QParameter("ambientOcclusionMap", texture2D));

            }

            createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("useParallaxMap"), localMaterialData.parallaxMap.isEmpty() ? false : true));

            if (!(localMaterialData.parallaxMap.isEmpty()))
            {
                Qt3DRender::QTextureImage* image = new Qt3DRender::QTextureImage;
                QString parallaxMapLocation = model.assetPath + localMaterialData.ambientOcclusionMap;
                qDebug() << parallaxMapLocation << "added as parallax map";
                image->setSource(QUrl::fromLocalFile(QString(parallaxMapLocation)));

                // Texture2D
                Qt3DRender::QTexture2D* texture2D = new Qt3DRender::QTexture2D;
                texture2D->setGenerateMipMaps(false);
                texture2D->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
                texture2D->setMinificationFilter(Qt3DRender::QTexture2D::Linear);

                texture2D->addTextureImage(image);

                createdMaterial->addParameter(new Qt3DRender::QParameter("parallaxMap", texture2D));

            }

            float shininess = localMaterialData.shininess;
            float transparency = localMaterialData.transparency;

            qDebug() << "is NormalMap null : " << localMaterialData.normalMap.isNull() << " And has value: " << localMaterialData.normalMap;
            qDebug() << "is TextureMap null : " << localMaterialData.textureMap.isNull() << " And has value: " << localMaterialData.textureMap;
            qDebug() << "is ambientOcclusion null : " << localMaterialData.ambientOcclusionMap.isNull() << " And has value: " << localMaterialData.ambientOcclusionMap;


            //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.8f, 0.5f, 0.3f, 1.0f)));
            //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("kd"), QColor::fromRgbF(0.9f, 0.9f, 0.9f, 1.0f)));
            //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.1f, 0.1f, 0.1f, 1.0f)));
            //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("shininess"), 50.0f));

            createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("shininess"), 300.0f));
            
            if (transparency)
            {
                createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("transparency"), transparency));
            }
            else
            {
                createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("transparency"), 0.0f));
            }

            //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("lightPosition"), QVector3D(3.0f, 20.0f, 0.0f)));
            //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("lightPosition"), lightEntity->findChild<Qt3DCore::QTransform*>()->translation()));
            //createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("lightIntensity"), lightEntity->findChild<Qt3DRender::QPointLight*>()->intensity()));

            createdMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("eyePosition"), view->camera()->position()));

            // Global parameters
            createdMaterial->addParameter(lightIntensityParam1);
            createdMaterial->addParameter(lightIntensityParam2);
            createdMaterial->addParameter(lightPositionParam1);
            createdMaterial->addParameter(lightPositionParam2);

            geometryRendererVector.append(qMakePair(customMeshRenderer, createdMaterial));
        }

        materialIndexOffset = 0;
        
        /*
        gizmo = new Qt3DGizmo(rootEntity);
        Qt3DRender::QObjectPicker* torusPicker = new Qt3DRender::QObjectPicker;

        customMeshRenderer->setInstanceCount(1);
        customMeshRenderer->setIndexOffset(0); // previously 0
        customMeshRenderer->setFirstInstance(0); // previosuly 0
        customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
        customMeshRenderer->setGeometry(customGeometry);
        //customMeshRenderer->setView(customMeshGeometryView);
        customMeshRenderer->setVertexCount(3 * faceSize); // previously indiceCount was faceSize
        */      
        
        for (auto renderer : geometryRendererVector)
        {
            qDebug() << "renderer added to entity";
            Qt3DCore::QEntity* subEntity = new Qt3DCore::QEntity(customMeshEntity);
            subEntity->addComponent(renderer.first);
            subEntity->addComponent(renderer.second); // Change it to material if you want to play with textures
            //subEntity->addComponent(outlineMaterial);
        }

        /*customMeshEntity->addComponent(transform); 
        customMeshEntity->addComponent(picker);
        customMeshEntity->addComponent(objectsLayer);*/

        //modelrootEntity->addComponent(transform);
        //modelrootEntity->addComponent(picker);
        //modelrootEntity->addComponent(objectsLayer);

        ////customMeshEntity->addComponent(torusPicker);
        //modelrootEntity->setObjectName(model.name);

        if (modelrootEntity->componentsOfType<Qt3DCore::QTransform>().isEmpty()) {
            modelrootEntity->addComponent(transform);
        }

        if (modelrootEntity->componentsOfType<Qt3DRender::QObjectPicker>().isEmpty()) {
            modelrootEntity->addComponent(picker);
        }

        if (modelrootEntity->componentsOfType<Qt3DRender::QLayer>().isEmpty()) {
            modelrootEntity->addComponent(objectsLayer);
        }

        modelrootEntity->setObjectName(model.name);
        
        //qDebug() << "\n\n" << transform->translation() << transform->rotation() << transform->scale3D() << "\n\n";

        updateTreeListWidget();
        
        //bool updateImplicitPointCheck = customMeshRenderer->updateImplicitBounds();
        //qDebug() << "Are implicit points updated? " << updateImplicitPointCheck;
        //qDebug() << "customMeshRenderer->implicitMinPoint()" << customMeshRenderer->implicitMinPoint();
        //qDebug() << "customMeshRenderer->implicitMaxPoint()" << customMeshRenderer->implicitMaxPoint();
        
        qDebug() << "\nObject Name" << model.name;
        qDebug() << "Object Version" << model.version << "\n";
        
        //Qt3DCore::QNodeId customMeshId = customMeshRenderer->id();
        Qt3DCore::QNodeId customMeshPickerId = picker->id();
        Qt3DCore::QNodeId customMeshLayerId = objectsLayer->id();
        Qt3DCore::QNodeId gizmoLayerId = gizmoLayer->id();

        //qDebug() << "Created Mesh id: " << customMeshId;
        qDebug() << "Created picker id: " << customMeshPickerId;
        qDebug() << "Created obj layer id: " << customMeshLayerId;
        qDebug() << "Gizmo layer id: " << gizmoLayerId;
        
        /*
        connect(
            picker, &Qt3DRender::QObjectPicker::clicked,
            [this, transform]() {
                
                    this->gizmo->setDelegateTransform(transform);
                
            });
        */
        
        connect(
            picker, &Qt3DRender::QObjectPicker::clicked,
            [this, transform, modelOffset]() {

                QVector3D objectTranslate = transform->translation();
                transform->setTranslation(objectTranslate);
                this->gizmo->setDelegateTransformOffset(modelOffset);
                this->gizmo->setDelegateTransform(transform);
                
                //this->gizmo->setDelegateTransform(transform);
                
                
                /*qDebug() << "\nobject World matrix:" << transform->worldMatrix();
                qDebug() << "\nobject matrix:" << transform->matrix();
                qDebug() << "\ndelegate World matrix:" << gizmo->delegateTransform()->worldMatrix();
                qDebug() << "\ndelegate matrix:" << gizmo->delegateTransform()->matrix();*/

                
            });

        connect(picker, &Qt3DRender::QObjectPicker::clicked, this, &RenderWindow::onEntityClicked);

        connect(
            picker, &Qt3DRender::QObjectPicker::clicked,
            [=](Qt3DRender::QPickEvent* event) {
      
                transformList.append(selectedTransform);
                //uncomment this
                /*
                if (selectedMaterial != nullptr) {
                    QList<Qt3DRender::QParameter*> parameters = selectedMaterial->parameters();
                    for (Qt3DRender::QParameter* param : parameters) {
                        if (param->name() == QStringLiteral("ka")) {
                            param->setValue(savedAmbientColor);
                            // Exit loop after setting the value since we've found our parameter
                        }
                        if (param->name() == QStringLiteral("kd")) {
                            param->setValue(savedDiffuseColor);
                            // Exit loop after setting the value since we've found our parameter
                        }
                    }
                    //selectedMaterial->property("ka").setValue(savedAmbientColor);
                    //selectedMaterial->property("kd").setValue(savedDiffuseColor);
                }
                */

                //Qt3DCore::QNodeId s = customMeshRenderer->id();
                Qt3DCore::QNodeId s1 = picker->id();

                Qt3DCore::QEntity* pickedEntity = qobject_cast<Qt3DCore::QEntity*>(event->entity());
                Qt3DCore::QEntity* pickedEntityParent = pickedEntity->parentEntity();


                qDebug() << "Picked Entity ID:" << pickedEntity->id() << "It's object tree dump";
                pickedEntity->dumpObjectTree();

                if (!pickedEntity)
                    return;
                
                Qt3DRender::QGeometryRenderer* geometryRenderer =
                    pickedEntity->findChild<Qt3DRender::QGeometryRenderer*>();
                //qDebug() << "pickedEntityLayer id" << geometryRenderer->id();

                if (!geometryRenderer)
                    return;

                Qt3DCore::QGeometry* geometry = geometryRenderer->geometry();

                if (!geometry)
                    return;

                QVector<QVector3D> positions;
                QVector<ushort> indices; 
                QVector<QVector2D> texCoords;
                QVector<QVector3D> normals;
                QVector<QVector3D> tangents;

                for (Qt3DCore::QAttribute* attribute : geometry->attributes()) {
                    QByteArray bufferData = attribute->buffer()->data();

                    if (attribute->name() == Qt3DCore::QAttribute::defaultPositionAttributeName() && attribute->count() > 0) {
                        const float* rawPositionData = reinterpret_cast<const float*>(bufferData.constData());
                        const int count = attribute->count();
                        positions.reserve(count);
                        //qDebug() << attribute->name();
                        for (int i = 0; i < count; ++i) {
                            QVector3D vertex(
                                rawPositionData[i * 3],
                                rawPositionData[i * 3 + 1],
                                rawPositionData[i * 3 + 2]
                            );
                            positions.push_back(vertex);
                        }
                        //qDebug() << "Selected position: " << positions.first();
                    }
                    else if (attribute->name() == Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName() && attribute->count() > 0) {
                        const float* rawTexCoordData = reinterpret_cast<const float*>(bufferData.constData());
                        const int count = attribute->count();
                        texCoords.reserve(count);
                        //qDebug() << attribute->name();
                        for (int i = 0; i < count; ++i) {
                            QVector2D texCoord(
                                rawTexCoordData[i * 2],
                                rawTexCoordData[i * 2 + 1]
                            );
                            texCoords.push_back(texCoord);
                        }
                        //qDebug() << texCoords.first();
                    }
                    else if (attribute->name() == Qt3DCore::QAttribute::defaultNormalAttributeName() && attribute->count() > 0) {
                        const float* rawNormalData = reinterpret_cast<const float*>(bufferData.constData());
                        const int count = attribute->count();
                        normals.reserve(count);
                        //qDebug() << attribute->name();
                        for (int i = 0; i < count; ++i) {
                            QVector3D normal(
                                rawNormalData[i * 3],
                                rawNormalData[i * 3 + 1],
                                rawNormalData[i * 3 + 2]
                            );
                            normals.push_back(normal);
                        }
                        //qDebug() << normals.first();
                    }
                    else if (attribute->name() == Qt3DCore::QAttribute::defaultJointIndicesAttributeName() && attribute->count() > 0) {
                        const quint16* rawIndexData = reinterpret_cast<const ushort*>(bufferData.constData());
                        const int count = attribute->count();
                        indices.reserve(count);
                        //qDebug() << attribute->name();
                        for (int i = 0; i < count; ++i) {
                            indices.push_back(rawIndexData[i]);
                        }
                        int startindex = 132;
                        //qDebug() << "Picked indice" << indices.first();
                    }

                    else if (attribute->name() == Qt3DCore::QAttribute::defaultTangentAttributeName() && attribute->count() > 0) { // Use "TANGENT" or whatever name you assigned to your tangent attribute
                        const float* rawTangentData = reinterpret_cast<const float*>(bufferData.constData());
                        const int count = attribute->count();
                        tangents.reserve(count);
                        //qDebug() << attribute->name();
                        for (int i = 0; i < count; ++i) {
                            QVector3D tangent(
                                rawTangentData[i * 3],     // x
                                rawTangentData[i * 3 + 1], // y
                                rawTangentData[i * 3 + 2]  // z, assuming your tangents are vec3, if vec4 adjust accordingly
                            );
                            tangents.push_back(tangent);
                        }
                        //qDebug() << tangents.first();
                    }
                }

                for (auto renderer : geometryRendererVector)
                {
                    //uncomment this
                    /*
                    QList<Qt3DRender::QParameter*> parameters = renderer.second->parameters();
                    for (Qt3DRender::QParameter* param : parameters) {
                        if (param->name() == QStringLiteral("ka")) {
                            savedAmbientColor = param->value();
                            //qDebug() << "savedAmbientColor" << savedAmbientColor;
                        }
                        if (param->name() == QStringLiteral("kd")) {
                            savedDiffuseColor = param->value();
                            //qDebug() << "savedDiffuseColor" << savedDiffuseColor;
                        }
                    }
                    */

                    selectedMaterial = renderer.second;
                    //selectedTransform = transform
                    
                }
               /* selectedTransform = nullptr;
                Qt3DCore::QEntity* currentEntity = pickedEntity;
                while (currentEntity && !selectedTransform) {
                    QList<Qt3DCore::QTransform*> transforms = currentEntity->componentsOfType<Qt3DCore::QTransform>();
                    if (!transforms.isEmpty()) {
                        selectedTransform = transforms.first();
                    }
                    else {
                        currentEntity = currentEntity->parentEntity();
                        qDebug() << "Going up to get transform";
                    }
                }

                if (!selectedTransform) {
                    qDebug() << "Transform is null?? ";
                }*/

                //selectedTransform = pickedEntityParent->findChild<Qt3DCore::QTransform*>();
                
                //savedAmbientColor = createdMaterial->property("ka").value<QColor>();
                //qDebug() << "savedAmbientColor" << savedAmbientColor << createdMaterial->property("ka");
                //savedDiffuseColor = createdMaterial->property("kd").value<QColor>();
                //qDebug() << "savedDiffuseColor" << savedDiffuseColor << createdMaterial->property("kd");
                //qDebug() << "Parameters of created material" << createdMaterial->parameters();
                //qDebug() << "Created material object dump";
                //createdMaterial->dumpObjectInfo();
                //qDebug() << "Created material object tree";
                //createdMaterial->dumpObjectInfo();

                //uncomment this
                /*
                QList<Qt3DRender::QParameter*> selectedParameters = selectedMaterial->parameters();
                for (Qt3DRender::QParameter* param : selectedParameters) {
                    if (param->name() == QStringLiteral("ka")) {
                        //param->setValue(QColor::fromRgbF(0.5f, 0.0f, 0.0f, 1.0f));
                        param->setValue(QColor(Qt::red));
                    }
                    if (param->name() == QStringLiteral("kd")) {
                        param->setValue(QColor(Qt::darkRed));
                    }
                }
                selectedMaterial->property("ka").setValue(QColor::fromRgbF(0.5f, 0.0f, 0.0f, 1.0f));
                qDebug() << "selectedMaterial->setProperty(\"ka\", QColor(Qt::darkRed));";
                qDebug() << "savedAmbientColor" << savedAmbientColor << selectedMaterial->property("ka");
                selectedMaterial->property("kd").setValue(QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f));
                qDebug() << "selectedMaterial->setProperty(\"kd\", QColor(Qt::red));";
                */

                if (event->modifiers() & Qt::ShiftModifier) {
                    dragStarted = true;
                }
            });

        connect(
            picker, &Qt3DRender::QObjectPicker::moved,
            [=](Qt3DRender::QPickEvent* event) {
                // Move selected object if dragging has started
                if (dragStarted && selectedTransform != nullptr) {
                    QVector3D oldPos = selectedTransform->translation();
                    QVector3D deltaMove(event->worldIntersection().x() - oldPos.x(), 0, event->worldIntersection().z() - oldPos.z());
                    selectedTransform->setTranslation(oldPos + deltaMove);
                }
            });

        connect(
            picker, &Qt3DRender::QObjectPicker::released,
            [=](Qt3DRender::QPickEvent* event) {
                dragStarted = false;
            });
        
        qDebug() << "All components added to custom Mesh Entity";
    }

    void RenderWindow::printVertices(Qt3DCore::QNodeId s)
    {
        //qDebug() << "printVertices" << rootEntity->findChild<Qt3DCore::QEntity>();
    }

    void RenderWindow::setCameraView()
    {
        // Working on this
        qDebug() << "\nScenegraph: " << rootEntity->children();
        //Qt3DCore::QEntity* firstEntity
        qDebug() << "\nChildren Count: " << rootEntity->childNodes().count();

        
    }
    Qt3DCore::QEntity* RenderWindow::findFirstEntity(Qt3DCore::QEntity* rootEntity) {
        if (!rootEntity) return nullptr;

        for (QObject* child : rootEntity->children()) {
            Qt3DCore::QEntity* entity = dynamic_cast<Qt3DCore::QEntity*>(child);
            if (entity && typeid(*entity) == typeid(Qt3DCore::QEntity)) {
                return entity; // Return the first QEntity found
            }
        }

        return nullptr;
    }
    bool RenderWindow::IsAdjacent(const CLine3D& pTempLine1, const CLine3D& pTempLine2)
    {
      return (std::abs(pTempLine1.GetP2().distanceToPoint(pTempLine2.GetP1())) < 0.0001);
    }

    QVector<CLine3D> RenderWindow::JoinLineSegments(QVector<CLine3D> segments)
    {
      QVector<CLine3D> boundaryList;
      boundaryList.push_back(segments[0]);
      segments.erase(segments.begin());

      while (!segments.empty())
      {
        bool found = false;
        for (int i = 0; i < segments.size(); i++)
        {
          if (IsAdjacent(boundaryList.back(), segments[i]))
          {
            boundaryList.push_back(segments[i]);
            segments.erase(segments.begin() + i);
            found = true;
            break;
          }
        }
        if (!found)
          break;
      }
      return boundaryList;
    }

    QVector<CLine3D> RenderWindow::GenerateLineSegments(const QVector<QVector3D>& vList)
    {
      QVector<CLine3D> linesList;
      for (int i = 0; i < vList.count(); i++)
      {
        CLine3D lineObj;
        if ((i + 1) % 3 == 0)
          lineObj.SetPoints(vList.at(i), vList.at(i - 2));
        else
          lineObj.SetPoints(vList.at(i), vList.at(i + 1));

        linesList.push_back(lineObj);
      }
      return linesList;
    }

    QVector<QVector<CLine3D>> RenderWindow::GenerateClosedLineSegments(const QVector<CLine3D>& linesList)
    {
      QVector<CLine3D> outerLinesList;
      QVector<CLine3D> innerLinesList;
      QVector<QVector<CLine3D>> generatedGroupBoundaryLinesList;
      

      for (int i = 0; i < linesList.count(); i++)
      {
        CLine3D cObj1 = linesList.at(i);
        for (int j = i + 1; j < linesList.count(); j++)
        {
          CLine3D cObj2 = linesList.at(j);
          if ((cObj1.GetP1().distanceToPoint(cObj2.GetP2()) < 0.0001 && cObj1.GetP2().distanceToPoint(cObj2.GetP1()) < 0.0001) ||
            (cObj1.GetP1().distanceToPoint(cObj2.GetP1()) < 0.0001 && cObj1.GetP2().distanceToPoint(cObj2.GetP2()) < 0.0001))
          {
            innerLinesList.push_back(cObj1);
            innerLinesList.push_back(cObj2);
          }
        }
      }

      foreach(CLine3D cObj1, linesList)
      {
        bool isFound = false;
        foreach(CLine3D cObj2, innerLinesList)
        {
          if (cObj1.GetP1().distanceToPoint(cObj2.GetP1()) < 0.0001 && cObj1.GetP2().distanceToPoint(cObj2.GetP2()) < 0.0001)
          {
            isFound = true;
            break;
          }
        }
        if (!isFound)
          outerLinesList.push_back(cObj1);
      }
      if (outerLinesList.isEmpty())
        return generatedGroupBoundaryLinesList;

      //generatedBoundaryLinesList = JoinLineSegments(outerLinesList);
      QVector<CLine3D> tempList = outerLinesList;
      bool done = false;
      while (!done)
      {
        QVector<CLine3D> generatedBoundaryLinesList = JoinLineSegments(outerLinesList);
        generatedGroupBoundaryLinesList.push_back(generatedBoundaryLinesList);
        if (generatedBoundaryLinesList.count() == outerLinesList.count()|| generatedBoundaryLinesList.isEmpty())
          done = true;
        else
        {
          QVector<CLine3D> tempList;
          for (int j = 0; j < outerLinesList.count(); j++)
          {
            bool issame = false;
            CLine3D cObj1 = outerLinesList.at(j);
            for (int k = 0; k < generatedBoundaryLinesList.count(); k++)
            {
              CLine3D cObj2 = generatedBoundaryLinesList.at(k);
              if ((cObj1.GetP1().distanceToPoint(cObj2.GetP2()) < 0.0001 && cObj1.GetP2().distanceToPoint(cObj2.GetP1()) < 0.0001) ||
                (cObj1.GetP1().distanceToPoint(cObj2.GetP1()) < 0.0001 && cObj1.GetP2().distanceToPoint(cObj2.GetP2()) < 0.0001))
              {
                issame = true;
                break;
              }
            }
            if (!issame)
              tempList.push_back(outerLinesList.at(j));
          }

          if (tempList.count() > 0)
          {
            outerLinesList.clear();
            outerLinesList = tempList;
          }
        }
      }
      return generatedGroupBoundaryLinesList;
    }

    QVector<QVector<CLine3D>> RenderWindow::GenerateBoundary(const QVector<QVector3D>& vList)
    {
      QVector<QVector<CLine3D>> generatedBoundaryLinesList;
      QVector<CLine3D> linesList = GenerateLineSegments(vList);
      if (linesList.isEmpty())
        return generatedBoundaryLinesList;

      generatedBoundaryLinesList = GenerateClosedLineSegments(linesList);
      return generatedBoundaryLinesList;
    }

    bool isPointOnPlane(const QVector3D& point, Plane3D& plane) {
      // Calculate the vector from the known point to the test point
      QVector3D vectorAB;
      vectorAB = point - plane.GetOrigin();

      // Calculate the dot product of the vectorAB and the plane's normal
      float dotProduct = QVector3D::dotProduct(vectorAB, plane.GetNormal());

      // Check if the dot product is close to zero (within a tolerance)
      const float tolerance = 0.0001;
      return std::abs(dotProduct) < tolerance;
    }

    QVector<QVector<CLine3D>> RenderWindow::generateBoundariesForWallAndItsSubmodels(QVector<QVector3D> positions, QVector<ushort> indices, QVector<QVector3D> normals, bool isWall, Plane3D plane = Plane3D(), BoundingBox3D box = BoundingBox3D())
    {
      QVector<QVector<CLine3D>> outerLinesList;
      QVector<QVector3D> vertices;
      QVector3D norm1;
      QVector3D norm2;
      QVector3D norm3;
      for (int i = 0; i < indices.count(); i = i + 3)
      {
        if (!isWall)
        {
          if (!plane.GetNormal().isNull()) {
            norm1 = QVector3D::crossProduct(normals.at(indices.at(i)), plane.GetNormal());
            norm2 = QVector3D::crossProduct(normals.at(indices.at(i + 1)), plane.GetNormal());
            norm3 = QVector3D::crossProduct(normals.at(indices.at(i + 2)), plane.GetNormal());
            if (norm1.length() > 0.0001 || norm2.length() > 0.0001 || norm3.length() > 0.0001)
              continue;
          }
        }
        QVector3D v1 = positions.at(indices.at(i));
        QVector3D v2 = positions.at(indices.at(i + 1));
        QVector3D v3 = positions.at(indices.at(i + 2));

        if (!isWall)
        {
          if ((isPointOnPlane(v1, plane)) &&
            (isPointOnPlane(v2, plane)) &&
            (isPointOnPlane(v3, plane)))
          {
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
          }
        }
        else
        {
          vertices.push_back(v1);
          vertices.push_back(v2);
          vertices.push_back(v3);
        }
      }
      if (vertices.isEmpty())
        return outerLinesList;

      if (!isWall)
      {
        bool found = false;
        for (int i = 0; i < vertices.count(); i++)
        {
          QVector3D v = vertices.at(i);
          if (!((box.minPt.x() < v.x() || std::abs(v.x() - box.minPt.x()) < 0.0001)
            && (v.x() < box.maxPt.x() || std::abs(box.maxPt.x() - v.x()) < 0.0001)
            && (box.minPt.y() < v.y() || std::abs(v.y() - box.minPt.y()) < 0.0001)
            && (v.y() < box.maxPt.y() || std::abs(box.maxPt.y() - v.y()) < 0.0001)
            && (box.minPt.z() < v.z() || std::abs(v.z() - box.minPt.z()) < 0.0001)
            && (v.z() < box.maxPt.z() || std::abs(box.maxPt.z() - v.z()) < 0.0001)))
          {
            found = true;
            break;
          }

        }

        if (found)
          return outerLinesList;
      }
      outerLinesList = GenerateBoundary(vertices);
      
      return outerLinesList;
    }

    void RenderWindow::CallParentEntity(Qt3DCore::QEntity** currentEntity)
    {
      Qt3DCore::QTransform* tempparentTransform = nullptr;
      while (currentEntity && !tempparentTransform) {
        QList<Qt3DCore::QTransform*> transforms = (*currentEntity)->componentsOfType<Qt3DCore::QTransform>();
        if (!transforms.isEmpty()) {
          tempparentTransform = transforms.first();
        }
        else {
          *currentEntity = (*currentEntity)->parentEntity();
          qDebug() << "Going up to get transform";
        }
      }
    }
    
    // Function to check if point q lies on segment p-r
    bool RenderWindow::onSegment(QPointF p, QPointF q, QPointF r)
    {
      if ((q.x() < std::max(p.x(), r.x()) || std::abs(q.x() - std::max(p.x(), r.x())) < 0.0001) &&
        (q.x() > std::min(p.x(), r.x()) || std::abs(q.x() - std::min(p.x(), r.x())) < 0.0001) &&
        (q.y() < std::max(p.y(), r.y()) || std::abs(q.y() - std::max(p.y(), r.y())) < 0.0001) &&
        (q.y() > std::min(p.y(), r.y()) || std::abs(q.y() - std::min(p.y(), r.y())) < 0.0001))
        return true;
      return false;
    }

    // Function to find orientation of ordered triplet (p, q, r)
    int RenderWindow::orientation(QPointF p, QPointF q, QPointF r) {
      float val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
      if (val < 0.0001 && val> -0.0001)
        return 0; // Collinear
      return (val > 0.0001) ? 1 : 2; // Clockwise or counterclockwise
    }
    
    // Check if a point is inside a polygon
    bool RenderWindow::isPointInPolygon(QPointF p, QVector<QPointF>& polygon) {
      int n = polygon.count();
      int count = 0; // Number of intersections

      for (int i = 0; i < n; ++i) {
        int next = (i + 1) % n;
        if (orientation(polygon[i], p, polygon[next]) == 0) {
          if (onSegment(polygon[i], p, polygon[next])) return true;
        }
        else {
          if (p.y() > std::min(polygon[i].y(), polygon[next].y()) &&
            (p.y() < std::max(polygon[i].y(), polygon[next].y()) ||
              std::abs(p.y() - std::max(polygon[i].y(), polygon[next].y())) < 0.0001) &&
            (p.x() < std::max(polygon[i].x(), polygon[next].x()) ||
              std::abs(p.x() - std::max(polygon[i].x(), polygon[next].x())) < 0.0001)) {
            if (polygon[i].y() != polygon[next].y()) {
              float xIntersection = (p.y() - polygon[i].y()) * (polygon[next].x() - polygon[i].x()) /
                (polygon[next].y() - polygon[i].y()) + polygon[i].x();
              if (p.x() < xIntersection || std::abs(p.x() - xIntersection) < 0.0001) count++;
            }
          }
        }
      }

      return (count % 2 == 1); // Inside if odd number of intersections
    }
    
    // Function to find the number of rectangular tiles that fit inside a polygon
    QVector<QPointF> RenderWindow::findTilesInPolygon(QVector<QPointF> polygon, const Rect2D& tile) 
    {
      int count = 0;
      float minX = 0, minY = 0;
      float maxX = 0, maxY = 0;
      QVector<QPointF> tilesList;
      for (const QPointF& p : polygon) {

        minX = std::min(minX, (float)p.x());
        minY = std::min(minY, (float)p.y());
        maxX = std::max(maxX, (float)p.x());
        maxY = std::max(maxY, (float)p.y());
      }

      // Iterate through each possible position of the top-left corner of the tile
      for (float y = minY; y < maxY; y = y + 0.1) {
        bool found = false;
        for (float x = minX; x < maxX; x = x + 0.1) {
          // Check if the top-left corner lies inside the polygon
          if (!isPointInPolygon(QPointF(x, y), polygon))
            continue;

          // Check if the entire tile fits inside the polygon
          if (isPointInPolygon(QPointF(x + tile.width, y + tile.height), polygon)) {
            tilesList.push_back(QPointF((x + tile.width + x) / 2, (y + tile.height + y) / 2));
            x = x + tile.width - 0.1;
            found = true;
            count++;
          }
        }
        if (found)
          y = y + tile.height - 0.1;
      }
      return tilesList;
    }

    void RenderWindow::Call2DWindow()
    {
      if (!selectedEntity)
        return;

      Qt3DCore::QTransform* tempparentTransform = nullptr;
      Qt3DCore::QEntity* pickedEntity = selectedEntity;
      while (pickedEntity && !tempparentTransform) {
        QList<Qt3DCore::QTransform*> transforms = pickedEntity->componentsOfType<Qt3DCore::QTransform>();
        if (!transforms.isEmpty()) {
          tempparentTransform = transforms.first();
        }
        else {
          pickedEntity = pickedEntity->parentEntity();
        }
      }

      QVector<CModel> modelList1 = calculatePositionsInRealWorld(pickedEntity);
      QVector<QVector3D> tempPts1;
      //for (int j = 0; j < modelList1.count(); j++)
      {
        for (int k = 0; k < modelList1.at(0).positions.count(); k++)
          tempPts1.append(tempparentTransform->matrix() * modelList1.at(0).positions.at(k));
      }
      BoundingBox3D box1 = getBoundingBox3D(tempPts1);

      if ((std::abs(box1.maxPt.x() - box1.minPt.x()) > 0.0001) && (std::abs(box1.maxPt.y() - box1.minPt.y()) > 0.0001) && (std::abs(box1.maxPt.z() - box1.minPt.z()) > 0.0001))
        return;

      CModel model1 = modelList1.at(0);
      QVector<QVector<CLine3D>> wallBoundary = generateBoundariesForWallAndItsSubmodels(tempPts1, model1.indices, model1.normals, true);
      if (wallBoundary.isEmpty())
        return;

      Plane3D wallPlane;
      bool normfound = false;
      for (int h = 0; h < wallBoundary.count(); h++)
      {
        for (int g = 0; g < wallBoundary.at(h).count() - 1; g++)
        {
          QVector3D v1 = wallBoundary.at(h).at(g).GetP1() - wallBoundary.at(h).at(g).GetP2();
          QVector3D v2 = wallBoundary.at(h).at(g + 1).GetP1() - wallBoundary.at(h).at(g + 1).GetP2();
          v1.normalize();
          v2.normalize();
          float dot = QVector3D::dotProduct(v1, v2);
          float ang = std::acos(dot / (v1.length() * v2.length()));

          if ((ang * 180 / 3.141592) < 0.0001 && (ang * 180 / 3.141592) > -0.0001)
            continue;

          QVector3D norm = QVector3D::crossProduct(v1, v2);
          norm.normalize();
          wallPlane.SetNormal(norm);
          normfound = true;
          break;
        }
        if (normfound)
          break;
      }

      QVector<Face> faces = GenerateCubeFaces(box1.minPt, box1.maxPt);
      for (Face fObj : faces)
      {
        if (fObj.p1.distanceToPoint(fObj.p2) <= 0 || fObj.p2.distanceToPoint(fObj.p3) <= 0)
          continue;
        wallPlane.SetOrigin((fObj.p1 + fObj.p3) / 2);
        break;
      }

      QQuaternion quad;
      QMatrix4x4 mat;
      QMatrix4x4 flipmat;
      bool matFound = false;
      int count = 0;
      mat.translate(-wallPlane.GetOrigin());
      flipmat.scale(1, -1, 1);

      quad = QQuaternion::rotationTo(wallPlane.GetNormal(), QVector3D(0, 0, 1));

      if (!wallBoundary.isEmpty() && !quad.isNull())
      {
        QVector<QVector<CLine3D>> tempGroupLinesList;
        for (int i = 0; i < wallBoundary.count(); i++)
        {
          QVector<CLine3D> tempLinesList;
          for (int j = 0; j < wallBoundary.at(i).count(); j++)
          {
            QVector3D v1 = mat * wallBoundary.at(i).at(j).GetP1();
            QVector3D v2 = mat * wallBoundary.at(i).at(j).GetP2();
            QVector3D v3 = quad * v1;
            QVector3D v4 = quad * v2;
            QVector3D v5 = flipmat * v3;
            QVector3D v6 = flipmat * v4;
            tempLinesList.push_back(CLine3D(v5, v6));
          }
          tempGroupLinesList.push_back(tempLinesList);
        }
        wallBoundary.clear();
        wallBoundary = tempGroupLinesList;

        if (!wallBoundary.isEmpty())
        {
          C2DViewDialog* dview = new C2DViewDialog(wallBoundary);
          dview->show();
          dview->setModal(true);
          if (dview->exec() == QDialog::Accepted)
          {
            Qt3DCore::QEntity* mainEntity = nullptr;
            for (int i = rootEntity->children().count() - 1; i >= 0; i--)
            {
              QObject* child = rootEntity->children().at(i);
              Qt3DCore::QEntity* entity = dynamic_cast<Qt3DCore::QEntity*>(child);
              if (entity && typeid(*entity) == typeid(Qt3DCore::QEntity))
              {
                mainEntity = entity;
                break;
              }
            }
            Qt3DCore::QTransform* tempparentTransform2 = nullptr;
            while (mainEntity && !tempparentTransform2) {
              QList<Qt3DCore::QTransform*> transforms = mainEntity->componentsOfType<Qt3DCore::QTransform>();
              if (!transforms.isEmpty()) {
                tempparentTransform2 = transforms.first();
              }
              else {
                mainEntity = mainEntity->parentEntity();
              }
            }
            QVector<CModel> modelsList = calculatePositionsInRealWorld(mainEntity);
            if (modelsList.isEmpty())
              return;

            QVector<QVector3D> tempPts;
            for (int j = 0; j < modelsList.count(); j++)
            {
              for (int k = 0; k < modelsList.at(j).positions.count(); k++)
              {
                //tempPts.append(modelsList.at(j).positions.at(k));
                tempPts.append(tempparentTransform2->matrix() * modelsList.at(j).positions.at(k));
              }
            }

            BoundingBox3D box = getBoundingBox3D(tempPts);
            QMatrix4x4 mm = tempparentTransform2->worldMatrix();
            float mainwidth;
            float length = std::abs(box.maxPt.x() - box.minPt.x());
            float height = std::abs(box.maxPt.y() - box.minPt.y());
            float width = std::abs(box.maxPt.z() - box.minPt.z());
            Rect2D tile;
            if (width < length && width < height)
            {
              mainwidth = width;
              tile.width = std::abs(box.maxPt.x() - box.minPt.x());
              tile.height = std::abs(box.maxPt.y() - box.minPt.y());
            }
            else if (length < width && length < height)
            {
              mainwidth = length;
              tile.width = std::abs(box.maxPt.z() - box.minPt.z());
              tile.height = std::abs(box.maxPt.y() - box.minPt.y());
            }
            else if (height < length && height < width)
            {
              mainwidth = height;
              tile.width = std::abs(box.maxPt.x() - box.minPt.x());
              tile.height = std::abs(box.maxPt.z() - box.minPt.z());
            }
            else
            {
              mainwidth = width;
              tile.width = std::abs(box.maxPt.x() - box.minPt.x());
              tile.height = std::abs(box.maxPt.y() - box.minPt.y());
            }
            QVector<QPointF> tilesList;

            QVector<QPointF> polygonmain;
            for (int h = 0; h < dview->mainPolygon.count(); h++)
            {
              polygonmain.push_back(flipmat * dview->mainPolygon.at(h));
            }
            tilesList = findTilesInPolygon(polygonmain, tile);

            if (tilesList.isEmpty())
              return;

            int cc = rootEntity->children().count();

            if (!dview->mainobjFileName.isEmpty() && !dview->mainobjFileName.isNull())
              {
              for (int i = 1; i < tilesList.count(); i++)
              {
                emit Mediator::getInstance().readFile(dview->mainobjFileName, true);
              }
            }
            update();

            int ind = 0;
            for (int j = cc - 1; j < rootEntity->children().count(); j++)
            {
              QObject* child = rootEntity->children().at(j);
              Qt3DCore::QEntity* entity = dynamic_cast<Qt3DCore::QEntity*>(child);
              if (entity && typeid(*entity) == typeid(Qt3DCore::QEntity))
              {
                Qt3DCore::QEntity* tileEntity = entity;
                Qt3DCore::QTransform* tileTransform = nullptr;
                while (tileEntity && !tileTransform) {
                  QList<Qt3DCore::QTransform*> transforms = tileEntity->componentsOfType<Qt3DCore::QTransform>();
                  if (!transforms.isEmpty()) {
                    tileTransform = transforms.first();
                  }
                  else {
                    tileEntity = tileEntity->parentEntity();
                  }
                }
                QVector<CModel> modelsListTile = calculatePositionsInRealWorld(tileEntity);
                if (modelsListTile.isEmpty())
                  return;

                QMatrix4x4 n1 = tileTransform->matrix();

                QMatrix4x4 mat2;
                QMatrix4x4 mat1;
                if (ind >= tilesList.count())
                  break;

                QMatrix4x4 matrix0;
                QQuaternion quad22 = QQuaternion::rotationTo(QVector3D(0, 0, 1), QVector3D(0, 0, -1));
                matrix0.rotate(quad22);

                mat1.translate(QVector3D(tilesList.at(ind).x(), tilesList.at(ind).y(), width / 2));
                QQuaternion quadinv = quad.inverted();
                QMatrix4x4 matrix1 = mat1 * n1;
                //QMatrix4x4 matrix1 = mat1* matrix0 * n1;
                QMatrix4x4 matrix2;
                matrix2.rotate(quadinv);
                QMatrix4x4 matrix3 = mat.inverted();

                mat2 = matrix3 * matrix2 * matrix1;
                ind++;
                tileTransform->setMatrix(mat2);
              }
            }
          }
          if (dview)
            delete dview;
        }
      }
    }

    void RenderWindow::onEntityClicked(Qt3DRender::QPickEvent* event) {
        selectedEntity = qobject_cast<Qt3DCore::QEntity*>(event->entity());
        qDebug() << "\n\n\n   Selected Entity:" << selectedEntity << "\n\n\n";
        emit Mediator::getInstance().cameraSettingsChanged(camController->linearSpeed(), camController->lookSpeed(), camController->acceleration(), camController->deceleration());
        emit Mediator::getInstance().entitySelected(selectedEntity);


        selectedTransform = nullptr;
        Qt3DCore::QEntity* currentEntity = selectedEntity;
        while (currentEntity && !selectedTransform) {
            QList<Qt3DCore::QTransform*> transforms = currentEntity->componentsOfType<Qt3DCore::QTransform>();
            if (!transforms.isEmpty()) {
                selectedTransform = transforms.first();
            }
            else {
                currentEntity = currentEntity->parentEntity();
                qDebug() << "Going up to get transform";
            }
        }

        if (!selectedTransform) {
            qDebug() << "Transform is null?? ";
        }


        // Connect the transform's signals to the mediator to update the UI in real-time
        if (selectedTransform)
        {
            qDebug() << "Selectedtransform is available";
            
            /*QVector3D position = selectedTransform->translation();
            //view->camera()->setViewCenter(position);
            QCursor cursor;
            cursor.setShape(Qt::CursorShape::CrossCursor);
            view->setCursor(cursor);*/

            connect(selectedTransform, &Qt3DCore::QTransform::translationChanged, this, []() {
                emit Mediator::getInstance().transformChanged();
                qDebug() << "Transform changed signal emitted";
                });
            connect(selectedTransform, &Qt3DCore::QTransform::rotationChanged, this, []() {
                emit Mediator::getInstance().transformChanged();
                });
            connect(selectedTransform, &Qt3DCore::QTransform::scale3DChanged, this, []() {
                emit Mediator::getInstance().transformChanged();
                });
        }
        if (selectedTransformObj)
        {
            //qDebug() << "SelectedtransformObj is available";
            connect(selectedTransformObj, &Qt3DCore::QTransform::translationChanged, this, []() {
                emit Mediator::getInstance().transformChanged();
                });
            connect(selectedTransformObj, &Qt3DCore::QTransform::rotationChanged, this, []() {
                emit Mediator::getInstance().transformChanged();
                });
            connect(selectedTransformObj, &Qt3DCore::QTransform::scale3DChanged, this, []() {
                emit Mediator::getInstance().transformChanged();
                });
        }

            
        if (lastSelectedEntity) {
            //lastSelectedEntity->removeComponent(outlineMaterial);
        }

        Qt3DCore::QEntity* pickedEntity = qobject_cast<Qt3DCore::QEntity*>(event->entity());

        if (pickedEntity) {
            //pickedEntity->addComponent(outlineMaterial);
            lastSelectedEntity = pickedEntity;
        }
        else {
            lastSelectedEntity = nullptr;
        }
        
    }

    bool RenderWindow::eventFilter(QObject* obj, QEvent* event)
    {
        const QVector3D upVector(0.0f, 1.0f, 0.0f);

        const float minHeight = 0.0f;
        const float maxHeight = 0.5f;

        if (obj == view) {
            if (event->type() == QEvent::KeyPress) {
                QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
                if (keyEvent->key() == Qt::Key_Delete) {
                    deleteSelectedEntity();
                    //qDebug() << "Delete key pressed";
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_W) {
                    QVector3D viewMatrix = view->camera()->viewVector();
                    QVector3D viewPosition = view->camera()->position();
                    viewMatrix.setY(0);
                    viewMatrix.setX(0);
                    viewMatrix.setZ(1);
                    viewMatrix.normalize();

                    this->view->camera()->translate(viewMatrix * camController->linearSpeed() * 0.05);

                }

                else if (keyEvent->key() == Qt::Key_S) {
                    QVector3D viewMatrix = view->camera()->viewVector();
                    viewMatrix.setY(0);
                    viewMatrix.setX(0);
                    viewMatrix.setZ(1);
                    viewMatrix.normalize();
                    this->view->camera()->translate(-viewMatrix * camController->linearSpeed() * 0.05);

                }

                else if (keyEvent->key() == Qt::Key_A) {
                    QVector3D viewMatrix = view->camera()->viewVector();
                    viewMatrix.setY(0);
                    viewMatrix.setX(0);
                    viewMatrix.setZ(1);
                    viewMatrix.normalize();
                    QVector3D leftVector = QVector3D::crossProduct(upVector, viewMatrix);
                    this->view->camera()->translate(-leftVector * camController->linearSpeed() * 0.05);

                }

                else if (keyEvent->key() == Qt::Key_D) {
                    QVector3D viewMatrix = view->camera()->viewVector();
                    viewMatrix.setY(0);
                    viewMatrix.setX(0);
                    viewMatrix.setZ(1);
                    viewMatrix.normalize();
                    QVector3D rightVector = QVector3D::crossProduct(upVector, viewMatrix);
                    this->view->camera()->translate(rightVector * camController->linearSpeed() * 0.05);

                }
                else
                {
                    //qDebug() << "Ate key press" << keyEvent->key();
                    return true;
                }
                // Ensure the camera stays above the minimum height
                /*QVector3D cameraPosition = this->view->camera()->position();

                cameraPosition.setY(qMax(cameraPosition.y(), minHeight));
                cameraPosition.setY(qMin(cameraPosition.y(), maxHeight));
                this->view->camera()->setPosition(cameraPosition);*/
            }
            else {
                return false;
            }
        }
        else {
            // pass the event on to the parent class
            return RenderWindow::eventFilter(obj, event);
        }
    }

    void RenderWindow::findMeshes(Qt3DCore::QNode* node, QList<Qt3DRender::QMesh*>& meshes) {
        if (auto* mesh = dynamic_cast<Qt3DRender::QMesh*>(node)) {
            meshes.append(mesh);
        }

        for (auto* child : node->childNodes()) {
            findMeshes(child, meshes);
        }
    }
    /*void RenderWindow::CallParentEntity(Qt3DCore::QEntity** currentEntity)
    {
      Qt3DCore::QTransform* tempparentTransform = nullptr;
      while (currentEntity && !tempparentTransform) {
        QList<Qt3DCore::QTransform*> transforms = (*currentEntity)->componentsOfType<Qt3DCore::QTransform>();
        if (!transforms.isEmpty()) {
          tempparentTransform = transforms.first();
        }
        else {
          *currentEntity = (*currentEntity)->parentEntity();
          qDebug() << "Going up to get transform";
        }
      }
    }*/

    QVector3D RenderWindow::centerGeometry(Qt3DRender::QGeometryRenderer* geometryRenderer) {
        auto* geometry = geometryRenderer->geometry();

        if (!geometry)
        {
            qWarning() << "Geometry not found";
            return QVector3D(0.0f, 0.0f, 0.0f);
        }

        Qt3DCore::QAttribute* positionAttribute = nullptr;
        for (Qt3DCore::QAttribute* attribute : geometry->attributes()) {
            if (attribute->name() == Qt3DCore::QAttribute::defaultPositionAttributeName()) {
                positionAttribute = attribute;
                break;
            }
        }

        if (!positionAttribute) {
            qWarning() << "Position attribute not found in geometry";
            return QVector3D(0.0f, 0.0f, 0.0f);
        }

        // Get the buffer containing vertex positions
        auto* buffer = positionAttribute->buffer();
        QByteArray vertexBufferData = buffer->data();
        const char* vertexData = vertexBufferData.constData();
        int stride = positionAttribute->byteStride();
        int offset = positionAttribute->byteOffset();
        int count = positionAttribute->count();
        int vertexSize = positionAttribute->vertexSize();

        QVector3D min, max;
        bool initialized = false;

        for (int i = 0; i < count; ++i) {
            const char* dataPtr = vertexData + i * stride + offset;
            QVector3D vertex;
            memcpy(&vertex, dataPtr, vertexSize * sizeof(float));

            if (!initialized) {
                min = max = vertex;
                initialized = true;
            }
            else {
                min.setX(qMin(min.x(), vertex.x()));
                min.setY(qMin(min.y(), vertex.y()));
                min.setZ(qMin(min.z(), vertex.z()));
                max.setX(qMax(max.x(), vertex.x()));
                max.setY(qMax(max.y(), vertex.y()));
                max.setZ(qMax(max.z(), vertex.z()));
            }
        }

        QVector3D center = (min + max) * 0.5;
        return center;

        //Qt3DCore::QTransform* transform = geometryRenderer->findChild<Qt3DCore::QTransform*>();
        //if (transform) {
        //    transform->setTranslation(-center);
        //}
    }
    
    void RenderWindow::loadAndSetMesh(const QString& url, const bool isAsset) 
    {
        QUrl mesh_url(url);

        QFileInfo fi(url);
        QString fileNameOfUrl = fi.baseName();
        absoluteCenterForObj = QVector3D(0.0f, 0.0f, 0.0f);
;       
        Qt3DCore::QEntity* modelEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DRender::QSceneLoader* sceneMesh = new Qt3DRender::QSceneLoader(modelEntity);
        sceneMesh->setSource(mesh_url);

        Qt3DCore::QTransform* outTransform = new Qt3DCore::QTransform;
        //Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial;
        //Qt3DCore::QTransform* transform = sceneMesh->findChild< Qt3DCore::QTransform*>();
        
        Qt3DRender::QObjectPicker* picker = new Qt3DRender::QObjectPicker;
        QObject::connect(sceneMesh, &Qt3DRender::QSceneLoader::statusChanged, [this, modelEntity, sceneMesh](Qt3DRender::QSceneLoader::Status status) {
            if (status == Qt3DRender::QSceneLoader::Ready) {
                QVector3D sum(0.0f, 0.0f, 0.0f);
                int totalVertexCount = 0;

                const QList<Qt3DCore::QEntity*> entities = modelEntity->findChildren<Qt3DCore::QEntity*>();
                for (Qt3DCore::QEntity* entity : entities) {
                    if (entity == modelEntity) {
                        continue;
                    }

                    Qt3DRender::QGeometryRenderer* geometryRenderer = entity->findChild<Qt3DRender::QGeometryRenderer*>();
                    if (!geometryRenderer) {
                        continue;
                    }

                    auto* geometry = geometryRenderer->geometry();
                    if (!geometry) {
                        qWarning() << "Geometry not found";
                        continue;
                    }

                    Qt3DCore::QAttribute* positionAttribute = nullptr;
                    for (Qt3DCore::QAttribute* attribute : geometry->attributes()) {
                        if (attribute->name() == Qt3DCore::QAttribute::defaultPositionAttributeName()) {
                            positionAttribute = attribute;
                            break;
                        }
                    }

                    if (!positionAttribute) {
                        qWarning() << "Position attribute not found in geometry";
                        continue;
                    }

                    auto* buffer = positionAttribute->buffer();
                    QByteArray vertexBufferData = buffer->data();
                    const char* vertexData = vertexBufferData.constData();
                    int stride = positionAttribute->byteStride();
                    int offset = positionAttribute->byteOffset();
                    int count = positionAttribute->count();
                    int vertexSize = positionAttribute->vertexSize();

                    for (int i = 0; i < count; ++i) {
                        const char* dataPtr = vertexData + i * stride + offset;
                        QVector3D vertex;
                        memcpy(&vertex, dataPtr, vertexSize * sizeof(float));
                        sum += vertex;
                    }

                    totalVertexCount += count;
                }

                if (totalVertexCount > 0) {
                    absoluteCenterForObj = sum / static_cast<float>(totalVertexCount);
                    emit centerCalculated(); 
                }
            }
            });

        QObject::connect(this, &RenderWindow::centerCalculated, [this, modelEntity, outTransform]() {
            const QList<Qt3DCore::QEntity*> entities = modelEntity->findChildren<Qt3DCore::QEntity*>();
            for (Qt3DCore::QEntity* entity : entities) {
                if (entity == modelEntity) {
                    continue;
                }

                entity->addComponent(objectsLayer);
                Qt3DRender::QObjectPicker* picker = new Qt3DRender::QObjectPicker(entity);
                entity->addComponent(picker);


                // Connect the picker's clicked signal to a slot
                connect(picker, &Qt3DRender::QObjectPicker::clicked, this, &RenderWindow::onEntityClicked);

                Qt3DCore::QTransform* transform = entity->findChild<Qt3DCore::QTransform*>();
                

                if (transform) {
                    connect(picker, &Qt3DRender::QObjectPicker::clicked, [this, transform]() {
                        this->gizmo->setDelegateTransform(transform);
                        });
                }

                connect(
                    picker, &Qt3DRender::QObjectPicker::clicked,
                    [=](Qt3DRender::QPickEvent* event) {

                        transformList.append(selectedTransform);


                        if (event->modifiers() & Qt::ShiftModifier) {
                            dragStarted = true;
                        }
                    });

                connect(
                    picker, &Qt3DRender::QObjectPicker::moved,
                    [=](Qt3DRender::QPickEvent* event) {
                        if (dragStarted && selectedTransformObj != nullptr) {
                            QVector3D oldPos = selectedTransformObj->translation();
                            QVector3D deltaMove(event->worldIntersection().x() - oldPos.x(), 0, event->worldIntersection().z() - oldPos.z());
                            selectedTransformObj->setTranslation(oldPos + deltaMove);
                        }
                    });

                connect(
                    picker, &Qt3DRender::QObjectPicker::released,
                    [=](Qt3DRender::QPickEvent* event) {
                        dragStarted = false;
                    });
            }
            outTransform->setTranslation(-absoluteCenterForObj);
            modelEntity->addComponent(outTransform);
            });
        
        
        // Here object layer is being added to both parent entity and all the child entities, this needs more testing
        // Also note outTransform is being added to modelEntity

        QString entityName;
        modelEntity->addComponent(sceneMesh);
        modelEntity->addComponent(objectsLayer);
        
        //modelEntity->addComponent(material);
        //modelEntity->addComponent(transform);
        
        if (!sceneMesh->entityNames().isEmpty())
        {
           entityName = sceneMesh->entityNames().first();
        }
        else
        {
            entityName = fileNameOfUrl + "-obj-" + QString::number(wavefrontObjCount);
            wavefrontObjCount++;
        }
        modelEntity->setObjectName(entityName);
        //modelEntity->addComponent(picker); // Toy picker
        
        updateTreeListWidget();

/*
        connect(mesh, &Qt3DRender::QMesh::statusChanged, [=](Qt3DRender::QMesh::Status status) {
            qDebug() << status;
            });

*/
       
/*
        connect(
            picker, &Qt3DRender::QObjectPicker::clicked,
            [this, transform]() {
                this->gizmo->setDelegateTransform(transform);
            });

        
        connect(picker, &Qt3DRender::QObjectPicker::clicked, this, &RenderWindow::onEntityClicked);

    */    
        emit Mediator::getInstance().meshLoaded(url);  // Emit signal when the mesh is loaded
   
    }

    void RenderWindow::updateTreeListWidget()
    {
        QObjectList childrenList = rootEntity->children();
        QStringList childrenStringList;
        Qt3DRender::QPointLight* pointLu = lightEntity1->findChild<Qt3DRender::QPointLight*>();
        //qDebug() << "Point Light intensity measured independently: " << pointLu->intensity();
        
        Qt3DCore::QTransform* lightTu = lightEntity1->findChild<Qt3DCore::QTransform*>();
        //qDebug() << "Light transformation measured independently: " << lightTu->translation();
        
        for (auto& child : childrenList)
        {
            if (!child->objectName().isEmpty())
                childrenStringList.append(child->objectName());
            else
                childrenStringList.append("Entity");
        }

        emit Mediator::getInstance().childrenUpdated(childrenStringList);
    }

    void RenderWindow::deleteSelectedEntity() {
        if (selectedEntity) {
            qDebug() << "Deleting" << selectedEntity;
            if (gizmo->delegateTransform() == selectedTransform) {
                Qt3DCore::QTransform* tempTransform = new Qt3DCore::QTransform;
                for (auto& transform : transformList)
                {
                    if (transform)
                        tempTransform = transform;
                }
                gizmo->setDelegateTransform(tempTransform);
            }
            selectedEntity->deleteLater();
            selectedEntity = nullptr; // Reset the selected entity
            selectedMaterial = nullptr;
            selectedTransform = nullptr;
            emit Mediator::getInstance().hidePropertiesWindow(); // To hide properties window
            updateTreeListWidget();
        }
        
        if (selectedEntityObj) {
            qDebug() << "Deleting" << selectedEntityObj;
            if (gizmo->delegateTransform() == selectedTransformObj) {
                Qt3DCore::QTransform* tempTransform  = new Qt3DCore::QTransform;
                for (auto& transform : transformList)
                {
                    if (transform)
                        tempTransform = transform;
                }
                gizmo->setDelegateTransform(tempTransform);
            }
            selectedEntityObj->deleteLater();
            selectedEntityObj = nullptr; // Reset the selected entity
            selectedMaterialObj = nullptr;
            selectedTransformObj = nullptr;
            updateTreeListWidget();
        }
    }

    void RenderWindow::mousePressEvent(QMouseEvent* event) {
        QWidget::mousePressEvent(event);

        if (event->button() == Qt::LeftButton && !dragStarted) {
            
                if (selectedMaterial != nullptr) {
                    //uncomment this
                    /*QList<Qt3DRender::QParameter*> parameters = selectedMaterial->parameters();
                    for (Qt3DRender::QParameter* param : parameters) {
                        if (param->name() == QStringLiteral("ka")) {
                            param->setValue(QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f));
                        }
                    }*/ 
                    //selectedMaterial->property("ka").setValue(QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f));
                    //qDebug() << "selectedMaterial->setProperty(\"ka\", QColor(Qt::white)); and nullptr";
                    selectedMaterial = nullptr;
                    selectedTransform = nullptr;
                }
                if (selectedMaterialObj != nullptr) {
                    
                    selectedMaterialObj->setAmbient(QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f));
                    //qDebug() << "selectedMaterial->setProperty(\"ka\", QColor(Qt::white)); and nullptr";
                    selectedMaterialObj = nullptr;
                    selectedTransformObj = nullptr;
                }
        }
    }

    void RenderWindow::onCameraLinearSpeedChanged(float linearSpeed)
    {
        if (camController)
        {
            camController->setLinearSpeed(linearSpeed);
        }
    }

    void RenderWindow::onCameraLookSpeedChanged(float lookSpeed)
    {
        if (camController)
        {
            camController->setLookSpeed(lookSpeed);
        }
    }

    void RenderWindow::onCameraAccelerationChanged(float acceleration)
    {
        if (camController)
        {
            camController->setAcceleration(acceleration);
        }
    }

    void RenderWindow::onCameraDecelerationChanged(float deceleration)
    {
        if (camController)
        {
            camController->setDeceleration(deceleration);
        }
    }

    void RenderWindow::onLightColorChanged(const QColor& color)
    {
        if (pointLight1)
        {
            pointLight1->setColor(color);
            //qDebug() << pointLight->color();
        }
    }

    void RenderWindow::onLightIntensityChanged(double intensity)
    {
        //qDebug() << "RenderWindow onLightIntensityChanged slot triggered";
        
        if (pointLight1)
        {
            Qt3DRender::QPointLight* pointL = lightEntity1->findChild<Qt3DRender::QPointLight*>();
            pointL->setIntensity(intensity);
            //qDebug() << pointL->intensity();
            lightIntensityParam1->setValue(pointL->intensity());
            //qDebug() << "PointLight intensity set successfully";

        }
        else
        {
            qDebug() << "Point light is nullptr";
        }
    }

    void RenderWindow::onLightPositionChanged(const QVector3D& position)
    {
        if (lightTransform1)
        {
            Qt3DCore::QTransform* lightT = lightEntity1->findChild<Qt3DCore::QTransform*>();
            lightT->setTranslation(position);
            lightPositionParam1->setValue(lightT->translation());

            //qDebug() << lightT->translation();
        }

    }
} 