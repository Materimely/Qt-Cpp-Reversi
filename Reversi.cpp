#include "Reversi.h"
#include "ui_Reversi.h"

#include <QDebug>
#include <QMessageBox>

#include "Cell.h"
#include "Player.h"

Reversi::Reversi(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Reversi),
    m_player(0) {

    ui->setupUi(this);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Cell* cell = this->findChild<Cell*>(QString("cell%1%2").arg(r).arg(c));
            Q_ASSERT(cell != 0);

            cell->setRow(r);
            cell->setCol(c);
            QObject::connect(cell, SIGNAL(released()), this, SLOT(play()));

            m_board[r][c] = cell;
        }
    }

    Player* white = Player::player(Player::White);
    Player* black = Player::player(Player::Black);

    QObject::connect(white, SIGNAL(countChanged(int)), this, SLOT(updateStatus()));
    QObject::connect(black, SIGNAL(countChanged(int)), this, SLOT(updateStatus()));

    QObject::connect(ui->actionNovo, SIGNAL(triggered()), this, SLOT(reset()));
    QObject::connect(ui->actionSair, SIGNAL(triggered()), qApp, SLOT(quit()));
    QObject::connect(ui->actionSobre, SIGNAL(triggered()), this, SLOT(showAbout()));

    this->reset();

    this->adjustSize();
    this->setFixedSize(this->size());
}

Reversi::~Reversi() {
    delete ui;
}

void Reversi::play() {
    Cell* cell = qobject_cast<Cell*>(QObject::sender());
    Q_ASSERT(cell != 0);

    if (cell->hasOwner())
        return;

    bool valid = false;
    QList<Reversi::Coordinates> coords;
    coords << Coordinates::North
           << Coordinates::NorthEast
           << Coordinates::East
           << Coordinates::SouthEast
           << Coordinates::South
           << Coordinates::SouthWest
           << Coordinates::West
           << Coordinates::NorthWest;
    foreach (Reversi::Coordinates c, coords) {
        if (this->canPlayOnCoords(cell, c)) {
            this->flipOnCoords(cell, c);
            valid = true;
        }
    }

    if (valid) {
        cell->setOwner(m_player);
        m_player = m_player->other();

        if (this->isGameOver())
            showGameOver();
    }
}

bool Reversi::canPlay(Cell* cell) {
    if (cell->hasOwner())
        return false;

    QList<Reversi::Coordinates> coords;
    coords << Coordinates::North
           << Coordinates::NorthEast
           << Coordinates::East
           << Coordinates::SouthEast
           << Coordinates::South
           << Coordinates::SouthWest
           << Coordinates::West
           << Coordinates::NorthWest;
    foreach (Reversi::Coordinates c, coords) {
        if (this->canPlayOnCoords(cell, c))
            return true;
    }

    return false;
}

bool Reversi::canPlayOnCoords(Cell* cell, Reversi::Coordinates coords) {
    Q_ASSERT(cell != 0);

    int r = cell->row();
    int c = cell->col();
    int count = 0;

    do {
        switch (coords) {
            case Reversi::North:
                r--;
                break;
            case Reversi::NorthEast:
                r--;
                c++;
                break;
            case Reversi::East:
                c++;
                break;
            case Reversi::SouthEast:
                r++;
                c++;
                break;
            case Reversi::South:
                r++;
                break;
            case Reversi::SouthWest:
                r++;
                c--;
                break;
            case Reversi::West:
                c--;
                break;
            case Reversi::NorthWest:
                r--;
                c--;
                break;
            default:
                Q_UNREACHABLE();
                break;
        }

        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            if (m_board[r][c]->owner() != m_player->other())
                break;

            count++;
        }
    } while (r >= 0 && r < 8 && c >= 0 && c < 8);

    return (r >= 0 && r < 8 && c >= 0 && c < 8 && count > 0 && m_board[r][c]->owner() == m_player);
}

bool Reversi::isGameOver() {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (this->canPlay(m_board[r][c]))
                return false;
        }
    }

    return true;
}

void Reversi::reset() {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            m_board[r][c]->reset();
        }
    }

    Player* white = Player::player(Player::White);
    m_board[3][3]->setOwner(white);
    m_board[4][4]->setOwner(white);

    Player* black = Player::player(Player::Black);
    m_board[3][4]->setOwner(black);
    m_board[4][3]->setOwner(black);

    m_player = white;
}

void Reversi::updateStatus() {
    ui->statusBar->showMessage(
        QString("Білих: %1 vs Чорних: %2")
            .arg(Player::player(Player::White)->count())
            .arg(Player::player(Player::Black)->count()));
}

void Reversi::showGameOver() {
    Player* white = Player::player(Player::White);
    Player* black = Player::player(Player::Black);

    if (white->count() > black->count())
        QMessageBox::information(this, tr("Переможець"), tr("Вітаємо, білий гравець переміг"));
    else if (white->count() < black->count())
        QMessageBox::information(this, tr("Переможець"), tr("Вітаємо, чорний гравець переміг."));
    else
        QMessageBox::information(this, tr("Нічия"), tr("Гравці мають однакову кількість очок."));
}

void Reversi::flipOnCoords(Cell* cell, Reversi::Coordinates coords) {
    Q_ASSERT(cell != 0);

    int r = cell->row();
    int c = cell->col();

    do {
        switch (coords) {
            case Reversi::North:
                r--;
                break;
            case Reversi::NorthEast:
                r--;
                c++;
                break;
            case Reversi::East:
                c++;
                break;
            case Reversi::SouthEast:
                r++;
                c++;
                break;
            case Reversi::South:
                r++;
                break;
            case Reversi::SouthWest:
                r++;
                c--;
                break;
            case Reversi::West:
                c--;
                break;
            case Reversi::NorthWest:
                r--;
                c--;
                break;
            default:
                Q_UNREACHABLE();
                break;
        }

        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            if (m_board[r][c]->owner() != m_player->other())
                break;

            m_board[r][c]->setOwner(m_player);
        }
    } while (r >= 0 && r < 8 && c >= 0 && c < 8);
}
