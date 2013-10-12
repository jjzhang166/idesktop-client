#include <QtGui>
#include <windows.h>
#include "contextmenuwidget.h"

#include "idesktopsettings.h"
#define BTN_H 19
//#define ICON_W 162
#define ICON_W 138


//miya add
QStringList MenuWidget::getAppList()
{
    qDebug() << __PRETTY_FUNCTION__ << "getNewList";
    QString newListPath("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\PostSetup\\ShellNew");
    QSettings reg(newListPath, QSettings::NativeFormat);
    QStringList list = reg.allKeys();
    QString strKey;
    QStringList strValue;
    int nCount = list.count();
    qDebug() << "nCount : " << nCount;
    if (nCount == 0)
        return list;
    strKey = list.at(0);
    strValue = reg.value(strKey).toStringList();
//    qDebug() << "strKey aaaaaa: " << strKey;
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
//    qDebug() << "after aaaaaaaaacccccc";
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
            strValue = tr("�ļ���");
        } else if(strValue == "Shortcut") {
            strValue = tr("��ݷ�ʽ��");
        } else if(strValue == "Briefcase") {
            strValue = tr("���İ�");
        } else if (strValue == "Bitmap Image") {
            strValue = tr("BMP ͼ��");
        } else if (strValue == "Contact File") {
            strValue = tr("��ϵ��");
        } else if (strValue == "Journal Document") {
            strValue = tr("�ռǱ��ĵ�");
        } else if (strValue == "Text Document") {
            strValue = tr("�ı��ĵ�");
        }

        strAppName.append(strValue);
//        qDebug() << "new name" << strValue;
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
        fileName += ("\\" + tr("�½�"));
        fileName += _appName.at(value);
        if(i > 1) {
            fileName += "(" + QString::number(i) + ")";
        }
        fileName += _appList.at(value);
        //fileName = _appList.at(value);
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
            } else {
                qDebug() << GetLastError();
                qDebug() << "createFile Error!";
            }
            newFile.close();
            emit hideDesktop();
            break;
        }
        fileName = "";
    }

    QTimer::singleShot(0, this, SLOT(closeUp()));
}

void MenuWidget::closeUp()
{
    QWidget *w;
    while ((w = qApp->activePopupWidget()) && w->inherits("MenuWidget")) {
        qDebug() << __PRETTY_FUNCTION__ << "closing" << (qint32)w;
        w->close();
    }
}

QStringList MenuWidget::getXPAppList()
{
    qDebug() << "getNewList";
    QString newListPath("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\PostSetup\\ShellNew");
    QSettings reg(newListPath, QSettings::NativeFormat);
    QStringList list = reg.childKeys();
    QByteArray value;
    QStringList appList;
    QStringList appName;
    QString app;
    int nCount = list.count();
    for(int i = 0; i < nCount; i++) {
        qDebug() << list.at(i);
        if(list.at(i) == "." || list.at(i) == "~reserved~" || list.at(i) == "Language")
            continue;
        value = reg.value(list.at(i)).toByteArray();
        for(int x=0; x < value.size(); x++)
        {
            if(value.at(x) == '.')
            {
                app = "";
                int j = 0;
                while(value.at(x) || j < 3) {
                    if(value.at(x))
                        app += value.at(x);
                    else {
                        j++;
                    }
                    x++;
                }
                break;
            }
        }
        qDebug() << app;
        if(app != ".bfc") {
            appList.append(app);
            appName.append(list.at(i));
        }
    }
    appList.insert(0, "Folder");
    appName.insert(0, tr("�ļ���"));
    appList.insert(1, ".lnk");
    appName.insert(1, tr("��ݷ�ʽ��"));
    appList.removeDuplicates();
    int len = appList.count();
    _appName = appName;
    _appList = appList;
    for(int j = 0; j < len ; j++)
        qDebug() << j << " " << _appList.at(j);

    for(int j = 0; j < len ; j++)
        qDebug() << j << " " << _appName.at(j);
    qDebug() <<"app list count" << _appList.count();
    qDebug() <<"app name count" << _appName.count();
    qDebug() << "app count " << getAppCount();
    return appList;
}

void MenuWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << __PRETTY_FUNCTION__ << (qint32)this;
    closeUp();
//    QMenu::mousePressEvent(event);
}

void MenuWidget::newCreateMenu()
{
    if(QSysInfo::windowsVersion() == QSysInfo::WV_XP)
        getXPAppList();
    else {
        _appList = getAppList();
        _appName = getAppName();
    }
    _appCount = getAppCount();
    MenuButton *newButton;
    for(int i = 0; i < _appCount; ++i) {
        newButton = new MenuButton("", ":images/menu_btn_hover.png", _appName.at(i), this, false);
        newButton->setGeometry(7, 8 + 19 * i + 2 * i, ICON_W_NEW, BTN_H);
        newButton->setValue(i);
        _menuButtons.append(newButton);
        connect(newButton, SIGNAL(buttonClicked(int)), this, SLOT(createNewFile(int)));
    }
    setFixedSize(ICON_W_NEW, 16 + 19 * _appCount + 2 * _appCount);
}

