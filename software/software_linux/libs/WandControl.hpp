#ifndef WAND_CONTROL_HPP
#define WAND_CONTROL_HPP

#include <unordered_map>
#include "bluetooth.hpp"
#include "GeometricRecognizer.hpp"
#include "NIOS_Processor.hpp"

/* A class that essentially glues together GestureRecognizer, Wand and NIOS_Processor
 *
 * This class determines what mode the wand is in and uses that info
 * to determine what to do with points received by the NIOS_Processor
 */
class WandControl {
public:
    using GeometricRecognizer = DollarRecognizer::GeometricRecognizer;
    using RecognitionResult = DollarRecognizer::RecognitionResult;

    WandControl(Wand &wand, NIOS_Processor &nios);

    ~WandControl();

    // Receive clicks, down = click is pressed, up = click is released
    // If a class only cares to get clicks they can leave up as nullptr
    typedef std::function<void(Point p)> CursorClickCB;
    void setCursorClickCB(CursorClickCB down, CursorClickCB up=nullptr) {
        m_clickDownCB = down;
        m_clickUpCB = up;
    }

    // Receive cursor movements
    typedef std::function<void(Point p)> CursorUpdateCB;
    void setCursorCB(CursorUpdateCB cb) { m_cursorCB = cb; }

    // Receive Typing
    typedef std::function<void(char c)> TypingLetterCB;
    void setTypingCB(TypingLetterCB cb) { m_typeCB = cb; }

    // Receive gestures
    typedef std::function<void(std::string gesture)> GestureCB;
    void setGestureCB(GestureCB cb) { m_gestureCB = cb; }

private:
    GeometricRecognizer m_recognizer;
    Wand &m_wand;
    NIOS_Processor &m_nios;
    std::vector<PointD> m_points;
    Point m_last_location;
    bool m_recv_points;
    Wand::Modes m_mode;
    CursorClickCB m_clickDownCB;
    CursorClickCB m_clickUpCB;
    CursorUpdateCB m_cursorCB;
    TypingLetterCB m_typeCB;
    GestureCB m_gestureCB;
    std::unordered_map<std::string, std::string> m_gesture_map;

    static constexpr double MIN_SIMILARITY = 0.5;
    static constexpr double MIN_POINTS = 20;

    void handle_result(RecognitionResult result);

    void dot_location_cb(const NIOS_HPS_Protocol::Dot_Location::body &body);

    void mode_cb(Wand::Modes mode);

    void startstop_cb(Wand::WandCommands command);

};

#endif // WAND_CONTROL_HPP
