#pragma once
#include <QVector3D>

class CLine3D {
private:
  QVector3D p1;
  QVector3D p2;
public:
  CLine3D() {}
  CLine3D(const QVector3D& pt1, const QVector3D& pt2) :p1(pt1), p2(pt2) {}

  void SetPoints(const QVector3D& pt1, const QVector3D& pt2)
  {
    p1 = pt1;
    p2 = pt2;
  }

  QVector3D GetP1() const
  {
    return p1;
  }
  QVector3D GetP2() const
  {
    return p2;
  }
};
