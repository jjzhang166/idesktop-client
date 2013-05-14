#include "dynamicbutton.h"
#include "iconarrangedlg.h"

#define CLOSE_WIDTH 46
#define CLOSE_HEIGHT 35

IconArrangeDlg::IconArrangeDlg(VirtualDesktop *desk, QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::ToolTip | Qt::Tool), \
      _titlePressed(false), vdesk(desk)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0xff,0xff,0xff,0x00));
    setPalette(pal);

    setFixedSize(500, 420);
    QPixmap closeIcon(":images/widget/window_close.png");
    QPixmap closeIconHover(":images/widget/window_close_hover.png");
    DynamicButton *closeButton = new DynamicButton(closeIcon, closeIconHover, this);
    closeButton->setGeometry(width()- 46, 0, CLOSE_WIDTH, CLOSE_HEIGHT);


    QPushButton *okButton = new QPushButton(this);
    okButton->setText("确定");
    okButton->setGeometry(350, 360, 60, 30);

    QPushButton *clButton = new QPushButton(this);
    clButton->setText("取消");
    clButton->setGeometry(420, 360, 60, 30);


    list = new QScrollArea(this);
    listWidget = new QWidget(this);
    list->setWidget(listWidget);
    list->setLineWidth(0);
    list->setAutoFillBackground(true);
    list->setFrameShape(QFrame::NoFrame);
    list->setWidgetResizable(true);
    list->setGeometry(40, 40, 455, 300);
    list->verticalScrollBar()->setAutoFillBackground(true);
    QPalette lpal = palette();
    lpal.setColor(QPalette::Background, QColor(0xff,0xff,0xff,0xff));
    list->setPalette(lpal);

    iconCounter.clear();
    QGridLayout *layout = new QGridLayout;
    listWidget->setLayout(layout);
    layout->setVerticalSpacing(10);
    int row = 0;
    int pageCount = vdesk->count();
    for (int i = 0; i < pageCount; i++) {
        iconCounter.insert(i, 0);
        for (int j = 0; j < vdesk->_nextIdx[i]; j++) {
            QLabel *pix = new QLabel(listWidget);
            pix->setPixmap(vdesk->_iconTable[i][j]->originPixmap().scaled(64, 64));
            layout->addWidget(pix, row, 0, Qt::AlignLeft);
            QLabel *l = new QLabel(listWidget);
            QString text = vdesk->_iconTable[i][j]->text();
            if (text.startsWith("/"))
                text = text.right(text.length() - 1);
            l->setText(text);
            layout->addWidget(l, row, 1, Qt::AlignLeft);
            layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum), row, 2);
            TheComboBox *box = new TheComboBox(this, listWidget);
            for (int k = 0; k < pageCount; k++)
                box->addItem(QString("第%1桌面").arg(k+1), QVariant(k));
            box->setCurrentIndex(i);
            iconCounter[i]++;
            layout->addWidget(box, row, 3);
            iconList.insert(row, vdesk->_iconTable[i][j]);
            boxList.insert(row, box);
            row++;
        }
    }
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(clButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void IconArrangeDlg::accept()
{
    int page = 0;
    int p = 0;
    int count = 0;
    vdesk->_iconTable.clear();
    vdesk->_count = 0;
    vdesk->_nextIdx.clear();
    vdesk->_pages.clear();

    int gridWidth = ICONWIDTH + SPACING * 2;
    int gridHeight = ICONHEIGHT + SPACING * 2;
    vdesk->_gridTable.clear();

    while (count < boxList.count()) {
        int idx = 0;
        for (int i = 0; i < boxList.count(); i++) {
            if (boxList.at(i)->currentIndex() == page) {
                if (vdesk->_iconTable.count() <= p) {
                    vdesk->_pages.insert(p, -(p * vdesk->_width));
                    QList<IconItem *> list;
                    for (int j = 0; j < vdesk->_col * vdesk->_row; j++)
                        list.insert(j, NULL);
                    vdesk->_iconTable.insert(p, list);
                    QList<QRect> gridList;
                    for (int j = 0; j < vdesk->_col * vdesk->_row; j++) {
                        int x =  vdesk->_pageSize.width() * p \
                         + (j / vdesk->_row) * gridWidth;
                        int y = (j % vdesk->_row) * gridHeight;
                        gridList.insert(j, \
                            QRect(x, y, gridWidth, gridHeight));
                    }
                    vdesk->_gridTable.insert(p, gridList);
                }
                vdesk->_iconTable[p][idx] = iconList.at(i);
                iconList.at(i)->setPage(p);
                iconList.at(i)->setIndex(idx);
                idx++;
                count++;
            }
        }
        if (idx > 0) {
            vdesk->_nextIdx.insert(p, idx);
            p++;
        }
        page++;
    }
    vdesk->_count = p;
    vdesk->_current = 0;
    vdesk->move(vdesk->_pages[0], vdesk->y());
    setFixedSize(vdesk->_width * vdesk->_count, vdesk->_height);

    for (int i = 0; i < vdesk->_iconTable.count(); i++) {
        for (int j = 0; j < vdesk->_iconTable[i].count() && vdesk->_iconTable[i][j]; j++) {
            vdesk->_iconTable[i][j]->move(vdesk->_gridTable[i][j].translated(SPACING, SPACING).topLeft());
        }
    }
    vdesk->reload();
    QDialog::accept();

}

void IconArrangeDlg::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    QPainter painter(this);

    painter.setPen(QPen(Qt::white, 4, Qt::SolidLine));
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    painter.drawRect(41, 36, width() - 38 * 2, height() - 38 * 2);
    QPixmap upleft(":images/widget/window_corner_upperLeft.png");
    QPixmap upright(":images/widget/window_corner_upperRight.png");
    QPixmap lowleft(":images/widget/window_corner_lowerLeft.png");
    QPixmap lowright(":images/widget/window_corner_lowerRight.png");
    QPixmap top(":images/widget/window_side_top.png");
    QPixmap bot(":images/widget/window_side_bottom.png");
    QPixmap left(":images/widget/window_side_left.png");
    QPixmap right(":images/widget/window_side_right.png");
    painter.drawPixmap(0, 0, upleft);
    painter.drawPixmap(0, height() - 39, lowleft);
    painter.drawPixmap(width() - 39, 0, upright);
    painter.drawPixmap(width() - 33, height() - 39, lowright);
    painter.drawPixmap(39, 0, top.scaled(width() - 39 * 2, 35));
    painter.drawPixmap(39, height() - 39, bot.scaled(width() - 37 - 35, 39));
    painter.drawPixmap(0, 35, left.scaled(39, height() - 35 - 39));
    painter.drawPixmap(width() - 33, 35, right.scaled(39, height() - 35 - 39));

    painter.drawPixmap(15, 5, QPixmap(":images/logo_login.png"));
    painter.setPen(Qt::gray);
    painter.setFont(QFont("", 11, QFont::Black));
    painter.drawText(45, 23, "设置");
}

void IconArrangeDlg::mousePressEvent(QMouseEvent *event)
{
    _titlePressed = true;
    startDrag = event->pos();
    if (QRect(10, 10, 350, 35).contains(event->pos())) {
        ;
    }
}

void IconArrangeDlg::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    _titlePressed = false;
}

void IconArrangeDlg::mouseMoveEvent(QMouseEvent *event)
{
    if (_titlePressed) {
        QPoint newPoint = pos() + event->pos() - startDrag;
        move(newPoint);
    }
}

