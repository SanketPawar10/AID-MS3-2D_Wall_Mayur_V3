#include "aidmainwindow.h"
#include "aidmediator.h"

/*
source:
->aidcommon/
-->aidcustdefines.h
-->aidmediator.h
-->aidobjmodel.h
-->aidutils.h
->application/
-->private/
--->aidsession.cpp
-->public/
--->aidsession.h
->parser/
-->private/
--->aidparser.cpp
-->public/
--->aidparser.h
->renderer/
-->private/
--->aidrenderwindow.cpp
--->aidcamera.cpp
-->public/
--->aidrenderwindow.h
--->aidcamera.h
->shader
-->default.vert
-->gizmo.frag
-->gizmo.vert
-->shaders.qrc
-->test.frag
->ui/
-->private/
--->aidmainwindow.cpp
-->public/
--->aidmainwindow.h
thirdparty
// Qt, miniz, etc
*/

// Constructor: Sets default values for various pointers and member variables
AID::MainWindow::MainWindow(QWidget* parent)
	:_mainwindow_id(0),
	_mainwindow(nullptr),
	_renderwindow(nullptr),
	_mainwindow_central_widget(nullptr),
	_mainwindow_layout(nullptr),

	_menu_bar(nullptr),
	_tool_bar(nullptr),
	_tool_bar_left(nullptr),
	_status_bar(nullptr),

	_file_menu(nullptr),
	_view_menu(nullptr),
	_tools_menu(nullptr),
	_help_menu(nullptr),

	_action_new_project(nullptr),
	_action_open_project(nullptr),
	_action_recent_projects(nullptr),
	_action_save_project(nullptr),
	_action_save_project_as(nullptr),
	_action_import(nullptr),
	_action_export(nullptr),
	_action_close(nullptr),
	_action_zoom_in(nullptr),
	_action_zoom_out(nullptr),
	_action_zoom_to_fit(nullptr),
	_action_pan_view(nullptr),
	_action_zoom_window(nullptr),
	_action_properties(nullptr),
	_action_select(nullptr),
	_action_move(nullptr),
	_action_rotate(nullptr),
	_action_scale(nullptr),
	_action_asset_store(nullptr),
	_action_user_guide_documentation(nullptr),
	_action_about(nullptr),
	_action_zoom_view(nullptr),
	_action_tile_placing_window(nullptr),
	_asset_store(nullptr),
	_properties_dockwidget(nullptr),
	_asset_store_widget_dock_contents(nullptr),
	_properties_widget_dock_contents(nullptr),
	_mainwindow_margins(0, 0, 0, 0),
	
	_translationXSpinBox(nullptr),
	_translationYSpinBox(nullptr),
	_translationZSpinBox(nullptr),
	_rotationXSpinBox(nullptr),
	_rotationYSpinBox(nullptr),
	_rotationZSpinBox(nullptr),
	_scaleXSpinBox(nullptr),
	_scaleYSpinBox(nullptr),
	_scaleZSpinBox(nullptr),
	_deleteEntityButton(nullptr),

	_linearSpeedSpinBox(nullptr),
	_lookSpeedSpinBox(nullptr),
	_accelerationSpinBox(nullptr),
	_decelerationSpinBox(nullptr),
	
	_linearSpeedSlider(nullptr),
	_lookSpeedSlider(nullptr),
	_accelerationSlider(nullptr),
	_decelerationSlider(nullptr),

	_selectedEntity(nullptr),
	_selectedTransform(nullptr),

	_lightColorDialog(nullptr),
	_lightIntensitySpinBox(nullptr),
	_lightPosXSpinBox(nullptr),
	_lightPosYSpinBox(nullptr),
	_lightPosZSpinBox(nullptr),

	treeView(nullptr),
	model(nullptr),

	renderWindow(nullptr),
	parser(nullptr)



{
	// Constructor code
	// main window settings
	_mainwindow_id = QUuid::createUuid();
	//_mainwindow_central_widget = std::make_unique<QWidget>(this);
	//_renderwindow = std::make_unique<AID::RenderWindow>();
	//_mainwindow_central_widget = new AID::RenderWindow(this);
	//setCentralWidget(_mainwindow_central_widget);

	renderWindow = new RenderWindow(this); // Instantiate RenderWindow

	QTextEdit* textEditWidget = new QTextEdit(this);
	//setCentralWidget(textEditWidget);

	setCentralWidget(renderWindow); // Set it as the central widget of the MainWindow


	parser = new Parser(this); // Instantiate Parser

	// Constructing mainwindow components
	_asset_store = new QDockWidget("Asset Store", this);
	_tool_bar = new QToolBar("Horizontal Tool Bar", this);
	_menu_bar = new QMenuBar(this);
	_tool_bar_left = new QToolBar("Vertical Tool Bar", this);
	_status_bar = new QStatusBar(this);
	_action_new_project = new QAction(tr("&New Project"), this);
	_action_open_project = new QAction(tr("&Open Project"), this);
	_action_recent_projects = new QAction(tr("&Recent Project"), this);
	_action_save_project = new QAction(tr("&Save Project"), this);
	_action_save_project_as = new QAction(tr("Save Project &As"), this);
	_action_import = new QAction(tr("&Import"), this);
	_action_export = new QAction(tr("E&xport"), this);
	_action_close = new QAction(tr("&Close"), this);
	_action_zoom_in = new QAction(tr("&Zoom In"), this);
	_action_zoom_out = new QAction(tr("Zoom &Out"), this);
	_action_zoom_to_fit = new QAction(tr("Zoom To &Fit"), this);
	_action_pan_view = new QAction(tr("&Pan View"), this);
	_action_zoom_window = new QAction(tr("Zoom &Window"), this);
	_action_properties = new QAction(tr("P&roperties Window"), this);
	_action_select = new QAction(tr("&Select"), this);
	_action_move = new QAction(tr("&Move"), this);
	_action_rotate = new QAction(tr("&Rotate"), this);
	_action_scale = new QAction(tr("S&cale"), this);
	_action_asset_store = new QAction(tr("&Asset Store"), this);
	_action_user_guide_documentation = new QAction(tr("&User Guide/Documentation"), this);
	_action_about = new QAction(tr("&About"), this);
	_action_zoom_view = new QAction(tr("Zoom Vie&w"), this);
	_action_tile_placing_window = new QAction(tr("&Tile Placing Window"), this);
	_asset_store_widget_dock_contents = new QWidget(_asset_store);
	
	_properties_dockwidget = new QDockWidget("Properties Window", this);
	_properties_widget_dock_contents = new QScrollArea(_properties_dockwidget);

	
	_tile_placing_dialog = new QDialog(this);

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
}

