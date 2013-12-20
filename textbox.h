#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <QPlainTextEdit>
#include "channel.h"

class MessageHistory;

class TextBox : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextBox(QWidget *parent = 0);
    virtual ~TextBox();
    void setChannel(Channel &chan);

protected:
    void keyPressEvent(QKeyEvent *event);
    bool event(QEvent *e);
    
signals:
    void returnPressed();
    
public slots:

private:
    MessageHistory *messageHistory;
    QStringList searchingUsernames;
    int userSearchIndex;
    Channel *channel;
    QString searchString;
    QString lastWord;

    void getLastArgument();
    void moveCursorToEnd();
    
};

#endif // TEXTBOX_H
