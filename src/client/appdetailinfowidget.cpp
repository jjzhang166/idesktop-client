#include "appdetailinfowidget.h"
#include "../custombtn.h"
#include "../appmessagebox.h"

#include "installbutton.h"
#include "uninstallbutton.h"
#include "updatebutton.h"
#include "appdata.h"
#include "appdatareadexe.h"
#include "appdatabase.h"
#include "appdetaildialogexe.h"
#include "scorewidget.h"

#include <QPainter>
#include <QImage>
#include <QProgressBar>
#include <QLabel>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QMouseEvent>
#include <QMovie>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QPalette>

const int Icon_Width = 96;
const int nEdgeWidth = 20;

const int Title_Font_Size  = 10;
const int DESP_Font_Size  = 10;

const int Shadow_Edge_Width = 45;


AppDetailInfoWidget::AppDetailInfoWidget(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::Tool)
  , _bPressTitle(false)
  , _InstallBtn(NULL)
  , _UpdateBtn(NULL)
  , _UnInstallBtn(NULL)
  , _TitleFont("", Title_Font_Size, QFont::Black)
  , _DespFont("", DESP_Font_Size, QFont::Normal)
  , _DespTitleFont("", Title_Font_Size, QFont::Black)
  , _appData(NULL)
  , _scoreSucceed(false)
  , _textAlpha(-1)
{

    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);

    _pCloseBtn = new CustomBtn(this);
    _pCloseBtn->setToolTip(QString::fromLocal8Bit("关闭"));
    _pCloseBtn->SetOnImg(":images/bg_dialog_close_hover.png");
    _pCloseBtn->SetOffImg(":images/bg_dialog_close.png");
    _pCloseBtn->setFixedSize(10, 10);
    _pCloseBtn->show();

    connect(_pCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
    _tabbg = new QImage();
    _succes = new QImage(":images/succes.png");

    QFontMetrics metrics(_TitleFont);
    _TitleTextHeight =  metrics.height();

    _TitleLabel = new QLabel(this);
    _TitleLabel->setGeometry(QRect(120, 15, 329, _TitleTextHeight));
    _TitleLabel->setWordWrap(true);
    _TitleLabel->setAlignment(Qt::AlignLeft);
    _TitleLabel->setFont(_TitleFont);

    QPalette titlePal =_TitleLabel->palette();
    titlePal.setColor(QPalette::WindowText, QColor("#006699"));
    _TitleLabel->setPalette(titlePal);


    _DescriptionLabel = new QLabel(this);
    _DescriptionLabel->setGeometry(QRect(120, 10 + _TitleTextHeight, 329, _TitleTextHeight*4));  //2倍行距
    _DescriptionLabel->setWordWrap(true);
    _DescriptionLabel->setAlignment(Qt::AlignTop);
    _DescriptionLabel->setFont(_DespFont);

    QPalette pal =_DescriptionLabel->palette();
    pal.setColor(QPalette::WindowText, QColor("#006699"));
    _DescriptionLabel->setPalette(pal);

    _detailTitle = new QLabel(this);
    _detailTitle->setFixedSize(100, _TitleTextHeight);
    _detailTitle->setFont(_DespTitleFont);
    _detailTitle->setText("应用介绍");

    QPalette detailTitlePal =_detailTitle->palette();
    detailTitlePal.setColor(QPalette::WindowText, QColor("#006699"));
    _detailTitle->setPalette(detailTitlePal);


    _detailInfo = new QTextEdit(this);
    _detailInfo->setFont(_DespFont);
    _detailInfo->setReadOnly(true);
    _detailInfo->setContentsMargins(0, 0, 0, 0);
    _detailInfo->setAlignment(Qt::AlignLeft);
    //控制分行
    _detailInfo->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    //控制皮肤
    _detailInfo->setStyleSheet("QTextEdit {Border:Opx none black;}");

    QPalette detailPal =_detailInfo->palette();
    detailPal.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    detailPal.setColor(QPalette::WindowText, QColor("#006699"));
    _detailInfo->setPalette(detailPal);

    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor("#006699")));
    _detailInfo->mergeCurrentCharFormat(fmt);

    //icon rect
    _rIcon.setLeft(nEdgeWidth + Shadow_Edge_Width);
    _rIcon.setRight(nEdgeWidth + Icon_Width + Shadow_Edge_Width);
    _rIcon.setTop(nEdgeWidth + Shadow_Edge_Width);
    _rIcon.setBottom(nEdgeWidth + Icon_Width + Shadow_Edge_Width);


    //安装按钮
    _UpdateBtn = new UpdateButton(this);
    _UpdateBtn->setFixedSize(86,24);
    _UpdateBtn->setState(UpdateButton::noUpdate);
    _UpdateBtn->setCursor(QCursor(Qt::PointingHandCursor));

