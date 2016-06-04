#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H

#include <QObject>
#include <QVariantList>

#include <vector>

enum Pieces {
    NONE = 0,
    KING = 1,
    QUEEN = 2,
    ROOK = 3,
    BISHOP = 4,
    KNIGHT = 5,
    PAWN = 6,
};

class ApplicationContext : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationContext(QObject *parent = 0);

    Q_INVOKABLE bool handleMove(
            int oldRow,
            int oldCol,
            int newRow,
            int newCol,
            QVariantList pieces);

    Q_INVOKABLE bool loadGame();
    Q_INVOKABLE void startGame();
    Q_INVOKABLE void saveGame();

    Q_PROPERTY(bool hasPrevMove READ hasPrevMove)
    bool hasPrevMove() const;
    Q_INVOKABLE QVariantMap prevMove();
    Q_PROPERTY(bool hasNextMove READ hasNextMove)
    bool hasNextMove() const;
    Q_INVOKABLE QVariantMap nextMove();

signals:

public slots:

private:
    struct Move
    {
        int oldRow;
        int oldCol;
        int newRow;
        int newCol;
        Pieces movingPiece;
        Pieces capturedPiece;

        void read(const QJsonObject &json);
        QJsonObject toJsonObject() const;
        QVariantMap toVariantMap() const;
    };

    std::vector<Move> moves;
    size_t moveIndex;
};

#endif // APPLICATIONCONTEXT_H
