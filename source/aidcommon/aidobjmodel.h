#pragma once

#include <QVector>
#include <QVector3D>
#include <QPointF>

struct ObjModel; // Forward declaration
struct ObjMaterial; // Forward declaration





struct Object3D {
    std::optional<uint> version; 
    QString name;
    QString description;
    QString arrangement;
    std::optional<QString> groupName;
    std::optional<double> floorDistance;

    Object3D() : version(0), groupName(std::nullopt), floorDistance(std::nullopt) {}
};

struct ObjMaterial {
    QString name;
    QVector3D ka; // Ambient color
    QVector3D kd; // Diffuse color
    QVector3D ks; // Specular color
    QVector3D ke; // Emission color
    float shininess; // Specular exponent
    float transparency; // Transparency (or "dissolve")
    QString textureMap; // Texture map file
    bool twoSided;
    bool flipped; // Flipped Tag

    // Extend
    double metalness;
    double roughness;
    double specularValue;
    QVector3D emissionColor;
    QString normalMap;
    QString parallaxMap;
    QString ambientOcclusionMap;



    /*ObjMaterial()
        : shininess(0.0f), transparency(1.0f), twoSided(false), flipped(false), 
          metalness(0.0), roughness(0.0),specularValue(0.0), emissionColor(QVector3D(0, 0, 0)) {}*/
};

struct ObjModel {
    QString name;
    QString type;
    uint version;
    QVector3D center;
    QVector3D localCenter;
    QVector<QVector3D> vertices;
    QVector<QVector3D> faces;
    QVector<QVector3D> normals;
    QVector<QPointF> texCoords;
    QVector<unsigned int> smoothingGroups;
    uint matGroupsCount;
    QVector<QPair<QString, QVector<int>>> matGroups;
    QVector<QPair<QString, ObjMaterial>> materials;
    Object3D properties;
    QString assetPath;
    uint count;
    bool isAsset;
};