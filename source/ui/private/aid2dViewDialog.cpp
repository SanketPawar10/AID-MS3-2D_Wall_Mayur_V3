#include "aid2dViewDialog.h"
#include <QGraphicsView>

AID::C2DView::C2DView(QVector<QVector<CLine3D>> wallBoundary, QStatusBar* bar1, QWidget* parent)
	: wallBoundaryList(wallBoundary),
	bar(bar1),
	QGraphicsView(parent)
{
	setMouseTracking(true);
	scene = new QGraphicsScene(this);
	this->setScene(scene);
	isPick = false;
}
AID::C2DView::~C2DView()
{
	if (scene)
		delete scene;
}

void AID::C2DView::mousePressEvent(QMouseEvent* evt)
{
	if (evt->button() == Qt::LeftButton)
	{
		pickedPt = mapToScene(evt->pos());
		isPick = true;
	}
	update();
}

void AID::C2DView::mouseMoveEvent(QMouseEvent* evt)
{
	bar->showMessage(QString("x: %1, y: %2").arg(mapToScene(evt->pos()).x()).arg((-1)*mapToScene(evt->pos()).y()));
}

// Function to check if point q lies on segment p-r
bool AID::C2DView::onSegment(QPointF p, QPointF q, QPointF r) {
	if ((q.x() < std::max(p.x(), r.x()) || std::abs(q.x() - std::max(p.x(), r.x())) < 0.0001) &&
		(q.x() > std::min(p.x(), r.x()) || std::abs(q.x() - std::min(p.x(), r.x())) < 0.0001) &&
		(q.y() < std::max(p.y(), r.y()) || std::abs(q.y() - std::max(p.y(), r.y())) < 0.0001) &&
		(q.y() > std::min(p.y(), r.y()) || std::abs(q.y() - std::min(p.y(), r.y())) < 0.0001))
		return true;
	return false;
}

AID::C2DViewDialog::C2DViewDialog(QVector<QVector<CLine3D>> wallBoundary, QWidget* parent)
	: wallBoundaryList(wallBoundary),
	  QDialog(parent)
{
	status_bar = new QStatusBar(this);
	view = new C2DView(wallBoundary, status_bar, this);
	view->show();
	
	this->setWindowTitle("Tile Placing Window");
	//this->setStyleSheet("background-color: black");
	this->setAutoFillBackground(true);
	
	view->setBackgroundBrush(QBrush(Qt::gray, Qt::SolidPattern));
	vLayout = new QVBoxLayout(this);
	hLayout = new QHBoxLayout(this);
	hBottomLayout = new QHBoxLayout(this);
	OKButton = new QPushButton(this);
	CloseButton = new QPushButton(this);
	PickButton = new QPushButton(this);
	asset_store = new QDockWidget("Dock", this);

	OKButton->setParent(this);
	CloseButton->setParent(this);
	PickButton->setParent(this);
	
	PickButton->setEnabled(true);
	PickButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	PickButton->setStyleSheet("background-color: gray");
	PickButton->setText("Pick");
	
	OKButton->setEnabled(true);
	OKButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	OKButton->setStyleSheet("background-color: gray");
	OKButton->setText("OK");

	CloseButton->setEnabled(true);
	CloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	CloseButton->setStyleSheet("background-color: gray");
	CloseButton->setText("Close");

	status_bar->setEnabled(true);
	status_bar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	asset_store->setEnabled(true);
	asset_store->setParent(this);

	asset_store->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QIcon icon = QIcon(":/icons/folder-open.svg");
	
	status_bar->setStyleSheet(QString::fromUtf8("QStatusBar {background: rgb(30, 40, 50); color: rgb(255, 255, 255)}"));
	view->setParent(this);
	QVector<CLine3D> tempBoxlist;
	for (int x = 0; x < wallBoundaryList.count(); x++)
	{
		tempBoxlist.append(wallBoundaryList.at(x));
	}
	BoundingBox2D box = getBoundingBox2D(tempBoxlist);
	selected = false;
	int xval = (int)(box.maxPt.x() - box.minPt.x());
	int yval = (int)(box.maxPt.y() - box.minPt.y());
	int finalval = xval > yval ? xval : yval;
	finalval += (int)(finalval / 5);
	set_asset_store_dockwidget();
	
	int mainheight = (finalval > 350) ? 350 : finalval;
	view->scene->setSceneRect((-finalval/2),(-finalval/2), finalval, finalval);
	view->setMinimumSize(QSize(finalval+(int)(finalval/25), mainheight+(int)(mainheight/25)));
	status_bar->setMinimumSize(QSize(finalval+(int)(finalval / 10), 15));
	
	hLayout->addWidget(PickButton);
	hBottomLayout->setAlignment(Qt::AlignRight);
	hBottomLayout->addWidget(OKButton);
	hBottomLayout->addWidget(CloseButton);
	vLayout->addLayout(hLayout);
	
	vLayout->addWidget(view);
	vLayout->addLayout(hBottomLayout);

	vLayout->addWidget(status_bar);
	vLayout->addWidget(asset_store);
	hLayout->addStretch(0);
	pen.setColor(Qt::blue);
	pen.setWidth(0.5);
	DisplayClicked();
	
	connect(PickButton, SIGNAL(clicked()), this, SLOT(PickClicked()));
	connect(OKButton, SIGNAL(clicked()), this, SLOT(OKUpdateClicked()));
	connect(CloseButton, SIGNAL(clicked()), this, SLOT(close()));
}
	
