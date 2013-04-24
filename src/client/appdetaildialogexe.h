#ifndef APPDETAILDIALOGEXE_H
#define APPDETAILDIALOGEXE_H

#include <QString>

class AppDetailInfoWidget;
class AppData;
class AppDetailDialogExe
{
public:
    static AppDetailDialogExe* Instance();
    static void Destroy();


    void AppError(const QString &error);

    void showApp(const QString &appName);
    void showApp(AppData* data);

    void hideApp();
    void showWidget();

    void defaultIcon(QString icon);
    QString defaultIcon();

    void Progress(qint64, qint64);
    void installFinished();
    void installDownloaded();
    void installCanceled();
    void loadFailed();

    void messageBox(const QString &msg,const QString &title,const QString &btn, QString state);

    void showMessage(QString msg);

private:

    AppDetailDialogExe();
    static AppDetailDialogExe* _instance;
    AppDetailInfoWidget* _appDetailWidget;

    QString _defaultIcon;//Ä¬ÈÏÍ¼Æ¬
};

#endif // APPDETAILDIALOGEXE_H
