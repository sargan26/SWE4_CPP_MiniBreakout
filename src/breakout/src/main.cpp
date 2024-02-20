//       $Id: main.cpp 2382 2020-01-09 09:31:58Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/se/sw/swo3/trunk/Uebungen/minilib/intro-1/src/main.cpp $
// $Revision: 2382 $
//     $Date: 2020-01-09 10:31:58 +0100 (Do., 09 J�n 2020) $
//   Creator: Peter Kulczycki
//  Creation: January, 2020
//   $Author: p20068 $
// Copyright: (c) 2020 Peter Kulczycki (peter.kulczycki<AT>fh-hagenberg.at)
//   License: This document contains proprietary information belonging to
//            University of Applied Sciences Upper Austria, Campus Hagenberg. It
//            is distributed under the Boost Software License, Version 1.0 (see
//            http://www.boost.org/LICENSE_1_0.txt).

#include <ml5/ml5.h>
#include <ranges>
#include <functional>
#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 600
#define PADDLE_HEIGHT 15
#define PADDLE_WIDTH 80
#define PADDLE_RADIUS 3
#define BRICK_HEIGHT 15
#define BRICK_WIDTH 60
#define ROWS 10
#define COLUMNS 4
#define TOP_OFFSET 60
#define BALL_RADIUS 6
#define GAME_SPEED 12 // niedriger Wert = schneller


struct shape : ml5::object {
    using context = ml5::paint_event::context_t; // in diesem Kontext malen wir, Abstraktion je nachdem ob wir Linux, Windows, Mac verwenden

    shape(wxPoint pos, const wxPen& pen, const wxBrush& brush) : aabb{ pos, pos }, pen{ pen }, brush{ brush } {};

    void draw(context& ctx) const {
        ctx.SetPen(pen);
        ctx.SetBrush(brush);
        do_draw(ctx);
    }

    auto contains(wxPoint pos) const noexcept -> bool {
        return aabb.Contains(pos);
    }

    const wxRect& GetAABB() const { return aabb; }

    void SetAABB(const wxRect& new_aabb) { aabb = new_aabb; }

    void move(wxPoint offset) { aabb.Offset(offset); }

    void Move(int dx, int dy) {
        aabb.Offset(dx, dy);
    }

    auto empty() const noexcept -> bool { return aabb.GetWidth() == 0 && aabb.GetHeight() == 0; }

    void set_right_bottom(wxPoint pos) { 
        aabb.SetRightBottom(pos);
    }

protected:
    wxRect aabb; // Axis-Aligned Bounding Box
private:
    virtual
        void do_draw(context& ctx) const = 0;

    const wxPen pen;
    const wxBrush brush;
};

struct paddle final : shape {
    paddle(wxPoint pos, const wxPen& pen, const wxBrush& brush)
        : shape(pos, pen, brush) {
        // Setze die Gr��e der AABB des Paddles
        aabb.SetWidth(PADDLE_WIDTH);
        aabb.SetHeight(PADDLE_HEIGHT);
        // Setze die Position der AABB des Paddles
        aabb.SetPosition(wxPoint(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - 60));
    }

private:
    void do_draw(context& ctx) const override {
        // Verwende AABB, um das Paddle zu zeichnen
        ctx.DrawRectangle(aabb);
    }
};

struct ball final : shape {
    ball(wxPoint pos, const wxPen& pen, const wxBrush& brush)
        : shape(pos, pen, brush) {
        // Setze die Gr��e der AABB des Balls
        aabb.SetWidth(BALL_RADIUS);
        aabb.SetHeight(BALL_RADIUS);
    }

private:
    void do_draw(context& ctx) const override {
        // Verwende AABB, um das Paddle zu zeichnen
        ctx.DrawCircle(aabb.GetX() + BALL_RADIUS, aabb.GetY() + BALL_RADIUS, BALL_RADIUS);
    }
};

struct brick final : shape {
    int points; // Punkte, die dieser Brick gibt
    brick(wxPoint pos, const wxPen& pen, const wxBrush& brush, int points_given)
        : shape(pos, pen, brush), points(points_given) {
        aabb.SetWidth(BRICK_WIDTH);
        aabb.SetHeight(BRICK_HEIGHT);
    }

private:
    void do_draw(context& ctx) const override {
        ctx.DrawRectangle(aabb);
    }
};

