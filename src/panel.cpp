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
    QPixmap desktopPix(":images/appbutton_localapp_hover.png");
    QPixmap desktopPixHover(":images/appbutton_localapp_hover.png");
    desktopButton = new DynamicButton(desktopPix, desktopPixHover, this);
    desktopButton->setGeometry(0, 0, 99, 39);
    //desktopButton->setFixedSize(99, 36);
//    layout->addWidget(desktopButton);
//    layout->setContentsMargins(10, 0, 10, 0);
//    layout->setSpacing(0);

    QPixmap bsPix(":images/appbutton_bs.png");
    QPixmap bsPixHover(":images/appbutton_bs_hover.png");
    bsButton = new DynamicButton(bsPix, bsPixHover, this);
    bsButton->setGeometry(99, 0, 108, 39);
//    bsButton->setFixedSize(99, 36);
//    layout->addWidget(bsButton);

//    QPixmap managePix(":images/appbutton_manage.png");
//    QPixmap managePixHover(":images/appbutton_manage_hover.png");
//    DynamicButton *manageButton = new DynamicButton(managePix, managePixHover, this);
//    //appButton->setGeometry(76, 0, 58, 58);
//    manageButton->setFixedSize(58, 58);
//    layout->addWidget(manageButton);
/*
    QPixmap vappPix(":images/appbutton_vapp.png");
    QPixmap vappPixHover(":images/appbutton_vapp_hover.png");
    DynamicButton *vappButton = new DynamicButton(vappPix, vappPixHover, this);
    //appButton->setGeometry(76, 0, 58, 58);
    vappButton->setFixedSize(58, 58);
    layout->addWidget(vappButton);
	
    QPixmap toolPix(":images/appbutton_toolbox.png");
    QPixmap toolPixHover(":images/appbutton_toolbox_hover.png");
    DynamicButton *toolButton = new DynamicButton(toolPix, toolPixHover, this);
    //appButton->setGeometry(76, 0, 58, 58);
    toolButton->setFixedSize(58, 58);
    layout->addWidget(toolButton);

    QPixmap storePix(":images/appbutton_store.png");
    QPixmap storePixHover(":images/appbutton_store_hover.png");
    DynamicButton *storeButton = new DynamicButton(storePix, storePixHover, this);
    //appButton->setGeometry(76, 0, 58, 58);
    storeButton->setFixedSize(58, 58);
    layout->addWidget(storeButton);
*/
    /*
    QPixmap addPix(":images/appbutton_add.png");
    QPixmap addPixHover(":images/appbutton_add_hover.png");
    DynamicButton *addButton = new DynamicButton(addPix, addPixHover, this);
    //addButton->setGeometry(146, 0, 58, 58);
    addButton->setFixedSize(58, 58);
    layout->addWidget(addButton);
    */
    
    //layout->addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QPixmap personPix(":images/appbutton_config.png");
    QPixmap personPixHover(":images/appbutton_config_hover.png");
    personButton = new DynamicButton(personPix, personPixHover, this);
    personButton->setGeometry(207, 0, 117, 39);
//    configButton->setFixedSize(109, 36);
//    layout->addWidget(configButton);

    QPixmap quitPix(":images/appbutton_quit.png");
    QPixmap quitPixHover(":images/appbutton_quit_hover.png");
    quitButton = new DynamicButton(quitPix, quitPixHover, this);
    quitButton->setGeometry(324, 0, 117, 39);
//    quitButton->setFixedSize(69, 36);
//    layout->addWidget(quitButton);

    connect(desktopButton, SIGNAL(clicked()), this, SLOT(showDesktop()));
    //connect(vappButton, SIGNAL(clicked()), this, SLOT(showVappDesktop()));
    connect(bsButton, SIGNAL(clicked()), this, SLOT(bsBtnClicked()));
    //connect(toolButton, SIGNAL(clicked()), this, SLOT(showToolDesktop()));
    //connect(addButton, SIGNAL(clicked()), this, SLOT(addApp()));
    //connect(storeButton, SIGNAL(clicked()), this, SLOT(runCenter()));
    //connect(manageButton, SIGNAL(clicked()), this, SLOT(showManageDesktop()));
    connect(personButton, SIGNAL(clicked()), this, SLOT(perBtnClicked()));
    connect(quitButton, SIGNAL(clicked()), this, SIGNAL(quit()));
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

    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry();

    QPoint end(x(), rect.height() - height() - 10);
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
    //QPoint end(x(), -(height() - 2));
    QPoint end(x(), rect.height() + height());
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

void Panel::showDesktop()
{
    typedef void (*tempFuc)(void);
    QLibrary myLib;

    #ifdef DEBUG
        myLib.setFileName("IconGetD.dll");
    #else
        myLib.setFileName("IconGet.dll");
    #endif

	/*
    tempFuc myFunction = (tempFuc) myLib.resolve("ToggleDesktop");
    if (myFunction) {
        myFunction();
        emit desktop();
    }
    */
	/*手动卸载dll */
	/*除非unload()函数被显示调用，否则库会保留在内存中直到程序终止。*/
    if(myLib.isLoaded ())
    {
        myLib.unload();
    }
    
	emit pageChanged(0);

    bsButton->setcurrentPixmap(QPixmap(":images/appbutton_bs.png"));
    personButton->setcurrentPixmap(QPixmap(":images/appbutton_config.png"));

    desktopButton->setcurrentPixmap(QPixmap(":images/appbutton_localapp_hover.png"));

}
/*
void Panel::showVappDesktop()
{
    emit pageChanged(2);
}
*/
void Panel::perBtnClicked()
{
    desktopButton->setcurrentPixmap(QPixmap(":images/appbutton_localapp.png"));
    bsButton->setcurrentPixmap(QPixmap(":images/appbutton_bs.png"));

    personButton->setcurrentPixmap(QPixmap(":images/appbutton_config_hover.png"));
    emit showPerDesktop();
}

void Panel::bsBtnClicked()
{
    desktopButton->setcurrentPixmap(QPixmap(":images/appbutton_localapp.png"));
    personButton->setcurrentPixmap(QPixmap(":images/appbutton_config.png"));

    bsButton->setcurrentPixmap(QPixmap(":images/appbutton_bs_hover.png"));

    emit showBsDesktop();
    //emit pageChanged(1);
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

