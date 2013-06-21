#include <QMutex>
#include "preferenceshelper.h"

const QString PreferencesHelper::displayPreferencesGroupKey = "display";
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

PreferencesHelper::PreferencesHelper() {

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

bool PreferencesHelper::getShouldUseColorUsernames()
{
    settings->beginGroup(PreferencesHelper::displayPreferencesGroupKey);
    bool colorUserNames = settings->value("colorusernames", true).toBool();
    settings->endGroup();
    return colorUserNames;
}
