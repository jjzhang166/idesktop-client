#include "configmenu.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QPainter>
#include <QWidget>
#include <QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTextCodec>
#include <QMouseEvent>
#include <QBitmap>
#include <QtDebug>
#include "config.h"


ConfigMenu::ConfigMenu(QWidget *parent)
    : QLabel(parent)
    , _enter(false)
{
    setFixedSize(143, 83);

    this->setAutoFillBackground(true);

    setStyleSheet("border: 0px solid gray;background:rgba(255,255,255,0);");

    _skinAc = new QPushButton(tr("换皮肤"), this);
    _skinAc->setGeometry(0, 0, 143, 42);
    _skinAc->setStyleSheet("QPushButton{background-image:url(:/images/menu_top.png);border-style:flat;color:white;}\
                           QPushButton:hover:pressed{border-image:url(:/images/menu_top.png);border-style:flat;color:white;}\
                           QPushButton:hover:!pressed{border-image:url(:/images/menu_top_hover.png);border-style:flat;color:white;}");
    _bsStyleAc = new QPushButton(tr("B/S应用样式"), this);
    _bsStyleAc->setGeometry(0, 42, 143, 41);
    _bsStyleAc->setStyleSheet("QPushButton{background-image:url(:/images/menu_bottom.png);border-style:flat;color:white;}\
                              QPushButton:hover:pressed{border-image:url(:/images/menu_bottom.png);border-style:flat;color:white;}\
                              QPushButton:hover:!pressed{border-image:url(:/images/menu_bottom_hover.png);border-style:flat;color:white;}");
//    _serverAc = new QPushButton(tr("服务器设置"), this);
//    _serverAc->setGeometry(0, 76, 143, 41);
//    _serverAc->setStyleSheet("QPushButton{background-image:url(:/images/menu_bottom.png);border-style:flat;color:white;}\
//                             QPushButton:hover:pressed{border-image:url(:/images/menu_bottom.png);border-style:flat;color:white;}\
//                             QPushButton:hover:!pressed{border-image:url(:/images/menu_bottom_hover.png);border-style:flat;color:white;}");

    connect(_skinAc, SIGNAL(clicked()), this, SLOT(setSkin()));
    connect(_bsStyleAc, SIGNAL(clicked()), this, SLOT(setBsStyle()));
//    connect(_serverAc, SIGNAL(clicked()), this, SLOT(setServer()));

}

ConfigMenu::~ConfigMenu()
{

}

//void ConfigMenu::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.drawPixmap(0, 0,QPixmap(":/images/menu_top.png"));
//    painter.drawPixmap(0, 42,QPixmap(":/images/menu_center.png"));
//    painter.drawPixmap(0, 76,QPixmap(":/images/menu_bottom.png"));
//}

void ConfigMenu::enterEvent(QEvent *event)
{
    _enter = true;
    QLabel::enterEvent(event);
}

void ConfigMenu::leaveEvent(QEvent *event)
{
    _enter = false;
    QLabel::leaveEvent(event);
}

bool ConfigMenu::mouseEnter()
{
    return _enter;
}

void ConfigMenu::setSkin()
{
    _skinWidget = new SkinWidget();
    _skinWidget->show();

    connect(_skinWidget, SIGNAL(setBgPixmap(const QString&)), this, SIGNAL(setBgPixmap(const QString&)));

    close();
}

void ConfigMenu::setBsStyle()
{
    close();
}

void ConfigMenu::setServer()
{
    _serverStgWidget = new ServerStgWidget();
    _serverStgWidget->show();
    connect(_serverStgWidget, SIGNAL(upLoad()), this, SIGNAL(upLoad()));

    close();
}

