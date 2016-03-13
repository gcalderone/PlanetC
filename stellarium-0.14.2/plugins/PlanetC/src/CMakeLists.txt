# 
#  Planetarium control (PlanetC) plug-in for Stellarium
# 
#  Copyright (C) 2016 Giorgio Calderone
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

SET(extLinkerOption ${OPENGL_LIBRARIES})

ADD_LIBRARY(PlanetC-static STATIC ${PLANETC_SRCS} ${PLANETC_RES_CXX} ${PLANETC_UIS_H})
QT5_USE_MODULES(PlanetC-static Core Declarative Network Test OpenGL)
SET_TARGET_PROPERTIES(PlanetC-static PROPERTIES OUTPUT_NAME "PlanetC")
TARGET_LINK_LIBRARIES(PlanetC-static ${extLinkerOption})
SET_TARGET_PROPERTIES(PlanetC-static PROPERTIES COMPILE_FLAGS "-DQT_STATICPLUGIN")
ADD_DEPENDENCIES(AllStaticPlugins PlanetC-static ${stellarium_UIS} stellarium_UIS_H )
