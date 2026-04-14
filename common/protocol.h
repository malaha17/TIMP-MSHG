#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QByteArray>
#include <QJsonObject>
#include <QString>

namespace Protocol {

QByteArray serialize(const QJsonObject &object);
QJsonObject deserialize(const QByteArray &data, QString *error = nullptr);
QJsonObject makeResponse(const QString &status, const QString &message = QString());

} // namespace Protocol

#endif // PROTOCOL_H
