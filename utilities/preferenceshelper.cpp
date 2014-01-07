#include <QMutex>
#include "preferenceshelper.h"

const QString PreferencesHelper::displayPreferencesGroupKey = "display";

const QString PreferencesHelper::suppressJoinNotificationsKey = "suppressjoinnotification";
const QString PreferencesHelper::joinOnConnectKey = "joinOnConnect";
const QString PreferencesHelper::colorUsernamesKey = "colorusernames";

PreferencesHelper* PreferencesHelper::preferencesInstance = 0;

PreferencesHelper* PreferencesHelper::sharedInstance()
{
    static QMutex mutex;
    if (!preferencesInstance)
    {
        mutex.lock();

        if (!preferencesInstance)
            preferencesInstance = new PreferencesHelper();

        mutex.unlock();
    }

    return preferencesInstance;
}

void PreferencesHelper::dropSharedInstance()
{
    static QMutex mutex;
    mutex.lock();
    delete preferencesInstance;
    preferencesInstance = 0;
    mutex.unlock();
}

PreferencesHelper::PreferencesHelper()
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "com.flashforwardlabs", "wirk", this);

    defaultUserColors << "#F26C4F" << "#F68E55" << "#FBAF5C" << "#FFF467" << "#ACD372" << "#7CC576" << "#3BB878" << "#1ABBB4" << "#00BFF3" << "#438CCA" << "#5574B9" << "#605CA8" << "#855FA8" << "#A763A8" << "#F06EA9" << "#F26D7D";
}

QSettings* PreferencesHelper::getSettings()
{
    return settings;
}

QStringList PreferencesHelper::getUserColors()
{
    return defaultUserColors;
}

bool PreferencesHelper::getShouldUseColorUsernames(QString channel, QString server)
{
    int channelSetting = getPropertyForChannel(colorUsernamesKey, channel, server);
    switch (channelSetting)
    {
        case Yes_Option:
            return true;
            break;
        case No_Option:
            return false;
            break;
        case Default_Option:
        default:
            return getPropertyDefault(colorUsernamesKey, true);
            break;
    }
}

bool PreferencesHelper::getShouldJoinOnConnect(QString channel, QString server)
{
    int joinOnConnectSetting = getPropertyForChannel(joinOnConnectKey, channel, server);
    switch (joinOnConnectSetting)
    {
        case Yes_Option:
            return true;
            break;
        case No_Option:
            return false;
            break;
        case Default_Option:
        default:
            return getPropertyDefault(joinOnConnectKey, true);
            break;
    }
}

bool PreferencesHelper::getShouldHideJoinNotifications(QString channel, QString server)
{
    int hideJoinNotificationsSetting = getPropertyForChannel(suppressJoinNotificationsKey, channel, server);
    switch (hideJoinNotificationsSetting)
    {
        case Yes_Option:
            return true;
            break;
        case No_Option:
            return false;
            break;
        case Default_Option:
        default:
            return getPropertyDefault(suppressJoinNotificationsKey, false);
            break;
    }
}

/*!
    Should only be used externally in preferences editing window.
    When trying to get preference, use appropriate specific helper.
 */
bool PreferencesHelper::getPropertyDefault(QString propertyKey, bool defaultValue)
{
    settings->beginGroup(PreferencesHelper::displayPreferencesGroupKey);
    bool propertyValue = settings->value(propertyKey, defaultValue).toBool();
    settings->endGroup();
    return propertyValue;
}

/*!
    Should only be used externally in preferences editing window.
    When trying to get preference, use appropriate specific helper.
 */
int PreferencesHelper::getPropertyForChannel(QString propertyKey, QString channel, QString server)
{
    if(server.isNull() || server.isEmpty() || channel.isNull() || channel.isEmpty()) {
        return (int)Default_Option;
    }

    settings->beginGroup(getChannelGroupKey(channel, server));
    int propertyValue = settings->value(propertyKey, (int)Default_Option).toInt();
    settings->endGroup();
    return propertyValue;
}

QString PreferencesHelper::getChannelGroupKey(QString channel, QString server)
{
    return QString("%1/%2").arg(server, channel);
}
