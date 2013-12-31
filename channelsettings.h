#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QComboBox>

namespace Ui {
class ChannelSettings;
}

class ChannelSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelSettings(QString channelName, QString serverName, QWidget *parent = 0);
    ~ChannelSettings();
    bool shouldHideJoinNotifications();
    bool shouldJoinOnConnect();
    int shouldColorUserNames();

    enum DropdownOptions { Default_Option = -1, No_Option = 0, Yes_Option = 1 };

private slots:
    void accept();
    QString getGroupKey();

private:
    static const QString suppressJoinNotificationsKey;
    static const QString joinOnConnectKey;
    static const QString colorUsernamesKey;

    Ui::ChannelSettings *ui;
    QString channelName;
    QString serverName;
    QSettings *settings;

    void populateDropdown(QComboBox *dropdown, QString propertyValueKey);
    int getPropertyValue(QString propertyKey);
};

#endif // CHANNELSETTINGS_H
