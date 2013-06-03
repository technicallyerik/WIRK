#ifndef USER_H
#define USER_H

#include <QStandardItem>

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

    QChar getMode();
    void setMode(QChar mode);

    QString getSortString();

    Channel* getChannel();
    QStandardItem* getMenuItem();

private:
    QString name;
    QChar mode;
    QStandardItem *menuItem;
};

#endif // USER_H
