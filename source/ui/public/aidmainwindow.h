#pragma once
/*
@purpose
@params
@author
@date
*/

// cpp includes
#include <memory>

// qt includes
#include <QMainWindow>
#include <QWidget>
#include <QUuid>
#include <QDebug>
//#include <QVBoxLayout>
#include <QPalette>
//#include <QMargins>
#include <QtWidgets>

#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QDialog>
#include <QDockWidget>
#include <QTextEdit>
#include <QGraphicsView>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QImageReader>
#include <QFileInfo>
#include <QIcon>
#include <QList>
#include <QFileDialog>

#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>

#include <QMouseEvent>

// local includes
#include "aidcustdefines.h"
#include "aidrenderwindow.h"
#include "aidparser.h"



namespace AID
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		MainWindow(QWidget* parent = nullptr);
		~MainWindow();

		void initialize();
		void show();
		void shutdown();

	public:
		void set_mainwindow_palette();
		void set_mainwindow_layout();
		void set_mainwindow_margins();
		void set_mainwindow_central_widget();
		void set_mainwindow_actions();
		void set_mainwindow_icons();
		void set_mainwindow_menubar();
		void set_mainwindow_toolbar();
		void set_mainwindow_toolbar_left();
		void set_mainwindow_statusbar();
		void set_mainwindow_connections();

		void set_asset_store_dockwidget();
		void set_treeviewlist_dockwidget();
		
		void set_tile_placing_window();

		void set_properties_dockwidget();
		void updateProperties(Qt3DCore::QTransform* transform);

	private slots:
		void onEntitySelected(Qt3DCore::QEntity* entity);
		void onTranslationChanged();
		void onRotationChanged();
		void onScaleChanged();
		void onDeleteEntityClicked();
		void onCameraSettingsChanged(float linearSpeed, float lookSpeed, float acceleration, float deceleration);

		void onLightColorChanged(const QColor& color);
		void onLightIntensityChanged(double intensity);
		void onLightPositionXChanged(double x);

		void onLightPositionYChanged(double y);

		void onLightPositionZChanged(double z);
		void onLightPositionChanged(const QVector3D& position);
	
	private:
		void blockSpinBoxSignals(bool block);
		void createPropertiesWidget();
		void connectTransformSignals();
	
	protected:
		//void contextMenuEvent(QContextMenuEvent* event) override;
	public:
		inline QUuid get_mainwindow_id()
		{
			return _mainwindow_id;
		}
		inline QPalette get_mainwindow_palette()
		{
			return _mainwindow_palette;
		}
