#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QLineEdit>
#include <QTimer>
#include <QModelIndex>
#include <QStringList>
#include <QListView>

/*
QListView::item{
    height:24px;
}
*/

class SearchListView;
class QStringListModel;
class QPushButton;
class QListView;
class QImage;
class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:

    enum search_style {
        none_button, button_available, button_unavailable
    };

    explicit SearchLineEdit(search_style style = button_available,
                            const QString &icon = "",
                            const QString &text = "",
                            QWidget *parent = 0);
    
    void gMove();
    void closeControls();

    void setSearchResult(QStringList results);

signals:

    void clickButton(const QString &text);
    void chooseItem(const QString &text);
    
public slots:

    void timerUpDate();
    void setCompleter(const QString &);
    void completeText(const QModelIndex &);

    void ButtonClicked();

protected:

    void resizeEvent( QResizeEvent * event);
    void keyPressEvent(QKeyEvent *e);
    void moveEvent ( QMoveEvent * event );
    void paintEvent(QPaintEvent *event);

private:

    QPushButton* _searchBtn;
    SearchListView* _listView;

    QTimer* _timer;


    QStringList _words; // 整个完成列表的单词
    QStringListModel *_model; // 完成列表的model

    search_style _style;
    QImage* _image;
};

#endif // SEARCHLINEEDIT_H
