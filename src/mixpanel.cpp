#include "mixpanel.h"
#include <QDateTime>
#include <serializer.h>

const QString Mixpanel::DEFAULT_ENDPOINT = "https://api.mixpanel.com/";
const QString Mixpanel::SET = "$set";
const QString Mixpanel::SET_ONCE = "$set_once";
const QString Mixpanel::ADD = "$add";
const QString Mixpanel::APPEND = "$append";
const QString Mixpanel::UNION = "$union";
// const QString Mixpanel::DEFAULT_OPERATION = SET;
const QString Mixpanel::UNSET = "$unset";
const QString Mixpanel::DELETE = "$delete";

const int MAX_BATCH_SIZE = 50;

Mixpanel::Mixpanel(const QString &token, int bufferSize) : endpoint(DEFAULT_ENDPOINT),
                                                           token(token),
                                                           distinct_id(QString()),
                                                           verbose(DEFAULT_VERBOSE),
                                                           maxBufferSize(bufferSize > MAX_BATCH_SIZE ? MAX_BATCH_SIZE : bufferSize)
{
    std::vector<QByteArray> event_buffer, engage_buffer, import_buffer;
    buffers["event"] = event_buffer;
    buffers["engage"] = engage_buffer;
    buffers["import"] = import_buffer;
}

bool Mixpanel::track(QString distinct_id, QString event, QVariantMap properties, bool buffered)
{
    QVariantMap parameters;
    parameters.insert("event", event);
    if (!properties.contains("time")) {
        properties.insert("time", QDateTime::currentDateTime().toTime_t());
    }

    properties.insert("token", token);
    if(distinct_id != QString())
        properties.insert("distinct_id", distinct_id);

    parameters.insert("properties", properties);
    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(parameters);

    if (buffered) {
        return sendBufferedRequest("track", json);
    } else {
        return sendRequest("track", json);
    }
}

bool Mixpanel::engage(QString distinct_id, QString operation, QVariantMap properties, bool buffered)
{
    QVariantMap parameters;
    parameters.insert("$token", token);
    if (!properties.contains("time")) {
        properties.insert("time", QDateTime::currentDateTime().toTime_t());
    }

    if(distinct_id == QString())
    {
        qCritical() << "distinct_id needed for engage";
        return false;
    }
    parameters.insert("$distinct_id", distinct_id);
    parameters.insert(operation, properties);

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(parameters);

    if (buffered) {
        return sendBufferedRequest("engage", json);
    } else {
        return sendRequest("engage", json);
    }
}

bool Mixpanel::sendBufferedRequest(QString path, const QByteArray &json) {
    qDebug() << json;
    if(QByteArray("") == json)
        return false;
    std::vector<QByteArray> &buffer = buffers[path];
    buffer.push_back(json);
    if ((int)buffer.size() >= maxBufferSize) {
        return flush(path);
    }
    return true;
}

bool Mixpanel::flush(QString path) {
    std::vector<QByteArray> &buffer = buffers[path];
    if (buffer.empty()) return true;
    QByteArray batch_json = "[";
    for (int i = 0; i < (int)buffer.size(); i++) {
        batch_json += buffer[i] + ",";
    }
    // replace the last "," with "]"
    batch_json.replace(batch_json.size() - 1, 1, "]");
    buffer.clear();
    return sendRequest(path, batch_json);
}

bool Mixpanel::flush_all() {
    return flush("event") && flush("engage") && flush("import");
}

bool Mixpanel::sendRequest(QString path, const QByteArray &json) {
    qDebug() << json;
    QByteArray data = json.toBase64();
    QString urlString = endpoint + path + "/?data=" + data;
    if(verbose)
        urlString += "&verbose=1";

    const QUrl url = QUrl(urlString);
    qDebug() << url;

    const QNetworkRequest networkRequest(url);
    QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager;
    QNetworkReply *networkReply = networkAccessManager->get(networkRequest);
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
    connect(networkReply, SIGNAL(finished()), this, SLOT(networkFinished()));
    return (networkReply->isFinished() || networkReply->isRunning());
}

void Mixpanel::networkError(QNetworkReply::NetworkError)
{
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(sender());
    qCritical() << networkReply->errorString();
    error();
}

void Mixpanel::networkFinished()
{
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "Network Answer" << networkReply->readAll();
    sended();
}

/*
QString Mixpanel::getDistinct_id() const
{
    return distinct_id;
}

void Mixpanel::setDistinct_id(const QString &value)
{
    distinct_id = value;
}
*/

QString Mixpanel::getToken() const
{
    return token;
}

void Mixpanel::setToken(const QString &value)
{
    token = value;
}

QString Mixpanel::getEndpoint() const
{
    return endpoint;
}

void Mixpanel::setEndpoint(const QString &value)
{
    endpoint = value;
}

bool Mixpanel::getVerbose() const
{
    return verbose;
}

void Mixpanel::setVerbose(bool value)
{
    verbose = value;
}