MenuWidget::MenuWidget(const MenuWidget::menu_type &type, QWidget *parent)
    : QMenu(parent)
    , _type(type)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoMouseReplay);

    switch(_type)
    {
    case MenuWidget::normal :
        _showBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("ͼ����ʾ"), this, true);
        _refreshBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                     tr("ˢ��"), this, false);
        _createBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                    tr("�½�"), this, true);

        _showBtn->setGeometry(2, 8 + 2, ICON_W, BTN_H);
        _refreshBtn->setGeometry(2, 27 + 2 * 2, ICON_W, BTN_H);
        _createBtn->setGeometry(2, 46 + 2 * 3, ICON_W, BTN_H);

        _showBtn->setValue(0);
        _refreshBtn->setValue(1);
        _createBtn->setValue(2);

        setFixedSize(ICON_W, 65 + 2 * 3 + 8);

        connect(_showBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
        connect(_createBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
        connect(_createBtn, SIGNAL(clicked()), this, SIGNAL(createDir()));
        connect(_refreshBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
        connect(_refreshBtn, SIGNAL(clicked()), this, SIGNAL(refresh()));

        break;

    case MenuWidget::showIcon :
        _largeBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("��ͼ��"), this, false);
        _mediumBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                    tr("�е�ͼ��"), this, false);
        _smallBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                        tr("Сͼ��"), this, false);

        _largeBtn->setGeometry(2, 8, ICON_W, BTN_H);
        _mediumBtn->setGeometry(2, 27 + 2, ICON_W, BTN_H);
        _smallBtn->setGeometry(2, 46 + 2 * 2, ICON_W, BTN_H);

        _largeBtn->setValue(0);
        _mediumBtn->setValue(1);
        _smallBtn->setValue(2);

        setFixedSize(ICON_W, 65 + 2 * 2 + 8);

        connect(_largeBtn, SIGNAL(clicked()), this, SLOT(largeBtnClicked()));
        connect(_mediumBtn, SIGNAL(clicked()), this, SLOT(mediumBtnClicked()));
        connect(_smallBtn, SIGNAL(clicked()), this, SLOT(smallBtnClicked()));

        switch(IDesktopSettings::instance()->iconSize())
        {
            case 0 :
                _largeBtn->setMenuSelect(true);
                _mediumBtn->setMenuSelect(false);
                _smallBtn->setMenuSelect(false);
                break;

            case 1 :
                _largeBtn->setMenuSelect(false);
                _mediumBtn->setMenuSelect(true);
                _smallBtn->setMenuSelect(false);

                break;

            case 2 :
                _largeBtn->setMenuSelect(false);
                _mediumBtn->setMenuSelect(false);
                _smallBtn->setMenuSelect(true);

                break;

            default:
                break;
        }

        break;

    case MenuWidget::create :
        newCreateMenu();
        break;

    case MenuWidget::iconMenu :
        _runBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                 tr("run"), this, false);
        _delBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("delete"), this, false);

        _runBtn->setGeometry(2, 8, ICON_W, BTN_H);
        _delBtn->setGeometry(2, 27 + 2, ICON_W, BTN_H);

        _runBtn->setValue(0);
        _delBtn->setValue(1);

        setFixedSize(ICON_W, 46 + 2 + 8);

        connect(_runBtn, SIGNAL(clicked()), this, SIGNAL(run()));
        connect(_delBtn, SIGNAL(clicked()), this, SIGNAL(del()));
        break;
    case MenuWidget::dustbinMenu :

        _restoreBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                     tr("restore"), this, false);
        _delBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("delete"), this, false);

        _restoreBtn->setGeometry(2, 8, ICON_W, BTN_H);
        _delBtn->setGeometry(2, 27 + 2, ICON_W, BTN_H);

        _restoreBtn->setValue(0);
        _delBtn->setValue(1);

        setFixedSize(ICON_W, 46 + 2 + 8);

        connect(_restoreBtn, SIGNAL(clicked()), this, SIGNAL(restore()));
        connect(_delBtn, SIGNAL(clicked()), this, SIGNAL(del()));
        break;
    case MenuWidget::dirMenu :

        _openBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                     tr("open"), this, false);
        _clearBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                     tr("clear"), this, false);
        _delBtn = new MenuButton("", ":images/menu_btn_hover.png",
                                  tr("delete"), this, false);

        _openBtn->setGeometry(2, 8, ICON_W, BTN_H);
        _clearBtn->setGeometry(2, 27 + 2, ICON_W, BTN_H);
        _delBtn->setGeometry(2, 46 + 2 * 2, ICON_W, BTN_H);

        _openBtn->setValue(0);
        _clearBtn->setValue(1);
        _delBtn->setValue(2);

        setFixedSize(ICON_W, 65 + 2 * 2 + 8);

        connect(_openBtn, SIGNAL(clicked()), this, SIGNAL(open()));
        connect(_clearBtn, SIGNAL(clicked()), this, SIGNAL(clear()));
        connect(_delBtn, SIGNAL(clicked()), this, SIGNAL(del()));
        break;
    default:
        break;
    }
}

