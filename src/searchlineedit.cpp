#include "searchlineedit.h"

#include <QPushButton>

#include <QString>
#include <QSize>
#include <QCursor>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QByteArray>
#include <QVBoxLayout>
#include <QPainter>
#include <QListView>
#include <QStringList>
#include <QStringListModel>
#include <QKeyEvent>
#include <QImage>

#include "searchlineview.h"
#include "client/appdatareadexe.h"

void DrawImg(QPainter* pPainter, const QString &strImgPath, const QRect &rect);

SearchLineEdit::SearchLineEdit(search_style style /* = button_available*/,
                               const QString &icon /* = "" */ ,
                               const QString &text /* = "" */,
                               QWidget *parent /* = 0 */) :
    QLineEdit(parent)
  , _searchBtn(NULL)
  , _style(style)
{

    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(215,255,255,0));
    setPalette(pal);

    _image = new QImage(":images/bg_search.png");

    if (_style & button_available || _style & button_unavailable) {
      _searchBtn = new QPushButton(this);

      _searchBtn->setFocusPolicy(Qt::NoFocus);    // �õ�����ʱ������ʾ���߿�
      _searchBtn->setFlat(true);
      _searchBtn->setText(text);
      _searchBtn->setCursor(QCursor(Qt::PointingHandCursor));

      QString qss;
      QByteArray strTmp = icon.toAscii();
      qss.sprintf(" border-image : url(%s)", strTmp.data());
     _searchBtn->setStyleSheet(qss);


      QHBoxLayout *buttonLayout = new QHBoxLayout();
      buttonLayout->setContentsMargins(0, 0, 0, 0);
      buttonLayout->addStretch();
      buttonLayout->addWidget(_searchBtn);
      setLayout(buttonLayout);

      if (_style & button_available)
          connect(_searchBtn, SIGNAL(clicked()), this, SLOT(ButtonClicked()));
      else
          _searchBtn->setEnabled(false);
    }
//    QTimer *_timer = new QTimer(this);

//    connect(_timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));

    //������ʱ�������źź���Ӧ�Ĳۺ���

 //   _timer->start(10);
//

/*    _listView = new SearchListView(this);
    _listView->setWindowFlags(Qt::ToolTip);
    _model = new  QStringListModel(_words, this);
    _listView->setModel(_model);

*/
    _listView = new SearchListView(parent);
    _listView->setFixedHeight(150);
//    _listView->show();

    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(setCompleter(const QString &)));
    connect(_listView, SIGNAL(listClicked(const QModelIndex &)), this, SLOT(completeText(const QModelIndex &)));
}

void SearchLineEdit::resizeEvent( QResizeEvent * event)
{
    QSize size(sizeHint().height()-3, sizeHint().height()-3);
    if (_searchBtn) {
         _searchBtn->setMinimumSize(size);
         _searchBtn->setMaximumSize(size);           // ���ð�ť�Ĵ�СΪedit�߶ȵĴ�С
    }
    // ������������ļ�����������������������ڱ������ڰ�ť��
    setTextMargins(0, 1, size.width(), 1);

    _listView->setMinimumWidth(width());
    _listView->setMaximumWidth(width());
}

void SearchLineEdit::timerUpDate()
{
    if (_listView->isHidden())
        return;

    QPoint p(0, height());
    int x = mapToGlobal(p).x();
    int y = mapToGlobal(p).y() + 1;

    _listView->move(x, y);
}

void SearchLineEdit::setCompleter(const QString &text)
{
    if (text.isEmpty()) {
        _listView->hide();
        return;
    }

    if ((text.length() > 1) && (!_listView->isHidden()))
        return;

    AppDataReadExe::Instance()->searchApp(text);
    // �������������б��е�ĳ�����ʰ���������ı��������Ҫ��ʾ������б���
}

void SearchLineEdit::completeText(const QModelIndex &index)
{
    QString text = index.data().toString();
    setText(text);
    _listView->hide();

    emit chooseItem(text);
}

void SearchLineEdit::keyPressEvent(QKeyEvent *e) {
    if (!_listView->isHidden()) {
        int key = e->key();
        int count = _listView->model()->rowCount();
        QModelIndex currentIndex = _listView->currentIndex();

        if (Qt::Key_Down == key) {
            // �����·����ʱ���ƶ����ѡ����һ������б��е���
            int row = currentIndex.row() + 1;
            if (row >= count)
                row = 0;

            QModelIndex index = _listView->model()->index(row, 0);
            _listView->setCurrentIndex(index);
        } else if (Qt::Key_Up == key) {
            // �����·����ʱ���ƶ����ѡ����һ������б��е���
            int row = currentIndex.row() - 1;
            if (row < 0)
                row = count - 1;

            QModelIndex index = _listView->model()->index(row, 0);
            _listView->setCurrentIndex(index);
        } else if (Qt::Key_Escape == key) {
            // ����Esc��ʱ����������б�
            _listView->hide();
        } else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
            // ���»س���ʱ��ʹ������б���ѡ�е������������б�
            if (currentIndex.isValid()) {
                QString text = _listView->currentIndex().data().toString();
                setText(text);
            }
            _listView->hide();
        } else {
            // �����������������б���ʹ��QLineEdit�ļ��̰����¼�
            _listView->hide();
            QLineEdit::keyPressEvent(e);
        }
    } else {
        QLineEdit::keyPressEvent(e);
    }
}

void SearchLineEdit::closeControls()
{
    if (_listView)
        _listView->hide();

    this->setText("");
}

void SearchLineEdit::ButtonClicked()
{
    emit clickButton(text());
}

void SearchLineEdit::moveEvent ( QMoveEvent * event )
{
    gMove();
}

void SearchLineEdit::gMove()
{
    timerUpDate();
}

void SearchLineEdit::setSearchResult(QStringList results)
{
/*    _model->setStringList(results);
   //    _listView->setGridSize();

    _listView->setModel(_model);
*/
    _listView->setStringList(results);
    if (_listView->model()->rowCount() == 0)
           return;
    _listView->show();
    timerUpDate();
}

void SearchLineEdit::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);
    QPainter painter(this);
    if (!text().isEmpty())
        return;

    QRect rt = rect();
    painter.setPen(Qt::gray);
    painter.drawText(rt.translated(8,1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextHideMnemonic, "������Ҫ����������");
  //  painter.drawImage(rect(), * _image);
 //   DrawImg(&painter, ":images/bg_search.png",rect());
}
