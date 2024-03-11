#include "aidsession.h"

AID::Session::Session()
	:_app_window(nullptr)
{
	_app_window = std::make_unique<AID::MainWindow>();
}

AID::Session::~Session()
{
	_app_window->shutdown();
}

void AID::Session::initialize()
{
	_app_window->initialize();
}

void AID::Session::start()
{
	qDebug() << "Session starting.";
	_app_window->show();
}

void AID::Session::shutdown()
{
	qDebug() << "Session shutting down.";
}


