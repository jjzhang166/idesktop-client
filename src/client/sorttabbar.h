/*
 *
 *  Portions Copyright (C) 2009 wind (lvlisong@redflag-linux.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _SORTTABBAR_H_
#define _SORTTABBAR_H_

#include <QTabBar>
#include <QTimer>
#include <QWeakPointer>

class QPropertyAnimation;

class SortTabBar : public QTabBar {
	Q_OBJECT
	Q_PROPERTY(qreal animValue READ animValue WRITE setAnimValue)

    signals:

	public:
        SortTabBar(const int &bgWidth, const int bgHeight,
                   const QString &tabBg, const QString &tabCurBg, QWidget* parent = NULL);
		qreal animValue() const;

	protected:
        QSize tabSizeHint(int index) const;
        void paintEvent(QPaintEvent* event);
        void leaveEvent(QEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent* event);
        void dragEnterEvent(QDragEnterEvent *event);

	protected slots:
		void switchToHoveredTab();
		void animationFinished();
		void startAnimation();
		void setAnimValue(qreal value);

	private:
		QPainterPath tabPath(const QRectF &r);
		void storeLastIndex();
		void setCurrentIndexWithoutAnimation(int index);
		int lastIndex() const;

		QTimer _tabSwitchTimer;
		QRectF _currentAnimRect;

		int _lastIndex[2];
		QWeakPointer<QPropertyAnimation> _animation;

        QImage* _tabCurbg;
        QImage* _tabbg;
		qreal _animProgress;
		int _hoveredTabIndex;
		bool _switchOnHover;
		bool _animateSwitch;

        int _bgHeight;
        int _bgWidth;

};

#endif // _TABBAR_H_
