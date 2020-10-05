#-------------------------------------------------
#
# Project created by QtCreator 2020-10-01T11:22:54
#
#-------------------------------------------------

#Copyright (C) 2020  Gabriele Salvato

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

QT += core
QT += gui
QT += widgets


TARGET = Buggy
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000


CONFIG += c++11

SOURCES += main.cpp
SOURCES += axesdialog.cpp
SOURCES += AxisFrame.cpp
SOURCES += AxisLimits.cpp
SOURCES += DataSetProperties.cpp
SOURCES += datastream2d.cpp
SOURCES += geometryengine.cpp
SOURCES += GLwidget.cpp
SOURCES += plot2d.cpp
SOURCES += plotpropertiesdlg.cpp
SOURCES += rpmmeter.cpp
SOURCES += ADXL345.cpp
SOURCES += HMC5883L.cpp
SOURCES += ITG3200.cpp
SOURCES += MadgwickAHRS.cpp
SOURCES += robot.cpp
SOURCES += dcmotor.cpp
SOURCES += mainwindow.cpp


HEADERS += mainwindow.h
HEADERS += rpmmeter.h
HEADERS += ADXL345.h
HEADERS += HMC5883L.h
HEADERS += ITG3200.h
HEADERS += MadgwickAHRS.h
HEADERS += robot.h
HEADERS += dcmotor.h
HEADERS += geometryengine.h
HEADERS += GLwidget.h
HEADERS += axesdialog.h
HEADERS += AxisFrame.h
HEADERS += AxisLimits.h
HEADERS += DataSetProperties.h
HEADERS += datastream2d.h
HEADERS += plot2d.h
HEADERS += plotpropertiesdlg.h


FORMS +=


RESOURCES += shaders.qrc
RESOURCES += textures.qrc


LIBS += -lpigpiod_if2 # To include libpigpiod_if2.so from /usr/local/lib
LIBS += -lrt
LIBS += -lpthread


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += fshader.glsl
DISTFILES += vshader.glsl
DISTFILES += cube.png
DISTFILES += plot.png