AID::MainWindow::~MainWindow()
{
}

// Initialization
void AID::MainWindow::initialize()
{
	// Initialize mainwindow
	setWindowTitle(_default_mainwindow_data.WINDOW_TITLE);
	resize(_default_mainwindow_data.WINDOW_WIDTH, _default_mainwindow_data.WINDOW_HEIGHT);
	set_mainwindow_palette();

	// Initialize mainwindow components
	set_mainwindow_actions();
	set_mainwindow_icons();
	set_mainwindow_menubar();
	set_mainwindow_toolbar();
	set_mainwindow_toolbar_left();
	set_mainwindow_statusbar();
	set_mainwindow_connections();
	set_treeviewlist_dockwidget();
	set_asset_store_dockwidget();
	set_properties_dockwidget();

	createPropertiesWidget();
	set_properties_dockwidget();

}

void AID::MainWindow::show()
{
	showMaximized();
}

void AID::MainWindow::shutdown()
{
	qDebug() << "Shutting down main window.";
	_renderwindow.reset();
	//_mainwindow_central_widget.reset();
	//_mainwindow_layout.reset();
}

void AID::MainWindow::set_mainwindow_palette()
{
	_mainwindow_palette.setColor(QPalette::Window, QColor(200, 200, 200));
	setPalette(_mainwindow_palette);
}

void AID::MainWindow::set_mainwindow_layout()
{
	_mainwindow_layout = new QVBoxLayout(_mainwindow_central_widget);
}

void AID::MainWindow::set_mainwindow_margins()
{
	_mainwindow_margins = _mainwindow_layout->contentsMargins();
}

void AID::MainWindow::set_mainwindow_central_widget()
{
	_mainwindow_layout->addWidget(_renderwindow.get());
}

void AID::MainWindow::set_mainwindow_menubar()
{
	// Add menu bar
	
	_menu_bar->setStyleSheet(QString::fromUtf8("QMenuBar {background: rgb(27, 36, 50); color: rgb(255, 255, 255)}"));
	_menu_bar->setContextMenuPolicy(Qt::ActionsContextMenu);
	// Add menus to the menu bar
	_file_menu = _menu_bar->addMenu(tr("&File"));
	_view_menu = _menu_bar->addMenu(tr("&View"));
	_tools_menu = _menu_bar->addMenu(tr("&Tools"));
	_help_menu = _menu_bar->addMenu(tr("&Help"));

	_file_menu->addAction(_action_new_project);
	_file_menu->addAction(_action_open_project);
	_file_menu->addAction(_action_recent_projects);
	_file_menu->addAction(_action_save_project);
	_file_menu->addAction(_action_save_project_as);
	_file_menu->addAction(_action_import);
	_file_menu->addAction(_action_export);
	_file_menu->addAction(_action_close);

	_view_menu->addAction(_action_zoom_in);
	_view_menu->addAction(_action_zoom_out);
	_view_menu->addAction(_action_zoom_to_fit);
	_view_menu->addAction(_action_pan_view);
	_view_menu->addAction(_action_zoom_window);
	_view_menu->addAction(_action_properties);

	_tools_menu->addAction(_action_select);
	_tools_menu->addAction(_action_move);
	//_tools_menu->addAction(_action_toggle_renderer);
	_tools_menu->addAction(_action_rotate);
	_tools_menu->addAction(_action_scale);
	_tools_menu->addAction(_action_asset_store);
	_tools_menu->addAction(_action_tile_placing_window);

	_help_menu->addAction(_action_user_guide_documentation);
	_help_menu->addAction(_action_about);

	setMenuBar(_menu_bar);
	qDebug() << "Menubar set";
}

void AID::MainWindow::set_mainwindow_toolbar()
{
	// Add toolbar
	
	_tool_bar->setAutoFillBackground(false);
	_tool_bar->setStyleSheet(QString::fromUtf8("QToolBar {background: rgb(18, 20, 32)}"));
	_tool_bar->setMovable(true);
	_tool_bar->setIconSize(QSize(32, 32));
	_tool_bar->setFloatable(true);
	_tool_bar->setContextMenuPolicy(Qt::ActionsContextMenu);
	// Add actions to the toolbar
	_tool_bar->addAction(_action_open_project);
	_tool_bar->addAction(_action_save_project);
	_tool_bar->addAction(_action_save_project_as);
	_tool_bar->addAction(_action_zoom_in);
	_tool_bar->addAction(_action_zoom_out);
	_tool_bar->addSeparator();
	_tool_bar->addAction(_action_zoom_view);
	_tool_bar->addAction(_action_pan_view);
	_tool_bar->addSeparator();
	_tool_bar->addAction(_action_tile_placing_window);

	addToolBar(_tool_bar);
	qDebug() << "Main Toolbar added";
}

