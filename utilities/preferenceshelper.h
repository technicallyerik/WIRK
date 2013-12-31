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

    static PreferencesHelper* sharedInstance();
    static void dropSharedInstance();

    QSettings* getSettings();
    QStringList getUserColors();
    bool getShouldUseColorUsernames();
    bool getShouldJoinOnConnect();
    bool getShouldHideJoinNotifications();

private:
    PreferencesHelper();  // Hide constructor
    PreferencesHelper(const PreferencesHelper &); // Hide copy constructor
    PreferencesHelper& operator=(const PreferencesHelper &); // Hide assign operator

    static PreferencesHelper* preferencesInstance;

    QSettings *settings;
    QStringList defaultUserColors;
    
    bool getBooleanProperty(QString propertyKey, bool defaultValue);
};

#endif // PREFERENCESHELPER_H
