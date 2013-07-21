#include <QPainter>
#include <QFontMetrics>
#include <QLabel>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include "contextmenuwidget.h"
//miya add
#include <QMessageBox>
#include <QDesktopServices>

//#define ICONTOP_H 54
//#define ICONCENTER_H 42
//#define ICONBOTTOM_H 55
#define BTN_H 19
#define ICON_W 162

//ContextMenuWidget::ContextMenuWidget(menu_type & type, QWidget *parent)
//    : QWidget(parent)
//    ,_type(type)
//{
//    switch(_type)
//    {
//        case normal :
//            _normalMenu = new MenuWidget(normal,this);

//            connect(_normalMenu, SIGNAL(showIcons()), this, SLOT(showIcons()));
//            connect(_normalMenu, SIGNAL(createIcons()), this, SLOT(createIcons()));
//            connect(_normalMenu, SIGNAL(leaveShowIcons()), this, SLOT(leaveShowIcons()));
//            connect(_normalMenu, SIGNAL(leaveCreateIcons()), this, SLOT(leaveCreateIcons()));
//            connect(_normalMenu, SIGNAL(changeSkin()), this, SIGNAL(changeSkin()));
//            connect(_normalMenu, SIGNAL(desktopTheme()), this, SIGNAL(desktopTheme()));
//            connect(_normalMenu, SIGNAL(del()), this, SIGNAL(del()));
//            connect(_normalMenu, SIGNAL(refresh()), this, SIGNAL(refresh()));

//            break;
//        case showIcon :
//            _showIconMenu = new MenuWidget(showIcon, this);

//            connect(_showIconMenu, SIGNAL(bigIcon()), this, SIGNAL(bigIcon()));
//            connect(_showIconMenu, SIGNAL(hitIcon()), this, SIGNAL(hitIcon()));
//            connect(_showIconMenu, SIGNAL(smallIcon()), this, SIGNAL(smallIcon()));

//            break;
//        case create :
//            _createMenu = new MenuWidget(create, this);

//            connect(_createMenu, SIGNAL(createDir()), this, SIGNAL(createDir()));
//            connect(_createMenu, SIGNAL(createLink()), this, SIGNAL(createLink()));
//            connect(_createMenu, SIGNAL(createDOC()), this, SIGNAL(createDOC()));
//            connect(_createMenu, SIGNAL(createEXCEL()), this, SIGNAL(createEXCEL()));
//            connect(_createMenu, SIGNAL(createPPT()), this, SIGNAL(createPPT()));

//            break;

//        default:
//            break;
//    }
//}

//ContextMenuWidget::~ContextMenuWidget()
//{

//}

//void ContextMenuWidget::showIcons()
//{
//    _showIconMenu->setGeometry(_normalMenu->pos().x() + ICON_W - 40, _normalMenu->pos().y(),
//                               _showIconMenu->width(), _showIconMenu->height());
//    _showIconMenu->setVisible(true);
//}

//void ContextMenuWidget::createIcons()
//{
//    _createMenu->setGeometry(_normalMenu->pos().x() + ICON_W - 40, _normalMenu->pos().y() + ICONTOP_H,
//                             _createMenu->width(), _createMenu->height());
//    _createMenu->setVisible(true);
//}

//void ContextMenuWidget::leaveShowIcons()
//{
//    _showIconMenu->setVisible(false);
//}

//void ContextMenuWidget::leaveCreateIcons()
//{
//    _createMenu->setVisible(false);
//}

