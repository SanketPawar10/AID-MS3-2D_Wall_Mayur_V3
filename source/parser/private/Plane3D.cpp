#include "Plane3D.h"

namespace AID {

  Plane3D::Plane3D() {}
  Plane3D::~Plane3D() {}
  
  Plane3D::Plane3D(const QVector3D& pt, const QVector3D& vec) : origin(pt), normal(vec)
  {
  }

  QVector3D Plane3D::ProjectPointsOnPlane(const QVector3D& point)
  {
    float dist = point.distanceToPlane(origin, normal);
    QVector3D projectedPoint = { point.x() - dist * normal.x(), point.y() - dist * normal.y(), point.z() - dist * normal.z() };
    return projectedPoint;
  }
} // namespace AID
