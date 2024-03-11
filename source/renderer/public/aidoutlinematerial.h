#pragma once

#include <Qt3DRender/QMaterial>

namespace AID {


    class AIDOutlineMaterial : public Qt3DRender::QMaterial
    {
        Q_OBJECT

    public:
        AIDOutlineMaterial(Qt3DCore::QNode* parent = nullptr);
        ~AIDOutlineMaterial();
    };

}