/*		inline QWidget* get_mainwindow_central_widget()
		{
			return _mainwindow_central_widget.get();
		}
*/
	signals:
		//void addMeshToScene(const QString& meshFilePath);  // Signal to add a mesh
		//void parseXml(const QString& importfilename); // Signal to parse xml

	public slots:
		void onTransformChanged();
		void onHidePropertiesWindow();
		void onLinearSpeedChanged(int value);
		void onLookSpeedChanged(int value);
		void onAccelerationChanged(int value);
		void onDecelerationChanged(int value);
		

		void on_new_project_clicked()
		{
			qDebug() << "New project menu clicked";
			_status_bar->showMessage("New project menu clicked");
		}
		void on_open_project_clicked();

		void on_recent_projects_clicked()
		{
			qDebug() << "Recent projects menu clicked";
			_status_bar->showMessage("Recent projects menu clicked");
		}
		void on_save_project_clicked()
		{
			qDebug() << "Save project menu clicked";
			_status_bar->showMessage("Save project menu clicked");
		}

		void on_save_project_as_clicked()
		{
			qDebug() << "Save project as menu clicked";
			_status_bar->showMessage("Save project as menu clicked");
		}

		void on_import_clicked();

		void onMeshLoaded(const QString& filePath) {
			// Slot to handle mesh loaded notification
			// TODO
			//qDebug() << "Mesh loaded from: " << filePath;
		}

		void on_export_clicked()
		{
			qDebug() << "Export menu clicked";
			_status_bar->showMessage("Export menu clicked");
		}

		void on_close_clicked()
		{
			qDebug() << "Close menu clicked";
			_status_bar->showMessage("Close menu clicked");
		}

		void on_zoom_in_clicked()
		{
			qDebug() << "Zoom in menu clicked";
			_status_bar->showMessage("Zoom in menu clicked");
		}

		void on_zoom_out_clicked()
		{
			qDebug() << "Zoom out menu clicked";
			_status_bar->showMessage("Zoom out menu clicked");
		}

		void on_zoom_to_fit_clicked()
		{
			qDebug() << "Zoom to fit menu clicked";
			_status_bar->showMessage("Zoom to fit menu clicked");
		}

		void on_pan_view_clicked()
		{
			qDebug() << "Pan view menu clicked";
			_status_bar->showMessage("Pan view menu clicked");
		}

		void on_zoom_window_clicked()
		{
			qDebug() << "Zoom window menu clicked";
			_status_bar->showMessage("Zoom window menu clicked");
		}

		void on_asset_store_clicked()
		{
			qDebug() << "Asset store menu clicked";
			_status_bar->showMessage("Asset store menu clicked");
			if (_asset_store->isHidden())
				_asset_store->show();
			else
				_asset_store->hide();
		}

		void on_properties_clicked()
		{
			qDebug() << "Properties window menu clicked";
			_status_bar->showMessage("Properties window menu clicked");
			if(_properties_dockwidget->isHidden())
				_properties_dockwidget->show();
			else
				_properties_dockwidget->hide();
		}

		void on_select_clicked()
		{
			qDebug() << "Select menu clicked";
			_status_bar->showMessage("Select menu clicked");
		}

		void on_move_clicked()
		{
			qDebug() << "Move menu clicked";
			_status_bar->showMessage("Move menu clicked");
		}

		void on_rotate_clicked()
		{
			qDebug() << "Rotate menu clicked";
			_status_bar->showMessage("Rotate menu clicked");
		}

		void on_scale_clicked()
		{
			qDebug() << "Scale menu clicked";
			_status_bar->showMessage("Scale menu clicked");
		}

		void on_user_guide_documentation_clicked()
		{
			qDebug() << "User guide/documentation menu clicked";
			_status_bar->showMessage("User guide/documentation menu clicked");
		}

		void on_about_clicked()
		{
			qDebug() << "About menu clicked";
			_status_bar->showMessage("About menu clicked");
		}
		void on_zoom_view_clicked()
		{
			qDebug() << "Zoom view menu clicked";
			_status_bar->showMessage("Zoom view menu clicked");
		}

		void updateObjectTree(const QStringList& childrenIDs);

	private:
		QUuid _mainwindow_id;
		QPalette _mainwindow_palette;
		QMargins _mainwindow_margins;

		QMenuBar* _menu_bar;
		QToolBar* _tool_bar;
		QToolBar* _tool_bar_left;
		QStatusBar* _status_bar;

		QAction* _action_new_project;
		QAction* _action_open_project;
		QAction* _action_recent_projects;
		QAction* _action_save_project;
		QAction* _action_save_project_as;
		QAction* _action_import;
		QAction* _action_export;
		QAction* _action_close;
		QAction* _action_zoom_in;
		QAction* _action_zoom_out;
		QAction* _action_zoom_to_fit;
		QAction* _action_pan_view;
		QAction* _action_zoom_window;
		QAction* _action_properties;
		QAction* _action_select;
		QAction* _action_move;
		QAction* _action_rotate;
		QAction* _action_scale;
		QAction* _action_asset_store;
		QAction* _action_user_guide_documentation;
		QAction* _action_about;
		QAction* _action_zoom_view;
		QAction* _action_tile_placing_window;

		QTreeView* treeView;
		QStandardItemModel* model;

		QMenu* _file_menu;
		QMenu* _view_menu;
		QMenu* _tools_menu;
		QMenu* _help_menu;

		QIcon _open_project_icon;
		QIcon _recent_projects_icon;
		QIcon _save_icon;
		QIcon _save_project_as_icon;
		QIcon _zoom_in_icon;
		QIcon _zoom_out_icon;
		QIcon _pan_view_icon;
		QIcon _asset_store_icon;
		QIcon _zoom_view_icon;
		QIcon _tile_placing_window_icon;

		QDockWidget* _asset_store;
		QWidget* _asset_store_widget_dock_contents;
		
		QDockWidget* _properties_dockwidget;
		//QWidget* _properties_widget_dock_contents;
		QScrollArea* _properties_widget_dock_contents;

		QDoubleSpinBox* _translationXSpinBox;
		QDoubleSpinBox* _translationYSpinBox;
		QDoubleSpinBox* _translationZSpinBox;
		QDoubleSpinBox* _rotationXSpinBox;
		QDoubleSpinBox* _rotationYSpinBox;
		QDoubleSpinBox* _rotationZSpinBox;
		QDoubleSpinBox* _scaleXSpinBox;
		QDoubleSpinBox* _scaleYSpinBox;
		QDoubleSpinBox* _scaleZSpinBox;
		QPushButton* _deleteEntityButton;

		QDoubleSpinBox* _linearSpeedSpinBox;
		QDoubleSpinBox* _lookSpeedSpinBox;
		QDoubleSpinBox* _accelerationSpinBox;
		QDoubleSpinBox* _decelerationSpinBox;

		QSlider* _linearSpeedSlider;
		QSlider* _lookSpeedSlider;
		QSlider* _accelerationSlider;
		QSlider* _decelerationSlider;

		Qt3DCore::QEntity* _selectedEntity;
		Qt3DCore::QTransform* _selectedTransform;

		QColorDialog* _lightColorDialog;
		QDoubleSpinBox* _lightIntensitySpinBox;
		QDoubleSpinBox* _lightPosXSpinBox;
		QDoubleSpinBox* _lightPosYSpinBox;
		QDoubleSpinBox* _lightPosZSpinBox;

		QDialog* _tile_placing_dialog;
		QString dockWidgetStylesheet;

		RenderWindow* renderWindow;
		Parser* parser;

		QWidget* _mainwindow_central_widget;
		QVBoxLayout* _mainwindow_layout;
		std::unique_ptr<QMainWindow> _mainwindow;
		std::unique_ptr<AID::RenderWindow> _renderwindow;
		DEF_WINDOW_DATA _default_mainwindow_data
		{
			"AID",
			1920,
			1080
		};
	};
}