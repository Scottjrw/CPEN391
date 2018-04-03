#include "WandControl.hpp"

using namespace NIOS_HPS_Protocol;

WandControl::WandControl(Wand &wand, NIOS_Processor &nios):
    m_wand(wand),
    m_nios(nios),
    m_points(),
    m_last_location(0, 0),
    m_recv_points(false),
    m_mode(wand.getMode()),
    m_clickDownCB(nullptr),
    m_clickUpCB(nullptr),
    m_cursorCB(nullptr),
    m_typeCB(nullptr),
    m_gestureCB(nullptr)
{
    using std::placeholders::_1;

    m_wand.setMode(std::bind(&WandControl::mode_cb, this, _1));
    m_nios.dot_location_cb(std::bind(&WandControl::dot_location_cb, this, _1));
}

WandControl::~WandControl() {
    m_wand.setMode(std::function<void(Wand::Modes)>(nullptr));
    m_nios.dot_location_cb(nullptr);
}

void WandControl::mode_cb(Wand::Modes mode) {
    m_mode = mode;
    m_recv_points = false;
}

void WandControl::startstop_cb(Wand::WandCommands command) {
    switch (command) {
        case Wand::wandStart:
            if (m_mode == Wand::cursorMode) {
                if (m_clickDownCB)
                    m_clickDownCB(m_last_location);
            } else
                m_recv_points = true;
            break;
        case Wand::wandStop:
            if (m_mode == Wand::cursorMode) {
                if (m_clickUpCB)
                    m_clickUpCB(m_last_location);
            } else {
                m_recv_points = false;

                if (m_points.size() > MIN_POINTS) {
                    RecognitionResult result = m_recognizer.recognize(m_points);
                    m_points.clear();
                    handle_result(result);
                }
            }

            break;
    }
}

void WandControl::dot_location_cb(const Dot_Location::body &body) {
    m_last_location = Point(body.avg_x, body.avg_y);

    switch (m_mode) {
        case Wand::gestureMode:
        case Wand::typingMode:
            if (m_recv_points) {
                m_points.emplace_back(m_last_location);
            }
        case Wand::cursorMode:
            if (m_cursorCB) {
                m_cursorCB(m_last_location);
            }

            break;
    }
}

void WandControl::handle_result(RecognitionResult result) {
    auto gesture = m_gesture_map.find(result.name);

    if (gesture != m_gesture_map.end() && result.score >= MIN_SIMILARITY) {
        switch (m_mode) {
            case Wand::gestureMode:
                if (m_gestureCB)
                    m_gestureCB((*gesture).second);
                break;
            case Wand::typingMode:
                if ((*gesture).second.length() == 1 && m_typeCB)
                    m_typeCB((*gesture).second[0]);
                break;
            case Wand::cursorMode:
                assert(0);
        }
    }
}
