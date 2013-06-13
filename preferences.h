#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT
    
public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

private slots:
    void accept();
    
private:
    Ui::Preferences *ui;
    static const QString settingsFileName;
    static const QString userPrefsName;
    QSettings *settings;
};

#endif // PREFERENCES_H