struct draw_application final : ml5::application {
    auto make_window() const -> std::unique_ptr<ml5::window> override { return std::make_unique <window>(); }

private:
    struct window final : ml5::window {
        window() : ml5::window{ "ML5.Draw" } {
            set_prop_initial_size(wxSize(WINDOW_WIDTH, WINDOW_HEIGHT));
            set_prop_allow_resize(false);
        }
    private:
        wxPen pen{ *wxBLACK_PEN };
        wxBrush brush{ *wxGREEN_BRUSH };
        std::unique_ptr<paddle> game_paddle;
        std::unique_ptr<ball> game_ball;
        int speed = -1;
        int deltaX = speed;
        int deltaY = speed;
        int score = 0;

        void on_init() override {
            // Init paddle in the middle
            game_paddle = std::make_unique<paddle>(wxPoint{ 0,0 }, *wxBLACK_PEN, *wxGREEN_BRUSH);
            game_ball = std::make_unique<ball>(wxPoint{ WINDOW_WIDTH / 2 - BALL_RADIUS, WINDOW_HEIGHT - 80 }, *wxBLACK_PEN, *wxBLUE_BRUSH);
            int x, y;
            for (size_t i = 0; i < COLUMNS; i++) {
                for (size_t j = 0; j < ROWS; j++) {
                    x = BRICK_WIDTH * j;
                    y = BRICK_HEIGHT * i + TOP_OFFSET;
                    int points = 0;
                    wxBrush brickBrush = *wxRED_BRUSH; // Standardfarbe

                    // Verschiedene Farben und Punkte f�r die Bricks
                    if (i < 2) {
                        points = 50;
                        brickBrush = *wxGREEN_BRUSH;
                    }
                    else if (i < 4) {
                        points = 30;
                        brickBrush = *wxYELLOW_BRUSH;
                    }
                    else if (i < 6) {
                        points = 20;
                        brickBrush = *wxBLUE_BRUSH;
                    }
                    else {
                        points = 10;
                        brickBrush = *wxRED_BRUSH;
                    }

                    auto new_brick = std::make_unique<brick>(wxPoint{ x, y }, *wxBLACK_PEN, brickBrush, points);
                    bricks.add(std::move(new_brick));
                }
            }

            add_menu("&Shape", {
                {"&Line", "The next shape will be a line."},
                {"&Rectangle", "The next shape will be a rectangle."},
                {"&Ellipse", "The next shape will be a ellipse."},
                });
            add_menu("&Brush", {
                {"&Red", "The new brush is red."},
                {"&Blue", "The new brush is blue."},
                {"&Green", "The new brush is green."},
                {"Blac&k", "The new brush is black."},
                });
            add_menu("&Pen", {
                {"&Red", "The new pen is red."},
                {"&Blue", "The new pen is blue."},
                {"&Green", "The new pen is green."},
                {"Blac&k", "The new pen is black."},
                });
            add_menu("Back&ground", {
                {"&White", "Draw white background"},
                {"&Black", "Draw black background"},
                });
            start_timer(std::chrono::milliseconds(GAME_SPEED));
        }

        void on_menu(const ml5::menu_event& event) override {
            const auto title{ event.get_title() };
            const auto item{ event.get_item() };

            if (title == "Background") {
                if (item == "White") set_prop_background_brush(*wxWHITE_BRUSH);
                else if (item == "Black") set_prop_background_brush(*wxBLACK_BRUSH);
                refresh(); 
            }
            else if (title == "Brush") {
                if (item == "Red") brush = *wxRED_BRUSH;
                else if (item == "Green") brush = *wxGREEN_BRUSH;
                else if (item == "Blue") brush = *wxBLUE_BRUSH;
                else if (item == "Black") brush = *wxBLACK_BRUSH;
            }
            else if (title == "Pen") {
                if (item == "Red") pen = *wxRED_PEN;
                else if (item == "Green") pen = *wxGREEN_PEN;
                else if (item == "Blue") pen = *wxBLUE_PEN;
                else if (item == "Black") pen = *wxBLACK_PEN;
            }
        }

        /*
      virtual void on_idle                    (idle_event const & event); // wird immer aufgerufen wenn sonst nichts passiert
      virtual void on_size                    (size_event const & event); // Bildschirm ausschnit vom Fenster verg��ern
        */