/*
void ConfigMenu::backupData()
{
    this->hide();

    QString sourceFile = Config::get("LocalDb");

    QString path = QFileDialog::getExistingDirectory(this, tr("选择一个备份本地桌面的位置"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

   if (path.isEmpty())
        return;

    if (!QFile::exists(sourceFile)){
        return;
    }
    path.replace('/','\\');
    QFileInfo fi(sourceFile);
    QDir toDir(path);

    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd");
    QFileInfo dt(str);

    if(toDir.exists(dt.fileName())){
        qDebug() <<dt.fileName()<<"已经存在，覆盖。";
        toDir.remove(dt.fileName());
    }

    if (!QFile::copy(fi.filePath(), toDir.filePath(dt.fileName()))){
        return;
    }
}

void ConfigMenu::recoverData()
{

}

void ConfigMenu::backupBs()
{

}

void ConfigMenu::recoverBs()
{

}

void ConfigMenu::setConfigure()
{

}
*/
//ConfigMenu::ConfigMenu(QWidget *parent) :
//    QMenu(parent)
//{
//    setFixedSize(143, 117);

//    //QPixmap mask(":/images/menu_top.png");
//    //mask = mask.scaled(143, 42, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//    //setMask(QBitmap(mask.mask()));
//    QPalette p;
//    p.setBrush(QPalette::Window, QBrush(QPixmap(":/images/menu_top.png")));
//    setPalette(p);


////    this->setAutoFillBackground(true);
////    setStyleSheet("\
////                  QMenu::item {border-image: url(:/images/menu_center.png);\
////                               position:absolute; top:0px; right:100px;\
////                  padding: 6px 40px 6px 200px; border: 0px solid transparent;color:white}\
////      QMenu::item:selected{border-image: url(:/images/menu_center_hover.png);color: white}\
////        QMenu::item:!enabled{color: gray}");
//    //setStyleSheet("border: 0px solid gray;background:rgba(255,255,255,0);");


//    _skinAc = new QAction(tr("换皮肤"), this);

//    _bsStyleAc = new QAction(tr("B/S应用样式"), this);
//    _serverAc = new QAction(tr("服务器设置"), this);
///*
//    _dataBackupAc = new QAction(tr("备份本地桌面"), this);
//    _dataRecoverAc= new QAction(tr("还原本地桌面"), this);
//    _bsBackupAc = new QAction(tr("备份服务组装"), this);
//    _bsRecoverAc = new QAction(tr("还原服务组装"), this);
//    _configureAc = new QAction(tr("配置中心"), this);
//*/
//    addAction(_skinAc);
//    addAction(_bsStyleAc);
//    addAction(_serverAc);
///*
//    _dataMenu = new QMenu(tr("本地桌面管理"), this);
//    _dataMenu->addAction(_dataBackupAc);
//    _dataMenu->addAction(_dataRecoverAc);

//    _bsMenu = new QMenu(tr("服务组装管理"), this);
//    _bsMenu->addAction(_bsBackupAc);
//    _bsMenu->addAction(_bsRecoverAc);

//    addMenu(_dataMenu);
//    addMenu(_bsMenu);
//    addAction(_configureAc);
//*/
//    connect(_skinAc, SIGNAL(triggered()), this, SLOT(setSkin()));
//    connect(_bsStyleAc, SIGNAL(triggered()), this, SLOT(setBsStyle()));
//    connect(_serverAc, SIGNAL(triggered()), this, SLOT(setServer()));
///*
//    connect(_dataBackupAc, SIGNAL(triggered()), this, SLOT(backupData()));
//    connect(_dataRecoverAc, SIGNAL(triggered()), this, SLOT(recoverData()));
//    connect(_bsBackupAc, SIGNAL(triggered()), this, SLOT(backupBs()));
//    connect(_bsRecoverAc, SIGNAL(triggered()), this, SLOT(recoverBs()));
//    connect(_configureAc, SIGNAL(triggered()), this, SLOT(setConfigure()));
//*/
//}

//ConfigMenu::~ConfigMenu()
//{

