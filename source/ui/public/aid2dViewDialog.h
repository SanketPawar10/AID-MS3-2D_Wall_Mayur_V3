#pragma once

#include <QPushButton>
#include <QDialog>
#include "Line3D.h"
#include <QGraphicsView>
#include <qlayout.h>
#include <QGraphicsItem>
#include <QMouseEvent>
#include "aidrenderwindow.h"
#include <qdockwidget.h>
#include <QtWidgets>
#include "aidmediator.h"


namespace AID
{
	struct BoundingBox2D {
		QPointF minPt;
		QPointF maxPt;
	};

	class C2DView : public QGraphicsView
	{
	public:
		C2DView(){}
		C2DView(QVector<QVector<CLine3D>> wallBoundary, QWidget* parent = nullptr);
		C2DView(QVector<QVector<CLine3D>> wallBoundary, QStatusBar* bar, QWidget* parent = nullptr);
		C2DView(QWidget* parent = nullptr);
		~C2DView();

		QGraphicsScene* scene;
		bool isPick;
		QPointF pickedPt;

	private:
		QPen pen;
		QVector<QVector<CLine3D>> wallBoundaryList;
		QStatusBar* bar;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		bool onSegment(QPointF p, QPointF q, QPointF r);
	};

	class C2DViewDialog : public QDialog
	{
		Q_OBJECT
	
	public:
		C2DViewDialog() {}
		C2DViewDialog(QWidget* parent = nullptr);
		C2DViewDialog(QVector<QVector<CLine3D>> wallBoundary, QWidget* parent = nullptr);
		C2DViewDialog(QVector<CLine3D> wallBoundary, QVector<QVector<QVector<CLine3D>>> modelBoundaries, QWidget* parent = nullptr);
		~C2DViewDialog();
	
	private:
		BoundingBox2D getBoundingBox2D(const QVector<CLine3D>& wblist);
		bool onSegment(QPointF p, QPointF q, QPointF r);
		int orientation(QPointF p, QPointF q, QPointF r);
		bool isPointInPolygon(QPointF p, QVector<QPointF>& polygon);
		void set_asset_store_dockwidget();
		
	signals:

	public slots:
		void DisplayClicked();
		void PickClicked();
		void OKUpdateClicked();
		void CloseUpdateClicked();
		
	private:
		QPushButton* PickButton;
		QPushButton* WidthButton;
		QPushButton* OKButton; 
		QPushButton* CloseButton;
		QVector<QVector<CLine3D>> wallBoundaryList;
		C2DView* view;
		QPen pen;
		QVBoxLayout* vLayout;
		QHBoxLayout* hLayout;
		QHBoxLayout* hBottomLayout;
		QGraphicsLineItem* line;
		QDockWidget* asset_store;
		QStatusBar* status_bar;
		bool selected;
	public:
		QVector<QPointF> mainPolygon;
		QString mainobjFileName;
	};
}