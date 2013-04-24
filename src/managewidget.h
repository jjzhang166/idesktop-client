#ifndef MANAGEWIDGET_H
#define MANAGEWIDGET_H

#include <QMainWindow>
#include <QtWebKit/QWebView>
#include <QMouseEvent>
#include "bswidget.h"

class ManageWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ManageWidget(QWidget *parent = 0);
    ~ManageWidget();

    void upLoad();
protected:

public slots:

    void link(const QUrl &url);

private:
    QWebView *_webview;

    QToolBar *_toolsMenu;
    WebPage *_webpage;
};

#endif // MANAGEWIDGET_H
