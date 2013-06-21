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

private:
    PreferencesHelper();  // Hide constructor
    PreferencesHelper(const PreferencesHelper &); // Hide copy constructor
    PreferencesHelper& operator=(const PreferencesHelper &); // Hide assign operator

    static PreferencesHelper* preferencesInstance;

    QSettings *settings;
    QStringList defaultUserColors;
    
};

#endif // PREFERENCESHELPER_H
