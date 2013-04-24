#include "tabpushbuttonbar.h"
#include "tablabel.h"
#include "custombtn.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

void DrawImg(QPainter* pPainter, const QString &strImgPath, const QRect &rect);
void DrawImg(QPainter* pPainter, const QString &strImgPath, const int &x, const int &y, const QSize &size);


TabPushButtonBarParent::TabPushButtonBarParent(QWidget *parent, Qt::WindowFlags f , const int &level) :
    OpacityWidget(parent, f, level)
{
}

void TabPushButtonBarParent::Current(TabLabel* pLabel)
{
    if (!pLabel)
        return;
    QString str = pLabel->text();
    this->showCurrent(str);
}


TabPushButtonBar::TabPushButtonBar(const int &tabSum/* = 0 */,
                             const int &maxNum /*= 5 */,
                             TabPushButtonBarParent* parentBase/* = 0*/) :
    QWidget(parentBase)
  , _tabSum(tabSum)
  , _currentPos(0)
  , _VSpacerLayout(NULL)
  , _HWidgetLayout(NULL)
  , _parentBase(parentBase)
{
    _maxNum = maxNum > 3 ? maxNum : 3;

    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(215,215,255,0));
    setPalette(pal);

    _preButton = new CustomBtn(this);
    _nextButton = new CustomBtn(this);

    _preButton->setFixedSize(35,23);
    _nextButton->setFixedSize(35,23);

    _preButton->setCursor(QCursor(Qt::PointingHandCursor));
    _nextButton->setCursor(QCursor(Qt::PointingHandCursor));

    _preButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    _nextButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    _preButton->SetOnImg(":images/icon_page_pre_hover.png");
    _preButton->SetOffImg(":images/icon_page_pre.png");
    _preButton->SetEnabledImg(":images/icon_page_pre_end.png");

    _nextButton->SetOnImg(":images/icon_page_next_hover.png");
    _nextButton->SetOffImg(":images/icon_page_next.png");
    _nextButton->SetEnabledImg(":images/icon_page_next_end.png");

    _preButton->show();
    _nextButton->show();


    _VSpacerLayout = new QVBoxLayout(this);
    _HWidgetLayout = new QHBoxLayout();
    _HWidgetLayout->addWidget(_preButton);

    _VSpacerLayout->addLayout(_HWidgetLayout);

    connect(_preButton, SIGNAL(clicked()), this, SLOT(PreButtonClicked()));
    connect(_nextButton, SIGNAL(clicked()), this, SLOT(NextButtonClicked()));


    ReShow();
}
void TabPushButtonBar::ReShow()
{
    //清除水平layout中的数字按钮和下一页按钮，重新装填
    for (int i = 0;i < _LabelList.size();i++) {
        _HWidgetLayout->removeWidget(_LabelList.at(i));
        delete _LabelList.at(i);
    }
    _LabelList.clear();
    _HWidgetLayout->removeWidget(_nextButton);

    int nNum =  _maxNum > _tabSum ? _tabSum : _maxNum;

    for (int i = 1;i <= nNum;i++) {
        TabLabel* pLabel = new TabLabel( this);
        pLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        pLabel->setFixedSize(24,23);

        _HWidgetLayout->addWidget(pLabel);
        _LabelList.push_back(pLabel);
        connect(pLabel, SIGNAL(Chosen(TabLabel*)), this, SLOT(Refresh(TabLabel*)));
        if (_parentBase)
           connect(pLabel, SIGNAL(Chosen(TabLabel*)), _parentBase , SLOT(Current(TabLabel*)));
    }
    _HWidgetLayout->addWidget(_nextButton);

    _currentPos = 1;
    Refresh("1");
}
void TabPushButtonBar::Refresh(QString url)
{
    _currentPos = url.toInt();

    if (_currentPos > _tabSum)
        return;

    for (int i = 0;i < _LabelList.size();i++) {
        _LabelList.at(i)->setEnabled(true);
        _LabelList.at(i)->current(false);
    }

    int nMid = _maxNum / 2 + 1;

    QString strIndex;
    for (int i = 1;i <= _LabelList.size();i++) {
        TabLabel* pLabel = _LabelList.at(i - 1);
        if (_maxNum >= _tabSum) {
            strIndex.sprintf("%d", i);
        }
        else {
            if (i == 1) {
                strIndex.sprintf("%d", i);
            }
            else if (i == _LabelList.size()) {
                strIndex.sprintf("%d", _tabSum);
            }
            else {
                if (_currentPos >= (_tabSum - (_maxNum - nMid)) ) {
                    strIndex.sprintf("%d", _tabSum - (_maxNum - i));
                }
                else if (_currentPos <= nMid) {
                    strIndex.sprintf("%d", i);
                }
                else {
                    strIndex.sprintf("%d", _currentPos - nMid + i);
                }
            }
            if ( i == ( _maxNum - 1) && _currentPos + _maxNum - nMid < _tabSum) {
                    strIndex = "..";
            }
            else if (i == 2 && _currentPos > nMid ) {
                    strIndex = "..";
            }
        }

        pLabel->setText(strIndex);

        if (strIndex == ".." || url == strIndex)
            pLabel->setEnabled(false);
        if (url == strIndex)
            pLabel->current(true);
    }
    RefreshFlipButtons();
}

void TabPushButtonBar::Refresh(TabLabel* pLabel)
{
//    int oldPos = _currentPos;
    if (!pLabel)
        return;
    Refresh(pLabel->text());

}

void TabPushButtonBar::PreButtonClicked()
{
    _currentPos--;
    QString strIndex;
    strIndex.sprintf("%d", _currentPos);
    this->Refresh(strIndex);
    RefreshFlipButtons();
    if (_parentBase)
        _parentBase->showCurrent(strIndex);
}

void TabPushButtonBar::NextButtonClicked()
{
    _currentPos++;
    QString strIndex;
    strIndex.sprintf("%d", _currentPos);
    this->Refresh(strIndex);
    RefreshFlipButtons();
    if (_parentBase)
        _parentBase->showCurrent(strIndex);
}

void TabPushButtonBar::RefreshFlipButtons()
{
    _nextButton->enabled(true);
    _preButton->enabled(true);
    int nLabelNum = _maxNum > _tabSum ? _tabSum : _maxNum;
    if (_currentPos >= nLabelNum)
    {
        _currentPos = nLabelNum;
        _nextButton->enabled(false);
    }
    if (_currentPos <= 1)
    {
        _currentPos = 1;
        _preButton->enabled(false);
    }
    _preButton->repaint();
    _nextButton->repaint();
}

void TabPushButtonBar::currentTabIndex(int tabIndex)
{
    QString strIndex;
    strIndex.sprintf("%d", tabIndex);
    Refresh(strIndex);
}

void TabPushButtonBar::tabSum(int tabSum)
{
    _tabSum = tabSum;
    if (_tabSum <= 0)
        _tabSum = 1;
    ReShow();
}
