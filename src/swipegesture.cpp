#include "swipegesture.h"

#include <QtGui>
#include <QGesture>
#include <QMouseEvent>
#include <math.h>

bool
SwipeGesture::IsValidMove(int dx, int dy)
{
    // The moved distance is to small to count as not just a glitch.
    if ((qAbs(dx) < MINIMUM_DISTANCE) && (qAbs(dy) < MINIMUM_DISTANCE)) {
        return false;
    }

    return true;
}


// virtual
QGesture*
SwipeGesture::create(QObject* pTarget)
{
    qDebug("SwipeGesture::create() called");
    QGesture *pGesture = new QSwipeGesture(pTarget);
    return pGesture;
}


// virtual
QGestureRecognizer::Result
SwipeGesture::recognize(QGesture* pGesture, QObject *pWatched, QEvent *pEvent)
{
    QGestureRecognizer::Result result = QGestureRecognizer::Ignore;
    QSwipeGesture *pSwipe = static_cast<QSwipeGesture*>(pGesture);

    switch(pEvent->type()) {
    case QEvent::MouseButtonPress: {
        QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
        if ((pMouseEvent->modifiers() != Qt::NoModifier) || pMouseEvent->button() != Qt::LeftButton) {
            return QGestureRecognizer::CancelGesture;
        }
        pSwipe->setProperty("startPoint", pMouseEvent->posF());
        result = QGestureRecognizer::MayBeGesture;
        qDebug("Swipe gesture started");
    }
        break;

    case QEvent::MouseButtonRelease: {
        QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
        if ((pMouseEvent->modifiers() != Qt::NoModifier) || pMouseEvent->button() != Qt::LeftButton) {
            return QGestureRecognizer::CancelGesture;
        }

        const QVariant& propValue = pSwipe->property("startPoint");
        QPointF startPoint = propValue.toPointF();
        QPointF endPoint = pMouseEvent->posF();

        // process distance and direction
        int dx = endPoint.x() - startPoint.x();
        int dy = endPoint.y() - startPoint.y();

        if (!IsValidMove(dx, dy)) {
            // Just a click, so no gesture.
            result = QGestureRecognizer::CancelGesture;
            qDebug("Swipe gesture canceled");
        } else {
            // Compute the angle.
            qreal angle = ComputeAngle(dx, dy);
            pSwipe->setSwipeAngle(angle);
            result = QGestureRecognizer::FinishGesture;
            qDebug("Swipe gesture finished");
        }
    }
        break;
    default:
        break;
    }

    return result;
}

void
SwipeGesture::reset(QGesture *pGesture)
{
    pGesture->setProperty("startPoint", QVariant(QVariant::Invalid));
    parent::reset(pGesture);
}

qreal
SwipeGesture::ComputeAngle(int dx, int dy)
{
    double PI = 3.14159265;

    // Need to convert from screen coordinates direction
    // into classical coordinates direction.
    dy = -dy;

    double result = atan2((double)dy, (double)dx) ;
    result = (result * 180) / PI;

    // Always return positive angle.
    if (result < 0) {
        result += 360;
    }

    return result;
}
