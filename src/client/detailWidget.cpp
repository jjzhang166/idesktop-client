#include <QApplication>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include "detailWidget.h"
#include "appnode.h"
#include "downloader.h"
#include "dlg.h"
#include "../localapps.h"
#include "../lnklist.h"
#include "../config.h"

#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"

DetailWidget::DetailWidget(QWidget * parent)
    : QWidget(parent), _finished(false)//, _action(INSTALL), _state(NOTHING)
{
    QVBoxLayout *mainLayout;
    QHBoxLayout *upperLayout;
    QVBoxLayout *labLayout;
    QSpacerItem *spacerUp;
    QSpacerItem *spacerMid;
    QHBoxLayout *midLayout;
    QVBoxLayout *butLayout;

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    mainLayout = new QVBoxLayout(this);
    upperLayout = new QHBoxLayout();

    iconBox = new QLabel(this);
    sizePolicy.setHeightForWidth(iconBox->sizePolicy().hasHeightForWidth());
    iconBox->setSizePolicy(sizePolicy);
    iconBox->setMinimumSize(QSize(96, 96));
    iconBox->setMaximumSize(QSize(96, 96));

    upperLayout->addWidget(iconBox);

    labLayout = new QVBoxLayout();
    nameLab = new QLabel(this);
    sizePolicy.setHeightForWidth(nameLab->sizePolicy().hasHeightForWidth());
    nameLab->setSizePolicy(sizePolicy);
    labLayout->addWidget(nameLab);

    briefLab = new QLabel(this);
    sizePolicy.setHeightForWidth(briefLab->sizePolicy().hasHeightForWidth());
    briefLab->setSizePolicy(sizePolicy);
    labLayout->addWidget(briefLab);

    sizeLab = new QLabel(this);
    sizePolicy.setHeightForWidth(sizeLab->sizePolicy().hasHeightForWidth());
    sizeLab->setSizePolicy(sizePolicy);
    sizeLab->setMinimumSize(QSize(40, 60));
    labLayout->addWidget(sizeLab);

    upperLayout->addLayout(labLayout);

    spacerUp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    upperLayout->addItem(spacerUp);

    butLayout = new QVBoxLayout();
    actionButton = new QPushButton();
    sizePolicy.setHeightForWidth(actionButton->sizePolicy().hasHeightForWidth());
    actionButton->setSizePolicy(sizePolicy);
    actionButton->setMinimumSize(QSize(100, 35));
    actionButton->setMaximumSize(QSize(100, 35));
    butLayout->addWidget(actionButton);

    /*
    upButton = new QPushButton();
    sizePolicy.setHeightForWidth(upButton->sizePolicy().hasHeightForWidth());
    upButton->setSizePolicy(sizePolicy);
    upButton->setMinimumSize(QSize(65, 35));
    upButton->setMaximumSize(QSize(65, 35));
    butLayout->addWidget(upButton);
    */

    backButton = new QPushButton();
    sizePolicy.setHeightForWidth(backButton->sizePolicy().hasHeightForWidth());
    backButton->setSizePolicy(sizePolicy);
    backButton->setMinimumSize(QSize(100, 35));
    backButton->setMaximumSize(QSize(100, 35));
    butLayout->addWidget(backButton);
    
    upperLayout->addLayout(butLayout);

    mainLayout->addLayout(upperLayout);

    midLayout = new QHBoxLayout();
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setVisible(false);
    midLayout->addWidget(progressBar);

    progressLab = new QLabel(this);
    sizePolicy.setHeightForWidth(progressLab->sizePolicy().hasHeightForWidth());
    progressLab->setSizePolicy(sizePolicy);
    midLayout->addWidget(progressLab);

    spacerMid = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    midLayout->addItem(spacerMid);
    mainLayout->addLayout(midLayout);

    textBrowser = new QTextBrowser(this);
    mainLayout->addWidget(textBrowser);

    nameLab->setText("");
    sizeLab->setText("");
    briefLab->setText("");
    actionButton->setText("");
    //upButton->setText("");
    backButton->setText("");
    progressLab->setText("");

    _d = new Downloader();
    _p = new QProcess(this);
    _l = LocalAppList::getList();

    connect(backButton, SIGNAL(clicked()), this, SLOT(onBack()));
    connect(actionButton, SIGNAL(clicked()), this, SLOT(onAction()));
    connect(_d, SIGNAL(progress(qint64, qint64)), this, SLOT(onProgress(qint64, qint64)));
    connect(_p, SIGNAL(started()), this, SLOT(onProcessStarted()));
    connect(_p, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    connect(_d, SIGNAL(done()), this, SLOT(onDone()));

};

void DetailWidget::onDone()
{
    _finished = true;
}

void DetailWidget::onProcessFinished(int exitCode, \
    QProcess::ExitStatus status) 
{
    _inProgress = false;
}

void DetailWidget::onProcessStarted()
{
    backButton->setEnabled(false);
}

void DetailWidget::onProgress(qint64 recieve, qint64 total)
{
    qint64 rate = total/100;
    recieve = recieve/rate;
    progressBar->setValue(recieve);
}

void DetailWidget::updateUI()
{
    if (_inProgress)
        progressBar->setVisible(true);
    else
        progressBar->setVisible(false);
    if (_view == UPDATE) {
        if (_inProgress) {
            actionButton->setText("Updating");
            actionButton->setEnabled(false);
            backButton->setText("Cancel");
            backButton->setEnabled(true);
        } else {
            actionButton->setText("Update");
            actionButton->setEnabled(true);
            backButton->setText("Back");
            backButton->setEnabled(true);
        }
    } else if (_view == UNINST) {
        progressBar->setVisible(false);
        if (_inProgress) {
            actionButton->setText("卸载中");
            actionButton->setEnabled(false);
            backButton->setText("取消");
            backButton->setEnabled(false);
        } else {
            actionButton->setText("卸载");
            actionButton->setEnabled(true);
            backButton->setText("返回");
            backButton->setEnabled(true);
        }
    } else {
        if (_node->status() == UNINSTALLED) {
            actionButton->setText("安装");
            actionButton->setEnabled(true);
            backButton->setText("返回");
            backButton->setEnabled(true);
            if (_inProgress) {
                actionButton->setText("安装中");
                actionButton->setEnabled(false);
                backButton->setText("取消");
            }
        } else {
            actionButton->setText("已安装");
            actionButton->setEnabled(false);
            backButton->setText("返回");
            backButton->setEnabled(true);
        }
    }
}

void DetailWidget::sendName(QString name, int view)
{
    Node *node = NodeList::instance()->getNode(name);
    if (!node) {
        return;
    }
    _node = node;
    _view = view;
    _inProgress = false;
    nameLab->setText(name);
    briefLab->setText(node->brief());
    textBrowser->setText(node->comments());
    QFileInfo info(_node->icon());
    _icon = Config::get("IconDir") + "\\" + info.fileName();
    _d->download(node->icon(), _icon);
    while (!_finished)
        QApplication::processEvents();
    _finished = false;
    iconBox->setPixmap(QPixmap(_icon).scaled(96,96));
    updateUI();
}

void DetailWidget::onAction()
{
    _inProgress = true;
    if (_view == UNINST) {
        /*
        Dialog *dlg = new Dialog(this);
        dlg->setText("are you sure?");
        if (!dlg->exec()) {
            _inProgress = false;
            return;
        }
        */
        LocalApp *app = _l->getAppByName(_node->name());
        QString uninstCommand = app->uninstName();
        if (!app->uninstName().startsWith("\""))
	    QString uninstCommand = QString("\"%1\"").arg(app->uninstName());
        _p->start(uninstCommand);
        backButton->setEnabled(false); 
        actionButton->setEnabled(false); 
        while (_inProgress)
            QApplication::processEvents();
        _inProgress = false;
        actionButton->setEnabled(true); 
        backButton->setEnabled(true);
        if (getUninstExec(_node->uninstName()).isEmpty()) {
           _l->delApp(_node->name());
           actionButton->setText("已卸载");
           actionButton->setEnabled(false);
           backButton->setText("返回");
           _node->setStatus(UNINSTALLED);
           QString message = QString("%1 已被成功卸载").arg(_node->name());
           QMessageBox::information(this, "操作已成功", message, "    确定    "); 
        }
        return;
    }
    //install and update
    progressBar->setValue(0);
    updateUI();
    QFileInfo info(_node->source());
    QString path(Config::get("InstDir"));
    _d->download(_node->source(), path + "\\" + info.fileName());
    while (!_finished)
        QApplication::processEvents();
    _finished = false;
    _p->start("\"" + path + "\\" + info.fileName() + "\"");
    backButton->setEnabled(false);
    while (_inProgress)
        QApplication::processEvents();
    _inProgress = false;
    backButton->setEnabled(true);
    QString execn = getExec(_node->execName());
    QString uninstExec = getUninstExec(_node->uninstName());
    if (execn.isEmpty() || uninstExec.isEmpty()) {
        QString message = QString("安装 %1 未成功, 可能是由于：\n"\
                                  "1. 用户取消安装程序.\n"\
                                  "2. 用户在安装程序中未选择在桌面建立图标\n").arg(_node->name());
        QMessageBox::critical(this, "操作未成功", message, "    确定    ");
        return;
    } else {
        LocalApp *app = new LocalApp();
        app->setName(_node->name());
        app->setIcon(_icon);
        app->setVersion(_node->version());
        app->setExecname(execn);
        app->setUninstName(getUninstExec(_node->uninstName()));
        app->setDate(_node->lastUpdate());
        app->setPage(0);
        app->setIndex(0);
        app->setHidden(0);
        _l->addApp(app);
        QString message = QString("%1 已被成功安装在您的系统上\n"\
                                  "您现在可以从桌面启动并运行该程序")\
                                  .arg(_node->name());
        QMessageBox::information(this, "安装成功", message, "    确定    ");
        actionButton->setEnabled(false);
        actionButton->setText("已安装");
        backButton->setText("返回");
        _node->setStatus(INSTALLED);
    }
}

void DetailWidget::onBack()
{
    if (_inProgress) {
        _inProgress = false;
        _d->close();
        updateUI();
        return;
    }
    emit cancelFromDetail();
}

QString DetailWidget::getUninstExec(QString display)
{
    QSettings reg(QSettings::NativeFormat, \
        QSettings::SystemScope, "Microsoft", KEY);
    for (int i = 0; i < reg.childGroups().count(); i++) {
        QSettings tmp(QSettings::NativeFormat, \
            QSettings::SystemScope, "Microsoft", \
            KEY + reg.childGroups().at(i)); 
        if (tmp.value("DisplayName").toString() == display) {
            return tmp.value("UninstallString").toString();
        }
    }
    return QString("");
}

QString DetailWidget::getExec(QString execname)
{
    LnkList *l = LnkList::getList();
    l->refresh();
    if (!execname.endsWith(".lnk", Qt::CaseInsensitive))
        execname.append(".lnk");
    return l->wholePath(execname);
}
