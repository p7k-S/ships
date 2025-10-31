#include <SFML/Graphics.hpp>

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    view.zoom(0.9f); // Zoom in
                } else {
                    view.zoom(1.1f); // Zoom out
                }
                window.setView(view); // Apply the new view
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                colScheme = (colScheme == COLORS) ? INVERT : COLORS;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Left)
                    view.move(-10.f, 0.f); // Move left
                if (event.key.code == sf::Keyboard::Right)
                    view.move(10.f, 0.f);  // Move right
                if (event.key.code == sf::Keyboard::Up)
                    view.move(0.f, -10.f); // Move up
                if (event.key.code == sf::Keyboard::Down)
                    view.move(0.f, 10.f);  // Move down
                window.setView(view); // Apply the updated view
            }

