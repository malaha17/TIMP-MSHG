#include "protocol.h"

#include <QJsonDocument>
#include <QJsonParseError>

namespace Protocol {

QByteArray serialize(const QJsonObject &object)
{
    QByteArray data = QJsonDocument(object).toJson(QJsonDocument::Compact);
    data.append('\n');
    return data;
}

QJsonObject deserialize(const QByteArray &data, QString *error)
{
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(data.trimmed(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (error) {
            *error = parseError.errorString().isEmpty()
                    ? QStringLiteral("Некорректный JSON")
                    : parseError.errorString();
        }
        return {};
    }

    return document.object();
}

QJsonObject makeResponse(const QString &status, const QString &message)
{
    QJsonObject object;
    object["status"] = status;
    object["message"] = message;
    return object;
}

} // namespace Protocol