AID::C2DViewDialog::~C2DViewDialog()
{
	if (PickButton)
		delete PickButton;
	if (status_bar)
		delete status_bar;
	if (asset_store)
		delete asset_store;
	if (OKButton)
		delete OKButton;
	if (CloseButton)
		delete CloseButton;
	if (view)
		delete view;
	if (hLayout)
		delete hLayout;
	if (hBottomLayout)
		delete hBottomLayout;
	if (vLayout)
		delete vLayout;
}

AID::BoundingBox2D AID::C2DViewDialog::getBoundingBox2D(const QVector<CLine3D>& wbList)
{
	QVector<QVector3D> points;
	for (int i = 0; i < wbList.count(); i++)
	{
		points.push_back(wbList.at(i).GetP1());
		points.push_back(wbList.at(i).GetP2());
	}

	if (points.empty()) {
		// Handle empty input
		return { {0.0f, 0.0f}, {0.0f, 0.0f} };
	}

	// Initialize min and max points
	QVector3D min_point = points[0];
	QVector3D max_point = points[0];

	// Update min and max coordinates
	for (const auto& p : points) {

		min_point.setX(std::min(min_point.x(), p.x()));
		min_point.setY(std::min(min_point.y(), p.y()));
		max_point.setX(std::max(max_point.x(), p.x()));
		max_point.setY(std::max(max_point.y(), p.y()));
	}
	QPointF min_point1(min_point.x(), min_point.y());
	QPointF max_point1(max_point.x(), max_point.y());

	return { min_point1, max_point1 };
}

void AID::C2DViewDialog::DisplayClicked()
{
	QBrush b;
	for (int i = 0; i < wallBoundaryList.count(); i++)
	{
		QList<QPointF> pts;
		for (int j = 0; j < wallBoundaryList.at(i).count(); j++)
		{
			QPointF p1(wallBoundaryList.at(i).at(j).GetP1().x(), wallBoundaryList.at(i).at(j).GetP1().y());
			pts.append(p1);
		}
		QPolygonF pl(pts);
		view->scene->addPolygon(pl, pen, b);
	}
}

void AID::C2DViewDialog::CloseUpdateClicked()
{
	this->close();
}

void AID::C2DViewDialog::PickClicked()
{
	if (view->pickedPt.isNull())
		return;
	
	QVector<QVector<QLineF>> polyList;
	for (int i = 0; i < wallBoundaryList.count(); i++)
	{
		QVector<QLineF> polyList1;
		for (int j = 0; j < wallBoundaryList.at(i).count(); j++)
		{
			QPointF p1(wallBoundaryList.at(i).at(j).GetP1().x(), wallBoundaryList.at(i).at(j).GetP1().y());
			QPointF p2(wallBoundaryList.at(i).at(j).GetP2().x(), wallBoundaryList.at(i).at(j).GetP2().y());
			polyList1.push_back(QLineF(p1, p2));
		}
	
		if(!polyList1.isEmpty())
		polyList.push_back(polyList1);
	}
	
	QVector<QPointF> polygon;
	for (int i = 0; i < polyList.count(); i++) {
		polygon.clear();
		for (int j = 0; j < polyList.at(i).count(); j++)
			polygon.push_back(polyList.at(i).at(j).p1());

		bool isInside = isPointInPolygon(view->pickedPt, polygon);
		if (isInside)
			break;
	}
	
	QPolygonF pl(polygon);
	mainPolygon = polygon;
	QPen pen2;
	pen2.setColor(Qt::green);
	pen2.setWidth(1);
	QBrush b;
	view->scene->addPolygon(pl, pen2, b);

	view->isPick = false;

}

