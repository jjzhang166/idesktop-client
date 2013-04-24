#include "appwidget.h"
#include "../custombtn.h"

#include "installbutton.h"
#include "uninstallbutton.h"
#include "updatebutton.h"
#include "appdata.h"
#include "appdetaildialogexe.h"
#include "scorewidget.h"
#include "appdatareadexe.h"

#include <QPainter>
#include <QPalette>
#include <QColor>
#include <QPen>
#include <QByteArray>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QPalette>
#include <QProgressBar>
#include <QTextEdit>
#include <QMouseEvent>

const int Title_Font_Size  = 9;
const int DESP_Font_Size  = 9;

const int Title_Count = 5;//默认标题中的字数，XXX...的形式
const int Icon_Width = 50;

const int Score_Width = 100;
const int Score_Height = 20;

void DrawImg(QPainter* pPainter, const QString &strImgPath, const QRect &rect);
void DrawImg(QPainter* pPainter, const QString &strImgPath, const int &x, const int &y, const QSize &size);

AppWidget::AppWidget(QWidget *parent) :
    QWidget(parent)
  , _bEnter(false)
  , _tabbg(NULL)
//  , _IconPath(strIconPath)
  , _TitleFont("", Title_Font_Size, QFont::Black)
  , _DespFont("", DESP_Font_Size, QFont::Normal)
  , _InstallBtn(NULL)
//  , _UpdateBtn(NULL)
//  , _UnInstallBtn(NULL)
  , _appData(NULL)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    hide();

    setCursor(QCursor(Qt::PointingHandCursor));

    CalRect();

    QFontMetrics metrics(_TitleFont);
    _TitleTextHeight =  metrics.height();

    setObjectName("appwidget");
    resize(QSize(App_Widget_Width, App_Widget_Height));

    ///
    _mainVerticalLayout = new QVBoxLayout(this);
    _spacerHorizontalLayout = new QHBoxLayout();
    _horizontalSpacer = new QSpacerItem(Icon_Width + 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    _spacerHorizontalLayout->addItem(_horizontalSpacer);

    _labelVerticalLayout = new QVBoxLayout();
    _TitleLabel = new QLabel(this);

    _briefLabel =  new QLabel(this);

    _labelVerticalLayout->addWidget(_TitleLabel);

    _DescriptionLabel = new QTextEdit(this);
    _DescriptionLabel->setFont(_DespFont);
    _DescriptionLabel->setReadOnly(true);
    _DescriptionLabel->setEnabled(false);
    _DescriptionLabel->setContentsMargins(0, 0, 0, 0);
    _DescriptionLabel->setAlignment(Qt::AlignLeft);
    //控制分行
    _DescriptionLabel->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    //控制皮肤
    _DescriptionLabel->setStyleSheet("QTextEdit {Border:Opx none black;}");

    QPalette detailPal =_DescriptionLabel->palette();
    detailPal.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    _DescriptionLabel->setPalette(detailPal);

    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor("#006699")));
    _DescriptionLabel->mergeCurrentCharFormat(fmt);

    _labelVerticalLayout->addWidget(_DescriptionLabel);
    _spacerHorizontalLayout->addLayout(_labelVerticalLayout);

    _mainVerticalLayout->addLayout(_spacerHorizontalLayout);

    _TitleLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed);
    _TitleLabel->setFixedHeight(_TitleTextHeight);
    _TitleLabel->setFont(_TitleFont);


    QPalette titlePal =_TitleLabel->palette();
    titlePal.setColor(QPalette::WindowText, QColor("#006699"));
    _TitleLabel->setPalette(titlePal);

    QPalette briefPal =_briefLabel->palette();
    briefPal.setColor(QPalette::WindowText, QColor("#666666"));
    _briefLabel->setPalette(briefPal);

    //安装按钮
    _InstallBtn = new InstallButton(this);
    _InstallBtn->setFixedSize(60,25);
    _InstallBtn->move(width() - 70, height() - 35);
    if (_appData != NULL && _appData->_added) {
        _InstallBtn->setText("已安装");
        _InstallBtn->enabled(false);
/*        _UpdateBtn = new UpdateButton(this);
        _UnInstallBtn = new UnInstallButton(this);

        _UpdateBtn->setFixedSize(30,25);
        _UpdateBtn->move(20, height() - 35);
        _UpdateBtn->SetText("更新");

        _UnInstallBtn->setFixedSize(30,25);
        _UnInstallBtn->move(50, height() - 35);
        _UnInstallBtn->SetText("卸载");*/
    }
    else {
        _InstallBtn->setText("安装");

        connect(_InstallBtn, SIGNAL(clicked()), this, SLOT(beginInstall()));
    }

    _progressBar = new QProgressBar(this);
    _progressBar->hide();
    _progressBar->setTextVisible(false);
    _progressBar->setGeometry(_rProgressBar);

