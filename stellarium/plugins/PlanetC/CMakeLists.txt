# 
#  Planetarium control (PlanetC) plug-in for Stellarium
# 
#  Copyright (C) 2016-2020 Giorgio Calderone
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

SET(PLANETC_MAJOR "0")
SET(PLANETC_MINOR "3")
SET(PLANETC_PATCH "0")
SET(PLANETC_VERSION "${PLANETC_MAJOR}.${PLANETC_MINOR}.${PLANETC_PATCH}")

IF(APPLE)
	SET(CMAKE_INSTALL_PREFIX $ENV{HOME}/Library/Application\ Support/Stellarium)
ElSE(APPLE)
	SET(CMAKE_INSTALL_PREFIX $ENV{HOME}/.stellarium)
ENDIF(APPLE)

ADD_DEFINITIONS(-DPLANETC_PLUGIN_VERSION="${PLANETC_VERSION}")

ADD_SUBDIRECTORY( src )

INSTALL(FILES DESTINATION "modules/PlanetC")


