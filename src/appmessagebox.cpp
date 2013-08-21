#include <QFontMetrics>

#include "appmessagebox.h"
#include "dynamicbutton.h"

#define WIDTH 280
#define HEIGHT 200

AppMessageBox::AppMessageBox(bool hasCancel, QWidget *parent)
    : QDialog(parent, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool)
    , _bPressTitle(false)
{
    QTextCodec *codec = QTextCodec::codecForName("System"); //System
    QTextCodec::setCodecForCStrings(codec);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0x00));
    setPalette(pal);

    setFixedSize(WIDTH, HEIGHT);

    QPixmap closePix(":images/message/message_close.png");
    QPixmap closePixHover(":images/message/message_close_hover.png");
    DynamicButton *b = new DynamicButton(closePix, closePixHover, this);
    b->move(WIDTH - 45, 30);            //WIDTH - 35, 10
    label = new QLabel(this);

    QPixmap okPix(":images/message/message_submit_normal.png");
    QPixmap okPixHover(":images/message/message_submit_hover.png");
    DynamicButton *okButton = new DynamicButton(okPix, okPixHover, this);
    okButton->setGeometry((WIDTH - 65) / 2, 140, 65, 30);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    if (hasCancel) {
        QPixmap clPix(":images/message/message_cancel_normal.png");
        QPixmap clPixHover(":images/message/message_cancel_hover.png");
        DynamicButton *clButton = new DynamicButton(clPix, clPixHover, this);
        connect(clButton, SIGNAL(clicked()), this, SLOT(reject()));
        okButton->move(52, 140);        //64 140
        clButton->move(163, 140);       //190 140
    }
    connect(b, SIGNAL(clicked()), this, SLOT(reject()));
}


void AppMessageBox::setText(const QString &text)
{
    QFont font(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), 10, QFont::Normal);
    QFontMetrics fm(font);
    int _textWidth = fm.width(text);

    label->resize(_textWidth, 12);
    label->setFont(font);
    label->setStyleSheet("color:white;");
    label->setText(text);
    label->move((WIDTH - label->width()) / 2, (HEIGHT - label->height()) / 2 - 5);
}

void AppMessageBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter pt(this);
    pt.drawPixmap(0, 0, WIDTH, HEIGHT, QPixmap(":images/message/message_bg.png").scaled(WIDTH
                  , HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void AppMessageBox::mousePressEvent ( QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
        return;

    _bPressTitle = true;
    _beginDrag = event->pos();
}

void AppMessageBox::mouseMoveEvent(QMouseEvent *event)
{
    if (!_bPressTitle) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (!(event->buttons() & Qt::LeftButton)) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    //convert to relative pos
    QPoint relaPos(QCursor::pos() - _beginDrag);
    move(relaPos);
    QDialog::mouseMoveEvent(event);
}

void AppMessageBox::mouseReleaseEvent(QMouseEvent *event)
{
    _bPressTitle = false;
    QDialog::mouseReleaseEvent(event);
}