/*    _UnInstallBtn = new UnInstallButton(this);
    _UnInstallBtn->setFixedSize(60,25);
    _UnInstallBtn->SetText("卸载");
    _UnInstallBtn->setCursor(QCursor(Qt::PointingHandCursor));
*/
    _InstallBtn = new InstallButton(this);
    _InstallBtn->setFixedSize(86,24);
    _InstallBtn->setState(InstallButton::unAdded);
    _InstallBtn->setCursor(QCursor(Qt::PointingHandCursor));


    connect(_InstallBtn, SIGNAL(install()), this, SLOT(beginInstall()));
    connect(_InstallBtn, SIGNAL(cancelInstall()), this, SLOT(cancelInstall()));

    connect(_UpdateBtn, SIGNAL(update()), this, SLOT(beginUpdate()));
    connect(_UpdateBtn, SIGNAL(cancelUpdate()), this, SLOT(cancelUpdate()));

    _progressBar = new QProgressBar(this);
    _progressBar->hide();
    _progressBar->setRange(0,100);

    _movieLabel = new QLabel(this);
    _movie = new QMovie(":/images/app_loader.gif");
    _movieLabel->setMovie(_movie);
    _movie->start();

    _scoreWdg = new ScoreWidget(this);
    _scoreWdg->setFixedSize(100,20);
    _scoreWdg->show();

    connect(_scoreWdg, SIGNAL(appscored(int)), this, SLOT(appScored(int)));

    _timer = new QTimer(this);
    _textTimer = new QTimer(this);

    connect(_timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    connect(_textTimer,SIGNAL(timeout()),this,SLOT(textTimerUpDate()));
}

void AppDetailInfoWidget::showApp(AppData* data)
{
    _appData = data;

    clearWidget();

    if (data == NULL) {
        _movieLabel->show();
        return;
    }
    refreshBtn();
    _movieLabel->hide();
    _scoreWdg->show();
    _detailTitle->show();
    _tabbg = new QImage;
    if (_appData->_strIconPath != "" && !_tabbg->load(_appData->_strIconPath))
        _tabbg->load(AppDetailDialogExe::Instance()->defaultIcon());

    QByteArray strTmp = _appData->_name.toAscii();
    _TitleLabel->setText(QString::fromLocal8Bit(strTmp.data()));

    QString strBrief = QString("版本：%1\n大小：%2\n开发者：%3").arg(_appData->_version).arg(_appData->_size).arg(_appData->_developer);
    strTmp = strBrief.toAscii();
    _DescriptionLabel->setText(QString::fromLocal8Bit(strTmp.data()));

    strTmp = _appData->_intro.toAscii();
    _detailInfo->setText(QString::fromLocal8Bit(strTmp.data()));

    _scoreWdg->setScore(_appData->_rate, _appData->_added);
    repaint();
}

void AppDetailInfoWidget::closeEvent ( QCloseEvent * event )
{
    Q_UNUSED(event);

    _pCloseBtn->close();
    _bPressTitle = false;

    if (_appData)
        delete _appData;
    _appData = NULL;

    clearWidget();
    QDialog::close();
}

void DrawImg(QPainter* pPainter, const QString &strImgPath, const QRect &rect);
void AppDetailInfoWidget::paintEvent( QPaintEvent *event )
{
    Q_UNUSED(event);

    QPainter painter(this);
    DrawImg(&painter, ":images/bg_dialog.png", rect());

    //draw icon
    if (_tabbg)
      painter.drawImage(_rIcon, *_tabbg, QRectF(0.0, 0.0, 0.0, 0.0));

    if (_error != "") {
         QFontMetrics metrics(this->font());
         int length =  metrics.width(_error);
         int textHeight = metrics.height();

         painter.setPen(Qt::gray);
         painter.drawText(QRect((width()-length)/2, height() / 2, length, textHeight + 5),_error);
    }

    if (_scoreSucceed) {

        QRect rSuccess = QRect(_scoreWdg->geometry().right() + 5,
                               _scoreWdg->geometry().top(),
                               27, 23);

        painter.drawImage(rSuccess, *_succes);
        painter.setPen(QPen(QColor(0,0,0,_alpha)));
     //   painter.fillRect(rSuccess, QBrush(QColor("#006699")));
        painter.fillRect(rSuccess, QBrush(QColor(235,235,235, _alpha)));

        if (_alpha == 255) {
            _timer->stop();
            _scoreSucceed = false;
        }
    }

    if (_textAlpha != -1) {

        painter.setPen(QPen(QColor(0,0,0,_textAlpha)));
        painter.drawText(QRect(_detailTitle->geometry().right() + 20, _rIcon.bottom() + 10, 200, 20), _state);
        if (_textAlpha == 0) {
            _textTimer->stop();
            _textAlpha = -1;
        }
    }
}