//}
///*
//void ConfigMenu::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.drawPixmap(0, 0,QPixmap(":/images/menu_top.png"));
//    painter.drawPixmap(0, 42,QPixmap(":/images/menu_center.png"));
//    painter.drawPixmap(0, 76,QPixmap(":/images/menu_bottom.png"));
//}
//*/
//void ConfigMenu::setSkin()
//{
//    _skinWidget = new SkinWidget();
//    _skinWidget->show();

//    connect(_skinWidget, SIGNAL(setBgPixmap(const QString&)), this, SIGNAL(setBgPixmap(const QString&)));
//}

//void ConfigMenu::setBsStyle()
//{

//}

//void ConfigMenu::setServer()
//{
//    _serverStgWidget = new ServerStgWidget();
//    _serverStgWidget->show();
//    connect(_serverStgWidget, SIGNAL(upLoad()), this, SIGNAL(upLoad()));
//}

///*
//void ConfigMenu::backupData()
//{
//    this->hide();

//    QString sourceFile = Config::get("LocalDb");

//    QString path = QFileDialog::getExistingDirectory(this, tr("选择一个备份本地桌面的位置"),
//                                                "/home",
//                                                QFileDialog::ShowDirsOnly
//                                                | QFileDialog::DontResolveSymlinks);

//   if (path.isEmpty())
//        return;

//    if (!QFile::exists(sourceFile)){
//        return;
//    }
//    path.replace('/','\\');
//    QFileInfo fi(sourceFile);
//    QDir toDir(path);

//    QDateTime time = QDateTime::currentDateTime();
//    QString str = time.toString("yyyy-MM-dd");
//    QFileInfo dt(str);

//    if(toDir.exists(dt.fileName())){
//        qDebug() <<dt.fileName()<<"已经存在，覆盖。";
//        toDir.remove(dt.fileName());
//    }

//    if (!QFile::copy(fi.filePath(), toDir.filePath(dt.fileName()))){
//        return;
//    }
//}

//void ConfigMenu::recoverData()
//{

//}

//void ConfigMenu::backupBs()
//{

//}

//void ConfigMenu::recoverBs()
//{

//}

//void ConfigMenu::setConfigure()
//{

//}
//*/

/************************** 换皮肤 *************************/
SkinWidget::SkinWidget(QWidget *parent)
    : QWidget(parent)
    , _pageNum(0)
{
    resize(790, 450);

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    _skinPixWidget = new SkinPixWidget(this);
    _skinPixWidget->move(0,0);

    _scrollBar = new QScrollBar(this);

    if (_skinPixWidget->height() % height() == 0)
        _pageNum = _skinPixWidget->height() / height();
    else
        _pageNum = _skinPixWidget->height() / height() + 1;

    _scrollBar->setSingleStep(height() -10);
    _scrollBar->setPageStep(height() - 10);

    _animation = new QPropertyAnimation(_skinPixWidget, "geometry");

    connect(_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
    connect(_skinPixWidget, SIGNAL(setBgPixmap(QString)), this, SIGNAL(setBgPixmap(QString)));
}

void SkinWidget::scrollBarValueChanged(int val)
{

    if ((_skinPixWidget->pos().y() + val) % (height() - 10) != 0)
    {
        return;
    }

    if (_animation->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animation->setDuration(80);
    _animation->setStartValue(QRect(0, _skinPixWidget->pos().y(), _skinPixWidget->width(), _skinPixWidget->height()));
    _animation->setEndValue(QRect(0, -val, _skinPixWidget->width(), _skinPixWidget->height()));

    _animation->start();
}

void SkinWidget::resizeEvent(QResizeEvent *event)
{
    int w = 15;
    int x = width() - w;
    x = x < 0 ? 0: x;
    int h = height();
    _scrollBar->setGeometry(x, 0, w, h);

    _scrollBar->setRange(0, _pageNum * height() - h - 10);
}

void SkinWidget::wheelEvent(QWheelEvent *event)
{
    _scrollBar->event(event);
}

SkinPixWidget::SkinPixWidget(QWidget *parent)
    : QWidget(parent)
    , _column(0)
    , _rowNum(0)
{
   resize(770, 890);

    for (int i = 0; i < 30; i++)
    {
        _pixmapWidget = new PixmapWidget(QString(":/images/wallpager/wp_%1.jpg").arg(i), this);

        _pixmapWidget->setSize(180, 100);

        if ((_rowNum + 1) * (10 + _pixmapWidget->getWidth()) >= this->width())
        {
            _rowNum = 0;
            _column++;
        }

         _pixmapWidget->move(((_rowNum + 1) *10 + _pixmapWidget->getWidth() * _rowNum),
                             ((_column + 1) * 10 + _pixmapWidget->getHeight() * _column));
         _rowNum++;


        connect(_pixmapWidget, SIGNAL(mouseClicked(const QString&)), \
                        this, SIGNAL(setBgPixmap(const QString&)));
    }

}

PixmapWidget::PixmapWidget(QString pixText, QWidget *parent)
    : QWidget(parent)
    , _pixText(pixText)
    , _width(180)
    , _height(100)
{
    _pixmap.load(_pixText);
}

void PixmapWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _width, _height, _pixmap);

    QWidget::paintEvent(event);
}

