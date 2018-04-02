#ifndef LOGINPANEL_HPP
#define LOGINPANEL_HPP

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <utility>
#include "UI.hpp"
#include <map>
#include "GeometricRecognizer.h"
#include "GeometricRecognizerTypes.h"
#include "SampleGestures.h"

#define MAX_USER 50

namespace UI
{

class LoginPanel : public Rectangle, public Touchable
{

  public:

    virtual void draw();
    virtual void undraw();

    // draw/undraw username and password input fields
    void us_draw();
    void us_draw();

    // draw/undraw 
    void f_draw();
    void f_undraw();

    // switch between username/password page and facial login page
    void switch_page();

    LoginPanel(SimpleGraphics &graphics, Wifi wifi, Bluetooth bt, GeometricRecognizer gr, Video video,
               SimpleGraphics::rgba_t btn_background_color, SimpleGraphics::rgba_t background_color, 
               SimpleGraphics::rgba_t text_color, SimpleGraphics::rgba_t hint_color,
               TouchControl &touch, Point p1, Point p2, NIOS_Processor nios);

    // call back function
    typedef std::function<void(int login_status, string user)> LoginStatusCB;
    void login_status_cb(LoginStatusCB cb) { m_login_status_cb = cb; }


    /* callded when each time an username input character is received
     * 
     * Modify:
     *      m_username_input should be updated with new inputs
     *      username field should display current inputs    
     */ 
    void updateInputField();


    void clear();




    /*
     * Description:
     *      1. triggered by clicking Login Button (either byclicking manually or by using Wand). 
     *      2. m_username_input will be sent to server over WIFI module
     *      3. wait for server for a match
     *      
     *      if matches:
     *      4. return to home screen
     *      5. display username at upper right corner
     *      6. create a private field for gesture mapping on Server
     * 
     *      if not matches:
     *      4. display error message
     *      5. wait for next input
     * 
     * Change m_login_status:
     *      1:  login successfully
     *      0:  account not existed
     *      -1: wrong password
     */
    void login();

    /* 
     * Feedback user whether or not login/register is successful
     */
    void UsernameFieldMsg(string feedback);

    /*
     * Print * 
     */
    void PasswordInputMsg();

    /*
     * Describe the reason of login/register failure
     */
    void PAsswordFieldMsg(string feedback);

    /*
     * Reset the login and password field
     */
    void inputFieldsClear();

    /*
     *  triggered when username field is chosen
     */
    void usernameFieldChose();

    /*
     *  triggered when password field is chosen
     */
    void passwordFieldChose();



  private:

    Wifi &m_wifi;
    Bluetooth &m_bluetooth;
    GeometricRecognizer &m_geometricRecognizer;
    NIOS_Processor &m_nios;
    Video m_video;

    LoginStatusCB m_login_status_cb;

    SimpleGraphics m_graphics;
    SimpleGraphics::rgba_t m_background_color = rgba(0,0,0,255);
    SimpleGraphics::rgba_t m_btn_background_color = rgba(102,102,102,255);
    SimpleGraphics::rgba_t m_text_color = rgba(226,226,226,255);
    SimpleGraphics::rgba_t m_hint_color = rgba(156,156,156,255);
    constexpr static SimpleGraphics::rgba_t m_picture_color = rgba(228,228,228,0);

    /*  Description:
     *      the input characters will be recorded into these vectors 
     *      one at a time
     */
    string m_username_input;
    string m_password_input;

    Button m_username_field;
    Button m_password_field;

    // login() called when clicked
    Button m_login_button;

    // interface to camera
    Button m_switch_login_mode_button;


    /*  Description:
     *      when clicked (selected), m_username_input keeps track of switch changes 
     *      until return key is pressed.
     * 
     *      meanwhile, user input will be displayed in this field
     */
    

    // login() called when clicked
    Button m_login_button;
 
    // determine whether in username/password page or facial page
    // false: username/password
    // true:  facial
    bool m_login_page;

    // determine either input field is chose
    // false: username
    // true:  password
    bool m_input_field_chose;

    int m_login_status;
};
}

#endif