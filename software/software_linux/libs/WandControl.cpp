#include "WandControl.hpp"

using namespace NIOS_HPS_Protocol;

WandControl::WandControl(GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios):
    Touchable(),
    m_mode(wand.getMode()),
    m_recognizer(geo),
    m_wand(wand),
    m_nios(nios),
    m_points(),
    m_last_location(0, 0),
    m_recv_points(false),
    m_typeCB(nullptr),
    m_gestureCB(nullptr),
    m_cur_point(m_last_points.begin())
{
    using std::placeholders::_1;

    m_wand.setMode(std::bind(&WandControl::mode_cb, this, _1));
    m_wand.setGesture(std::bind(&WandControl::startstop_cb, this, _1));
    m_nios.dot_location_cb(std::bind(&WandControl::dot_location_cb, this, _1));
}

WandControl::~WandControl() {
    m_wand.setMode(Wand::ModeCB(nullptr));
    m_wand.setGesture(Wand::WandCommandCB(nullptr));
    m_nios.dot_location_cb(nullptr);
}

void WandControl::mode_cb(Wand::Modes mode) {
    Wand::Modes old = m_mode;
    m_mode = mode;
    m_recv_points = false;
    cursorModeChange(old);
}

void WandControl::startstop_cb(Wand::WandCommands command) {
    switch (command) {
        case Wand::wandStart:
            if (m_mode == Wand::cursorMode) {
                std::cout << "WC Touch" << std::endl;
                this->touch(m_last_location);
            } else {
                std::cout << "WC Gesture Start" << std::endl;
                m_recv_points = true;
                startDrawPath();
            }
            break;
        case Wand::wandStop:
            if (m_mode == Wand::cursorMode) {
                std::cout << "WC Touch" << std::endl;
                this->touch(m_last_location);
            } else {
                std::cout << "WC Gesture Stop" << std::endl;
                m_recv_points = false;
                stopDrawPath();
                clearDrawPath();

                if (m_points.size() > MIN_POINTS) {
                    RecognitionResult result = m_recognizer.recognize(m_points);
                    m_points.clear();
                    handle_result(result);
                } else {
                    std::cout << "Not enough POints" << std::endl;
                }
            }
            break;
        default:
            assert(0);
    }
}

void WandControl::dot_location_cb(const Dot_Location::body &body) {
    *m_cur_point  = Point(body.avg_x, body.avg_y);
    m_cur_point++;

    if (m_cur_point == m_last_points.end()) {
        m_cur_point = m_last_points.begin();

        PointD p(0, 0);
        for (auto i = m_last_points.begin(); i != m_last_points.end(); i++) {
            p.x += i->x;
            p.y += i->y;
        }

        p.x /= AVG_POINTS;
        p.y /= AVG_POINTS;

        m_last_location = p;

        switch (m_mode) {
            case Wand::gestureMode:
            case Wand::typingMode:
                if (m_recv_points) {
                    m_points.emplace_back(m_last_location);
                }
            case Wand::cursorMode:
                updateCursor(m_last_location);
                break;
        }
    }
}

void WandControl::handle_result(RecognitionResult result) {
    if (result.name.length() == 0) {
        std::cout << "Nothing" << std::endl;
        return;
    }

    switch (m_mode) {
        case Wand::gestureMode:
            if (result.score >= MIN_SIMILARITY) {
                if (m_gestureCB)
                    m_gestureCB(result.name);
            } else {
                std::cout << "Bad gesture" << std::endl;
            }
            std::cout << "Gesture: " << result.name << std::endl;
            break;
        case Wand::typingMode:
            // Top two results are single characters, if they are a upper and lowercase of the same character
            // we take the lowercase
            if (result.name.length() == result.name2.length() && result.name.length() == 1) {
                char c = result.name[0];
                char c2 = result.name2[0];
                const char lowercasediff = 'a' - 'A';

                if (c > c2) {
                    if (c - lowercasediff == c2) {
                        result.name = c;
                    }
                } else {
                    if (c2 - lowercasediff == c) {
                        result.name = c2;

                    }
                }
            }

            std::cout << "Typing: " << result.name << std::endl;

            if (result.name.length() == 1 && m_typeCB)
                m_typeCB(result.name[0]);
            break;
        case Wand::cursorMode:
            assert(0);
    }
}
