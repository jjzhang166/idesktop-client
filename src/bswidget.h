#ifndef BSWIDGET_H
#define BSWIDGET_H

#include <QMainWindow>
#include <QtWebKit/QWebView>
#include <QToolBar>
#include <QMouseEvent>
#include <ActiveQt/QAxWidget>
#include <QWidget>
#include <QTabWidget>
#include <QUrl>
#include "windows.h"
#include <QList>

class AxWidget;
class LeftWebKit;
class RightAxWidget;

class WebPage : public QWebPage
{
    virtual QString userAgentForUrl(const QUrl& url) const {
        return "Chrome/1.0";
    }
};

//class WebAxWidget : public QAxWidget
//{
//    Q_OBJECT
//public:

//    WebAxWidget(QWidget* parent = 0, Qt::WindowFlags f = 0)
//        : QAxWidget(parent, f)
//    {
//        createHostWindow(false);
//    }
//signals:
//    //void NewWindow(QString URL, int Flags, QString TargetFrameName, QVariant& PostData, QString
//                   //Headers, bool& Processed);
//protected:
//    virtual bool translateKeyEvent(int message, int keycode) const
//    {
//        if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
//            return true;
//        else
//            return QAxWidget::translateKeyEvent(message, keycode);
//    }
//};

class MyWebView : public QWebView
{
    Q_OBJECT
public:
    explicit MyWebView(QWidget *parent = 0);
    ~MyWebView() {}

signals:
    void hideMenu();
    void startAssembly();
    void stopAssembly();

public slots:
    //void setNewUrl(const QUrl &url);
    void setNewUrl(QString url);
    void assembling();
    void assembled();
};

class BsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit BsWidget(int width, int height, QWidget *parent = 0);
    ~BsWidget();

signals:
    void hideMenu();

public slots:
    void startAssembly();
    void stopAssembly();

private:
    LeftWebKit *_leftWebKit;
    RightAxWidget *_rightAxWidget;

    int _width;
    int _height;
};

class LeftWebKit : public QMainWindow
{
    Q_OBJECT

public:
    explicit LeftWebKit(QWidget *parent = 0);
    ~LeftWebKit();

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void hideMenu();
    void link(const QUrl &url);
    void startAssembly();
    void stopAssembly();
    void webviewIconChanged();

public slots:
    //void loadStart();
    //void urlChange()

private:
    //QWebView *_webview;
    MyWebView *_webview;
    //QToolBar *_toolsMenu;
    WebPage *_webpage;
};

class RightAxWidget : public QTabWidget
{
    Q_OBJECT
public:
    RightAxWidget(QWidget *parent = 0);
    ~RightAxWidget() {}

public slots:
    void createTab(const QUrl &url);
    //void setUrl(const QUrl &url);
    void titleChange(int index, QString title);
    void removeTabWidget(int i);

    void iconChange(int index, QIcon icon);

protected:
    //void resizeEvent(QResizeEvent *event);

private:
    QPalette _pal;
    AxWidget *_axWidget;
    QList<AxWidget*> _axWs;
    QUrl _url;
};

class AxWidget : public QWidget
{
    Q_OBJECT
public:
    AxWidget(int id = 0, QWidget *parent = 0);
    ~AxWidget() {}

    void setUrl(const QUrl &url);
    void setId(int id);
    int getId();

signals:
    void createTab(const QUrl &url);
    void titleChange(int index, QString title);
    void iconChange(int index, QIcon icon);

public slots:
//    void newWindow(QString URL, int Flags, QString TargetFrameName, \
//                  QVariant& PostData, QString Headers, bool& Processed);
    void newWindow2 (IDispatch** ppDisp, bool& Cancel);
    void newWindow3(IDispatch** ppDisp, bool& Cancel, \
                    uint dwFlags, QString bstrUrlContext, QString bstrUrl);
    void titleChange(QString title);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QAxWidget *_ax;
    //WebAxWidget *_ax;
    int _id;
};

#endif // BSWIDGET_H
