# CMakeList.txt : CMake project for AID, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("AID")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(QT_OPENGL_IMPLEMENTATION 1)
set(QT_DLL_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt6/msvc2019_64/bin")
set(QT_PLUGIN_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt6/msvc2019_64/plugins")
set(QT_LIB_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt6/msvc2019_64/lib")
set(QT_MODULES_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt6/msvc2019_64/modules")
set(QT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt6/msvc2019_64/include")
set(MINIZ_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/miniz")
set(QT3DGIZMO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt3dgizmo/include")
set(QT3DGIZMO_DLL_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt3dgizmo/dynamic")
set(QT3DGIZMO_LIB_DIR "${CMAKE_SOURCE_DIR}/thirdparty/qt3dgizmo/static")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/thirdparty/qt6/msvc2019_64" ${QT3DGIZMO_LIB_DIR} ${QT3DGIZMO_DLL_DIR} ${QT3DGIZMO_INCLUDE_DIR} ${QT_DLL_DIR} ${QT_INCLUDE_DIR} ${QT_PLUGIN_DIR} ${QT_LIB_DIR} ${QT_MODULES_DIR}) #${QT_IMG_PLUGIN_DIR}

# Find_library command to locate qt3dgizmo.lib
find_library(QT3DGIZMO_LIBRARY NAMES qt3dgizmo PATHS ${QT3DGIZMO_LIB_DIR} NO_DEFAULT_PATH)
if(NOT QT3DGIZMO_LIBRARY)
  message(FATAL_ERROR "qt3dgizmo.lib not found")
endif()

find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Svg 3DAnimation 3DCore 3DExtras 3DInput 3DRender 3DLogic Concurrent Network REQUIRED)

#project include directories
include_directories(${CMAKE_SOURCE_DIR}/source/aidcommon)
include_directories(${CMAKE_SOURCE_DIR}/source/renderer/public)
include_directories(${CMAKE_SOURCE_DIR}/source/application/public)
include_directories(${CMAKE_SOURCE_DIR}/source/ui/public)
include_directories(${CMAKE_SOURCE_DIR}/source/parser/public)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/miniz)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/qt3dgizmo/include)



#add_subdirectory(${CMAKE_SOURCE_DIR}/build/x64-debug/plugins/sceneparsers)

# Add source to this project's executable.
add_executable (AID  
"aidapp.cpp" 
"source/aidcommon/aidcustdefines.h" 
"source/aidcommon/aidutils.h" 
"source/aidcommon/aidmediator.h"
"source/aidcommon/aidobjmodel.h"
"thirdparty/qt3dgizmo/include/qt3dgizmo.h"
"source/ui/public/aidmainwindow.h" 
"source/ui/private/aidmainwindow.cpp" 
"source/application/public/aidsession.h" 
"source/application/private/aidsession.cpp" 
"source/renderer/public/aidrenderwindow.h"
"source/renderer/private/aidrenderwindow.cpp"
"resources/resources.qrc"
"thirdparty/miniz/miniz.h"
"thirdparty/miniz/miniz.c"
"source/parser/private/aidparser.cpp" 
"source/parser/public/aidparser.h"
"source/shader/shaders.qrc"
 "source/renderer/private/aidcamera.cpp" 
 "source/renderer/public/aidcamera.h" 
 "source/aidcommon/aidtempdirmanager.h" 
 "source/renderer/public/aidoutlinematerial.h" 
 "source/renderer/private/aidoutlinematerial.cpp"
 "source/ui/public/aid2dViewDialog.h"
 "source/ui/private/aid2dViewDialog.cpp" 
 "source/parser/public/Line3D.h"
 "source/parser/private/Plane3D.cpp" 
 "source/parser/public/Plane3D.h")

target_link_libraries(AID PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	Qt6::OpenGL
	Qt6::OpenGLWidgets
	Qt6::Svg
	Qt6::3DCore
	Qt6::3DAnimation
	Qt6::3DExtras
	Qt6::3DInput
	Qt6::3DRender
	Qt6::3DLogic
	Qt6::Concurrent
	Qt6::Network
	${QT3DGIZMO_LIBRARY}
)

set(REQUIRED_QT_DLLS
	Qt6Core.dll
	Qt6Gui.dll
	Qt6Widgets.dll
	Qt6OpenGL.dll
	Qt6OpenGLWidgets.dll
	Qt6Svg.dll
	Qt63DAnimation.dll
	Qt63DCore.dll
	Qt63DExtras.dll
	Qt63DInput.dll
	Qt63DRender.dll
	Qt63DLogic.dll
	Qt6Concurrent.dll
	Qt6Network.dll
	#Opengl library
	opengl32sw.dll 

	
	Qt6Cored.dll
	Qt6Guid.dll
	Qt6Widgetsd.dll
	Qt6OpenGLd.dll
	Qt6OpenGLWidgetsd.dll
	Qt6Svgd.dll
	Qt63DAnimationd.dll
	Qt63DCored.dll
	Qt63DExtrasd.dll
	Qt63DInputd.dll
	Qt63DRenderd.dll
	Qt63DLogicd.dll
	Qt6Concurrentd.dll
	Qt6Networkd.dll
)

#qt_add_binary_resources(resources resources/interiorequipmentobjects/assets.qrc DESTINATION assets.rcc)
#add_dependencies(AID resources)

qt_add_binary_resources(resources ${CMAKE_SOURCE_DIR}/resources/interiorequipmentobjects/assets.qrc ${CMAKE_SOURCE_DIR}/resources/interiorTileObjects/tiles.qrc DESTINATION assets.rcc)

add_dependencies(AID resources)

set(REQUIRED_QT3DGIZMO_DLL
	qt3dgizmo.dll
)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${TARGET_FILE_DIR})

foreach(QT_DLL ${REQUIRED_QT_DLLS})
	message(${QT_DLL})
	add_custom_command(TARGET AID POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${QT_DLL_DIR}/${QT_DLL}"
	$<TARGET_FILE_DIR:AID>
	)
endforeach()


add_custom_command(TARGET AID POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
${QT_PLUGIN_DIR}
$<TARGET_FILE_DIR:AID>/plugins
)

add_custom_command(TARGET AID POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
${QT_PLUGIN_DIR}/platforms
$<TARGET_FILE_DIR:AID>/platforms
)

add_custom_command(TARGET AID POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
${QT3DGIZMO_DLL_DIR}
$<TARGET_FILE_DIR:AID>
)

add_custom_command(TARGET AID POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
${QT3DGIZMO_LIB_DIR}
$<TARGET_FILE_DIR:AID>
)
  

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AID PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
