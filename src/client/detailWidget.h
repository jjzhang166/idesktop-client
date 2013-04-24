#ifndef DETAILWIDGET_H
#define DETAILWIDGET_H

#include <QtGui>

class LocalAppList;

#define RECOM   0
#define ALL     1
#define UPDATE  2
#define UNINST  3

class QWidget;
class QHBoxLayout;
class QVBoxLayout;
class QSpacerItem;
class QProgressBar;
class QGraphicsView;
class QLabel;
class Downloader;
class QProcess;
class Node;

//enum Action { INSTALL, UPDATE, UNINSTALL };
//enum InstState { NOTHING, DOWNLOADING, INSTALLING };

class DetailWidget : public QWidget
{
    Q_OBJECT
public:
    DetailWidget(QWidget *parent = NULL);
    ~DetailWidget(){};
    void sendName(QString name, int view);
    //void setAction(Action action);
signals:
    void cancelFromDetail();
    void doneFromDetail();
private slots:
    void onBack();
    void onAction();
    void onProcessFinished(int, QProcess::ExitStatus); 
    void onProcessStarted(); 
    void onProgress(qint64, qint64);
    void onDone();
private:
    void updateUI();
    QString getUninstExec(QString display);
    QString getExec(QString execname);
    //QGraphicsView *iconBox;
    QLabel *iconBox;
    QLabel *nameLab;
    QLabel *sizeLab;
    QLabel *briefLab;
    QPushButton *actionButton;
    //QPushButton *upButton;
    QPushButton *backButton;
    QProgressBar *progressBar;
    QLabel *progressLab;
    QTextBrowser *textBrowser;

    int _view;
    bool _inProgress;
    bool _finished;
    QString _icon;
    Downloader *_d;
    QProcess *_p;
    LocalAppList *_l;
    Node *_node;
};

#endif
