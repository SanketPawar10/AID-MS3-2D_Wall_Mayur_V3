#include "aidoutlinematerial.h"
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QRenderPass>
#include <Qt3DCore/QNode>
#include <QUrl>


namespace AID {

    AIDOutlineMaterial::AIDOutlineMaterial(Qt3DCore::QNode* parent)
        : Qt3DRender::QMaterial(parent)
    {
        Qt3DRender::QEffect* outlineEffect = new Qt3DRender::QEffect(this);

        Qt3DRender::QTechnique* outlineTechnique = new Qt3DRender::QTechnique(outlineEffect);

        Qt3DRender::QRenderPass* outlineRenderPass = new Qt3DRender::QRenderPass(outlineTechnique);
        Qt3DRender::QShaderProgram* outlineShaderProgram = new Qt3DRender::QShaderProgram(outlineRenderPass);

        outlineShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/outline.vert"))));
        outlineShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/outline.frag"))));

        outlineRenderPass->setShaderProgram(outlineShaderProgram);

        outlineTechnique->addRenderPass(outlineRenderPass);

        outlineEffect->addTechnique(outlineTechnique);

        this->setEffect(outlineEffect);
    }

    AIDOutlineMaterial::~AIDOutlineMaterial()
    {
    }
}