//miya add
QStringList MenuWidget::getAppList()
{
    qDebug() << "getNewList";
    QString newListPath("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\PostSetup\\ShellNew");
    QSettings reg(newListPath, QSettings::NativeFormat);
    QStringList list = reg.allKeys();
    QString strKey;
    QStringList strValue;
    int nCount = list.count();
    qDebug() << "nCount : " << nCount;
    strKey = list.at(0);
    strValue = reg.value(strKey).toStringList();
    qDebug() << "strKey aaaaaa: " << strKey;
start:
    int len = strValue.length();
    for(int i = 0; i < len; ++i) {
        if((strValue.at(i) == "Folder") && i != 0) {
            strValue.removeAt(i);
            strValue.insert(0, "Folder");
            goto start;
        } else if((strValue.at(i) == ".lnk") && (i != 1)) {
            strValue.removeAt(i);
            strValue.insert(1, ".lnk");
            goto start;
        } else if((strValue.at(i) == "Briefcase") && (i != len - 1)) {
            strValue.removeAt(i);
            goto start;
        } else if((strValue.at(i) == ".library-ms")) {
            strValue.removeAt(i);
            goto start;
        } else if((strValue.at(i) == ".contact")) {
            strValue.removeAt(i);
            goto start;
        }
    }
    len = strValue.length();
    qDebug() << "after aaaaaaaaacccccc";
    for(int j = 0; j < len ; j++)
        qDebug() << j << " " << strValue.at(j);

    return strValue;
}

QStringList MenuWidget::getAppName()
{
    QStringList appList = getAppList();
    qDebug() << "in getAppName";
    int nCount = appList.length();
    QString tempPath;
    QSettings *settings;
    QSettings *reg;
    QStringList list;
    QString appName;
    QString strKey;
    QString strValue;
    QStringList strAppName;
    for(int i = 0; i < nCount; ++i)
    {
        tempPath = "HKEY_CLASSES_ROOT\\";
        tempPath += appList.at(i);
        //qDebug() << "tempPath " << tempPath << "index " << i << "nCount " << nCount;
        settings = new QSettings(tempPath, QSettings::NativeFormat);
        list = settings->allKeys();
        strKey = list.at(0);
        strValue = settings->value(strKey).toString();
        //qDebug() << "strValue" << strValue ;
        tempPath = "HKEY_CLASSES_ROOT\\";
        tempPath += strValue;
        //qDebug() << "new tempPath" << tempPath;
        reg = new QSettings(tempPath, QSettings::NativeFormat);
        list = reg->allKeys();
        strKey = list.at(0);
        strValue = reg->value(strKey).toString();
        if(strValue == "Folder") {
            strValue = tr("文件夹");
        } else if(strValue == "Shortcut") {
            strValue = tr("快捷方式…");
        } else if(strValue == "Briefcase") {
            strValue = tr("公文包");
        } else if (strValue == "Bitmap Image") {
            strValue = tr("BMP 图像");
        } else if (strValue == "Contact File") {
            strValue = tr("联系人");
        } else if (strValue == "Journal Document") {
            strValue = tr("日记本文档");
        } else if (strValue == "Text Document") {
            strValue = tr("文本文档");
        }

        strAppName.append(strValue);
        qDebug() << "new name" << strValue;
     }
    return strAppName;
}
int MenuWidget::getAppCount()
{
    if(_appList.length() == 0)
        return 0;
    if(_appList.length() == _appName.length()) {
        return _appList.length();
    } else {
        return -1;
    }
}

