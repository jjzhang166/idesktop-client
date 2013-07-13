#include <QWidget>
#include <QEvent>
#include <QPainter>
#include <QDebug>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QUrl>
#include <QLibrary>

#include <windows.h>
#include <shellapi.h>

#include "config.h"
#include "panel.h"
#include "dynamicbutton.h"

//#include "client/appcenterwdg.h"
//#include "client/appdatareadexe.h"
#include "appmessagebox.h"

Panel::Panel(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool), \
      autoHide(true), visible(false), animation(NULL)//, _center(NULL)
//    , _vacBtnHover(true)
//    , _localBtnHover(true)
//    , _skinBtnHover(true)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0xdd));
    setPalette(pal);

    animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(300);
#if 0
    //QHBoxLayout *layout = new QHBoxLayout(this);
    //QPixmap desktopPix(":images/appbutton_localapp.png");

    _switcherPix.load(":images/appbutton_desktop.png");
    _switcherPixHover.load(":images/appbutton_desktop_hover.png");
    _switcherButton = new DynamicButton(_switcherPix, _switcherPixHover, this);
    _switcherButton->setGeometry(0, 0, 85, 86);


    _vacPix.load(":images/appbutton_vapp.png");
    _vacPixHover.load(":images/appbutton_vapp_hover.png");
    _vacButton = new DynamicButton(_vacPix, _vacPixHover, this);
    _vacButton->setGeometry(0, 86, 85, 78);
    //desktopButton->setFixedSize(99, 36);
//    layout->addWidget(desktopButton);
//    layout->setContentsMargins(10, 0, 10, 0);
//    layout->setSpacing(0);

//    _localPix(":images/appbutton_app.png");
//    _localPixHover(":images/appbutton_app_hover.png");
//    _localButton = new DynamicButton(localPix, localPixHover, this);
//    _localButton->setGeometry(0, 164, 85, 78);
//    bsButton->setFixedSize(99, 36);
//    layout->addWidget(bsButton);

    _dirPix.load(":images/appbutton_add.png");
    _dirPixHover.load(":images/appbutton_add_hover.png");
    _dirButton = new DynamicButton(_dirPix, _dirPixHover, this);
    _dirButton->setGeometry(0, 164, 85, 78);
    _dirButton->setVisible(false);

//    addButton->setFixedSize(58, 58);
//    layout->addWidget(_addButton);

    
    //layout->addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    _personPix.load(":images/appbutton_toolbox.png");
    _personPixHover.load(":images/appbutton_toolbox_hover.png");
    _personButton = new DynamicButton(_personPix, _personPixHover, this);
    //_personButton->setGeometry(0, 242, 85, 78);
    _personButton->setGeometry(0, 164, 85, 78);
//    configButton->setFixedSize(109, 36);
//    layout->addWidget(configButton);

    _quitPix.load(":images/appbutton_store.png");
    _quitPixHover.load(":images/appbutton_store_hover.png");
    _quitButton = new DynamicButton(_quitPix, _quitPixHover, this);
//    _quitButton->setGeometry(0, 320, 85, 86);
    _quitButton->setGeometry(0, 242, 85, 86);

//    quitButton->setFixedSize(69, 36);
//    layout->addWidget(quitButton);

    connect(_switcherButton, SIGNAL(clicked()), this, SLOT(switcherBtnClicked()));
    connect(_vacButton, SIGNAL(clicked()), this, SLOT(vacBtnClicked()));
//    connect(_localButton, SIGNAL(clicked()), this, SLOT(localBtnClicked()));
    connect(_dirButton, SIGNAL(clicked()), this, SLOT(dirBtnClicked()));
    connect(_personButton, SIGNAL(clicked()), this, SLOT(perBtnClicked()));
    connect(_quitButton, SIGNAL(clicked()), this, SLOT(quitBtnClicked()));