void AppDetailInfoWidget::resizeEvent( QResizeEvent* event)
{
    Q_UNUSED(event);

    _UpdateBtn->move(width() - 106 - Shadow_Edge_Width, _rIcon.bottom() - 24);

    _InstallBtn->move(width() - 106 - Shadow_Edge_Width, _rIcon.bottom() - 24);

    _pCloseBtn->move(width() - Shadow_Edge_Width - 20, Shadow_Edge_Width + 10);

    _TitleLabel->move(_rIcon.right() + 8, _rIcon.top() + 2);
    _DescriptionLabel->move(_rIcon.right() + 8,_rIcon.top() +  _TitleTextHeight + 20);

    _scoreWdg->move(_rIcon.right() + 8, _rIcon.bottom() - 20);

    _detailTitle->move(_rIcon.left(), _rIcon.bottom() + 25);
    QRect detai(_rIcon.left(), _rIcon.bottom() + 25 + _TitleTextHeight,
                width() - 2 * _rIcon.left(), height() - _rIcon.bottom() - 20 - Shadow_Edge_Width);
    _detailInfo->setGeometry(detai);

    QRect rProgressBar(_rIcon.left(), _rIcon.bottom() + 10,
                   width() - 106 - Shadow_Edge_Width - _rIcon.left(), 10);
    _progressBar->setGeometry(rProgressBar);


    int wi = width();
    int h = height();

    int movieWidth = 50;
    int r = movieWidth / 2;
    _movieLabel->setGeometry(wi / 2 - r,  h / 2- r, movieWidth,movieWidth);

}

void AppDetailInfoWidget::showEvent ( QShowEvent * event )
{
    _pCloseBtn->show();
    QDialog::showEvent(event);
}

void AppDetailInfoWidget::mousePressEvent ( QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
        return;

    _bPressTitle = true;
    _beginDrag = event->pos();

    QDialog::mousePressEvent(event);
}

void AppDetailInfoWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!_bPressTitle)
        return;

    if (!(event->buttons() & Qt::LeftButton))
        return;

    //convert to relative pos
    QPoint relaPos(QCursor::pos() - _beginDrag);
    move(relaPos);

    QDialog::mouseMoveEvent(event);
}

void AppDetailInfoWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _bPressTitle = false;
    QDialog::mouseReleaseEvent(event);
}

void AppDetailInfoWidget::beginInstall()
{
    _progressBar->show();
    _progressBar->setValue(0);

    _InstallBtn->setState(InstallButton::loading);
    AppDataReadExe::Instance()->installApp(_appData);
}

void AppDetailInfoWidget::refreshBtn()
{
    if (_appData == NULL) {

        _InstallBtn->hide();
 //       _UnInstallBtn->hide();
        _UpdateBtn->hide();

        return;
    }
    if (_appData->_added) {
        _InstallBtn->hide();
        _UpdateBtn->show();

        if (_appData->_isCanUpdated)
            _UpdateBtn->setState(UpdateButton::canUpdate);
        else
            _UpdateBtn->setState(UpdateButton::noUpdate);
    }
    else {
        _InstallBtn->resetState();
        _InstallBtn->show();
 //       _UnInstallBtn->hide();
        _UpdateBtn->hide();
    }
}

void AppDetailInfoWidget::finishInstall()
{
    if (_appData == NULL)
        return;
    _appData->_added = true;
    refreshBtn();
}

void AppDetailInfoWidget::finishUnInstall()
{
    if (_appData == NULL)
        return;
    _appData->_added = false;
    _InstallBtn->resetState();
    refreshBtn();
}

void AppDetailInfoWidget::AppError(const QString &error)
{
    _error = error;
}

void AppDetailInfoWidget::cancelInstall()
{
/*    switch(QMessageBox::information(this, "提示", "确定取消下载安装文件吗？", " 确定 ", "取消",0,1 ) ) {
        case 0:
            break;
        case 1:
            return;
    }
*/
    AppDataReadExe::Instance()->InstallCanceled();
    _progressBar->hide();
    _InstallBtn->setState(InstallButton::unAdded);
}


void AppDetailInfoWidget::Progress(qint64 recieve, qint64 total)
{
    if (total < 1024)
        return;
    qint64 rate = total/100;
    recieve = recieve/rate;
    _progressBar->setValue(recieve);
}