MenuWidget::~MenuWidget()
{

}

#if 0
void MenuWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPixmap pm_top = QPixmap(":images/menu_top.png")
            .scaled(width(), 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap pm_center = QPixmap(":images/menu_center.png")
            .scaled(width(), height() - 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap pm_btm = QPixmap(":images/menu_bottom.png")
            .scaled(width(), 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap pm_menulinescaled = QPixmap(":images/menu_line.png")
            .scaled(width(), 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    painter.drawPixmap(0, 0, width(), 20, pm_top);
    painter.drawPixmap(0, 20, width(), height() - 40, pm_center);

    if (_type == MenuWidget::normal) {
        painter.drawPixmap(0, 20 + 19, QPixmap(":images/menu_line.png"));
    } else if (_type == MenuWidget::create) {
        painter.drawPixmap(0, 20 + 19 * 2 + 2, width(), 2, pm_menulinescaled);
    } else if (_type == MenuWidget::iconMenu) {
        painter.drawPixmap(0, 20 + 19, QPixmap(":images/menu_line.png"));
    }

    painter.drawPixmap(0, height() - 20, width(), 20, pm_btm);
}
#endif

void MenuWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap pm_top = QPixmap(":images/menu_top.png");
    QPixmap pm_center = QPixmap(":images/menu_center.png");
    QPixmap pm_btm = QPixmap(":images/menu_bottom.png");
    QPixmap pm_menulinescaled = QPixmap(":images/menu_line.png");

    pm_top = pm_top.copy(12, 12, pm_top.width()-24, 8)
            .scaled(width(), 8, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pm_center = pm_center.copy(12, 0, pm_center.width()-24, 1)
            .scaled(width(), 1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pm_btm = pm_btm.copy(12, 0, pm_btm.width()-24, 8)
            .scaled(width(), 8, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pm_menulinescaled = pm_menulinescaled.copy(12, 0, pm_menulinescaled.width()-24, 2)
            .scaled(width(), 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    painter.drawPixmap(0, 0, pm_top);
    painter.drawPixmap(0, 8, width(), height() - 16, pm_center);

    if (_type == MenuWidget::normal) {
        painter.drawPixmap(0, 8 + 19, pm_menulinescaled);
    } else if (_type == MenuWidget::create) {
        painter.drawPixmap(0, 8 + 19 * 2, pm_menulinescaled);
    } else if (_type == MenuWidget::iconMenu) {
        painter.drawPixmap(0, 8 + 19, pm_menulinescaled);
    }

    painter.drawPixmap(0, height() - 8, pm_btm);
}

void MenuWidget::largeBtnClicked()
{
    _largeBtn->setMenuSelect(true);
    _mediumBtn->setMenuSelect(false);
    _smallBtn->setMenuSelect(false);
    emit largeIcon();

}

void MenuWidget::mediumBtnClicked()
{
    _largeBtn->setMenuSelect(false);
    _mediumBtn->setMenuSelect(true);
    _smallBtn->setMenuSelect(false);
    emit mediumIcon();
}

void MenuWidget::smallBtnClicked()
{

    _largeBtn->setMenuSelect(false);
    _mediumBtn->setMenuSelect(false);
    _smallBtn->setMenuSelect(true);
    emit smallIcon();
}

QSize MenuWidget::getSize()
{
    return size();
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
    QFontMetrics fm = this->fontMetrics();
    _fontWidth = fm.width(_hint);

    _pixmap.load(_normal);

    if (_subMenu)
        _subPixmap.load(":images/sub_menu.png");
    else
        _subPixmap.load("");

    _menuSelectLabel = new QLabel(this);
    _menuSelectLabel->setGeometry(5, 3, 9, 10);
    _menuSelectLabel->setPixmap(QPixmap(":images/iconmenu_select.png"));
    _menuSelectLabel->setVisible(false);
}

MenuButton::~MenuButton()
{

}

void MenuButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (_entered)
    {
        if (this->width() > ICON_W + 1)
            painter.drawPixmap(20, 0, width() - 55, height(), QPixmap(_hover));
        else
        {
            painter.drawPixmap(0, 0, QPixmap(_hover));
        }
    }
    else
        painter.drawPixmap(0, 0, QPixmap(_normal));

    if (!isEnabled())
        painter.setPen(QPen(QColor(Qt::gray)));
    else
        painter.setPen(QPen(QColor(Qt::white)));

    painter.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), 10, QFont::Normal));
    painter.drawPixmap(width() - 50, 5, _subPixmap);
    painter.drawText(25, 13, _hint);

}

void MenuButton::enterEvent(QEvent *event)
{
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

void MenuButton::setMenuSelect(bool select)
{
    _menuSelectLabel->setVisible(select);
}
