TEMPLATE = app
#CONFIG += release
CONFIG += qaxcontainer
DEFINES -= UNICODE
TARGET = idesktop-client
DEPENDPATH += .
INCLUDEPATH += .

UI_DIR = ./tmp
MOC_DIR = ./tmp
OBJECTS_DIR = ./tmp
RCC_DIR = ./tmp

Qt += QtGui
Qt += QtCore
Qt += DEBUG
Qt += QtSql
QT += webkit
QT += network
#Qt += QAxContainer
#Qt += QAxServer
QT += script

#vac
QT += core gui
QT += xml

CONFIG(debug,debug|release) {
    win32:LIBS += -lQtNetworkD
    win32:LIBS += -lQtSqlD
    win32:LIBS += -lQtScriptD
} else {
    win32:LIBS += -lQtNetwork
    win32:LIBS += -lQtSql
    win32:LIBS += -lQtScript
}

TRANSLATIONS = appstore.ts
RESOURCES += appstore.qrc \
    appstore.qrc
RC_FILE += icon.rc
#vac
RC_FILE += vac.rc

HEADERS += src/config.h \
           src/dashboard.h \
           src/dynamicbutton.h \
           src/hintlineEdit.h \
           src/localapps.h \
           src/logindialog.h \
           src/movingdialog.h \
           src/panel.h \
           src/switcher.h \
           src/virtualdesktop.h \
           src/ToolsDll.h \
    src/appmessagebox.h \
    src/qtsingleapplication/qtlocalpeer.h \
    src/qtsingleapplication/qtsingleapplication.h \
    src/qtsingleapplication/qtsinglecoreapplication.h \
    src/qtsingleapplication/qtlockedfile.h \
    src/qtipaddressedit/qipaddressedit.h \
    src/qtipaddressedit/qipaddressedititem.h \
    src/bswidget.h \
    src/commuinication.h \
    src/common.h \
    src/arrangewidget.h \
    src/vacserverwidget.h \
    src/hovericonitem.h \
    src/vacwidget.h \
    src/skinwidget.h \
    src/dirwidget.h \
    src/movewidget.h \
    src/dirminwidget.h \
    src/graphicsloginitem.h \
    src/paascommuinication.h \
    src/iconitem.h \
    src/contextmenuwidget.h \
    src/public.h \
    src/nodebutton.h \
    src/toolbarwidget.h \
    src/dustbin.h \
    src/qitemmanager.h \
    src/runapp.h


win32 {

SOURCES += src/qtsingleapplication/qtlockedfile_win.cpp
	#vac
    	LIBS +=  -lUser32 -lShell32
    	LIBS += -L"binary"
        #LIBS += -leay32
        #INCLUDEPATH +=  openssl
}

unix {

SOURCES += src/qtsingleapplication/qtlockedfile_unix.cpp

}

#vac
TRANSLATIONS=lang/lang_chn.ts

SOURCES += src/config.cpp \
           src/dashboard.cpp \
           src/dynamicbutton.cpp \
           src/hintlineEdit.cpp \
           src/localapps.cpp \
           src/logindialog.cpp \
           src/movingdialog.cpp \
           src/panel.cpp \
           src/switcher.cpp \
           src/virtualdesktop.cpp \
    src/appmessagebox.cpp \
    src/qtsingleapplication/qtlocalpeer.cpp \
    src/qtsingleapplication/qtsingleapplication.cpp \
    src/qtsingleapplication/qtsinglecoreapplication.cpp \
    src/qtsingleapplication/qtlockedfile.cpp \
    src/qtipaddressedit/qipaddressedit.cpp \
    src/qtipaddressedit/qipaddressedititem.cpp \
    src/bswidget.cpp \
    src/main.cpp \
    src/commuinication.cpp \
    src/arrangewidget.cpp \
    src/vacserverwidget.cpp \
    src/hovericonitem.cpp \
    src/vacwidget.cpp \
    src/skinwidget.cpp \
    src/dirwidget.cpp \
    src/movewidget.cpp \
    src/dirminwidget.cpp \
    src/graphicsloginitem.cpp \
    src/paascommuinication.cpp \
    src/iconitem.cpp \
    src/contextmenuwidget.cpp \
    src/nodebutton.cpp \
    src/toolbarwidget.cpp \
    src/dustbin.cpp \
    src/qitemmanager.cpp \
    src/runapp.cpp


FORMS +=

OTHER_FILES +=

