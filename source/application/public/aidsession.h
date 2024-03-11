#pragma once

// cpp includes
#include <memory>

// qt includes
#include <QObject>
#include <QDebug>
#include <QUuid>

// local includes
#include "aidcustdefines.h"
#include "aidmainwindow.h"


namespace AID
{
	class Session
	{

	public:
		Session();
		~Session();

		void initialize();
		void start();
		void shutdown();

		

	public:
		inline QUuid get_session_id() noexcept
		{
			return _session_id;
		}

	private:
		QUuid _session_id;

		// Main application window stuff
		
		std::unique_ptr<AID::MainWindow> _app_window;
		
	};
}