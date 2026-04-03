import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 950
    height: 733
    color: "#1a1a1a"
    focus: true

    property int cellSize: 20
    property int cols: 20
    property int rows: 20
    property var snake: [{x: 10, y: 10}, {x: 9, y: 10}, {x: 8, y: 10}]
    property int dx: 1
    property int dy: 0
    property var food: ({x: 15, y: 10})
    property int score: 0
    property bool gameOver: false
    property bool started: false

    function reset() {
        snake = [{x: 10, y: 10}, {x: 9, y: 10}, {x: 8, y: 10}]
        dx = 1
        dy = 0
        score = 0
        gameOver = false
        started = true
        spawnFood()
        gameTimer.start()
    }

    function spawnFood() {
        var fx, fy, onSnake
        do {
            fx = Math.floor(Math.random() * cols)
            fy = Math.floor(Math.random() * rows)
            onSnake = false
            for (var i = 0; i < snake.length; i++) {
                if (snake[i].x === fx && snake[i].y === fy) {
                    onSnake = true
                    break
                }
            }
        } while (onSnake)
        food = {x: fx, y: fy}
    }

    function tick() {
        if (gameOver) return

        var head = {x: snake[0].x + dx, y: snake[0].y + dy}

        // Wrap around
        if (head.x < 0) head.x = cols - 1
        if (head.x >= cols) head.x = 0
        if (head.y < 0) head.y = rows - 1
        if (head.y >= rows) head.y = 0

        // Self collision
        for (var i = 0; i < snake.length; i++) {
            if (snake[i].x === head.x && snake[i].y === head.y) {
                gameOver = true
                gameTimer.stop()
                return
            }
        }

        var newSnake = [head].concat(snake)

        // Eat food
        if (head.x === food.x && head.y === food.y) {
            score++
            spawnFood()
        } else {
            newSnake.pop()
        }

        snake = newSnake
    }

    Keys.onPressed: function(event) {
        if (gameOver && event.key === Qt.Key_Space) {
            reset()
            event.accepted = true
            return
        }
        if (!started && event.key === Qt.Key_Space) {
            reset()
            event.accepted = true
            return
        }

        if (event.key === Qt.Key_Left && dx !== 1) { dx = -1; dy = 0 }
        else if (event.key === Qt.Key_Right && dx !== -1) { dx = 1; dy = 0 }
        else if (event.key === Qt.Key_Up && dy !== 1) { dx = 0; dy = -1 }
        else if (event.key === Qt.Key_Down && dy !== -1) { dx = 0; dy = 1 }
        event.accepted = true
    }

    Timer {
        id: gameTimer
        interval: 200
        running: false
        repeat: true
        onTriggered: root.tick()
    }

    // Game board
    Item {
        id: board
        anchors.centerIn: parent
        width: root.cols * root.cellSize
        height: root.rows * root.cellSize

        // Border
        Rectangle {
            anchors.fill: parent
            anchors.margins: -1
            color: "transparent"
            border.color: "#333333"
            border.width: 1
        }

        // Food
        Rectangle {
            x: root.food.x * root.cellSize + 2
            y: root.food.y * root.cellSize + 2
            width: root.cellSize - 4
            height: root.cellSize - 4
            radius: root.cellSize / 2
            color: "#f44336"
        }

        // Snake
        Repeater {
            model: root.snake.length
            Rectangle {
                x: root.snake[index].x * root.cellSize + 1
                y: root.snake[index].y * root.cellSize + 1
                width: root.cellSize - 2
                height: root.cellSize - 2
                radius: index === 0 ? 4 : 2
                color: index === 0 ? "#4caf50" : "#66bb6a"
            }
        }
    }

    // Score
    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        text: "Score: " + root.score
        color: "#ffffff"
        font.pixelSize: 24
        font.weight: Font.Bold
    }

    // Start / Game Over overlay
    Text {
        anchors.centerIn: parent
        visible: !root.started || root.gameOver
        text: root.gameOver ? "Game Over — Score: " + root.score + "\nPress SPACE to restart" : "Press SPACE to start"
        color: root.gameOver ? "#f44336" : "#a0a0a0"
        font.pixelSize: root.gameOver ? 28 : 24
        font.weight: Font.Medium
        horizontalAlignment: Text.AlignHCenter
    }
}