void AID::MainWindow::set_mainwindow_toolbar_left()
{
	
	_tool_bar_left->setObjectName("toolBarLeft");
	_tool_bar_left->setStyleSheet(QString::fromUtf8("QToolBar {background: rgb(27, 36, 50)}"));
	_tool_bar_left->setMovable(true);
	_tool_bar->setFloatable(true);
	_tool_bar_left->setIconSize(QSize(32, 32));
	addToolBar(Qt::LeftToolBarArea, _tool_bar_left);
	_tool_bar_left->addAction(_action_asset_store);
	_tool_bar_left->setContextMenuPolicy(Qt::ActionsContextMenu);
	qDebug() << "Left toolbar created";
}

void AID::MainWindow::set_mainwindow_statusbar()
{
	
	_status_bar->setStyleSheet(QString::fromUtf8("QStatusBar {background: rgb(30, 40, 50); color: rgb(255, 255, 255)}"));
	setStatusBar(_status_bar);
}

void AID::MainWindow::set_mainwindow_icons()
{
	QList<QByteArray> supported_image_list = QImageReader::supportedImageFormats();
	
	_open_project_icon = QIcon(":/icons/folder-open.svg");
	_action_open_project->setIcon(_open_project_icon);
	
	_recent_projects_icon.addFile(QString::fromUtf8(":/icons/file.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_recent_projects->setIcon(_recent_projects_icon);

	_save_icon.addFile(QString::fromUtf8(":/icons/save.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_save_project->setIcon(_save_icon);

	_save_project_as_icon.addFile(QString::fromUtf8(":/icons/save-as.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_save_project_as->setIcon(_save_project_as_icon);

	_zoom_in_icon.addFile(QString::fromUtf8(":/icons/zoom-in.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_zoom_in->setIcon(_zoom_in_icon);

	_zoom_out_icon.addFile(QString::fromUtf8(":/icons/zoom-out.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_zoom_out->setIcon(_zoom_out_icon);

	_pan_view_icon.addFile(QString::fromUtf8(":/icons/pan_view.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_pan_view->setIcon(_pan_view_icon);

	_asset_store_icon.addFile(QString::fromUtf8(":/icons/asset_store.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_asset_store->setIcon(_asset_store_icon);

	_zoom_view_icon.addFile(QString::fromUtf8(":/icons/zoom_view.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_zoom_view->setIcon(_zoom_view_icon);

	_tile_placing_window_icon.addFile(QString::fromUtf8(":/icons/tile_placing_window.svg"), QSize(), QIcon::Normal, QIcon::Off);
	_action_tile_placing_window->setIcon(_tile_placing_window_icon);
	qDebug() << "Icons created";
}

void AID::MainWindow::set_mainwindow_actions()
{
	// Create actions and their respective hints
	_action_new_project->setStatusTip(tr("Create new project"));
	_action_new_project->setToolTip(tr("New Project"));
	_action_open_project->setStatusTip(tr("Open an existing project"));
	_action_open_project->setToolTip(tr("Open Project"));
	_action_recent_projects->setToolTip(tr("Recent Project"));
	_action_recent_projects->setStatusTip(tr("View recent projects"));
	_action_save_project->setToolTip(tr("Save Project"));
	_action_save_project->setStatusTip(tr("Save the current project"));
	_action_save_project_as->setToolTip(tr("Save Project As"));
	_action_save_project_as->setStatusTip(tr("Save the current project with a new name"));
	_action_import->setToolTip(tr("Import"));
	_action_import->setStatusTip(tr("Import assets"));
	_action_export->setToolTip(tr("Export"));
	_action_export->setStatusTip(tr("Export assets"));
	_action_close->setToolTip(tr("Close"));
	_action_close->setStatusTip(tr("Close application"));
	_action_zoom_in->setToolTip(tr("Zoom In"));
	_action_zoom_in->setStatusTip(tr("Zoom in on the view"));
	_action_zoom_out->setToolTip(tr("Zoom Out"));
	_action_zoom_out->setStatusTip(tr("Zoom out on the view"));
	_action_zoom_to_fit->setToolTip(tr("Zoom To Fit"));
	_action_zoom_to_fit->setStatusTip(tr("Zoom the view to fit the window"));
	_action_pan_view->setToolTip(tr("Pan View"));
	_action_pan_view->setStatusTip(tr("Activate pan tool"));
	_action_zoom_window->setToolTip(tr("Zoom Window"));
	_action_zoom_window->setStatusTip(tr("Zoom to a specific object"));
	_action_properties->setToolTip(tr("Properties Window"));
	_action_properties->setStatusTip(tr("Open Properties Window"));
	_action_select->setToolTip(tr("Select"));
	_action_select->setStatusTip(tr("Select object(s)"));
	_action_move->setToolTip(tr("Move"));
	_action_move->setStatusTip(tr("Move selected object(s)"));
	_action_rotate->setToolTip(tr("Rotate"));
	_action_rotate->setStatusTip(tr("Rotate selected object(s)"));
	_action_scale->setToolTip(tr("Scale"));
	_action_scale->setStatusTip(tr("Scale selected object(s)"));
	_action_asset_store->setToolTip(tr("Asset Store"));
	_action_asset_store->setStatusTip(tr("Access the asset store for additional resources"));
	_action_user_guide_documentation->setToolTip(tr("User Guide"));
	_action_user_guide_documentation->setStatusTip(tr("Open the user guide"));
	_action_about->setToolTip(tr("About"));
	_action_about->setStatusTip(tr("Information about the application"));
	_action_zoom_view->setToolTip(tr("Zoom View"));
	_action_zoom_view->setStatusTip(tr("Activate zoom tool"));
	_action_tile_placing_window->setToolTip(tr("Tile Placing Window"));
	_action_tile_placing_window->setStatusTip(tr("Place holder icon, will be removed later"));

	//_action_toggle_renderer->setToolTip(tr("Toggle Rendering mode"));
	//_action_toggle_renderer->setStatusTip(tr("Change between QMesh and QGeometry Renderer"));
	//_action_toggle_renderer->setCheckable(true);

	qDebug() << "Actions created";
}

void AID::MainWindow::set_mainwindow_connections()
{
	// Create connections between signals and slots
	connect(_action_tile_placing_window, &QAction::triggered, this, &MainWindow::set_tile_placing_window);
	//connect(_action_asset_store, &QAction::triggered, this, &MainWindow::set_asset_store_dialog);
	connect(_action_new_project, &QAction::triggered, this, &MainWindow::on_new_project_clicked);
	connect(_action_open_project, &QAction::triggered, this, &MainWindow::on_open_project_clicked);
	connect(_action_recent_projects, &QAction::triggered, this, &MainWindow::on_recent_projects_clicked);
	connect(_action_save_project, &QAction::triggered, this, &MainWindow::on_save_project_clicked);
	connect(_action_save_project_as, &QAction::triggered, this, &MainWindow::on_save_project_as_clicked);
	connect(_action_import, &QAction::triggered, this, &MainWindow::on_import_clicked);
	connect(_action_export, &QAction::triggered, this, &MainWindow::on_export_clicked);
	connect(_action_close, &QAction::triggered, this, &MainWindow::on_close_clicked);
	connect(_action_zoom_in, &QAction::triggered, this, &MainWindow::on_zoom_in_clicked);
	connect(_action_zoom_out, &QAction::triggered, this, &MainWindow::on_zoom_out_clicked);
	connect(_action_zoom_to_fit, &QAction::triggered, this, &MainWindow::on_zoom_to_fit_clicked);
	connect(_action_pan_view, &QAction::triggered, this, &MainWindow::on_pan_view_clicked);
	connect(_action_zoom_window, &QAction::triggered, this, &MainWindow::on_zoom_window_clicked);
	connect(_action_properties, &QAction::triggered, this, &MainWindow::on_properties_clicked);
	connect(_action_select, &QAction::triggered, this, &MainWindow::on_select_clicked);
	connect(_action_move, &QAction::triggered, this, &MainWindow::on_move_clicked);
	
	connect(_action_rotate, &QAction::triggered, this, &MainWindow::on_rotate_clicked);
	connect(_action_scale, &QAction::triggered, this, &MainWindow::on_scale_clicked);
	connect(_action_zoom_view, &QAction::triggered, this, &MainWindow::on_zoom_view_clicked);
	connect(_action_asset_store, &QAction::triggered, this, &MainWindow::on_asset_store_clicked);
	connect(_action_user_guide_documentation, &QAction::triggered, this, &MainWindow::on_user_guide_documentation_clicked);
	connect(_action_about, &QAction::triggered, this, &MainWindow::on_about_clicked);

	connect(&Mediator::getInstance(), &Mediator::meshLoaded, this, &MainWindow::onMeshLoaded);

	connect(&Mediator::getInstance(), &Mediator::entitySelected, this, &MainWindow::onEntitySelected);

	connect(&Mediator::getInstance(), &Mediator::hidePropertiesWindow, this, &MainWindow::onHidePropertiesWindow);

	connect(&Mediator::getInstance(), &Mediator::cameraSettingsChanged, this, &MainWindow::onCameraSettingsChanged);

	// These signals were potential loopbacks, need more test
	//connect(&Mediator::getInstance(), &Mediator::lightColorChanged, this, &MainWindow::onLightColorChanged);
	//connect(&Mediator::getInstance(), &Mediator::lightIntensityChanged, this, &MainWindow::onLightIntensityChanged);
	//connect(&Mediator::getInstance(), &Mediator::lightPositionChanged, this, &MainWindow::onLightPositionChanged);


	qDebug() << "Connections set";
}

void AID::MainWindow::on_import_clicked()
{
	qDebug() << "Import menu clicked";

	QDir homePath = QDir::home();
	QString documentPath = homePath.absolutePath() + "/Documents";
	QString importname = QFileDialog::getOpenFileName(this,
		tr("Import assets"), documentPath, tr("3D Files (*.xobj3d *.obj)"));
	qDebug() << importname;

	
	_status_bar->showMessage("Selected asset imported");
	emit Mediator::getInstance().readFile(importname, true);
}



void AID::MainWindow::on_open_project_clicked()
{
	qDebug() << "Open project menu clicked";
	_status_bar->showMessage("Open project menu clicked");

	QDir homePath = QDir::home();
	QString documentPath = homePath.absolutePath() + "/Documents";
	QString importname = QFileDialog::getOpenFileName(this,
		tr("Open Project"), documentPath, tr("AID Project (*.xobj3d)"));
	qDebug() << importname;

	_status_bar->showMessage("Project opened");
	emit Mediator::getInstance().readFile(importname, false);
}


void AID::MainWindow::set_asset_store_dockwidget()
{
	_asset_store->setAllowedAreas(Qt::BottomDockWidgetArea);
	_asset_store->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	//_asset_store_widget_dock_contents->setStyleSheet(QString::fromUtf8("QWidget {background: rgb(255, 255, 255); border: 1px solid black;}"));
	_asset_store->setContentsMargins(0, 0, 0, 0);
	
	_asset_store->hide();
	_asset_store->setWindowTitle("Asset Store");
	_asset_store->setMinimumSize(500, 300);
	_asset_store->setStyleSheet(dockWidgetStylesheet);
	_asset_store->setContextMenuPolicy(Qt::ActionsContextMenu);

	QWidget* multiWidget = new QWidget();
	QHBoxLayout* layout = new QHBoxLayout();
	QListWidget* listWidget = new QListWidget(_asset_store);
	listWidget->setContentsMargins(0, 0, 0, 0);

	listWidget->setViewMode(QListView::IconMode);

	QString itemPath = ":/interiorequipmentobjects/"; 
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

		QString objPath = ":/interiorequipmentobjects"; 
		QString objFileName = extenstionLessPath + ".xobj3d";
		qDebug() << "objFileName: " << objFileName;

		connect(listWidget, &QListWidget::itemDoubleClicked, this, [this, objPath, objFileName](QListWidgetItem* item) {
			if (item->text() == QFileInfo(objFileName).completeBaseName()) {
				qDebug() << objFileName << "Added to scene";
				emit Mediator::getInstance().readFile(objFileName, true);
			}
			});
		connect(listWidget, &QListWidget::itemClicked, this, [this, objPath, objFileName](QListWidgetItem* item) {
			if (item->text() == QFileInfo(objFileName).completeBaseName()) {
				qDebug() << objFileName << "clicked";
			}
			});
	}


	multiWidget->setLayout(layout);
	layout->addWidget(listWidget); 

	_asset_store->setWidget(multiWidget);
	addDockWidget(Qt::BottomDockWidgetArea, _asset_store);

	qDebug() << "Asset store dock widget set";
}

void AID::MainWindow::set_treeviewlist_dockwidget() {
	QDockWidget* dock = new QDockWidget("Objects Tree View", this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	treeView = new QTreeView(dock);
	model = new QStandardItemModel(treeView);
	model->setColumnCount(1);
	model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
	treeView->setModel(model);

	dock->setWidget(treeView);
	dock->setStyleSheet(dockWidgetStylesheet);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	connect(&Mediator::getInstance(), &Mediator::childrenUpdated, this, &MainWindow::updateObjectTree);
}

void AID::MainWindow::updateObjectTree(const QStringList& childrenIDs) {
	model->clear(); 

	for (const QString& entityID : childrenIDs) {
		if (entityID != "Entity")
		{
			QStandardItem* item = new QStandardItem();
			item->setText(entityID);
			model->appendRow(item);
			item->setEditable(false);
			// storing the entity ID using setData
			item->setData(entityID, Qt::UserRole);
		}
		
	}

}

void AID::MainWindow::createPropertiesWidget()
{
	//parentWidget = new QScrollArea();
	_properties_widget_dock_contents->setWidgetResizable(true);
	_properties_widget_dock_contents->setContentsMargins(10, 10, 10, 10);

	QFormLayout* layout = new QFormLayout();

	QString formLayoutStylesheet = R"(
        QFormLayout {
            background-color: #333;
            margin: 5px;
        }
        QLabel {
            color: #111;
            font-weight: bold;
        }
        QDoubleSpinBox {
            padding: 2px;
            border: 1px solid #555;
            border-radius: 4px;
            background-color: #222;
            color: #DDD;
        }
        QDoubleSpinBox::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 16px;
            border-left-width: 1px;
            border-left-color: darkgray;
            border-left-style: solid;
            border-top-right-radius: 3px;
        }
        QDoubleSpinBox::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 16px;
            border-top-width: 1px;
            border-top-color: darkgray;
            border-top-style: solid;
            border-bottom-right-radius: 3px;
        }
        QDoubleSpinBox::up-arrow {
            image: url(:/icons/caret-up.png);
            width: 10px;
            height: 10px;
        }
        QDoubleSpinBox::down-arrow {
            image: url(:/icons/caret-down.png);
            width: 10px;
            height: 10px;
        }
    )";

	layout->setContentsMargins(10, 10, 10, 10);
	layout->setSpacing(5);
	_properties_widget_dock_contents->setStyleSheet(formLayoutStylesheet);


	_translationXSpinBox = new QDoubleSpinBox();
	_translationYSpinBox = new QDoubleSpinBox();
	_translationZSpinBox = new QDoubleSpinBox();
	_rotationXSpinBox = new QDoubleSpinBox();
	_rotationYSpinBox = new QDoubleSpinBox();
	_rotationZSpinBox = new QDoubleSpinBox();
	_scaleXSpinBox = new QDoubleSpinBox();
	_scaleYSpinBox = new QDoubleSpinBox();
	_scaleZSpinBox = new QDoubleSpinBox();

	//_linearSpeedSpinBox = new QDoubleSpinBox();
	//_lookSpeedSpinBox = new QDoubleSpinBox();
	//_accelerationSpinBox = new QDoubleSpinBox();
	//_decelerationSpinBox = new QDoubleSpinBox();
	//_deleteEntityButton = new QPushButton("Delete Entity");
	

	_linearSpeedSlider  = new QSlider(Qt::Horizontal);
	_lookSpeedSlider	= new QSlider(Qt::Horizontal);
	_accelerationSlider = new QSlider(Qt::Horizontal);
	_decelerationSlider = new QSlider(Qt::Horizontal);



	_lightColorDialog = new QColorDialog();
	_lightIntensitySpinBox = new QDoubleSpinBox();
	_lightPosXSpinBox = new QDoubleSpinBox();
	_lightPosYSpinBox = new QDoubleSpinBox();
	_lightPosZSpinBox = new QDoubleSpinBox();


	layout->addRow(new QLabel("Translation X:"), _translationXSpinBox);
	layout->addRow(new QLabel("Translation Y:"), _translationYSpinBox);
	layout->addRow(new QLabel("Translation Z:"), _translationZSpinBox);
	layout->addRow(new QLabel("Rotation X:"), _rotationXSpinBox);
	layout->addRow(new QLabel("Rotation Y:"), _rotationYSpinBox);
	layout->addRow(new QLabel("Rotation Z:"), _rotationZSpinBox);
	layout->addRow(new QLabel("Scale X:"), _scaleXSpinBox);
	layout->addRow(new QLabel("Scale Y:"), _scaleYSpinBox);
	layout->addRow(new QLabel("Scale Z:"), _scaleZSpinBox);
	layout->addRow(new QLabel("Camera Settings"));
	layout->addRow(new QLabel("Linear Speed:"), _linearSpeedSlider);
	layout->addRow(new QLabel("Look up Speed:"), _lookSpeedSlider);
	layout->addRow(new QLabel("Acceleration:"), _accelerationSlider);
	layout->addRow(new QLabel("Deceleration:"), _decelerationSlider);
	layout->addRow(new QLabel("Light Settings"));
	//layout->addRow(new QLabel("Color:"), _lightColorDialog);
	layout->addRow(new QLabel("Intensity:"), _lightIntensitySpinBox);
	layout->addRow(new QLabel("Translation X:"), _lightPosXSpinBox);
	layout->addRow(new QLabel("Translation Y:"), _lightPosYSpinBox);
	layout->addRow(new QLabel("Translation Z:"), _lightPosZSpinBox);

	//layout->addRow(_deleteEntityButton);

	// Translation settings
	_translationXSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_translationYSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_translationZSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_rotationXSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_rotationYSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_rotationZSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_scaleXSpinBox->setMinimum(0);
	_scaleYSpinBox->setMinimum(0);
	_scaleZSpinBox->setMinimum(0);

	_translationXSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_translationYSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_translationZSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_rotationXSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_rotationYSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_rotationZSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_scaleXSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_scaleYSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_scaleZSpinBox->setMaximum(std::numeric_limits<double>::infinity());

	_translationXSpinBox->setValue(0.0);
	_translationYSpinBox->setValue(0.0);
	_translationZSpinBox->setValue(0.0);
	_rotationXSpinBox->setValue(0.0);
	_rotationYSpinBox->setValue(0.0);
	_rotationZSpinBox->setValue(0.0);
	_scaleXSpinBox->setValue(1.0);
	_scaleYSpinBox->setValue(1.0);
	_scaleZSpinBox->setValue(1.0);

	//_linearSpeedSpinBox->setMinimum(10);
	//_lookSpeedSpinBox->setMinimum(10);
	//_accelerationSpinBox->setMinimum(10);
	//_decelerationSpinBox->setMinimum(10);
	
	// Camera settings
	_linearSpeedSlider->setRange(0, 1000);
	_lookSpeedSlider->setRange(0, 1000);
	_accelerationSlider->setRange(0, 1000);
	_decelerationSlider->setRange(0, 1000);

	_linearSpeedSlider->setTickPosition(QSlider::TicksBothSides);
	_lookSpeedSlider->setTickPosition(QSlider::TicksBothSides);
	_accelerationSlider->setTickPosition(QSlider::TicksBothSides);
	_decelerationSlider->setTickPosition(QSlider::TicksBothSides);

	_linearSpeedSlider->setTickInterval(100);
	_lookSpeedSlider->setTickInterval(100);
	_accelerationSlider->setTickInterval(100);
	_decelerationSlider->setTickInterval(100);



	// Light settings
	_lightIntensitySpinBox->setMinimum(0.0);
	_lightPosXSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_lightPosYSpinBox->setMinimum(std::numeric_limits<double>::lowest());
	_lightPosZSpinBox->setMinimum(std::numeric_limits<double>::lowest());

	_lightIntensitySpinBox->setMaximum(10.0);
	_lightIntensitySpinBox->setSingleStep(0.05);
	_lightPosXSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_lightPosYSpinBox->setMaximum(std::numeric_limits<double>::infinity());
	_lightPosZSpinBox->setMaximum(std::numeric_limits<double>::infinity());



	_properties_widget_dock_contents->setLayout(layout);

	connect(_translationXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onTranslationChanged()));
	connect(_translationYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onTranslationChanged()));
	connect(_translationZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onTranslationChanged()));
	connect(_rotationXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onRotationChanged()));
	connect(_rotationYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onRotationChanged()));
	connect(_rotationZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onRotationChanged()));
	connect(_scaleXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onScaleChanged()));
	connect(_scaleYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onScaleChanged()));
	connect(_scaleZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onScaleChanged()));

	connect(_linearSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(onLinearSpeedChanged(int)));
	connect(_lookSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(onLookSpeedChanged(int)));
	connect(_accelerationSlider, SIGNAL(valueChanged(int)), this, SLOT(onAccelerationChanged(int)));
	connect(_decelerationSlider, SIGNAL(valueChanged(int)), this, SLOT(onDecelerationChanged(int)));

	connect(_lightColorDialog, &QColorDialog::currentColorChanged, this, &MainWindow::onLightColorChanged);
	connect(_lightIntensitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(onLightIntensityChanged(double)));
	connect(_lightPosXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onLightPositionXChanged(double)));
	connect(_lightPosYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onLightPositionYChanged(double)));
	connect(_lightPosZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onLightPositionZChanged(double)));


	//connect(_deleteEntityButton, SIGNAL(clicked()), this, SLOT(onDeleteEntityClicked()));

	_linearSpeedSlider->setValue(500);
	_lookSpeedSlider->setValue(500);
	_accelerationSlider->setValue(500);
	_decelerationSlider->setValue(10);

	_lightIntensitySpinBox->setValue(0.6);
	_lightPosXSpinBox->setValue(325.0);
	_lightPosYSpinBox->setValue(100.0);
	_lightPosZSpinBox->setValue(134.0);
}

void AID::MainWindow::onCameraSettingsChanged(float linearSpeed, float lookSpeed, float acceleration, float deceleration)
{
	_linearSpeedSlider->setValue(linearSpeed);
	_lookSpeedSlider->setValue(lookSpeed);
	_accelerationSlider->setValue(acceleration);
	_decelerationSlider->setValue(deceleration);
}

void AID::MainWindow::onLightColorChanged(const QColor& color)
{
	//_lightColorDialog->setCurrentColor(color);
	emit Mediator::getInstance().lightColorChanged(color);
}

void AID::MainWindow::onLightIntensityChanged(double intensity)
{
	//_lightIntensitySpinBox->setValue(intensity);
	//qDebug() << "From MainWindow, lightIntensityChanged signal is emitted";
	emit Mediator::getInstance().lightIntensityChanged(intensity);

}

void AID::MainWindow::onLightPositionXChanged(double x)
{
	QVector3D position(x, _lightPosYSpinBox->value(), _lightPosZSpinBox->value());
	emit Mediator::getInstance().lightPositionChanged(position);
}

void AID::MainWindow::onLightPositionYChanged(double y)
{
	QVector3D position(_lightPosXSpinBox->value(), y, _lightPosZSpinBox->value());
	emit Mediator::getInstance().lightPositionChanged(position);
}

void AID::MainWindow::onLightPositionZChanged(double z)
{
    QVector3D position(_lightPosXSpinBox->value(), _lightPosYSpinBox->value(), z);
    emit Mediator::getInstance().lightPositionChanged(position);
}

void AID::MainWindow::onLightPositionChanged(const QVector3D& position)
{
	_lightPosXSpinBox->setValue(position.x());
	_lightPosYSpinBox->setValue(position.y());
	_lightPosZSpinBox->setValue(position.z());
}


void AID::MainWindow::onLinearSpeedChanged(int value)
{
	float linearSpeed = static_cast<float>(value);
	emit Mediator::getInstance().cameraLinearSpeedChanged(linearSpeed);
	//emit Mediator::getInstance().cameraLinearSpeedChanged(static_cast<float>(value));
}

void AID::MainWindow::onLookSpeedChanged(int value)
{
	float lookSpeed = static_cast<float>(value);
	emit Mediator::getInstance().cameraLookSpeedChanged(lookSpeed);
	//emit Mediator::getInstance().cameraLookSpeedChanged(static_cast<float>(value));
}

void AID::MainWindow::onAccelerationChanged(int value)
{
	float acceleration = static_cast<float>(value);
	emit Mediator::getInstance().cameraAccelerationChanged(acceleration);
	//emit Mediator::getInstance().cameraAccelerationChanged(static_cast<float>(value));
}

void AID::MainWindow::onDecelerationChanged(int value)
{
	float deceleration = static_cast<float>(value);
	emit Mediator::getInstance().cameraDecelerationChanged(deceleration);
	//emit Mediator::getInstance().cameraDecelerationChanged(static_cast<float>(value));
}

void AID::MainWindow::onHidePropertiesWindow()
{
	if (_properties_dockwidget)
	{
		_properties_dockwidget->hide();
	}
}

void AID::MainWindow::onEntitySelected(Qt3DCore::QEntity* entity)
{
	blockSpinBoxSignals(true); // Blocking signals to prevent recursive updates, hacky
	
	_selectedEntity = entity;
	
	Qt3DCore::QEntity* currentEntity = entity;
	_selectedTransform = nullptr;
	while (currentEntity && !_selectedTransform) {
		QList< Qt3DCore::QTransform*> transforms = currentEntity->componentsOfType<Qt3DCore::QTransform>();
		if (!transforms.isEmpty()) {
			_selectedTransform = transforms.first();
		}
		else {
			currentEntity = currentEntity->parentEntity();
		}
	}

	if (!_selectedTransform) {
		qDebug() << "Transform not set to _selectedEntity";
	}


	updateProperties(_selectedTransform);
	
	if (_properties_dockwidget->isHidden())
		_properties_dockwidget->show();

	blockSpinBoxSignals(false); // Unblocking signals after updating

	connectTransformSignals();

}

void AID::MainWindow::updateProperties(Qt3DCore::QTransform* transform)
{
	//if (!entity) return;

	//Qt3DCore::QTransform* transform = entity->findChild<Qt3DCore::QTransform*>();
	if (transform)
	{
		_translationXSpinBox->setValue(transform->translation().x());
		_translationYSpinBox->setValue(transform->translation().y());
		_translationZSpinBox->setValue(transform->translation().z());

		_rotationXSpinBox->setValue(transform->rotationX());
		_rotationYSpinBox->setValue(transform->rotationY());
		_rotationZSpinBox->setValue(transform->rotationZ());

		_scaleXSpinBox->setValue(transform->scale3D().x());
		_scaleYSpinBox->setValue(transform->scale3D().y());
		_scaleZSpinBox->setValue(transform->scale3D().z());
	}
}

void AID::MainWindow::connectTransformSignals()
{
	if (_selectedTransform)
	{
		// Disconnect any previous connections, this is hacky as well
		disconnect(_selectedTransform, nullptr, this, nullptr);

		connect(_selectedTransform, &Qt3DCore::QTransform::translationChanged, this, &MainWindow::onTransformChanged);
		connect(_selectedTransform, &Qt3DCore::QTransform::rotationChanged, this, &MainWindow::onTransformChanged);
		connect(_selectedTransform, &Qt3DCore::QTransform::scale3DChanged, this, &MainWindow::onTransformChanged);
	}
}

void AID::MainWindow::onTranslationChanged()
{
	if (_selectedTransform)
	{
		QVector3D translation(_translationXSpinBox->value(), _translationYSpinBox->value(), _translationZSpinBox->value());
		_selectedTransform->setTranslation(translation);
		qDebug() << _translationXSpinBox->value() << _translationYSpinBox->value() << _translationZSpinBox->value();
		updateProperties(_selectedTransform);

	}
}

void AID::MainWindow::onRotationChanged()
{
	if (_selectedTransform)
	{
		_selectedTransform->setRotationX(_rotationXSpinBox->value());
		_selectedTransform->setRotationY(_rotationYSpinBox->value());
		_selectedTransform->setRotationZ(_rotationZSpinBox->value());
		//updateProperties(_selectedTransform);

	}
}

void AID::MainWindow::onScaleChanged()
{
	if (_selectedTransform)
	{
		QVector3D scale(_scaleXSpinBox->value(), _scaleYSpinBox->value(), _scaleZSpinBox->value());
		_selectedTransform->setScale3D(scale);
		//updateProperties(_selectedTransform);

	}
}

void AID::MainWindow::onTransformChanged()
{
	blockSpinBoxSignals(true); // Block signals to prevent recursive updates, needs more attention

	if (_selectedTransform)
	{
		_translationXSpinBox->setValue(_selectedTransform->translation().x());
		_translationYSpinBox->setValue(_selectedTransform->translation().y());
		_translationZSpinBox->setValue(_selectedTransform->translation().z());
		_rotationXSpinBox->setValue(_selectedTransform->rotationX());
		_rotationYSpinBox->setValue(_selectedTransform->rotationY());
		_rotationZSpinBox->setValue(_selectedTransform->rotationZ());
		_scaleXSpinBox->setValue(_selectedTransform->scale3D().x());
		_scaleYSpinBox->setValue(_selectedTransform->scale3D().y());
		_scaleZSpinBox->setValue(_selectedTransform->scale3D().z());
	}

	blockSpinBoxSignals(false); // Unblock signals after updating
}

void AID::MainWindow::blockSpinBoxSignals(bool block)
{
	_translationXSpinBox->blockSignals(block);
	_translationYSpinBox->blockSignals(block);
	_translationZSpinBox->blockSignals(block);
	_rotationXSpinBox->blockSignals(block);
	_rotationYSpinBox->blockSignals(block);
	_rotationZSpinBox->blockSignals(block);
	_scaleXSpinBox->blockSignals(block);
	_scaleYSpinBox->blockSignals(block);
	_scaleZSpinBox->blockSignals(block);
}

void AID::MainWindow::onDeleteEntityClicked()
{
	// Not being used now, needs changes
	if (_selectedEntity)
	{
		_selectedEntity->deleteLater();
		_selectedEntity = nullptr;
		_selectedTransform = nullptr;

	}
}


void AID::MainWindow::set_properties_dockwidget()
{
	// Create Properties windows dockwidget
	
	_properties_dockwidget->setAllowedAreas(Qt::RightDockWidgetArea);
	_properties_dockwidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	_properties_dockwidget->hide();
	_properties_widget_dock_contents->setStyleSheet(dockWidgetStylesheet);
	//_properties_widget_dock_contents->setStyleSheet(QString::fromUtf8("QWidget {background: rgb(255, 255, 255); border: 1px solid black;}"));
	_properties_dockwidget->setWidget(_properties_widget_dock_contents);

	addDockWidget(Qt::RightDockWidgetArea, _properties_dockwidget);
	qDebug() << "Properties dock widget set";
}

void AID::MainWindow::set_tile_placing_window()
{
  
	renderWindow->Call2DWindow();
	
	qDebug() << "Tile placing window set";
}