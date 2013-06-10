#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <QLineEdit>
#include "channel.h"

class MessageHistory;

class TextBox : public QLineEdit
{
    Q_OBJECT
public:
    explicit TextBox(QWidget *parent = 0);
    virtual ~TextBox();
    void setUsernames(QStringList users);
    void setChannel(Channel &chan);

protected:
    void keyPressEvent(QKeyEvent *event);
    bool event(QEvent *e);
    
signals:
    
public slots:

private:
    MessageHistory *messageHistory;
    QStringList usernames, searchingUsernames;
    int userSearchIndex;
    Channel *channel;
    QString searchString;
    QString lastWord;

    void getLastArgument();

    
};

#endif // TEXTBOX_H
