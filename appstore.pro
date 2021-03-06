TEMPLATE = app
#CONFIG += release
CONFIG+=debug_and_release
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
#QT += webkit
QT += network
#Qt += QAxContainer
#Qt += QAxServer
QT += script

#vac
QT += core gui
QT += xml

#Json
LIBS += -lqjson

CONFIG(debug,debug|release) {
    win32:LIBS += -lQtNetworkD
    win32:LIBS += -lQtSqlD
    win32:LIBS += -lQtScriptD
} else {
    win32:LIBS += -lQtNetwork
    win32:LIBS += -lQtSql
    win32:LIBS += -lQtScript
}

TRANSLATIONS = idesktop-client.ts
RESOURCES += appstore.qrc

RC_FILE += icon.rc
#vac
RC_FILE += vac.rc

HEADERS += src/config.h \
           src/dashboard.h \
           src/dynamicbutton.h \
           src/hintlineEdit.h \
           src/localapps.h \
           src/logindialog.h \
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
    src/commuinication.h \
    src/common.h \
    src/vacwidget.h \
    src/skinwidget.h \
    src/dirwidget.h \
    src/movewidget.h \
    src/dirminwidget.h \
    src/paascommuinication.h \
    src/iconitem.h \
    src/public.h \
    src/nodebutton.h \
    src/toolbarwidget.h \
    src/dustbin.h \
    src/qitemmanager.h \
    src/runapp.h \
    src/struct.h \
    src/idesktopsettings.h \
    src/gridcontainer.h \
    src/gridmodel.h \
    src/indexedlist.h \
    src/iconwidget.h \
    src/swipegesture.h \
    src/dircontainer.h \
    src/docker.h \
    src/rotatewarnningitem.h \
    src/mask.h \
    src/movingdialog.h \
    src/contextmenuwidget.h



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
#TRANSLATIONS=lang/lang_chn.ts

SOURCES += src/config.cpp \
           src/dashboard.cpp \
           src/dynamicbutton.cpp \
           src/hintlineEdit.cpp \
           src/localapps.cpp \
           src/logindialog.cpp \
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
    src/main.cpp \
    src/vacwidget.cpp \
    src/skinwidget.cpp \
    src/dirwidget.cpp \
    src/movewidget.cpp \
    src/dirminwidget.cpp \
    src/paascommuinication.cpp \
    src/iconitem.cpp \
    src/nodebutton.cpp \
    src/toolbarwidget.cpp \
    src/dustbin.cpp \
    src/qitemmanager.cpp \
    src/runapp.cpp \
    src/idesktopsettings.cpp \
    src/gridcontainer.cpp \
    src/gridmodel.cpp \
    src/indexedlist.cpp \
    src/iconwidget.cpp \
    src/swipegesture.cpp \
    src/dircontainer.cpp \
    src/docker.cpp \
    src/rotatewarnningitem.cpp \
    src/contextmenuwidget.cpp \
    src/mask.cpp \
    src/commuinication.cpp \
    src/movingdialog.cpp


FORMS +=

OTHER_FILES +=