#endif


    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry();

    _switcherQuit = new Switcher(this);
    _switcherQuit->setParent(this);
    _switcherQuit->setPixmap(QString(":images/panel_quit_normal.png"));
    _switcherQuit->setGeometry(rect.width() - _switcherQuit->width(), 0
                               , _switcherQuit->width(), _switcherQuit->height());
    _switcherQuit->setVisible(true);


    _switcherSoftware = new Switcher(this);
    _switcherSoftware->setPixmap(QString(":images/panel_software_normal.png"));
    _switcherSoftware->setGeometry(rect.width() - _switcherQuit->width() - _switcherSoftware->width(), 0
                                   , _switcherSoftware->width(), _switcherSoftware->height());
    _switcherSoftware->setVisible(true);

    _switcherSkin = new Switcher(this);
    _switcherSkin->setPixmap(QString(":images/panel_skin_normal.png"));
    _switcherSkin->setGeometry(rect.width() - _switcherQuit->width() - _switcherSoftware->width() - _switcherSkin->width(), 0
                               , _switcherSkin->width(), _switcherSkin->height());
    _switcherSkin->setVisible(true);


    connect(_switcherSkin, SIGNAL(switcherActivated()), this, SLOT(skinClicked()));
    connect(_switcherSoftware, SIGNAL(switcherActivated()), this, SLOT(softwareClicked()));
    connect(_switcherQuit, SIGNAL(switcherActivated()), this, SLOT(quitClicked()));
}



void Panel::runCenter()
{
//    if (_center == NULL) {
//        _center = new AppCenterWdg();
//      //  _center->setTitle("软件中心");
//        _center->resize(753,550);
//      //  _center->move(10, 50);
//    } else {
//        _center->refresh();
//    }

/*    if (_center == NULL) {
         _center = new TestWidget();
     //    _center->setTitle("软件中心");
         _center->resize(950,630);
         _center->move(100, 50);
     }// else {
     //    _center->refresh();
    // }*/
//    _center->centerWidget();
//    _center->show();

    //emit showStore();
}

void Panel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), 28, QPixmap(":images/panel_bg.png"));

    QWidget::paintEvent(event);
}

void Panel::enterEvent(QEvent *event)
{
    emit checkDirState();
    if (!visible)
        animationShow();

    Q_UNUSED(event);
}

void Panel::leaveEvent(QEvent *event)
{
    if (autoHide)
        animationHide();

    Q_UNUSED(event);
}

void Panel::animationShow()
{
    if (animation->state() == QAbstractAnimation::Running) {
        animation->stop();
    }

//    QDesktopWidget *desktop = QApplication::desktop();
//    QRect rect = desktop->availableGeometry();

    QPoint end(0, 0);
    animation->setStartValue(pos());
    animation->setEndValue(end);
    animation->start();
    visible = true;
}

void Panel::animationHide()
{
    if (animation->state() == QAbstractAnimation::Running) {
        animation->stop();
    }
//    QDesktopWidget *desktop = QApplication::desktop();
//    QRect rect = desktop->availableGeometry();

    QPoint end(0, -27);
    animation->setStartValue(pos());
    animation->setEndValue(end);
    animation->start();
    visible = false;
}

void Panel::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Panel::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

#if 0
void Panel::addApp()
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择一个应用程序或快捷方式"),
                                             Config::get("CommonProgramDir"),
                                             tr("app (*.lnk *.exe)"));

    if (path.isEmpty())
        return;

    path.replace('/','\\');
    AppDataReadExe::Instance()->addLocalApp(path);
}
#endif

#if 0
void Panel::switcherBtnClicked()
{
//    typedef void (*tempFuc)(void);
//    QLibrary myLib;

//    #ifdef DEBUG
//        myLib.setFileName("IconGetD.dll");
//    #else
//        myLib.setFileName("IconGet.dll");
//    #endif

//	/*
//    tempFuc myFunction = (tempFuc) myLib.resolve("ToggleDesktop");
//    if (myFunction) {
//        myFunction();
//        emit desktop();
//    }
//    */
//	/*手动卸载dll */
//	/*除非unload()函数被显示调用，否则库会保留在内存中直到程序终止。*/
//    if(myLib.isLoaded ())
//    {
//        myLib.unload();
//    }
    
    emit pageChanged(0);
    emit showSwitcherDesktop();

//    _switcherButton->setCurrentPixmap(_switcherPix);
    _vacButton->setCurrentPixmap(_vacPix);
//    _localButton->setCurrentPixmap(_localPix);
    _dirButton->setCurrentPixmap(_dirPix);
    _personButton->setCurrentPixmap(_personPix);
//    _quitButton->setCurrentPixmap(_quitPix);
}

