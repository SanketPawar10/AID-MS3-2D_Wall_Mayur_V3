#include <iostream>

#include <QCoreApplication>
#include <QApplication>
#include <QStyleFactory>

#include "aidsession.h"

/*	
 * Intended Workflow

 1. Kindly navigate to the "File" menu and select "Import" to open either a .xobj3d or an .obj file.

 2. For illustrative purposes, a basic mouse control feature has been implemented.

 3. Please be aware that certain imported objects may be positioned outside the camera view. 
    To view them, you can utilize the mouse to explore and adjust your perspective accordingly.
*/
int main(int argc, char* argv[])
{
	std::cout << "Initializing AID3D application!" << std::endl;
	
	bool envvar = qputenv("QT3D_RENDERER", "opengl");
	qDebug() << "Opengl Environment variable set:" << envvar;

	//Q_IMPORT_PLUGIN(assimpsceneimport)

	//qputenv("QT_DEBUG_PLUGINS", QByteArray("1"));
	
	if (!QResource::registerResource("assets.rcc")) {
		qWarning("Failed to load assets.rcc");
		return -1;
	}

	qDebug() << QCoreApplication::libraryPaths();
	QCoreApplication::addLibraryPath(QString::fromUtf8("D:/dev/AID_MS3/build/x64-debug/assetimporters/"));
	QCoreApplication::addLibraryPath(QString::fromUtf8("D:/dev/AID_MS3/build/x64-debug/sceneparsers/"));

	qDebug() << QCoreApplication::libraryPaths();

	QApplication app(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	AID::Session *app_session = new AID::Session();
	app_session->initialize();
	app_session->start();
	app_session->shutdown();

	return app.exec();
}