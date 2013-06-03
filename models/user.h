#ifndef USER_H
#define USER_H

#include <QStandardItem>
#include <QSet>

class Channel;

class User : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)
public:
    static const int UserDataName = 64;
    static const int UserDataSort = 128;

    static const char USER_MODE_OWNER = '~';
    static const char USER_MODE_ADMIN = '&';
    static const char USER_MODE_OPERATOR = '@';
    static const char USER_MODE_HALF_OP = '%';
    static const char USER_MODE_VOICED = '+';

    explicit User(QString inName, QChar inMode, QStandardItem *inMenuItem, Channel *parent = 0);
    virtual ~User();

    QString getName();
    void setName(QString name);

    QSet<QChar> getModes();
    void addMode(QChar mode);
    void removeMode(QChar mode);
    void refreshUserDisplay();
    QChar getModeDisplayString();
    char convertFlagToDisplayMode(QChar flag);

    QString getSortString();

    Channel* getChannel();
    QStandardItem* getMenuItem();

private:
    QString name;
    QSet<QChar> modes;
    QStandardItem *menuItem;
};

#endif // USER_H