void  AppDetailInfoWidget::installFinished()
{
    if (!_InstallBtn->isHidden()) {
        _InstallBtn->hide();
        _UpdateBtn->show();
        showMessage("安装完成");
    }
    else if (!_UpdateBtn->isHidden()) {
        _UpdateBtn->setState(UpdateButton::noUpdate);
        showMessage("更新完毕");
    }
    _progressBar->hide();

    if (_appData) {
         AppData* app = AppDatabase::Instance()->getData(_appData->_name);
         if (app)
            app->_added = true;
           _scoreWdg->setScore(3, true);
    }
}

void AppDetailInfoWidget::installDownloaded()
{
    _progressBar->setValue(100);
    if (!_InstallBtn->isHidden())
         _InstallBtn->setState(InstallButton::adding);
    if (!_UpdateBtn->isHidden())
         _UpdateBtn->setState(UpdateButton::updating);
}


void AppDetailInfoWidget::messageBox(const QString &msg,const QString &title,
                                     const QString &btn, QString state)
{
    if (state == "critical")
        QMessageBox::critical(this, title, msg, btn);
    else
        QMessageBox::information(this, title, msg, btn);
}

void AppDetailInfoWidget::beginUpdate()
{
    _progressBar->show();
    _progressBar->setValue(0);

    _UpdateBtn->setState(UpdateButton::loading);
    AppDataReadExe::Instance()->installApp(_appData);
}

void AppDetailInfoWidget::cancelUpdate()
{
    AppDataReadExe::Instance()->InstallCanceled();
    _progressBar->hide();
    _UpdateBtn->setState(UpdateButton::canUpdate);
}

void AppDetailInfoWidget::installCanceled()
{
    _progressBar->hide();
    if (!_InstallBtn->isHidden())
         _InstallBtn->setState(InstallButton::unAdded);
    if (!_UpdateBtn->isHidden())
         _UpdateBtn->setState(UpdateButton::canUpdate);
}

QString AppDetailInfoWidget::convertByte(qint64 total)
{
    Q_UNUSED(total);

    return "";
}

void AppDetailInfoWidget::loadFailed()
{
    AppMessageBox box(false, NULL);
    box.setText("下载安装文件失败，请检查网络连接");
    box.exec();
    if (!_InstallBtn->isHidden()) {
        //QMessageBox::critical(this, "错误", "下载安装文件失败，请检查网络连接", "确定");
        _InstallBtn->setState(InstallButton::unAdded);
    }
    else if (!_UpdateBtn->isHidden()) {
        //QMessageBox::critical(this, "错误", "下载更新文件失败，请检查网络连接", "确定");
        _UpdateBtn->setState(UpdateButton::canUpdate);
    }
    _progressBar->hide();
}

void AppDetailInfoWidget::centerWidget()
{
    QPoint pos;
    if (parentWidget() == NULL) {
        pos.setX(QApplication::desktop()->width()/2 - width()/2);
        pos.setY(QApplication::desktop()->height()/2 - height()/2);
    }
    else {
        pos.setX(parentWidget()->width()/2 - width()/2);
        pos.setY(parentWidget()->height()/2 - height()/2);
    }
    move(pos);
}

void AppDetailInfoWidget::clearWidget()
{
    _movieLabel->hide();
    _TitleLabel->setText("");
    _DescriptionLabel->setText("");
    _detailInfo->setText("");
    _detailTitle->hide();
    _progressBar->hide();
    _scoreWdg->hide();
    _scoreSucceed = false;
    _textAlpha = -1;

    if (_tabbg)
        delete _tabbg;
    _tabbg = NULL;

    _InstallBtn->hide();
    _UpdateBtn->hide();

    _error = "";
    repaint();
}

void AppDetailInfoWidget::appScored( int score)
{
    if (!_appData)
        return;

    QString str = QString("rate=%1").arg(score);
    AppDataReadExe::Instance()->postData(_appData->_name,str.toAscii() );
}

void AppDetailInfoWidget::OnUpdateScore(QString appName, int score)
{
    if (_appData && _appData->_name == appName) {
        _scoreSucceed = true;
        _alpha = 0;
        _timer->start(100);
        _scoreWdg->setScore(score, _appData->_added);
    }
}

void AppDetailInfoWidget::timerUpDate()
{
    _alpha += 10;
    if (_alpha > 255)
        _alpha = 255;
    repaint();
}
void AppDetailInfoWidget::textTimerUpDate()
{
    _textAlpha -= 8;
    if (_textAlpha < 0)
        _textAlpha = 0;
    repaint();
}
void AppDetailInfoWidget::showMessage(QString msg)
{
    _state = msg;
    _textAlpha = 255;
    _textTimer->stop();
    _textTimer->start(100);
}
