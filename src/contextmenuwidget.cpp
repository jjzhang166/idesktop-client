#include <QPainter>
#include <QFontMetrics>
#include <QLabel>

#include "contextmenuwidget.h"

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
                                    tr("新建文件夹"), this, false);
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
//        connect(_createBtn, SIGNAL(hover(int)), this, SIGNAL(menuChanged(int)));
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

        setFixedSize(ICON_W, 96 + 19 + 2 * 5 + 20);

        connect(_dirBtn, SIGNAL(clicked()), this, SIGNAL(createDir()));
        connect(_linkBtn, SIGNAL(clicked()), this, SIGNAL(createLink()));
        connect(_docBtn, SIGNAL(clicked()), this, SIGNAL(createDOC()));
        connect(_excelBtn, SIGNAL(clicked()), this, SIGNAL(createEXCEL()));
        connect(_pptBtn, SIGNAL(clicked()), this, SIGNAL(createPPT()));

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

void MenuButton::setValue(int value)
{
    _value = value;
}

bool MenuButton::hasSubMenu()
{
    return _subMenu;
}
