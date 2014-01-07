#ifndef PREFERENCESHELPER_H
#define PREFERENCESHELPER_H

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QStringList>

class PreferencesHelper : public QObject
{
    Q_OBJECT

public:
    static const QString displayPreferencesGroupKey;
    static const QString suppressJoinNotificationsKey;
    static const QString joinOnConnectKey;
    static const QString colorUsernamesKey;

    static PreferencesHelper* sharedInstance();
    static void dropSharedInstance();

    QSettings* getSettings();
    QStringList getUserColors();
    bool getShouldUseColorUsernames(QString channel, QString server);
    bool getShouldJoinOnConnect(QString channel, QString server);
    bool getShouldHideJoinNotifications(QString channel, QString server);

    QString getChannelGroupKey(QString channel, QString server);

    enum DropdownOptions { Default_Option = -1, No_Option = 0, Yes_Option = 1 };

    bool getPropertyDefault(QString propertyKey, bool defaultValue);
    int getPropertyForChannel(QString propertyKey, QString channel, QString server);

private:
    PreferencesHelper();  // Hide constructor
    PreferencesHelper(const PreferencesHelper &); // Hide copy constructor
    PreferencesHelper& operator=(const PreferencesHelper &); // Hide assign operator

    static PreferencesHelper* preferencesInstance;

    QSettings *settings;
    QStringList defaultUserColors;
};

#endif // PREFERENCESHELPER_H
