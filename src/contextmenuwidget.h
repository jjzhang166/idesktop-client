#ifndef CONTEXTMENUWIDGET_H
#define CONTEXTMENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QEvent>
#include <QPixmap>

class MenuButton;
//class MenuWidget;

//class ContextMenuWidget : public QWidget
//{
//    Q_OBJECT
    
//public:
//    enum menu_type
//    {
//        showIcon, create, normal
//    };

//    explicit ContextMenuWidget(menu_type & type = normal,QWidget *parent = 0);
//    ~ContextMenuWidget();
//signals:
//    //normal
//    void changeSkin();
//    void desktopTheme();
//    void del();
//    void refresh();
//    //showIcon
//    void bigIcon();
//    void hitIcon();
//    void smallIcon();
//    //create
//    void createDir();
//    void createLink();
//    void createDOC();
//    void createEXCEL();
//    void createPPT();

//public slots:
//    void showIcons();
//    void createIcons();
//    void leaveShowIcons();
//    void leaveCreateIcons();

//private:

//    MenuWidget *_normalMenu;
//    MenuWidget *_showIconMenu;
//    MenuWidget *_createMenu;

//    int _type;

//};

class MenuWidget : public QWidget
{
    Q_OBJECT
public:

    enum menu_type
    {
        showIcon, create, iconMenu, normal
    };

    explicit MenuWidget(const MenuWidget::menu_type &type = MenuWidget::normal,
                        QWidget *parent = 0);
    ~MenuWidget();

signals:
    //normal
    void menuChanged(int value);
    void changeSkin();
    void desktopTheme();
    void refresh();
    //showIcon
    void largeIcon();
    void mediumIcon();
    void smallIcon();
    //create
    void createDir();
    void createLink();
    void createDOC();
    void createEXCEL();
    void createPPT();

    //iconMenu
    void run();
    void del();
    void rename();

protected:
    void paintEvent(QPaintEvent *event);

private:
    int _type;
    //normal
    MenuButton *_showBtn;
    MenuButton *_createBtn;
    MenuButton *_changeSkinBtn;
    MenuButton *_themeBtn;
    MenuButton *_refreshBtn;
    //showIcon
    MenuButton *_largeBtn;
    MenuButton *_mediumBtn;
    MenuButton *_smallBtn;
    //create
    MenuButton *_dirBtn;
    MenuButton *_linkBtn;
    MenuButton *_docBtn;
    MenuButton *_excelBtn;
    MenuButton *_pptBtn;
    //
    MenuButton *_openBtn;
    MenuButton *_delBtn;
    MenuButton *_renameBtn;

};

class MenuButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MenuButton(QString normal, QString hover, const QString &hint, QWidget *parent = 0, bool subMenu = false);
    ~MenuButton();

    void setValue(int value);
    bool hasSubMenu();

signals:
//    void clicked();
    void valueClicked(int value);
    void hover(int value);
protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QString _hint;
    QPixmap _pixmap;
    QPixmap _subPixmap;
    QString _normal;
    QString _hover;

    int _fontWidth;

    bool _entered;
    bool _subMenu;

    int _value;
};

#endif // CONTEXTMENUWIDGET_H
