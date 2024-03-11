#include "aidcamera.h"

#include <Qt3DRender/QCamera>

QT_BEGIN_NAMESPACE

namespace AID {

    /*!
        
        The controls are:
        \table
        \header
            \li Input
            \li Action
        \row
            \li Left mouse button
            \li While the left mouse button is pressed, mouse movement along x-axis pans the camera and
            movement along y-axis tilts it.
        \row
            \li Mouse scroll wheel
            \li Zooms the camera in and out without changing the view center.
        \row
            \li Shift key
            \li Turns the fine motion control active while pressed. Makes mouse pan and tilt less
            sensitive.
        \row
            \li Arrow keys
            \li Move the camera horizontally relative to camera viewport.
        \row
            \li Page up and page down keys
            \li Move the camera vertically relative to camera viewport.
        \row
            \li Escape
            \li Moves the camera so that entire scene is visible in the camera viewport.
        \endtable
    */

    AIDCamera::AIDCamera(Qt3DCore::QNode* parent)
        : QAbstractCameraController(parent)
    {
    }

    AIDCamera::~AIDCamera()
    {
    }


    void AIDCamera::moveCamera(const QAbstractCameraController::InputState& state, float dt)
    {
        Qt3DRender::QCamera* theCamera = camera();

        if (theCamera == nullptr)
            return;
       

        const QVector3D upVector(0.0f, 1.0f, 0.0f);
        
        //theCamera->translate(QVector3D(state.txAxisValue * linearSpeed(),
        //    state.tyAxisValue * linearSpeed(),
        //    state.tzAxisValue * linearSpeed()) * dt);
        //if (state.rightMouseButtonActive) {
        //    float theLookSpeed = lookSpeed();
        //    if (state.shiftKeyActive) {
        //        theLookSpeed *= 0.2f;
        //    }

        //    //const QVector3D upVector(0.0f, 1.0f, 0.0f);

        //    theCamera->pan(state.rxAxisValue * theLookSpeed * dt, upVector);
        //    theCamera->tilt(state.ryAxisValue * theLookSpeed * dt);
        //}
        //else if (state.middleMouseButtonActive) {
        //    // Orbit
        //    theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, upVector);
        //    theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
        //}

        theCamera->translate(QVector3D(state.txAxisValue * linearSpeed(),
            state.tyAxisValue * linearSpeed(),
            state.tzAxisValue * linearSpeed()) * dt);


        if (state.rightMouseButtonActive) {

            theCamera->pan(state.rxAxisValue * lookSpeed() * dt, upVector);
            theCamera->tilt(state.ryAxisValue * lookSpeed() * dt);
        }
        else if (state.middleMouseButtonActive) {
            // Orbit
            theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt , upVector);
            theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
        }
    }

} // AID

QT_END_NAMESPACE

