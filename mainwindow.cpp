#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "irc.h"
#include "ircsession.h"
#include "ircsender.h"
#include <QSslSocket>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    IrcSession* session = new IrcSession(this);
    connect(session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(processMessage(IrcMessage*)));
    session->setHost("chat.freenode.net");
    session->setPort(7070);

    session->setUserName("testing123456");
    session->setNickName("testing123456");
    session->setRealName("Testing 123456");

    QSslSocket* sslSocket = new QSslSocket(this);
    sslSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
    sslSocket->ignoreSslErrors();
    session->setSocket(sslSocket);

    session->open();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processMessage(IrcMessage * message)
{
    switch (message->type()) {
        case IrcMessage::Invite: {
            IrcInviteMessage *invite = static_cast<IrcInviteMessage*>(message);
            break;
        }
        case IrcMessage::Join: {
            IrcJoinMessage *join = static_cast<IrcJoinMessage*>(message);
            break;
        }
        case IrcMessage::Kick: {
            IrcKickMessage *kick = static_cast<IrcKickMessage*>(message);
            break;
        }
        case IrcMessage::Mode: {
            IrcModeMessage *mode = static_cast<IrcModeMessage*>(message);
            break;
        }
        case IrcMessage::Nick: {
            IrcNickMessage *nick = static_cast<IrcNickMessage*>(message);
            break;
        }
        case IrcMessage::Notice: {
            IrcNoticeMessage *notice = static_cast<IrcNoticeMessage*>(message);
            QString formattedNotice = QString("[%1] %2").arg(message->sender().name(), notice->message());
            ui->mainText->append(formattedNotice);
            break;
        }
        case IrcMessage::Numeric: {
            IrcNumericMessage *numeric = static_cast<IrcNumericMessage*>(message);
            this->handleNumericMessage(numeric);
            break;
        }
        case IrcMessage::Part: {
            IrcPartMessage *part = static_cast<IrcPartMessage*>(message);
            break;
        }
        case IrcMessage::Pong: {
            IrcPongMessage *pong = static_cast<IrcPongMessage*>(message);
            break;
        }
        case IrcMessage::Private: {
            IrcPrivateMessage *pm = static_cast<IrcPrivateMessage*>(message);
            break;
        }
        case IrcMessage::Quit: {
            IrcQuitMessage *quit = static_cast<IrcQuitMessage*>(message);
            break;
        }
        case IrcMessage::Topic: {
            IrcTopicMessage *topic = static_cast<IrcTopicMessage*>(message);
            break;
        }
        case IrcMessage::Unknown: {

            break;
        }
        default: {
            break;
        }
    }
}

void MainWindow::handleNumericMessage(IrcNumericMessage* message)
{
    QString formattedNumeric = QString("[%1] %2").arg(message->sender().name(), QString::number(message->code()));
    ui->mainText->append(formattedNumeric);

    switch (message->code()) {
        case Irc::RPL_ENDOFWHO: {
            break;
        }
        case Irc::RPL_WHOREPLY: {
            break;
        }
        case Irc::RPL_UNAWAY: {
            break;
        }
        case Irc::RPL_NOWAWAY: {
            break;
        }
        case Irc::RPL_AWAY: {
            break;
        }
        case Irc::RPL_WHOISOPERATOR: {
            break;
        }
        case Irc::RPL_WHOISMODES: {
            break;
        }
        case Irc::RPL_WHOISREGNICK: {
            break;
        }
        case Irc::RPL_WHOISHELPOP: {
            break;
        }
        case Irc::RPL_WHOISSPECIAL: {
            break;
        }
        case Irc::RPL_WHOISHOST: {
            break;
        }
        case Irc::RPL_WHOISSECURE: {
            break;
        }
        case Irc::RPL_WHOISUSER: {
            break;
        }
        case Irc::RPL_WHOISSERVER: {
            break;
        }
        case Irc::RPL_WHOISACCOUNT: {
            break;
        }
        case Irc::RPL_WHOWASUSER: {
            break;
        }
        case Irc::RPL_WHOISIDLE: {
            break;
        }
        case Irc::RPL_WHOISCHANNELS: {
            break;
        }
        case Irc::RPL_ENDOFWHOIS: {
            break;
        }
        case Irc::RPL_INVITING: {
            break;
        }
        case Irc::RPL_VERSION: {
            break;
        }
        case Irc::RPL_TIME: {
            break;
        }
        case Irc::RPL_ENDOFBANLIST: {
            break;
        }
        case Irc::RPL_ENDOFEXCEPTLIST: {
            break;
        }
        case Irc::RPL_ENDOFINFO: {
            break;
        }
        case Irc::RPL_ENDOFINVITELIST: {
            break;
        }
        case Irc::RPL_ENDOFLINKS: {
            break;
        }
        case Irc::RPL_ENDOFSTATS: {
            break;
        }
        case Irc::RPL_ENDOFUSERS: {
            break;
        }
        case Irc::RPL_ENDOFWHOWAS: {
            break;
        }
        case Irc::RPL_CHANNELMODEIS: {
            break;
        }
        case Irc::RPL_CHANNEL_URL: {
            break;
        }
        case Irc::RPL_CREATIONTIME: {
            break;
        }
        case Irc::RPL_NOTOPIC: {
            break;
        }
        case Irc::RPL_TOPIC: {
            break;
        }
        case Irc::RPL_TOPICWHOTIME: {
            break;
        }
        case Irc::RPL_NAMREPLY: {
            break;
        }
        case Irc::RPL_ENDOFNAMES: {
            break;
        }
        default:{
            break;
        }
    }
}
