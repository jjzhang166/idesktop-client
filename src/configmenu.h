#ifndef CONFIGMENU_H
#define CONFIGMENU_H

//#include <QMenu>
//#include <QAction>
//#include <QPixmap>
//#include <QScrollBar>
//#include <QPaintEvent>
//#include <QMouseEvent>
//#include <QResizeEvent>
//#include <QWheelEvent>
//#include <QLabel>
//#include <QLineEdit>
//#include <QPropertyAnimation>
//#include <QPushButton>

//class ServerStgWidget;
//class PixmapWidget;
//class SkinPixWidget;
//class SkinWidget;

//class ConfigMenu : public QLabel
//{
//    Q_OBJECT

//public:
//    explicit ConfigMenu(QWidget *parent = 0);
//    ~ConfigMenu();

//    bool mouseEnter();

//signals:
//    void setBgPixmap(const QString &pixText);
//    void upLoad();

//protected:
////    void paintEvent(QPaintEvent *event);
//    void enterEvent(QEvent *event);
//    void leaveEvent(QEvent *event);



//public slots:
//    void setSkin();
//    void setBsStyle();
//    void setServer();

//private:
//    QMenu *_menu;
//    QPushButton *_skinAc;

//    QPushButton *_bsStyleAc;
//    QPushButton *_serverAc;

//    SkinWidget *_skinWidget;
//    ServerStgWidget *_serverStgWidget;

//    bool _enter;

//};

//class ConfigMenu : public QMenu
//{
//    Q_OBJECT
    
//public:
//    explicit ConfigMenu(QWidget *parent = 0);
//    ~ConfigMenu();

//signals:
//    void setBgPixmap(const QString &pixText);
//    void upLoad();

//protected:
//    //void paintEvent(QPaintEvent *event);

//public slots:
//    void setSkin();
///*
//    void backupData();
//    void recoverData();
//    void backupBs();
//    void recoverBs();
//    void setConfigure();
//*/
//    void setBsStyle();
//    void setServer();

//private:
//    QAction *_skinAc;
//    //QAction *_dataBackupAc;
//    //QAction *_dataRecoverAc;
//    //QAction *_bsBackupAc;
//    //QAction *_bsRecoverAc;
//    //QAction *_configureAc;
//    QAction *_bsStyleAc;
//    QAction *_serverAc;

//    //QMenu *_dataMenu;
//    //QMenu *_bsMenu;

//    SkinWidget *_skinWidget;
//    ServerStgWidget *_serverStgWidget;

//};


//class SkinWidget : public QWidget
//{
//    Q_OBJECT
//public :
//    explicit SkinWidget(QWidget *parent = 0);
//    ~SkinWidget() {}

//signals:
//    void setBgPixmap(const QString &pixText);

//public slots:
//    void scrollBarValueChanged(int val);

//protected:
//    void resizeEvent(QResizeEvent *event);
//    void wheelEvent(QWheelEvent *event);

//private:
//    PixmapWidget *_pixmapWidget;
//    QScrollBar *_scrollBar;

//    SkinPixWidget *_skinPixWidget;

//    QPropertyAnimation *_animation;

//    int _pageNum;
//};

//class SkinPixWidget : public QWidget
//{
//    Q_OBJECT
//public :
//    explicit SkinPixWidget(QWidget *parent = 0);
//    ~SkinPixWidget() {}

//signals:
//    void setBgPixmap(const QString &pixText);

//private:
//    PixmapWidget *_pixmapWidget;

//    int _column;
//    int _rowNum;
//};

//class PixmapWidget : public QWidget
//{
//    Q_OBJECT
//public :
//    explicit PixmapWidget(QString pixText, QWidget *parent = 0);
//    ~PixmapWidget() {}

//    void setSize(int w, int h);
//    QString getPixmapText();
//    int getWidth();
//    int getHeight();

//signals:
//    void mouseClicked(const QString &pixText);

//protected:
//    void paintEvent(QPaintEvent *event);
//    void mousePressEvent(QMouseEvent *event);

//private:
//    int _width;
//    int _height;
//    QString _pixText;
//    QPixmap _pixmap;

//};

//class ServerStgWidget : public QWidget
//{
//    Q_OBJECT
//public :
//    explicit ServerStgWidget(QWidget *parent = 0);
//    ~ServerStgWidget() {}

//signals:
//    void upLoad();

//public slots:
//    void saveIp();

//protected:
//    void paintEvent(QPaintEvent *event);

//private:
//    QLabel *_virtualLabel;
//    QLabel *_serviceLabel;

//    QLineEdit *_virtualEdit;
//    QLineEdit *_serviceEdit;

//    QPushButton *_saveBtn;
//    QPushButton *_cancelBtn;

//};
#endif // CONFIGMENU_H
