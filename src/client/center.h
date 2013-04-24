#ifndef CENTER_H
#define CENTER_H
#include <QWidget>
#include <QStackedLayout>

class PackageWidget;
class DetailWidget;
class TabBar;

class AppCenter : public QWidget {
    Q_OBJECT
public:
    AppCenter(QWidget * parent = NULL);
    ~AppCenter();
    void refresh();
public slots:
    void onCancel();
    void onDone();
    void onDetail(const QString &);
signals:
	void clearSearch();
protected:
	void resizeEvent(QResizeEvent * event);
private:
	QStackedLayout * stackedLayout;
    PackageWidget * pWidget;
    DetailWidget * dWidget;
    TabBar*  leftBar;
};
#endif