        void on_timer(const ml5::timer_event& event) override {
            // Bewege den Ball um die delta-Werte
            game_ball->move(wxPoint(deltaX, deltaY));

            // Hole die neue Position der AABB des Balls, nachdem er bewegt wurde
            auto ball_pos = game_ball->GetAABB();
            auto paddle_pos = game_paddle->GetAABB();

            // Pr�fe Kollision mit den Fensterr�ndern und invertiere die Richtung bei Bedarf
            if (ball_pos.GetLeft() <= 0 || ball_pos.GetRight() >= WINDOW_WIDTH) {
                deltaX = -deltaX; // Invertiere die horizontale Richtung
                game_ball->move(wxPoint(2 * deltaX, 0)); // Korrigiere die Position, um zu verhindern, dass der Ball stecken bleibt
            }

            // Pr�fe Kollision mit dem oberen Rand
            if (ball_pos.GetTop() <= 0) {
                deltaY = -deltaY; // Invertiere die vertikale Richtung
                game_ball->move(wxPoint(0, 2 * deltaY)); // Korrigiere die Position, um zu verhindern, dass der Ball stecken bleibt
            }

            // Pr�fe Kollision mit den Ziegeln
            auto it = bricks.begin();
            while (it != bricks.end()) {
                if (game_ball->GetAABB().Intersects((*it)->GetAABB())) {
                    // Kollision! F�ge die Punkte des Bricks hinzu und entferne den getroffenen Ziegel
                    score += 10;
                    bricks.remove(*it);

                    // �ndere die Bewegungsrichtung des Balls
                    deltaY = -deltaY;

                    // Element entfernt
                    break;
                }
                else {
                    ++it; // Keine Kollision, gehe zum n�chsten Ziegel
                }
            }


            // Pr�fe Kollision mit dem Schl�ger
            if (ball_pos.Intersects(paddle_pos)) {
                // Berechne die Differenz zwischen der Mitte des Schl�gers und der Position des Ballmittelpunktes
                int paddle_center = paddle_pos.GetX() + paddle_pos.GetWidth() / 2;
                int ball_center = ball_pos.GetX() + ball_pos.GetWidth() / 2;
                int hit_position = ball_center - paddle_center;

                // Der maximale Effekt tritt auf, wenn der Ball am Rand des Schl�gers trifft
                int max_effect_distance = paddle_pos.GetWidth() / 2;

                // Berechne den Einfluss auf die X-Bewegung basierend auf dem Treffpunkt
                // Der Einfluss nimmt linear mit der N�he zur Mitte des Schl�gers ab
                float influence = static_cast<float>(hit_position) / max_effect_distance;

                // Stelle sicher, dass der Einfluss zwischen -1 und 1 liegt
                influence = std::max(-1.0f, std::min(1.0f, influence));

                // Berechne neuen deltaX basierend auf dem Einfluss
                // Hier kannst du die maximale �nderung der X-Richtung anpassen
                int max_deltaX_change = 5;
                deltaX = static_cast<int>(influence * max_deltaX_change);

                // Invertiere die vertikale Richtung
                deltaY = -deltaY;

                // Korrigiere die Position, um den Ball �ber den Schl�ger zu bewegen
                game_ball->move(wxPoint(0, -(ball_pos.GetBottom() - paddle_pos.GetTop() + BALL_RADIUS)));
            }

            // Pr�fe, ob der Ball den unteren Rand des Fensters erreicht hat
            if (ball_pos.GetBottom() >= WINDOW_HEIGHT) {
                // TODO Logik f�r das Spielende oder das Zur�cksetzen des Balls implementieren
            }

            refresh();
        }


        void on_paint(const ml5::paint_event& event) override {
            auto& ctx{ event.get_context() };

            // Zeichne alle Bricks, den Paddle und den Ball
            for (const auto& b : bricks) b->draw(ctx);
            if (game_paddle) { game_paddle->draw(ctx); }
            if (game_ball) { game_ball->draw(ctx); }

            // Zeichne den Spielstand
            wxString scoreText = wxString::Format(wxT("Score: %d"), score);
            ctx.DrawText(scoreText, 10, WINDOW_HEIGHT - 40); // Positioniere den Text unten links im Fenster
        }


        void on_key(const ml5::key_event& event) override {
            const auto key = event.get_key_code();
            if (key == WXK_LEFT) {
                game_paddle->move(wxPoint(-5, 0));
            }
            else if (key == WXK_RIGHT) {
                game_paddle->move(wxPoint(5, 0));
            }
            refresh();
        }

        void on_mouse_move(const ml5::mouse_event& event) override {
                auto mouse_pos = event.get_position();
                auto paddle_pos = game_paddle->GetAABB();

                int new_x = mouse_pos.x - paddle_pos.GetWidth() / 2;
                new_x = std::max(0, std::min(WINDOW_WIDTH - paddle_pos.GetWidth(), new_x));



                game_paddle->SetAABB(wxRect(new_x, paddle_pos.GetY(), paddle_pos.GetWidth(), paddle_pos.GetHeight()));

                refresh();
        }

        ml5::vector<std::unique_ptr<shape>> bricks;
    };

};



int main(int argc, char* argv[]) {

    draw_application app;
    return app.run(argc, argv);

}