//create menu from weile
void MenuWidget::oldCreateMenu()
{
    _dirBtn = new MenuButton("", ":images/menu_btn_hover.png",
                             tr("文件夹"), this, false);
    _linkBtn = new MenuButton("", ":images/menu_btn_hover.png",
                              tr("快捷方式"), this, false);
    _docBtn = new MenuButton("", ":images/menu_btn_hover.png",
                             tr("DOC 文档"), this, false);
    _excelBtn = new MenuButton("", ":images/menu_btn_hover.png",
                               tr("Excel 工作表"), this, false);
    _pptBtn = new MenuButton("", ":images/menu_btn_hover.png",
                             tr("PPT 演示文稿"), this, false);



    _dirBtn->setGeometry(14, 20, ICON_W, BTN_H);
    _linkBtn->setGeometry(14, 39 + 2, ICON_W, BTN_H);
    _docBtn->setGeometry(14, 58 + 2 * 3, ICON_W, BTN_H);
    _excelBtn->setGeometry(14, 77 + 2 * 4, ICON_W, BTN_H);
    _pptBtn->setGeometry(14, 96 + 2 * 5, ICON_W, BTN_H);


    _dirBtn->setValue(0);
    _linkBtn->setValue(1);
    _docBtn->setValue(2);
    _excelBtn->setValue(3);
    _pptBtn->setValue(4);


    //setFixedSize(ICON_W, 96 + 19 + 2 * 5 + 20);
    setFixedSize(ICON_W, 40 + 19 * 6 + 2 * 5);

    connect(_dirBtn, SIGNAL(clicked()), this, SIGNAL(createDir()));
    connect(_linkBtn, SIGNAL(clicked()), this, SIGNAL(createLink()));
    connect(_docBtn, SIGNAL(clicked()), this, SIGNAL(createDOC()));
    connect(_excelBtn, SIGNAL(clicked()), this, SIGNAL(createEXCEL()));
    connect(_pptBtn, SIGNAL(clicked()), this, SIGNAL(createPPT()));
}

void MenuWidget::createNewFile(int value)
{
    int i = 1;
    QString fileName;
    while(1) {
        if(_appList.at(value) == "Folder") {
            emit createDir();
            break;
        }

        fileName += QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
        fileName += ("\\" + tr("新建"));
        fileName += _appName.at(value);
        if(i > 1) {
            fileName += "(" + QString::number(i) + ")";
        }
        fileName += _appList.at(value);
        qDebug() << "filename" << fileName;
        QFile newFile(fileName);
        if(newFile.exists()) {
            qDebug() << "file exsit : " << fileName;
            ++i;
        } else {
            if(_appList.at(value) == ".lnk") {
                emit addDesktopLink();
                break;
            }
            if(newFile.open(QIODevice::ReadWrite)) {
                qDebug() << "createFile success";
            }
            newFile.close();
            emit hideDesktop();
            break;
        }
        fileName = "";
    }
}

void MenuWidget::newCreateMenu()
{
    _appList = getAppList();
    _appName = getAppName();
    _appCount = getAppCount();
#if 0
    oldCreateMenu();
#else
    MenuButton *newButton;
    for(int i = 0; i < _appCount; ++i) {
        newButton = new MenuButton("", ":images/menu_btn_hover.png", _appName.at(i), this, false);
        newButton->setGeometry(7, 20 + 19 * i + 2 * i, ICON_W_NEW, BTN_H);
        newButton->setValue(i);
        _menuButtons.append(newButton);
        connect(newButton, SIGNAL(buttonClicked(int)), this, SLOT(createNewFile(int)));
    }
    setFixedSize(ICON_W_NEW, 40 + 19 * _appCount + 2 * _appCount);
#endif
}
MenuWidget::MenuWidget(const MenuWidget::menu_type &type, QWidget *parent)
    : QWidget(parent)
    , _type(type)
{
    switch(_type)
    {
    case MenuWidget::normal :
        _showBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("图标显示"), this, true);
        _refreshBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                     tr("刷新"), this, false);
        _createBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                    tr("新建"), this, true);
//        _changeSkinBtn = new MenuButton("", ":images/menu_btn_hover.png",
//                                        tr("更换壁纸"), this, false);
//        _themeBtn = new MenuButton("", ":images/menu_btn_hover.png",
//                                   tr("桌面主题"), this, false);
//        QLabel *l1 = new QLabel(this);
//        l1->setPixmap(QPixmap(":images/menu_line.png"));


        _showBtn->setGeometry(14, 20 + 2, ICON_W, BTN_H);
        _refreshBtn->setGeometry(14, 39 + 2 * 2, ICON_W, BTN_H);
        _createBtn->setGeometry(14, 58 + 2 * 3, ICON_W, BTN_H);
