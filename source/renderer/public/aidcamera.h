#pragma once

#include <Qt3DExtras/qabstractcameracontroller.h>

QT_BEGIN_NAMESPACE

namespace AID {

    class AIDCamera : public Qt3DExtras::QAbstractCameraController
    {
        Q_OBJECT

    public:
        explicit AIDCamera(Qt3DCore::QNode* parent = nullptr);
        ~AIDCamera();

    private:
        void moveCamera(const QAbstractCameraController::InputState& state, float dt) override;
    };

} // AID

QT_END_NAMESPACE

