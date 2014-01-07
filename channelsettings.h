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

private slots:
    void accept();

private:
    void populateDropdown(QComboBox *dropdown, int selectedDropdownValue);

    Ui::ChannelSettings *ui;
    QString channelName;
    QString serverName;
};

#endif // CHANNELSETTINGS_H