//        _changeSkinBtn->setGeometry(14, 77 + 2 * 4, ICON_W, BTN_H);
//        _themeBtn->setGeometry(14, 96 + 2 * 5, ICON_W, BTN_H);

        _showBtn->setValue(0);
        _refreshBtn->setValue(1);
        _createBtn->setValue(2);
        //_changeSkinBtn->setValue(3);
        //_themeBtn->setValue(4);

//        setFixedSize(ICON_W, 115 + 2 * 5 + 20);
        setFixedSize(ICON_W, 77 + 2 * 3 + 20);

        connect(_showBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
        connect(_createBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
       connect(_createBtn, SIGNAL(clicked()), this, SIGNAL(createDir()));
//        connect(_changeSkinBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
//        connect(_themeBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
        //connect(_delBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
        connect(_refreshBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
//        connect(_changeSkinBtn, SIGNAL(clicked()), this, SIGNAL(changeSkin()));
//        connect(_themeBtn, SIGNAL(clicked()), this, SIGNAL(desktopTheme()));
//        connect(_delBtn, SIGNAL(clicked()), this, SIGNAL(del()));
        connect(_refreshBtn, SIGNAL(clicked()), this, SIGNAL(refresh()));

        break;

    case MenuWidget::showIcon :
        _largeBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("大图标"), this, false);
        _mediumBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                    tr("中等图标"), this, false);
        _smallBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                        tr("小图标"), this, false);

        _largeBtn->setGeometry(14, 20, ICON_W, BTN_H);
        _mediumBtn->setGeometry(14, 39 + 2, ICON_W, BTN_H);
        _smallBtn->setGeometry(14, 59 + 2 * 2, ICON_W, BTN_H);

        _largeBtn->setValue(0);
        _mediumBtn->setValue(1);
        _smallBtn->setValue(2);

        setFixedSize(ICON_W, 78 + 2 * 2 + 20);

        connect(_largeBtn, SIGNAL(clicked()), this, SIGNAL(largeIcon()));
        connect(_mediumBtn, SIGNAL(clicked()), this, SIGNAL(mediumIcon()));
        connect(_smallBtn, SIGNAL(clicked()), this, SIGNAL(smallIcon()));

        break;

    case MenuWidget::create :
        newCreateMenu();
        break;

    case MenuWidget::iconMenu :
        _openBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                 tr("打开"), this, false);
        _delBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("删除"), this, false);
//        _renameBtn = new MenuButton("", ":images/menu_btn_hover.png",
//                                 tr("重命名"), this, false);

        _openBtn->setGeometry(14, 20, ICON_W, BTN_H);
        _delBtn->setGeometry(14, 39 + 2, ICON_W, BTN_H);
//        _renameBtn->setGeometry(14, 58 + 2 * 3, ICON_W, BTN_H);

        _openBtn->setValue(0);
        _delBtn->setValue(1);
//        _renameBtn->setValue(2);

//        setFixedSize(ICON_W, 78 + 2 * 2 + 20);
        setFixedSize(ICON_W, 58 + 2 + 20);

        connect(_openBtn, SIGNAL(clicked()), this, SIGNAL(run()));
        connect(_delBtn, SIGNAL(clicked()), this, SIGNAL(del()));
 //       connect(_renameBtn, SIGNAL(clicked()), this, SIGNAL(rename()));
        break;

    default:
        break;
    }
}

MenuWidget::~MenuWidget()
{

}

void MenuWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, QPixmap(":images/menu_top.png"));

    painter.drawPixmap(0, 20, width(), height() - 40, QPixmap(":images/menu_center.png"));

    if (_type == MenuWidget::normal)
    {
        painter.drawPixmap(0, 20 + 19, QPixmap(":images/menu_line.png"));
//        painter.drawPixmap(0, 20 + 19 * 3 + 2 * 2, QPixmap(":images/menu_line.png"));
    }
    else if (_type == MenuWidget::create)
    {
        painter.drawPixmap(0, 20 + 19 * 2 + 2, QPixmap(":images/menu_line.png"));
    }
    else if (_type == MenuWidget::iconMenu)
    {
        painter.drawPixmap(0, 20 + 19, QPixmap(":images/menu_line.png"));
    }

    painter.drawPixmap(0, height() - 20, QPixmap(":images/menu_bottom.png"));

    QWidget::paintEvent(event);
}

//MenuButton

MenuButton::MenuButton(QString normal, QString hover, const QString &hint, QWidget *parent, bool subMenu)
    : QPushButton(parent)
    , _hint(hint)
    , _normal(normal)
    , _hover(hover)
    , _subMenu(subMenu)
    , _entered(false)
{
//    QFont fm("", 10, QFont::Normal);

    QFontMetrics fm = this->fontMetrics();
    _fontWidth = fm.width(_hint);

 //   setStyleSheet("QPushButton{text-align:left;");

    _pixmap.load(_normal);

    if (_subMenu)
        _subPixmap.load(":images/sub_menu.png");
    else
        _subPixmap.load("");
}

MenuButton::~MenuButton()
{

}

void MenuButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (_entered)
        painter.drawPixmap(0, 0, QPixmap(_hover));
    else
        painter.drawPixmap(0, 0, QPixmap(_normal));

    if (!isEnabled())
        painter.setPen(QPen(QColor(Qt::gray)));
    else
        painter.setPen(QPen(QColor(Qt::white)));

//    if (_value == 0)
//    {
//        painter.drawPixmap(width() - _subPixmap.width() - 20, (ICON_W - _subPixmap.height()) / 2 + 12, _subPixmap);
//        painter.drawText((width() - _fontWidth) / 2, (ICON_W + 10) / 2 + 12, _hint);
//    }
//    else if (height() > ICON_W)
//    {
//        painter.drawPixmap(width() - _subPixmap.width() - 20, (ICON_W - _subPixmap.height()) / 2, _subPixmap);
//        painter.drawText((width() - _fontWidth) / 2, (ICON_W - 13 + 10) / 2, _hint);
//    }
//    else
//    {
//        painter.drawPixmap(width() - _subPixmap.width() - 20, (ICON_W - _subPixmap.height()) / 2, _subPixmap);
//        painter.drawText((width() - _fontWidth) / 2, (ICON_W + 10) / 2, _hint);
// }
    painter.setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 10, QFont::Normal));
    painter.drawPixmap(width() - 50, 5, _subPixmap);
    painter.drawText(20, 13, _hint);

}

void MenuButton::enterEvent(QEvent *event)
{
//    QMessageBox::information(NULL, "TIP", "ENTER EVNET");
    if (!isEnabled())
    {
        _entered = false;
        _pixmap.load(_normal);
        repaint();
        return;
    }

    _entered = true;
    _pixmap.load(_hover);
    repaint();

    emit hover(_value);

    Q_UNUSED(event);
}

void MenuButton::leaveEvent(QEvent *event)
{
//    QMessageBox::information(NULL, "TIP", "LEAVE");
    _entered = false;
    _pixmap.load(_normal);
    repaint();

    Q_UNUSED(event);
}

void MenuButton::mousePressEvent(QMouseEvent *event)
{
    //miya add
    emit buttonClicked(_value);
    _pixmap.load(_normal);
    repaint();
    Q_UNUSED(event);
}

void MenuButton::mouseReleaseEvent(QMouseEvent *event)
{
    _pixmap.load(_hover);
    repaint();
    if (!_subMenu)
    {
        emit clicked();
//        emit valueClicked(_value);
    }
    Q_UNUSED(event);
}

int MenuButton::getValue()
{
    return _value;
}

void MenuButton::setValue(int value)
{
    _value = value;
}

bool MenuButton::hasSubMenu()
{
    return _subMenu;
}
