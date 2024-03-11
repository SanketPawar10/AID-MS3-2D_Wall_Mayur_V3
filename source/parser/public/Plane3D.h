#pragma once

#include <QVector3D>

namespace AID {

  class Plane3D {

  private:

    QVector3D origin;
    QVector3D normal;
  
  public:
    
    Plane3D();
    ~Plane3D();
    
    Plane3D(const QVector3D& pt, const QVector3D& vec);

    QVector3D GetOrigin()
    {
      return origin;
    }
    
    QVector3D GetNormal()
    {
      return normal;
    }
    void SetOrigin(QVector3D org)
    {
      origin = org;
    }

    void SetNormal(QVector3D norm)
    {
      normal = norm;
    }

    QVector3D ProjectPointsOnPlane(const QVector3D& point);
  
  };
} // namespace AID




