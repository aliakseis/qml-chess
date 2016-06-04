.pragma library


var maxC = 8;
var maxR = 8;

var applicationContext = null;

var gameCanvas = null;

var gameSquares = new Array(maxR * maxC);
var gamePieces = new Array;

var squareComponent = Qt.createComponent("GameSquare.qml");
var pieceComponent = Qt.createComponent("GamePiece.qml");

var whitesMove = true;

var KING = 1;
var QUEEN = 2;
var ROOK = 3;
var BISHOP = 4;
var KNIGHT = 5;
var PAWN = 6;

var PIECES = [ ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK, ];

function idx2D(r, c) {
    return r * maxC + c;
}

function clearTable() {
    for(var i = 0; i < gamePieces.length; i++) {
        gamePieces[i].destroy();
    }

    gamePieces = new Array;
}

function initTable() {
    clearTable();

    gamePieces = new Array(24);

    for (var white = 0; white < 2; white++)
        for (var i = 0; i < 8; i++) {
            gamePieces[white * 16 + i] = pieceComponent.createObject(gameCanvas,
                {"row": white * 7, "col": i, "white":white, "piece":PIECES[i], "canvas":gameCanvas});
            gamePieces[white * 16 + 8 + i] = pieceComponent.createObject(gameCanvas,
                {"row": white * 5 + 1, "col": i, "white":white, "piece":PAWN, "canvas":gameCanvas});
        }

    whitesMove = true;
}


//////////////////////////////////////////////////////////////////////////////

function initContext(context) {
    applicationContext = context;
}

function initCanvas(canvasItem) {
    gameCanvas = canvasItem;
    for(var i = 0; i<maxR; i++)
        for(var j = 0; j<maxC; j++)
            gameSquares[idx2D(i,j)] = squareComponent.createObject(canvasItem,
                    {"row":i, "col":j, "canvas": canvasItem});
    console.log("Canvas Initialization performed.");
}


function start() {
    applicationContext.startGame();
    initTable();

    gameCanvas.historyState = false;
    gameCanvas.playingState = true;
}

function load() {
    if (applicationContext.loadGame()) {
        initTable();

        gameCanvas.historyState = true;
        gameCanvas.playingState = false;

        gameCanvas.hasPrevMove = applicationContext.hasPrevMove;
        gameCanvas.hasNextMove = applicationContext.hasNextMove;
    }
}

function stop() {
    clearTable();

    gameCanvas.historyState = false;
    gameCanvas.playingState = false;
}

function save() {
    applicationContext.saveGame();
}

function prev() {
    var move = applicationContext.prevMove();

    if (!move.movingPiece)
        return;

    for (var i = 0; i < gamePieces.length; i++) {
        if (gamePieces[i].row === move.newRow && gamePieces[i].col === move.newCol) {
            var item = gamePieces[i];
            item.col = move.oldCol;
            item.row = move.oldRow;
            item.x = move.oldCol * gameCanvas.tileSize;
            item.y = move.oldRow * gameCanvas.tileSize;
            item.piece = move.movingPiece;

            break;
        }
    }

    if (move.capturedPiece) {
        gamePieces.push(pieceComponent.createObject(gameCanvas,
             {"row": move.newRow, "col": move.newCol, "white":whitesMove, "piece":move.capturedPiece, "canvas":gameCanvas}));
    }

    whitesMove = !whitesMove;

    gameCanvas.hasPrevMove = applicationContext.hasPrevMove;
    gameCanvas.hasNextMove = applicationContext.hasNextMove;
}

function next() {
    var move = applicationContext.nextMove();

    if (!move.movingPiece)
        return;

    var i;
    for (i = 0; i < gamePieces.length; i++) {
        if (gamePieces[i].row === move.newRow && gamePieces[i].col === move.newCol) {
            gamePieces[i].destroy();
            gamePieces.splice(i, 1);
            break;
        }
    }

    for (i = 0; i < gamePieces.length; i++) {
        if (gamePieces[i].row === move.oldRow && gamePieces[i].col === move.oldCol) {
            var item = gamePieces[i];
            item.col = move.newCol;
            item.row = move.newRow;
            item.x = move.newCol * gameCanvas.tileSize;
            item.y = move.newRow * gameCanvas.tileSize;

            if (item.piece === PAWN && item.row === (item.white? 0 : 7))
                item.piece = QUEEN; // TODO choice

            break;
        }
    }

    whitesMove = !whitesMove;

    gameCanvas.hasPrevMove = applicationContext.hasPrevMove;
    gameCanvas.hasNextMove = applicationContext.hasNextMove;
}


function startMove(item)
{
}

function finishMove(item)
{
    var slipFactor = gameCanvas.tileSize / 2;

    var newRow = Math.floor((item.y + slipFactor) / gameCanvas.tileSize);
    var newCol = Math.floor((item.x + slipFactor) / gameCanvas.tileSize);

    if (whitesMove === item.white &&
            (newRow !== item.row || newCol !== item.col) &&
            newRow >= 0 && newRow < maxR && newCol >= 0 && newCol < maxC &&
            applicationContext.handleMove(item.row, item.col, newRow, newCol, gamePieces)) {
        for(var i = 0; i < gamePieces.length; i++) {
            if (gamePieces[i].row === newRow && gamePieces[i].col === newCol) {
                gamePieces[i].destroy();
                gamePieces.splice(i, 1);
                break;
            }
        }

        whitesMove = !whitesMove;

        item.col = newCol;
        item.row = newRow;
        item.x = newCol * gameCanvas.tileSize;
        item.y = newRow * gameCanvas.tileSize;

        if (item.piece === PAWN && item.row === (item.white? 0 : 7))
            item.piece = QUEEN; // TODO choice

        return; // succeeded
    }

    //Not valid
    item.x = item.col * gameCanvas.tileSize; //Drag overrode binding
    item.y = item.row * gameCanvas.tileSize; //Drag overrode binding
}
