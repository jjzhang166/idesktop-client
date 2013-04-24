#include "widgetrefreshbase.h"
#include "widgetsrefreshexe.h"

WidgetRefreshBase::WidgetRefreshBase()
{
    WidgetsRefreshExe::Instance()->atech(this);
}
