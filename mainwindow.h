#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QPixmap>
#include "channel.h"

namespace Ui {
    class MainWindow;
}

class Session;
class Server;
class Channel;
class QNetworkAccessManager;
class QNetworkReply;
class QTextDocument;
class AnimationViewModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum ChannelHighlightType
    {
        ChannelHighlightTypeNone,
        ChannelHighlightTypeNew,
        ChannelHighlightTypeMention
    };
    
private:
    Ui::MainWindow *ui;
    Session *session;
    QNetworkAccessManager *networkAccessManager;
    QTextDocument *document;
    QList<AnimationViewModel *> animations;
    QTimer *timer;

    void changeToServer(Server *newServer);
    void changeToChannel(Channel *newChannel);
    void scrollToBottom();

    void highlightServer(Server *server, ChannelHighlightType highlight);
    void highlightChannel(Channel *channel, ChannelHighlightType highlight, Channel::MessageType type);
    QBrush getColorForHighlightType(ChannelHighlightType ht);

private slots:
    void sendMessage();
    void treeItemClicked(const QModelIndex& index);
    void handleMessage(Server *server, Channel *channel, QString message, Channel::MessageType type);
    void imageDownloaded(QNetworkReply *networkReply);
    void anchorClicked(QUrl url);
    void refreshImages();

};

#endif // MAINWINDOW_H
