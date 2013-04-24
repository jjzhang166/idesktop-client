#include "appdetaildialogexe.h"
#include "appdetailinfowidget.h"
#include "appdata.h"
#include "appdatareadexe.h"

AppDetailDialogExe* AppDetailDialogExe::_instance = NULL;

AppDetailDialogExe::AppDetailDialogExe()
    : _appDetailWidget(NULL)
{
}

AppDetailDialogExe* AppDetailDialogExe::Instance()
{
    if (_instance == NULL)
        _instance = new AppDetailDialogExe;
    return _instance;
}

void AppDetailDialogExe::Destroy()
{
    if (_instance)
        delete _instance;
    _instance = NULL;
}

void AppDetailDialogExe::showApp(const QString &appName)
{
    AppDataReadExe::Instance()->readAppData(appName);
}

void AppDetailDialogExe::hideApp()
{
    if (_appDetailWidget)
        _appDetailWidget->close();
}

void AppDetailDialogExe::showApp(AppData* data)
{
    _appDetailWidget->show();
    _appDetailWidget->showApp(data);
}

void AppDetailDialogExe::showWidget()
{
    if (_appDetailWidget == NULL)
    {
        _appDetailWidget = new AppDetailInfoWidget;

        _appDetailWidget->resize(476,402);
        _appDetailWidget->setModal(true);
    }
    _appDetailWidget->showApp(NULL);
    _appDetailWidget->centerWidget();
    _appDetailWidget->show();
}

void AppDetailDialogExe::AppError(const QString &error)
{
    _appDetailWidget->AppError(error);
}

void AppDetailDialogExe::Progress(qint64 recieve , qint64 total)
{
    if (_appDetailWidget)
        _appDetailWidget->Progress(recieve,total);
}


void  AppDetailDialogExe::installFinished()
{
    if (_appDetailWidget)
        _appDetailWidget->installFinished();
}

void AppDetailDialogExe::installDownloaded()
{
    if (_appDetailWidget)
        _appDetailWidget->installDownloaded();
}

void AppDetailDialogExe::messageBox(const QString &msg,const QString &title,
                                    const QString &btn, QString state)
{
    if (_appDetailWidget)
        _appDetailWidget->messageBox(msg, title, btn,state);
}

void AppDetailDialogExe::installCanceled()
{
    if (_appDetailWidget)
        _appDetailWidget->installCanceled();
}

void AppDetailDialogExe::loadFailed()
{
    if (_appDetailWidget)
        _appDetailWidget->loadFailed();
}

void AppDetailDialogExe::defaultIcon(QString icon)
{
    _defaultIcon = icon;
}

QString AppDetailDialogExe::defaultIcon()
{
    return _defaultIcon;
}

void AppDetailDialogExe::showMessage(QString msg)
{
    if (_appDetailWidget)
        _appDetailWidget->showMessage(msg);
}
