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
QT += opengl
QT += widgets
QT += serialport


TARGET = Buggy
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000


CONFIG += c++11

SOURCES += main.cpp \
    car.cpp \
    compass.cpp \
    dashboardwidget.cpp \
    floor.cpp
SOURCES += GrCamera.cpp
SOURCES += mesh.cpp
SOURCES += model.cpp
SOURCES += skyboxwidget.cpp
SOURCES +=
SOURCES += controlsdialog.cpp
SOURCES += axesdialog.cpp
SOURCES += AxisFrame.cpp
SOURCES += AxisLimits.cpp
SOURCES += DataSetProperties.cpp
SOURCES += datastream2d.cpp
SOURCES += geometryengine.cpp
SOURCES += \
    roomwidget.cpp
SOURCES += plot2d.cpp
SOURCES += plotpropertiesdlg.cpp
SOURCES += mainwindow.cpp


HEADERS += mainwindow.h \
    car.h \
    compass.h \
    dashboardwidget.h \
    floor.h
HEADERS += GrCamera.h
HEADERS += mesh.h
HEADERS += model.h
HEADERS += skyboxwidget.h
HEADERS +=
HEADERS += controlsdialog.h
HEADERS += geometryengine.h
HEADERS += \
    roomwidget.h
HEADERS += axesdialog.h
HEADERS += AxisFrame.h
HEADERS += AxisLimits.h
HEADERS += DataSetProperties.h
HEADERS += datastream2d.h
HEADERS += plot2d.h
HEADERS += plotpropertiesdlg.h


FORMS += controlsdialog.ui


RESOURCES += shaders.qrc
RESOURCES += textures.qrc


LIBS += -lrt
LIBS += -lpthread
LIBS += -lassimp


DISTFILES += fshader.glsl
DISTFILES += BuggyDocs/0_DcMotorModel.pdf
DISTFILES += BuggyDocs/1_DCMotorSpeedSystemAnalysis.pdf
DISTFILES += BuggyDocs/DcMotor_Digital_Control.pdf
DISTFILES += BuggyDocs/DcMotor_Digital_PID_Control.pdf
DISTFILES += BuggyDocs/DcMotor_PI_Control.pdf
DISTFILES += BuggyDocs/DcMotor_PID_Control.pdf
DISTFILES += BuggyDocs/DcMotor_ROOT_Locus.pdf
DISTFILES += BuggyDocs/DcMotorControl.pdf
DISTFILES += BuggyDocs/DcMotorFrequencyDomain.pdf
DISTFILES += BuggyDocs/DcMotorModel.pdf
DISTFILES += BuggyDocs/DcMotorSimscapeModeling.pdf
DISTFILES += BuggyDocs/DcMotorSimulinkControllerDesign.pdf
DISTFILES += BuggyDocs/DcMotorSimulinkModeling.pdf
DISTFILES += BuggyDocs/DcMotorStateSpace.pdf
DISTFILES += BuggyDocs/Lagging Effect Associated with a Hold.pdf
DISTFILES += BuggyDocs/Lagging Effect Associated with a Hold.pdf
DISTFILES += BuggyDocs/SystemAnalysis.pdf
DISTFILES += BuggyDocs/SystemModeling.pdf
DISTFILES += Car/4_1_2_d.jpg
DISTFILES += Car/4_blues.jpg
DISTFILES += Car/4_greens.jpg
DISTFILES += Car/4_greys.jpg
DISTFILES += Car/4_redsml.jpg
DISTFILES += Car/4_whites.jpg
DISTFILES += Car/Car 3.3ds
DISTFILES += Car/Car 3.gsm
DISTFILES += Car/Car 3.mtl
DISTFILES += Car/Car 3.obj
DISTFILES += Car/Car3.mtl
DISTFILES += Car/Car3.obj
DISTFILES += Car/Wo_16sml.jpg
DISTFILES += vshader.glsl
DISTFILES += cube.png
DISTFILES += plot.png


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
