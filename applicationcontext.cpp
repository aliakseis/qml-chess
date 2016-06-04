#include "applicationcontext.h"

#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {


enum { DIMENSION = 8 };


inline int sign(int x) { return (x > 0)? 1 : ((x < 0)? -1 : 0); }


QString ShowSaveFileDialog(QWidget *parent,
                           const QString &title,
                           const QString &directory,
                           const QString &filter) {
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  return QFileDialog::getSaveFileName(parent,
                                      title,
                                      directory,
                                      filter);
#else
  QFileDialog dialog(parent, title, directory, filter);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  if (parent) {
    dialog.setWindowModality(Qt::WindowModal);
  }
  QRegExp filter_regex(QLatin1String("(?:^\\*\\.(?!.*\\()|\\(\\*\\.)(\\w+)"));
  QStringList filters = filter.split(QLatin1String(";;"));
  if (!filters.isEmpty()) {
    dialog.setNameFilter(filters.first());
    if (filter_regex.indexIn(filters.first()) != -1) {
      dialog.setDefaultSuffix(filter_regex.cap(1));
    }
  }
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (dialog.exec() == QDialog::Accepted) {
    QString file_name = dialog.selectedFiles().first();
    QFileInfo info(file_name);
    if (info.suffix().isEmpty() && !dialog.selectedNameFilter().isEmpty()) {
      if (filter_regex.indexIn(dialog.selectedNameFilter()) != -1) {
        QString extension = filter_regex.cap(1);
        file_name += QLatin1String(".") + extension;
      }
    }
    return file_name;
  } else {
    return QString();
  }
#endif  // Q_WS_MAC || Q_WS_WIN
}

class Validator
{
public:
    Validator(const QVariantList& pieces)
        : oldSquare(0)
        , newSquare(0)
    {
        memset(squares, 0, sizeof(squares));
        for (const auto& variant : pieces)
        {
            auto object = variant.value<QObject*>();
            const int col = object->property("col").toInt();
            const int row = object->property("row").toInt();
            const bool white = object->property("white").toBool();
            const int piece = object->property("piece").toInt();

            squares[row][col] = white? piece : -piece;
        }
    }

    bool validate(
            int oldRow,
            int oldCol,
            int newRow,
            int newCol)
    {
        oldSquare = squares[oldRow][oldCol];
        newSquare = squares[newRow][newCol];

        if (!oldSquare)
            return false;

        const bool white = oldSquare > 0;

        if (white? (newSquare > 0) : (newSquare < 0))
            return false;

        const int deltaRow = newRow - oldRow;
        const int deltaCol = newCol - oldCol;

        const Pieces piece = (Pieces) abs(squares[oldRow][oldCol]);
        switch (piece)
        {
        case KING:
            if (abs(deltaRow) > 1 || abs(deltaCol) > 1)
                return false;
            break;

        case QUEEN:
        case ROOK:
        case BISHOP:
            {
                if (!((piece != ROOK && abs(deltaRow) == abs(deltaCol)) ||
                      (piece != BISHOP && (!deltaRow || !deltaCol))))
                    return false;

                const int signRow = sign(deltaRow);
                const int signCol = sign(deltaCol);

                for (int row(oldRow + signRow), col(oldCol + signCol)
                     ; row != newRow && col != newCol
                     ; row += signRow, col += signCol)
                {
                    if (squares[row][col])
                        return false;
                }
            }
            break;

        case KNIGHT:
            if (abs(deltaRow) * abs(deltaCol) != 2)
                return false;
            break;

        case PAWN:
            {
                if (abs(deltaCol) > 1)
                    return false;

                const int dir = white? -1 : 1;
                if (!(deltaRow == dir ||
                      (deltaRow == 2 * dir && oldRow == (white? 6 : 1) &&
                        !deltaCol &&
                        !squares[oldRow + dir][oldCol])))
                    return false;

                if ((newSquare != 0) != (deltaCol != 0))
                    return false;
            }
            break;

        default:
            return false; // TODO diagnostics
        }

        return true;
    }

    Pieces movingPiece() const { return (Pieces) abs(oldSquare); }
    Pieces capturedPiece() const { return (Pieces) abs(newSquare); }

private:
    int squares[DIMENSION][DIMENSION];
    int oldSquare;
    int newSquare;
};


} // namespace


void ApplicationContext::Move::read(const QJsonObject &json)
{
    oldRow = json["oldRow"].toInt();
    oldCol = json["oldCol"].toInt();
    newRow = json["newRow"].toInt();
    newCol = json["newCol"].toInt();
    movingPiece = (Pieces) json["movingPiece"].toInt();
    capturedPiece = (Pieces) json["capturedPiece"].toInt();
}

QJsonObject ApplicationContext::Move::toJsonObject() const
{
    QJsonObject result;
    result["oldRow"] = oldRow;
    result["oldCol"] = oldCol;
    result["newRow"] = newRow;
    result["newCol"] = newCol;
    result["movingPiece"] = movingPiece;
    result["capturedPiece"] = capturedPiece;

    return result;
}

QVariantMap ApplicationContext::Move::toVariantMap() const
{
    QVariantMap result;
    result["oldRow"] = oldRow;
    result["oldCol"] = oldCol;
    result["newRow"] = newRow;
    result["newCol"] = newCol;
    result["movingPiece"] = movingPiece;
    result["capturedPiece"] = capturedPiece;

    return result;
}

ApplicationContext::ApplicationContext(QObject *parent) :
    QObject(parent), moveIndex(0)
{
}

bool ApplicationContext::handleMove(
            int oldRow,
            int oldCol,
            int newRow,
            int newCol,
            QVariantList pieces)
{
    Validator validator(pieces);
    bool result = validator.validate(
                oldRow,
                oldCol,
                newRow,
                newCol);

    if (result)
    {
        moves.push_back(Move{
               oldRow,
               oldCol,
               newRow,
               newCol,
               validator.movingPiece(),
               validator.capturedPiece()
        });
    }

    return result;
}

bool ApplicationContext::loadGame()
{
    auto fileName = QFileDialog::getOpenFileName(
                nullptr,
                tr("Open History File"),
                QDir::homePath(),
                tr("Chess Files (*.chess)"));

    QFile loadFile(fileName);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    auto json = loadDoc.object();

    QJsonArray npcArray = json["moves"].toArray();

    moves.clear();
    for (const auto& el : npcArray)
    {
        Move move;
        move.read(el.toObject());
        moves.push_back(move);
    }

    moveIndex = 0;
    return true;
}

void ApplicationContext::startGame()
{
    moveIndex = 0;
    moves.clear();
}

void ApplicationContext::saveGame()
{
    auto fileName = ShowSaveFileDialog(
                nullptr,
                tr("Save History File"),
                QDir::homePath(),
                tr("Chess Files (*.chess)")
                );

    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonObject json;

    QJsonArray npcArray;
    for (const auto& move : moves) {
        QJsonObject npcObject(move.toJsonObject());
        npcArray.append(npcObject);
    }
    json["moves"] = npcArray;

    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
}

bool ApplicationContext::hasPrevMove() const
{
    return moveIndex > 0;
}

QVariantMap ApplicationContext::prevMove()
{
    if (moveIndex == 0)
        return QVariantMap();

    return moves[--moveIndex].toVariantMap();
}

bool ApplicationContext::hasNextMove() const
{
    return moveIndex < moves.size();
}

QVariantMap ApplicationContext::nextMove()
{
    if (moveIndex >= moves.size())
        return QVariantMap();

    return moves[moveIndex++].toVariantMap();
}
