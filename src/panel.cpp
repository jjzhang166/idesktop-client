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
      autoHide(false), visible(false), animation(NULL)//, _center(NULL)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0xdd));
    setPalette(pal);

    animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(300);

    //QHBoxLayout *layout = new QHBoxLayout(this);
    //QPixmap desktopPix(":images/appbutton_localapp.png");

    QPixmap switcherPix(":images/appbutton_desktop.png");
    QPixmap switcherPixHover(":images/appbutton_desktop_hover.png");
    _switcherButton = new DynamicButton(switcherPix, switcherPixHover, this);
    _switcherButton->setGeometry(0, 0, 85, 86);

    QPixmap vacPix(":images/appbutton_vapp.png");
    QPixmap vacPixHover(":images/appbutton_vapp_hover.png");
    _vacButton = new DynamicButton(vacPix, vacPixHover, this);
    _vacButton->setGeometry(0, 86, 85, 78);
    //desktopButton->setFixedSize(99, 36);
//    layout->addWidget(desktopButton);
//    layout->setContentsMargins(10, 0, 10, 0);
//    layout->setSpacing(0);

    QPixmap localPix(":images/appbutton_app.png");
    QPixmap localPixHover(":images/appbutton_app_hover.png");
    _localButton = new DynamicButton(localPix, localPixHover, this);
    _localButton->setGeometry(0, 164, 85, 78);
//    bsButton->setFixedSize(99, 36);
//    layout->addWidget(bsButton);

    QPixmap dirPix(":images/appbutton_add.png");
    QPixmap dirPixHover(":images/appbutton_add_hover.png");
    _dirButton = new DynamicButton(dirPix, dirPixHover, this);
    _dirButton->setGeometry(0, 242, 85, 78);
//    addButton->setFixedSize(58, 58);
//    layout->addWidget(_addButton);

    
    //layout->addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QPixmap personPix(":images/appbutton_toolbox.png");
    QPixmap personPixHover(":images/appbutton_toolbox_hover.png");
    _personButton = new DynamicButton(personPix, personPixHover, this);
    _personButton->setGeometry(0, 320, 85, 78);
//    configButton->setFixedSize(109, 36);
//    layout->addWidget(configButton);

    QPixmap quitPix(":images/appbutton_store.png");
    QPixmap quitPixHover(":images/appbutton_store_hover.png");
    _quitButton = new DynamicButton(quitPix, quitPixHover, this);
    _quitButton->setGeometry(0, 398, 85, 86);

//    quitButton->setFixedSize(69, 36);
//    layout->addWidget(quitButton);

    connect(_switcherButton, SIGNAL(clicked()), this, SLOT(switcherBtnClicked()));
    connect(_vacButton, SIGNAL(clicked()), this, SLOT(vacBtnClicked()));
    connect(_localButton, SIGNAL(clicked()), this, SLOT(localBtnClicked()));
    connect(_dirButton, SIGNAL(clicked()), this, SLOT(dirBtnClicked()));
    connect(_personButton, SIGNAL(clicked()), this, SLOT(perBtnClicked()));
    connect(_quitButton, SIGNAL(clicked()), this, SIGNAL(quit()));
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

//void Panel::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.drawPixmap(0, 0, 7, 41, QPixmap(":images/topNav_left.png"));
//    painter.drawPixmap(width() - 7, 0, 7, 41, QPixmap(":images/topNav_right.png"));
//    painter.drawPixmap(7, 0, QPixmap(":images/topNav_bg_center.png").scaled(width() - 14, 41));
//}

void Panel::enterEvent(QEvent *event)
{
    if (!visible)
        animationShow();

    qDebug() << "enterEvententerEvententerEvententerEvententerEvententerEvententerEvententerEvent";

    Q_UNUSED(event);
}

void Panel::leaveEvent(QEvent *event)
{
    if (autoHide)
        animationHide();

    qDebug() << "leaveEventleaveEventleaveEventleaveEventleaveEventleaveEventleaveEventleaveEventleaveEvent";

    Q_UNUSED(event);
}

void Panel::animationShow()
{
    if (animation->state() == QAbstractAnimation::Running) {
        animation->stop();
    }

    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry();

    QPoint end(rect.width() - width(), y());
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
    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry();

    QPoint end(rect.width() - 9, y());
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

    _switcherButton->setcurrentPixmap(QPixmap(":images/appbutton_desktop_hover.png"));
    _vacButton->setcurrentPixmap(QPixmap(":images/appbutton_vapp.png"));
    _localButton->setcurrentPixmap(QPixmap(":images/appbutton_app.png"));
    _dirButton->setcurrentPixmap(QPixmap(":images/appbutton_add.png"));
    _personButton->setcurrentPixmap(QPixmap(":images/appbutton_toolbox.png"));
    _quitButton->setcurrentPixmap(QPixmap(":images/appbutton_store.png"));

}

void Panel::vacBtnClicked()
{
//    emit pageChanged(2);

    emit showVacDesktop();

    _switcherButton->setcurrentPixmap(QPixmap(":images/appbutton_desktop.png"));
    _vacButton->setcurrentPixmap(QPixmap(":images/appbutton_vapp_hover.png"));
    _localButton->setcurrentPixmap(QPixmap(":images/appbutton_app.png"));
    _dirButton->setcurrentPixmap(QPixmap(":images/appbutton_add.png"));
    _personButton->setcurrentPixmap(QPixmap(":images/appbutton_toolbox.png"));
    _quitButton->setcurrentPixmap(QPixmap(":images/appbutton_store.png"));

}
void Panel::localBtnClicked()
{
//    emit pageChanged(2);

    emit showLocalDesktop();

    _switcherButton->setcurrentPixmap(QPixmap(":images/appbutton_desktop.png"));
    _vacButton->setcurrentPixmap(QPixmap(":images/appbutton_vapp.png"));
    _localButton->setcurrentPixmap(QPixmap(":images/appbutton_app_hover.png"));
    _dirButton->setcurrentPixmap(QPixmap(":images/appbutton_add.png"));
    _personButton->setcurrentPixmap(QPixmap(":images/appbutton_toolbox.png"));
    _quitButton->setcurrentPixmap(QPixmap(":images/appbutton_store.png"));

}

void Panel::dirBtnClicked()
{
//    emit pageChanged(2);

    emit showDirDesktop();

    _switcherButton->setcurrentPixmap(QPixmap(":images/appbutton_desktop.png"));
    _vacButton->setcurrentPixmap(QPixmap(":images/appbutton_vapp.png"));
    _localButton->setcurrentPixmap(QPixmap(":images/appbutton_app.png"));
    _dirButton->setcurrentPixmap(QPixmap(":images/appbutton_add_hover.png"));
    _personButton->setcurrentPixmap(QPixmap(":images/appbutton_toolbox.png"));
    _quitButton->setcurrentPixmap(QPixmap(":images/appbutton_store.png"));

}

void Panel::perBtnClicked()
{
    emit showPerDesktop();

    _switcherButton->setcurrentPixmap(QPixmap(":images/appbutton_desktop.png"));
    _vacButton->setcurrentPixmap(QPixmap(":images/appbutton_vapp.png"));
    _localButton->setcurrentPixmap(QPixmap(":images/appbutton_app.png"));
    _dirButton->setcurrentPixmap(QPixmap(":images/appbutton_add.png"));
    _personButton->setcurrentPixmap(QPixmap(":images/appbutton_toolbox_hover.png"));
    _quitButton->setcurrentPixmap(QPixmap(":images/appbutton_store.png"));

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

