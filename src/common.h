  #ifndef COMMON_H
#define COMMON_H

#include <QList>
#include <QString>
#include <QDebug>

#define XML_NAME ("/info.xml")


typedef struct _APP_LIST
{
    QString id;
    QString name;
    QString alias;
    QString addr;
    QString type;
    QString icon;   
    int page;
    int index;
    bool hidden;
} APP_LIST, *PAPP_LIST;

typedef struct _Commui
{
    QString _userType;//用户类型，0，工作组，1，ad
    QString _domain;//Ad用户类型下对应的ad域名
    QString _type; //应用类型 普通程序（默认）/Normal ：0 托盘程序/SystemTray ：1 桌面 /Desktop ：2 文件夹 /Directory ：3
                    //命令行程序 /Cmd ：4 浏览器 /InternetExplore ：5 系统管理程序 /SystemManageExe ：6 帮助程序 /Help ：7 MSN /MSN ：8
    QString _ip;
    QString _ipPort;
    QString _name;
    QString _applogin;
    QString _pswd;
    QString _id;
    QString _localime;
    QString _svrIp;
    QString _rdpIpPort;
    QString _viewLimited_carryWallpaper;//视觉限制——壁纸携带, 1允许0禁用
    QString _viewLimited_dragWithWindowContent;//视觉限制——窗体跟随, 1允许0禁用
    //QString _viewLimited_resolution;//视觉限制——屏幕分辨率, 0-9 对应的分辨率
    QString _viewLimited_resolution_width;//视觉限制——屏幕分辨率-宽，数值，若为isDefault，则用默认值
    QString _viewLimited_resolution_height;//视觉限制——屏幕分辨率-高，数值，若为isDefault，则用默认值
    QString _viewLimited_colorDepth;//视觉限制——色彩, 0-4 对应的色彩
    QString _viewLimited_advanced_windowWidth;//视觉限制——高级——窗口尺寸——长
    QString _viewLimited_advanced_windowHeight;//视觉限制——高级——窗口尺寸——宽
    QString _viewLimited_advanced_dragEffect;//视觉限制——高级——拖拽效果, 1允许0禁用
    QString _viewLimited_advanced_activeEffect;//视觉限制——高级——动态效果, 1允许0禁用
    QString _viewLimited_advanced_fontSmooth;//视觉限制——高级——字体平滑, 1允许0禁用ni
    QString _bandwidthLimited_audio;//客户端带宽限制——音频带宽(Kbps)
    QString _bandwidthLimited_clipboard;//客户端带宽限制——剪切板带宽(Kbps)
    QString _bandwidthLimited_COM;//客户端带宽限制——COM口带宽(Kbps)
    QString _bandwidthLimited_driver;//客户端带宽限制——驱动器带宽(Kbps)
    QString _bandwidthLimited_LPT;//客户端带宽限制—— LPT端口带宽(Kbps)
    QString _bandwidthLimited_session;//客户端带宽限制——整体会话带宽(Kbps)
    QString _bandwidthLimited_printer;//客户端带宽限制——打印机带宽(Kbps)
    QString _bandwidthLimited_TWAIN;//客户端带宽限制—— TWAIN带宽(Kbps)
    QString _audio_micInput;//音频——本地输入, 1允许0禁用
    QString _audio_clientAudioOutput;//音频——本地输出, 1允许0禁用
    QString _audio_quality;//音频——声音质量, 0-3 低中高
    QString _driver_loadClientDriverOnLogin;//驱动器——自动挂载, 1允许0禁用
    QString _driver_mapClientStorageOnLogin_check_floppy;//驱动器——映射——软驱, 1允许0禁用
    QString _driver_mapClientStorageOnLogin_check_harddisk;//驱动器——映射——硬盘, 1允许0禁用
    QString _driver_mapClientStorageOnLogin_check_CD;//驱动器——映射——光驱, 1允许0禁用
    QString _driver_mapClientStorageOnLogin_check_shareStorage;//驱动器——映射——共享存储, 1允许0禁用
    QString _interface_COM;//端口——COM端口, 1允许0禁用
    QString _interface_LPT;//端口——LPT打印端口, 1允许0禁用
    QString _other_TWAINRedirect_compress_value;//其它——TWAIN重定向——压缩级别, 0-3 高中低压缩级别
    QString _other_closeClipboard;//其它——剪切板, 1允许0禁用
    QString _printer_supportServerPrint;//打印机——服务器打印, 1允许0禁用
    QString _printer_supportVirtualPrint;//打印机——虚拟打印, 1允许0禁用
    QString _redirect_contentRedirect;//重定向——内容重定向, 1允许0禁用
    QString _redirect_applicationRedirect;//重定向——应用重定向, 1允许0禁用
    QString _redirect_inputMethodRedirect;//重定向——输入法重定向, 1允许0禁用
    QString _onceLogin_loginInfoStore;//单点登录——中央凭据存储, 中央凭据存储路径
    QString _onceLogin_rememberPassword;//单点登录——记住密码, 1允许0禁用
    QString _encryptLevel_encryptLevel;//加密等级——加密等级, 0-5 加密等级
    QString _userSpatialIsolation;//用户空间隔离——用户空间隔离, 空间路径
    QString _userPriorityLevel;//用户优先级——用户优先级, 0-3 高中低
    QString _appIcoColorDeep;//【服】应用图标色深, 1-3 16/24/32位色深
    QString _picCompress;//【服】图片压缩,, 1-4高/中/低/根据带宽设置图片压缩级别
    QString _appRedirect;//【服】应用重定向,, 是1否0
    QString _contentRedirect;//【服】内容重定向, 是1否0
    QString _DesktopHeight;
    QString _DesktopWidth;
    QString _FontSmooth; //字体平滑 1：允许
    QString _RdpPort;    //rdp端口号 
    QString _path;     //应用程序路径
    QString _param;    //程序参数  param=_param+" "+_extraParam;
    QString _applicationForbid_cmd; //1.限制，0.不限制
    QString _applicationForbid_explorer; //1.限制，0.不限制
    QString _applicationForbid_SysManage; //1.限制，0.不限制
    QString _applicationForbid_help; //1.限制，0.不限制

};

#endif // COMMON_H
