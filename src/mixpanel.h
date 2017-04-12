#ifndef MIXPANEL_H
#define MIXPANEL_H

#include "mixpanel-qt_global.h"
#include <QtCore>
#include <QtNetwork>
#include <vector>
#include <map>
#include <QTimer>

class MIXPANELQTSHARED_EXPORT Mixpanel: public QObject
{
    Q_OBJECT
public:
    const static QString SET;
    const static QString SET_ONCE;
    const static QString ADD;
    const static QString APPEND;
    const static QString UNION;
    const static QString UNSET;
    const static QString DELETE;

    const static QString DEFAULT_ENDPOINT;
    const static bool DEFAULT_VERBOSE = false;
    // const static QString DEFAULT_OPERATION;

private:
    QString endpoint;
    QString token;
    QString distinct_id;
    bool verbose;

    std::map<QString, std::vector<QByteArray> > buffers;

    QMutex mutex;
    /*
    std::vector<QByteArray> event_buffer;
    std::vector<QByteArray> engage_buffer;
    std::vector<QByteArray> import_buffer;
    */
    int maxBufferSize;

    QTimer *flushTimer;

public:
    // Mixpanel(const QString &token);
    Mixpanel(const QString &token, int bufferSize = 0, int interval = 60000);

    QString getEndpoint() const;
    void setEndpoint(const QString &value);
    QString getToken() const;
    void setToken(const QString &value);
    // QString getDistinct_id() const;
    // void setDistinct_id(const QString &value);
    bool getVerbose() const;
    void setVerbose(bool value);

    bool track(QString distinct_id, QString event, QVariantMap properties, bool buffered = true);
    bool engage(QString distinct_id, QString operation, QVariantMap properties, bool buffered = true);

public slots:
    bool flush_all();

signals:
    void sended();
    void error();

private:
    // bool sendRequest(QString path, const QVariantMap & parameters);
    bool sendRequest(QString path, const QByteArray &json);
    bool sendBufferedRequest(QString path, const QByteArray &json);
    bool flush(QString path);

private slots:
    void networkError(QNetworkReply::NetworkError);
    void networkFinished();

};


#endif // MIXPANEL_H
