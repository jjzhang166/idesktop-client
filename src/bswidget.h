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
#include <windows.h>
#include <QList>
#include <QPushButton>
#include <QScrollBar>

#include "dynamicbutton.h"

class BtnLabel;
class AxWidget;
class LeftWebKit;
class TabWidget;
class RightWidget;

class WebPage : public QWebPage
{
    virtual QString userAgentForUrl(const QUrl& url) const {
        Q_UNUSED(url);
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

    void setUrl(const QString &url);
    void setPixmap(const QString &pix);
signals:
    void goBack();
public slots:
    void startAssembly();
    void stopAssembly();

    void rightFullScreen();

protected:
    //void paintEvent(QPaintEvent *event);

private:
//    LeftWebKit *_leftWebKit;
    RightWidget *_rightWidget;

    int _width;
    int _height;

    QPixmap _pixmap;

    bool _fullScreen;
//    QPushButton *_fullScreenBtn;
//    DynamicButton *_backBtn;
    BtnLabel *_btnLabel;
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
class RightWidget : public QWidget
{
    Q_OBJECT
public:
    RightWidget(QWidget *parent = 0);
    ~RightWidget();

    void createTab(const QUrl &url);

public slots:
//    void createTab(const QUrl &url);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    TabWidget *_tabWidget;
    QPixmap _rightTopPix;
    QPixmap _rightCenterPix;
    QPixmap _rightBottomPix;

    int _width;
    int _height;
};

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget *parent = 0);
    ~TabWidget();

public slots:
    void createTab(const QUrl &url);
    //void setUrl(const QUrl &url);
    void titleChange(int index, QString title);
    void removeTabWidget(int i);

    void iconChanged(int index, QIcon icon);

    void curChanged(int index);
    void goBack();
    void goForward();
    void refresh();
    void comStateChange (int command, bool enable);
    void setCommandState(bool backState, bool forwardState, bool refresh);

protected:
    //void resizeEvent(QResizeEvent *event);

private:
    QPalette _pal;
    AxWidget *_axWidget;
    QList<AxWidget*> _axWs;
    QUrl _url;
    QPushButton *_goBackBtn;
    QPushButton *_goForwardBtn;
    QPushButton *_refreshBtn;
};

class AxWidget : public QWidget
{
    Q_OBJECT
public:

    enum tagREADYSTATE
    {
        READYSTATE_UNINITIALIZED = 0,
        READYSTATE_LOADING = 1,
        READYSTATE_LOADED = 2,
        READYSTATE_INTERACTIVE = 3,
        READYSTATE_COMPLETE = 4
    };

    AxWidget(int id = 0, QWidget *parent = 0);
    ~AxWidget();

    void setUrl(const QUrl &url);
    void setId(int id);
    int getId();
    QIcon icon(const QUrl &url);

    void getCommandState();
    void goBack();
    void goForward();
    void refresh();
signals:
    void createTab(const QUrl &url);
    void titleChange(int index, QString title);
    void iconChanged(int index, QIcon icon);
    void comStateChange (int command, bool enable);
    void commandState(bool backState, bool forwardState, bool refresh);

    //void setWidth(int w);

public slots:

    void newWindow2 (IDispatch** ppDisp, bool& Cancel);
    void newWindow3(IDispatch** ppDisp, bool& Cancel,
                    uint dwFlags, QString bstrUrlContext, QString bstrUrl);
//    void newWindow(QString URL, int Flags, QString TargetFrameName,
//                  QVariant& PostData, QString Headers, bool& Processed);
    void titleChange(QString title);
    void readyStateChanged(tagREADYSTATE ReadyState);
    void postLaunch();
    void downloadComplete();
	void commandStateChange (int command, bool enable);

    void scrollBarValueChanged(int value);
    //void visbleRectChanged();

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);

//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);

private:
    QAxWidget *_ax;
    //WebAxWidget *_ax;
    int _id;
    bool _backState;
    bool _forwardState;
    bool _refreshState;

    QScrollBar *_scrollBar;
//    SCROLLINFO getVScrollBarInfo();
//    SCROLLINFO getHScrollBarInfo();
//    WId getVScroll();
//    WId getHScroll();

//    double xScale;
//    double yScale;
};

class BtnLabel : public QLabel
{
    Q_OBJECT
public:
    BtnLabel(QWidget *parent = 0);
    ~BtnLabel();

signals:
    void goBack();

private:
    DynamicButton *_backBtn;
    QPixmap _backPix;
    QPixmap _backPixHover;
};

#endif // BSWIDGET_H
