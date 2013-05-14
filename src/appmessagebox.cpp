#include "appmessagebox.h"
#include "dynamicbutton.h"

#define WIDTH 350
#define HEIGHT 200

AppMessageBox::AppMessageBox(bool hasCancel, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::Tool)
  , _bPressTitle(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0x00));
    setPalette(pal);

    setFixedSize(WIDTH, HEIGHT);

    QPixmap closePix(":images/message/close.png");
    DynamicButton *b = new DynamicButton(closePix, closePix, this);
    b->move(WIDTH - 35, 10);
    label = new QLabel(this);
    label->move(40, 80);
    label->setFont(QFont("", 12, QFont::Normal));

    QPixmap okPix(":images/message/submit.png");
    QPixmap okPixHover(":images/message/submit_hover.png");
    DynamicButton *okButton = new DynamicButton(okPix, okPixHover, this);
    okButton->move(121, 140);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    if (hasCancel) {
        QPixmap clPix(":images/message/cancel.png");
        QPixmap clPixHover(":images/message/cancel_hover.png");
        DynamicButton *clButton = new DynamicButton(clPix, clPixHover, this);
        connect(clButton, SIGNAL(clicked()), this, SLOT(reject()));
        okButton->move(64, 140);
        clButton->move(190, 140);
    }
    connect(b, SIGNAL(clicked()), this, SLOT(reject()));
}


void AppMessageBox::setText(const QString &text)
{
    label->setText(text);
}

void AppMessageBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter pt(this);
    pt.drawPixmap(21, 21, QPixmap(":images/message/bg.png").scaled(WIDTH - 21 * 2, HEIGHT - 21 * 2));
    pt.drawPixmap(0, 0, QPixmap(":images/message/top_left_bg.png"));
    pt.drawPixmap(WIDTH - 21, 0, QPixmap(":images/message/top_right_bg.png"));
    pt.drawPixmap(0, HEIGHT - 21, QPixmap(":images/message/bottom_left_bg.png"));
    pt.drawPixmap(WIDTH - 21, HEIGHT - 21, QPixmap(":images/message/bottom_right_bg.png"));
    pt.drawPixmap(21, 0, QPixmap(":images/message/top_bg.png").scaled(WIDTH-21*2, 21));
    pt.drawPixmap(21, HEIGHT - 21, QPixmap(":images/message/bottom_bg.png").scaled(WIDTH-21*2, 21));
    pt.drawPixmap(0, 21, QPixmap(":images/message/left_bg.png").scaled(21, HEIGHT-21*2));
    pt.drawPixmap(WIDTH - 21, 21, QPixmap(":images/message/right_bg.png").scaled(21, HEIGHT-21*2));
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
