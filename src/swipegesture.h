#ifndef SWIPEGESTURE_H
#define SWIPEGESTURE_H
#include <QGestureRecognizer>

class SwipeGesture: public QGestureRecognizer
{
private:
   static const int MINIMUM_DISTANCE = 100;

   typedef QGestureRecognizer parent;

   bool IsValidMove(int dx, int dy);

   qreal ComputeAngle(int dx, int dy);

   virtual QGesture* create(QObject* pTarget);

   virtual QGestureRecognizer::Result recognize(QGesture* pGesture, QObject *pWatched, QEvent *pEvent);

   void reset (QGesture *pGesture);
};

#endif // SWIPEGESTURE_H
