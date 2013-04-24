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

      _searchBtn->setFocusPolicy(Qt::NoFocus);    // 得到焦点时，不显示虚线框
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

    //关联定时器计满信号和相应的槽函数

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
         _searchBtn->setMaximumSize(size);           // 设置按钮的大小为edit高度的大小
    }
    // 设置输入框中文件输入区，不让输入的文字在被隐藏在按钮下
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
    // 如果完整的完成列表中的某个单词包含输入的文本，则加入要显示的完成列表串中
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
            // 按向下方向键时，移动光标选中下一个完成列表中的项
            int row = currentIndex.row() + 1;
            if (row >= count)
                row = 0;

            QModelIndex index = _listView->model()->index(row, 0);
            _listView->setCurrentIndex(index);
        } else if (Qt::Key_Up == key) {
            // 按向下方向键时，移动光标选中上一个完成列表中的项
            int row = currentIndex.row() - 1;
            if (row < 0)
                row = count - 1;

            QModelIndex index = _listView->model()->index(row, 0);
            _listView->setCurrentIndex(index);
        } else if (Qt::Key_Escape == key) {
            // 按下Esc键时，隐藏完成列表
            _listView->hide();
        } else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
            // 按下回车键时，使用完成列表中选中的项，并隐藏完成列表
            if (currentIndex.isValid()) {
                QString text = _listView->currentIndex().data().toString();
                setText(text);
            }
            _listView->hide();
        } else {
            // 其他情况，隐藏完成列表，并使用QLineEdit的键盘按下事件
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
    painter.drawText(rt.translated(8,1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextHideMnemonic, "请输入要搜索的内容");
  //  painter.drawImage(rect(), * _image);
 //   DrawImg(&painter, ":images/bg_search.png",rect());
}
