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
    appstore.qrc \
    VAC.qrc
RC_FILE += icon.rc
#vac
RC_FILE += vac.rc

HEADERS += src/config.h \
           src/constants.h \
           src/dashboard.h \
           src/dynamicbutton.h \
           src/hintlineEdit.h \
           src/lnklist.h \
           src/localapps.h \
           src/logindialog.h \
           src/movingdialog.h \
           src/panel.h \
           src/settingDialog.h \
           src/switcher.h \
           src/virtualdesktop.h \
           src/ToolsDll.h \
           src/tabpushbuttonbar.h \
           src/tablabel.h \
           src/searchlineview.h \
           src/searchlineedit.h \
           src/customwndbase.h \
           src/custombtn.h \
           src/client/downloader.h \
           src/client/widgetsrefreshexe.h \
           src/client/widgetrefreshbase.h \
           src/client/updatebutton.h \
           src/client/uninstallbutton.h \
           src/client/tabwidgets.h \
           src/client/tabtitlewidget.h \
           src/client/sorttabbar.h \
           src/client/packageswidgets.h \
           src/client/installbutton.h \
           src/client/appwidget.h \
           src/client/appdetailinfowidget.h \
           src/client/appdetaildialogexe.h \
           src/client/appdatareadexe.h \
           src/client/appdatabase.h \
           src/client/appdata.h \
           src/client/appcenterwdg.h \
    src/client/scorewidget.h \
    src/opacitywidget.h \
    src/scrollbuttonwidget.h \
    src/iconarrangedlg.h \
    src/appmessagebox.h \
    src/scrollsubwidget.h \
    src/scrollupwardbutton.h\
    src/qtsingleapplication/qtlocalpeer.h \
    src/qtsingleapplication/qtsingleapplication.h \
    src/qtsingleapplication/qtsinglecoreapplication.h \
    src/qtsingleapplication/qtlockedfile.h \
    src/qtipaddressedit/qipaddressedit.h \
    src/qtipaddressedit/qipaddressedititem.h \
    src/bswidget.h \
    src/managewidget.h \
    commuinication.h \
    common.h \
    applist.h \
    vappvirtualdesktop.h \
    src/personalizationwidget.h


win32 {

SOURCES += src/qtsingleapplication/qtlockedfile_win.cpp
	#vac
    	LIBS +=  -lUser32 -lShell32
    	LIBS += -L"binary"
    	LIBS += -leay32
    	INCLUDEPATH +=  openssl
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
           src/lnklist.cpp \
           src/localapps.cpp \
           src/logindialog.cpp \
           src/movingdialog.cpp \
           src/panel.cpp \
           src/settingDialog.cpp \
           src/switcher.cpp \
           src/virtualdesktop.cpp \
           src/tabpushbuttonbar.cpp \
           src/tablabel.cpp \
           src/searchlineview.cpp \
           src/searchlineedit.cpp \
           src/customwndbase.cpp \
           src/custombtn.cpp \
           src/client/downloader.cpp \
           src/client/widgetsrefreshexe.cpp \
           src/client/widgetrefreshbase.cpp \
           src/client/updatebutton.cpp \
           src/client/uninstallbutton.cpp \
           src/client/tabwidgets.cpp \
           src/client/tabtitlewidget.cpp \
           src/client/sorttabbar.cpp \
           src/client/packageswidgets.cpp \
           src/client/installbutton.cpp \
           src/client/appwidget.cpp \
           src/client/appdetailinfowidget.cpp \
           src/client/appdetaildialogexe.cpp \
           src/client/appdatareadexe.cpp \
           src/client/appdatabase.cpp \
           src/client/appdata.cpp \
           src/client/appcenterwdg.cpp \
    src/client/scorewidget.cpp \
    src/opacitywidget.cpp \
    src/scrollbuttonwidget.cpp \
    src/iconarrangedlg.cpp \
    src/appmessagebox.cpp \
    src/scrollsubwidget.cpp \
    src/scrollupwardbutton.cpp\
    src/qtsingleapplication/qtlocalpeer.cpp \
    src/qtsingleapplication/qtsingleapplication.cpp \
    src/qtsingleapplication/qtsinglecoreapplication.cpp \
    src/qtsingleapplication/qtlockedfile.cpp \
    src/qtipaddressedit/qipaddressedit.cpp \
    src/qtipaddressedit/qipaddressedititem.cpp \
    src/bswidget.cpp \
    src/main.cpp \
    src/managewidget.cpp \
    commuinication.cpp \
    applist.cpp \
    vappvirtualdesktop.cpp \
    src/personalizationwidget.cpp


FORMS +=\
    dlgchangepassword.ui

OTHER_FILES +=