/*    _scoreWdg = new ScoreWidget(this);
    _scoreWdg->setFixedSize(Score_Width,Score_Height);
    _scoreWdg->hide();

    connect(_scoreWdg, SIGNAL(appscored(int)), this, SLOT(appScored(int)));
*/
}

void AppWidget::enterEvent ( QEvent * event )
{
    _bEnter = true;
    repaint();
//    _scoreWdg->show();
}

void AppWidget::leaveEvent ( QEvent * event )
{
    _bEnter = false;
    repaint();
//    _scoreWdg->hide();
}
void AppWidget::resizeEvent ( QResizeEvent * event )
{
    _briefLabel->setGeometry(_rIcon.left(), _rIcon.bottom() + 10, width() - 2* _rIcon.left(), _TitleTextHeight + 3);

//    _scoreWdg->move(width() - Score_Width - 10, height() - Score_Height - 5);
}

void AppWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    //如果鼠标进入窗口，切换图片
    if (_bEnter) {
        DrawImg(&painter, ":/images/bg_appBox_hover.png", rect());
    }
    else {
        DrawImg(&painter, ":/images/bg_appBox.png", rect());
    }

    //draw icon

    painter.setRenderHints (QPainter::HighQualityAntialiasing |QPainter::Antialiasing |
                          QPainter::SmoothPixmapTransform);
    painter.drawImage(_rIcon, *_tabbg, QRectF(0.0, 0.0, 0.0, 0.0));
}

void AppWidget::CalRect()
{
    int nEdgeWidth = 10;
    //icon rect
    _rIcon.setLeft(nEdgeWidth);
    _rIcon.setRight(nEdgeWidth + Icon_Width);
    _rIcon.setTop(nEdgeWidth);
    _rIcon.setBottom(nEdgeWidth + Icon_Width);

    //progress rect
    _rProgressBar.setLeft(_rIcon.left() + 20);
    _rProgressBar.setRight(_rIcon.right() - 20);
    _rProgressBar.setTop(_rIcon.bottom() - 12);
    _rProgressBar.setBottom(_rIcon.bottom() - 6);
}

void AppWidget::beginInstall()
{
    _progressBar->show();
    _progressBar->setValue(20);
}

void AppWidget::setData(AppData* appData)
{
    _BriefTitle = "";
    _Desription = "";

    QString brief;
 //   appData->_brief = "abcdefghia";
 //   int nnnn = appData->_brief.length();

    _appData = appData;
    if (appData)
    {
        QFontMetrics metrics(_TitleFont);
 //       int nLength = metrics.width(appData->_brief);

        if (appData->_name.length() > 10) {
            _BriefTitle = appData->_name.mid(0, 10);
            _BriefTitle += "..";
        }
        else {
            _BriefTitle = appData->_name;
        }

        _Desription = "版本: ";
        _Desription += appData->_version;
        _Desription += "\n";
        _Desription += "大小: ";
        _Desription += appData->_size;
   //     _Desription += "\n";


   //     metrics.width()
//        int briefLength =  metrics.width(appData->_brief);
        if (appData->_brief.length() > 20) {
            brief = appData->_brief.mid(0, 20);
            brief += "..";
        }
        else {
            brief = appData->_brief;
        }
        if (_tabbg)
            delete _tabbg;
        _tabbg = new QImage;

        if (!_tabbg->load(appData->_strIconPath)) {
            _tabbg->load(AppDetailDialogExe::Instance()->defaultIcon());
        }
//        _scoreWdg->setScore(appData->_rate, false,false);
    }


    QByteArray strTmp = _BriefTitle.toAscii();
    _TitleLabel->setText(QString::fromLocal8Bit(strTmp.data()));

    //draw description
    strTmp = _Desription.toAscii();
    _DescriptionLabel->setText(QString::fromLocal8Bit(strTmp.data()));

    strTmp = brief.toAscii();
    _briefLabel->setText(QString::fromLocal8Bit(strTmp.data()));
}

void AppWidget::mousePressEvent ( QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QPoint pos = _InstallBtn->pos();

    QRect rInstall(pos.x(), pos.y(), _InstallBtn->rect().width(),_InstallBtn->rect().height());

//    if (_InstallBtn && rInstall.contains(event->pos()))
 //       return;
    AppDetailDialogExe::Instance()->showApp(_appData->_name);
}

void AppWidget::appScored(int score)
{
    if (!_appData)
        return;

    QString str = QString("rate=%1").arg(score);
    AppDataReadExe::Instance()->postData(_appData->_name,str.toAscii());
}

void AppWidget::UpdateScore(QString appName, int score)
{
//    if (_appData && _appData->_name == appName)
//        _scoreWdg->setScore(score, _appData->_added);
}
