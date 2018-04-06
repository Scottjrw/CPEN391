#ifndef WAND_CONTROL_HPP
#define WAND_CONTROL_HPP

#include <unordered_map>
#include "bluetooth.hpp"
#include "GeometricRecognizer.hpp"
#include "NIOS_Processor.hpp"
#include "UI.hpp"

/* A class that essentially glues together GestureRecognizer, Wand and NIOS_Processor
 *
 * This class determines what mode the wand is in and uses that info
 * to determine what to do with points received by the NIOS_Processor
 */
class WandControl : public UI::Touchable {
public:
    using GeometricRecognizer = DollarRecognizer::GeometricRecognizer;
    using RecognitionResult = DollarRecognizer::RecognitionResult;

    WandControl(GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios);

    ~WandControl();

    void start() { m_nios.start(); }

    // Receive Typing
    typedef std::function<void(char c)> TypingLetterCB;
    void setTypingCB(TypingLetterCB cb) { m_typeCB = cb; }

    // Receive gestures
    typedef std::function<void(std::string gesture)> GestureCB;
    void setGestureCB(GestureCB cb) { m_gestureCB = cb; }

protected:
    virtual void updateCursor(Point p) = 0;
    virtual void cursorModeChange(Wand::Modes old_mode) = 0;
    virtual void startDrawPath() = 0;
    virtual void stopDrawPath() = 0;
    virtual void clearDrawPath() = 0;
    Wand::Modes m_mode;

private:
    GeometricRecognizer &m_recognizer;
    Wand &m_wand;
    NIOS_Processor &m_nios;
    std::vector<PointD> m_points;
    Point m_last_location;
    bool m_recv_points;
    TypingLetterCB m_typeCB;
    GestureCB m_gestureCB;

    static constexpr double MIN_SIMILARITY = 0.0;
    static constexpr double MIN_POINTS = 10;
    static constexpr unsigned AVG_POINTS = 16;

    std::array<Point, AVG_POINTS> m_last_points;
    std::array<Point, AVG_POINTS>::iterator m_cur_point;

    void handle_result(RecognitionResult result);

    void dot_location_cb(const NIOS_HPS_Protocol::Dot_Location::body &body);

    void mode_cb(Wand::Modes mode);

    void startstop_cb(Wand::WandCommands command);

};

#endif // WAND_CONTROL_HPP