void AID::C2DViewDialog::set_asset_store_dockwidget()
{
	QString dockWidgetStylesheet = R"(
    QDockWidget::title {
        text-align: left;
        background: rgb(27, 36, 50);
        padding-left: 5px;
        color: rgb(255, 255, 255);
    }
    QDockWidget {
        border: 1px solid #555;
    }
    QDockWidget::close-button, QDockWidget::float-button {
        border: none;
        background: transparent;
    }
    QDockWidget::close-button:hover, QDockWidget::float-button:hover {
        background: rgb(40, 50, 60);
    }
    QDockWidget > QWidget {
        background: rgb(18, 20, 32);
    }
)";

	asset_store->setAllowedAreas(Qt::BottomDockWidgetArea);
	asset_store->setFeatures(QDockWidget::DockWidgetClosable);
	//_asset_store_widget_dock_contents->setStyleSheet(QString::fromUtf8("QWidget {background: rgb(255, 255, 255); border: 1px solid black;}"));
	asset_store->setContentsMargins(0, 0, 0, 0);

	//asset_store->hide();
	asset_store->setWindowTitle("Tiles Store");
	//asset_store->setMinimumSize(500, 100);
	asset_store->setStyleSheet(dockWidgetStylesheet);
	asset_store->setContextMenuPolicy(Qt::ActionsContextMenu);

	QWidget* multiWidget = new QWidget();
	QHBoxLayout* layout = new QHBoxLayout();
	QListWidget* listWidget = new QListWidget(asset_store);
	listWidget->setContentsMargins(0, 0, 0, 0);

	listWidget->setViewMode(QListView::IconMode);

	QString itemPath = ":/interiorTileObjects/";
	QDirIterator it(itemPath, { "*.png" }, QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString imagePath = it.next();
		QString imgBaseName = QFileInfo(imagePath).baseName();
		QString imgCompleteBaseName = QFileInfo(imagePath).completeBaseName();
		int index = imagePath.lastIndexOf(".png");
		QString extenstionLessPath = imagePath.left(index);

		QListWidgetItem* newItem = new QListWidgetItem;
		newItem->setText(imgCompleteBaseName);
		QIcon tempIconForList;
		QPixmap iconPixmap(imagePath);

		if (iconPixmap.size().width() < 150 || iconPixmap.size().height() < 150) {
			iconPixmap = iconPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		tempIconForList.addPixmap(iconPixmap, QIcon::Normal, QIcon::Off);
		newItem->setIcon(tempIconForList);
		listWidget->addItem(newItem);

		QString objPath = ":/interiorTileObjects";
		QString objFileName = extenstionLessPath + ".xobj3d";
		qDebug() << "objFileName: " << objFileName;

		connect(listWidget, &QListWidget::itemClicked, this, [this, objPath, objFileName](QListWidgetItem* item) {
			if (item->text() == QFileInfo(objFileName).completeBaseName()) {
				if (mainPolygon.isEmpty())
					return;
				mainobjFileName = objFileName;
				selected = true;
			}
			});
	}

	multiWidget->setLayout(layout);
	layout->addWidget(listWidget);

	asset_store->setWidget(multiWidget);
}

// Function to check if point q lies on segment p-r
bool AID::C2DViewDialog::onSegment(QPointF p, QPointF q, QPointF r) {
	if ((q.x() < std::max(p.x(), r.x()) || std::abs(q.x() - std::max(p.x(), r.x()))<0.0001)&&
		(q.x() > std::min(p.x(), r.x()) || std::abs(q.x() - std::min(p.x(), r.x()))<0.0001) &&
		(q.y() < std::max(p.y(), r.y()) ||std::abs(q.y() - std::max(p.y(), r.y()))<0.0001) &&
		(q.y() > std::min(p.y(), r.y())|| std::abs(q.y() - std::min(p.y(), r.y()))<0.0001))
		return true;
	return false;
}

// Function to find orientation of ordered triplet (p, q, r)
int AID::C2DViewDialog::orientation(QPointF p, QPointF q, QPointF r) {
	float val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
	if (val < 0.0001 && val> -0.0001)
		return 0; // Collinear
	return (val > 0.0001) ? 1 : 2; // Clockwise or counterclockwise
}

// Check if a point is inside a polygon
bool AID::C2DViewDialog::isPointInPolygon(QPointF p, QVector<QPointF>& polygon) {
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
				std::abs(p.y() - std::max(polygon[i].y(), polygon[next].y()))<0.0001) &&
				(p.x() < std::max(polygon[i].x(), polygon[next].x()) || 
				std::abs(p.x() - std::max(polygon[i].x(), polygon[next].x()))<0.0001)) {
				if (polygon[i].y() != polygon[next].y()) {
					float xIntersection = (p.y() - polygon[i].y()) * (polygon[next].x() - polygon[i].x()) /
						(polygon[next].y() - polygon[i].y()) + polygon[i].x();
					if (p.x() < xIntersection || std::abs(p.x()- xIntersection)<0.0001) count++;
				}
			}
		}
	}
	return (count % 2 == 1); // Inside if odd number of intersections
}

void AID::C2DViewDialog::OKUpdateClicked()
{
	if(!mainobjFileName.isEmpty() && !mainobjFileName.isNull())
		emit Mediator::getInstance().readFile(mainobjFileName, true);

	if (selected)
		accept();
}