void PixmapWidget::setSize(int w, int h)
{
    _width = w;
    _height = h;
    resize(_width, _height);
    update();
}

QString PixmapWidget::getPixmapText()
{
    return _pixText;
}

int PixmapWidget::getWidth()
{
    return _width;
}

int PixmapWidget::getHeight()
{
    return _height;
}

void PixmapWidget::mousePressEvent(QMouseEvent *event)
{
    emit mouseClicked(_pixText);

    QWidget::mousePressEvent(event);
}

/************************** 服务器设置 *************************/
ServerStgWidget::ServerStgWidget(QWidget *parent)
    : QWidget(parent)
{
    resize(395, 300);

    _virtualLabel = new QLabel(tr("虚拟应用服务器："), this);
    _virtualLabel->move(20, height() / 9);

    _serviceLabel = new QLabel(tr("服务平台服务器："), this);
    _serviceLabel->move(20, height() / 3);

    _virtualEdit = new QLineEdit(this);
    _virtualEdit->setGeometry(150, height() / 9 - 10, 208, 35);
    _virtualEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:2px;");

    _serviceEdit = new QLineEdit(this);
    _serviceEdit->setGeometry(150, height() / 3 - 10, 208, 35);
    _serviceEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0);margin-left:2px;");

    _saveBtn = new QPushButton(tr("应用"), this);
    _saveBtn->move(220, height() / 9 * 7);

    _cancelBtn = new QPushButton(tr("取消"), this);
    _cancelBtn->move(300, height() / 9 * 7);

/*
    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT virtualurl FROM urls where id=\'%1\';").arg(1));
    while (query.next())
        _virtualEdit->setText(query.value(0).toString());
    query = QSqlDatabase::database("local").exec(QString("SELECT serviceurl FROM urls where id=\'%1\';").arg(1));
        while (query.next())
            _serviceEdit->setText(query.value(0).toString());
*/
    connect(_saveBtn, SIGNAL(clicked()), this, SLOT(saveIp()));
    connect(_cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
}

void ServerStgWidget::saveIp()
{
    qDebug() << "1232132312321";
    QSqlQuery query(QSqlDatabase::database("local"));

    QString qstr = QString("update urls "\
                           "set virtualurl=\'%1\', serviceurl=\'%2\' where id=1;")\
                           .arg(_virtualEdit->text()).arg(_serviceEdit->text());
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return;
    }

    emit upLoad();

    close();
}

void ServerStgWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(0, 0, width(), height());
    painter.drawPixmap(150, height() / 9 - 10, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(150, height() / 3 - 10, QPixmap(":images/login_input_bg.png"));

    QWidget::paintEvent(event);
}
