# 
#  Planetarium control (PlanetC) plug-in for Stellarium
# 
#  Copyright (C) 2016-2018 Giorgio Calderone
# 
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public icense
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <http://www.gnu.org/licenses/>.
# 

INCLUDE_DIRECTORIES(
  .
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/plugins/PlanetC/src)

LINK_DIRECTORIES(${BUILD_DIR}/src)

SET(PLANETC_SRCS
  PlanetC_VideoPlayer.hpp
  PlanetC_VideoPlayer.cpp
  PlanetC.hpp
  PlanetC.cpp
  PlanetC_Settings.hpp
  PlanetC_Settings.cpp
  PlanetC_UI.hpp
  PlanetC_UI.cpp
  PlanetC_Widgets.hpp
  PlanetC_DateTimeDialog.hpp
  PlanetC_DateTimeDialog.cpp
  )

SET(PLANETC_UIS
  planetc_DateTimeDialogGui.ui
  planetc_UI.ui
  planetc_Settings.ui
)

QT5_WRAP_UI(PLANETC_UIS_H ${PLANETC_UIS})



################# compiles resources files ############
SET(PLANETC_RES ../resources/PlanetC.qrc)
QT5_ADD_RESOURCES(PLANETC_RES_CXX ${PLANETC_RES})

ADD_LIBRARY(PlanetC-static STATIC ${PLANETC_SRCS} ${PLANETC_RES_CXX} ${PLANETC_UIS_H})

SET_TARGET_PROPERTIES(PlanetC-static PROPERTIES OUTPUT_NAME "PlanetC")

IF(ENABLE_QTAV)
    TARGET_INCLUDE_DIRECTORIES(PlanetC-static PRIVATE ${QTAVWIDGETS_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(PlanetC-static Qt5::Core Qt5::Network Qt5::OpenGL ${QTAVWIDGETS_LIBRARIES})
ELSE()
    # SET(extLinkerOption ${OPENGL_LIBRARIES} ${STEL_GLES_LIBS} -lGLU -lGL)
    TARGET_LINK_LIBRARIES(PlanetC-static Qt5::Core Qt5::Network Qt5::OpenGL Qt5::Multimedia Qt5::MultimediaWidgets)
ENDIF()



SET_TARGET_PROPERTIES(PlanetC-static PROPERTIES COMPILE_FLAGS "-DQT_STATICPLUGIN")
ADD_DEPENDENCIES(AllStaticPlugins PlanetC-static ${stellarium_UIS} stellarium_UIS_H )