void Panel::vacBtnClicked()
{
//    emit pageChanged(2);

    emit showVacDesktop();

//    _switcherButton->setCurrentPixmap(_switcherPix);

    if (_vacBtnHover)
    {
        _vacButton->setCurrentPixmap(_vacPixHover);
        _vacBtnHover = false;
    }
    else
    {
        _vacButton->setCurrentPixmap(_vacPix);
        _vacBtnHover = true;
    }

//    _localButton->setCurrentPixmap(_localPix);
    _dirButton->setCurrentPixmap(_dirPix);
    _personButton->setCurrentPixmap(_personPix);
//    _quitButton->setCurrentPixmap(_quitPix);

}

void Panel::localBtnClicked()
{
//    emit pageChanged(2);

//    emit showLocalDesktop();

//    _switcherButton->setCurrentPixmap(_switcherPix);
//    _vacButton->setCurrentPixmap(_vacPix);
//    if (_localBtnHover)
//    {
//        _localButton->setCurrentPixmap(_localHoverPix);
//        _localBtnHover = false;
//    }
//    else
//    {
//        _localButton->setCurrentPixmap(_localPix);
//        _localBtnHover = true;
//    }
//    _dirButton->setCurrentPixmap(_dirPix);
//    _personButton->setCurrentPixmap(_personPix);
//    _quitButton->setCurrentPixmap(_quitPix);

}

void Panel::dirBtnClicked()
{
//    emit pageChanged(2);

    emit showDirDesktop();

//    _switcherButton->setCurrentPixmap(_switcherPix);
    _vacButton->setCurrentPixmap(_vacPix);
//    _localButton->setCurrentPixmap(_localPix);
//    _dirButton->setCurrentPixmap(_dirPix);
    _personButton->setCurrentPixmap(_personPix);
//    _quitButton->setCurrentPixmap(_quitPix);
}

void Panel::perBtnClicked()
{
    emit showPerDesktop();
#if 0
//    _switcherButton->setCurrentPixmap(_switcherPix);
    _vacButton->setCurrentPixmap(_vacPix);
//    _localButton->setCurrentPixmap(_localPix);
    _dirButton->setCurrentPixmap(_dirPix);
    if (_skinBtnHover)
    {
        _personButton->setCurrentPixmap(_personPixHover);
        _skinBtnHover = false;
    }
    else
    {
       _personButton->setCurrentPixmap(_personPix);
       _skinBtnHover = true;
    }
//    _quitButton->setCurrentPixmap(_quitPix);
#endif
}

void Panel::quitBtnClicked()
{
    emit quit();
#if 0
//    _switcherButton->setCurrentPixmap(_switcherPix);
    _vacButton->setCurrentPixmap(_vacPix);
//    _localButton->setCurrentPixmap(_localPix);
    _dirButton->setCurrentPixmap(_dirPix);
    _personButton->setCurrentPixmap(_personPix);

//    _quitButton->setCurrentPixmap(_quitPix);
#endif
}

/*
void Panel::showToolDesktop()
{
    emit pageChanged(3);
}
*/
//void Panel::showManageDesktop()
//{
//    emit pageChanged(2);
//}

#endif

void Panel::skinClicked()
{
    emit showSkinWidget();
}

void Panel::softwareClicked()
{
    emit showSoftwareWidget();
}

void Panel::quitClicked()
{
    emit quit();
}

QPixmap Panel::setTransparentPixmap(const QString &pix)
{
    QImage normal = QImage(pix);

    for (int i = 0; i < normal.width(); i++) {
        for (int j = 0; j < normal.height(); j++) {
            QRgb pixel = normal.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel) * 0.6, qGreen(pixel) * 0.6, \
                                    qBlue(pixel) * 0.6, a * 180 / 255);
            normal.setPixel(i, j, lightPixel);
        }
    }
    return QPixmap::fromImage(normal);
}
