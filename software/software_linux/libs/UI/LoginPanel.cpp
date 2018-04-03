#include "LoginPanel.hpp"

#define BUTTON_HEIGHT 50
#define BUTTON_WIDTH 200
#define STATUS_DEFAULT 5
#define PICTURE_HEIGHT 60
#define PICTURE_WIDTH 80
#define INPUT_FIELD_WIDTH 300
#define INPUT_FIELD_HEIGHT 40

namespace UI
{

LoginPanel::LoginPanel(SimpleGraphics &graphics, Wifi &wifi, Bluetooth &bt, GeometricRecognizer &gr, Video &video,
            TouchControl &touch, Point p1, Point p2, NIOS_Processor &nios,
            rgba_t background_color,
            rgba_t btn_background_color,
            rgba_t text_color,
            rgba_t hint_color):
    Rectangle(graphics, p1, p2, background_color),
    Touchable(),
    m_wifi(wifi),
    m_bluetooth(bt),
    m_geometricRecognizer(gr),
    m_nios(nios),
    m_video(video),
    m_login_status_cb(nullptr),
    m_background_color(background_color),
    m_btn_background_color(btn_background_color),
    m_text_color(text_color),
    m_hint_color(hint_color),
    // ############################################### //
    // ##  should be modified to clickable objects  ## //
    m_username_field(graphics, 
            {p1.x + (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3}, 
            {p2.x - (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + BUTTON_HEIGHT}, 
            "Username", m_hint_color, m_btn_background_color),
    m_password_field(graphics, 
            {p1.x + (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + BUTTON_HEIGHT + 20}, 
            {p2.x - (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + 2*BUTTON_HEIGHT + 20}, 
            "Password", m_hint_color, m_btn_background_color),
    // ##                                           ## //
    // ############################################### //
    m_login_button(graphics, 
            {p2.x - (p2.x - p1.x - 2*BUTTON_WIDTH)/3 - BUTTON_WIDTH, p2.y - 3*BUTTON_HEIGHT/2}, 
            {p2.x - (p2.x - p1.x - 2*BUTTON_WIDTH)/3, p2.y - BUTTON_HEIGHT/2},
            "Login", m_text_color, m_btn_background_color),
    m_switch_login_mode_button(graphics,
            {p1.x + (p2.x - p1.x - 2*BUTTON_WIDTH)/3, p2.y - 3*BUTTON_HEIGHT/2},
            {p1.x + (p2.x - p1.x - 2*BUTTON_WIDTH)/3 + BUTTON_WIDTH, p2.y - BUTTON_HEIGHT/2},
            "Switch Login Mode", m_text_color, m_btn_background_color),
    m_login_page(false),
    m_input_field_chose(false),
    m_login_status(STATUS_DEFAULT)
{   
    // add Drawables and Touchables
    addDrawable(this);
    addDrawable(m_login_button);
    addDrawable(m_switch_login_mode_button);
    addDrawable(m_password_field);
    addDrawable(m_username_field);

    addTouchable(m_login_button);
    addTouchable(m_switch_login_mode_button);
    addTouchable(m_password_field);
    addTouchable(m_username_field);

    // ############ add on touch functuon 
    
    m_login_button.onTouch(std::bind(&LoginPanel::login, this));
    m_switch_login_mode_button.onTouch(std::bind(&LoginPanel::switch_page, this));
    m_username_field.onTouch(std::bind(&LoginPanel::usernameFieldChose, this));
    m_password_field.onTouch(std::bind(&LoginPanel::passwordFieldChose, this));


}

void LoginPanel::draw(){
    Rectangle::draw();
    if(m_login_page){
        us_draw();
        m_login_button.draw();
        m_switch_login_mode_button.draw();
    } else {
        Rectangle::draw();
        f_draw();
        m_login_button.draw();
        m_switch_login_mode_button.draw();
    }
}

void LoginPanel::undraw(){
    m_login_button.undraw();
    m_switch_login_mode_button.undraw();

    if(!m_login_page){
        us_undraw();
    }else{
        f_undraw();
    }
    Rectangle::undraw();
}

bool LoginPanel::touch(Point p) {
    return m_login_button.touch(p) || m_switch_login_mode_button.touch(p)
        || m_username_field.touch(p) || m_password_field.touch(p);
}

void LoginPanel::us_draw(){
    m_username_field.draw();
    m_password_field.draw();
}

void LoginPanel::us_undraw(){
    m_username_field.undraw();
    m_password_field.undraw();
}

void LoginPanel::f_draw(){
    m_graphics.draw_rect(m_picture_color, {280,40}, {280+PICTURE_WIDTH, 40+PICTURE_HEIGHT});
}

void LoginPanel::f_undraw(){
    m_graphics.clear();
}



void LoginPanel::switch_page(){
    m_login_page = ~m_login_page;

    draw();

}

void LoginPanel::usernameFieldChose(){
    m_input_field_chose = 0;
}

void LoginPanel::passwordFieldChose(){
    m_input_field_chose = 1;
}

void LoginPanel::updateInputField(std::string str){

    PasswordFieldMsg("");
    
    while(m_bluetooth.ReadResponse() != '1'){
        // doing nothing if no button click received
    }

    // now we get start button click
    // start reading the path
    Path2D newPath;

    while(m_bluetooth.ReadResponse() != '1'){
        m_nios.start();
        nios.dot_location_cb([&newPath](unsigned x, unsigned y){
            newPath.push_back(Point2D(x,y));
        });
    }
    
    // now stop button is clicked
    // we recognize the path   
    m_nios.stop();  
    RecognitionResult result = m_geometricRecognizer.recognize(newPath);
    
    // ############## 把两个input fields  从Button 改成 Textbox ##############//
    // ############## 根据哪个Textbox 被Cursor 选中，来update username or password ##############//

    // username field is chosen
    if(!m_input_field_chose){
        if(name == "Delete"){
            if(m_username_input.length() > 0){
                m_username_input.pop_back();
            }
            return;
        }
        else{
            m_username_input += result->name;
            UsernameFieldMsg(m_username_input);
            return;
        }
    }
    // password field is chosen
    else{
        if(name == "Delete"){

            if(m_password_input.length() > 0){
                m_password_input.pop_back();
            }
            return;
        }
        else{
            m_username_input += result->name;
            updatePasswordField();
            return;
        }
    }
    
    while(true){
        m_nios.trypoll();
    }
}

void LoginPanel::login()
{   
    // in username/password page
    if(!m_login_page){
        string result;

        /* WIFI connection goes here */
        m_wifi.SendUsername(m_username_input, m_password_input);
        result = m_wifi.ReadResponse();

        if(result == "done"){
            UsernameFieldMsg("Login Complete!");
            m_login_status = 1;
            //m_login_status_cb(m_login_status, m_username_input);
            stop(0);
        }

        else if(result == "fail"){
            UsernameFieldMsg("Login Failed.");
            PasswordFieldMsg("Wrong Password.");
            m_login_status = -1;
        }
    }

    // in facial login page
    else{
        std::string picResult;

        m_wifi.SendPicture(m_video.takeRawPicture());

        picResult = m_wifi.ReadResponse();

        if(picResult == "done"){
            m_login_status = 1;
            m_login_status_cb(m_login_status, m_username_input);
        }

        else if(picResult == "fail"){
            m_login_status = -1;
        }
    }
}

void LoginPanel::UsernameFieldMsg(std::string feedback)
{
    m_username_field = Button(m_graphics, {p1.x + (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3}, 
                     {p2.x - (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + BUTTON_HEIGHT}, 
                     feedback, m_hint_color, m_btn_background_color);
    m_username_field.draw();
}

void LoginPanel::updatePasswordField(){
    if(m_password_input.length() == 0){
        return;
    }

    std::string starSatr = "";
    int i = 0;

    while(i < m_password_input.length()-1){
        starStr += "*";
        i++
    }

    starStr += m_password_input.at[i];

    PasswordFieldMsg(starStr);
}

void LoginPanel::PasswordFieldMsg(std::string feedback)
{   
    m_password_field = Button(m_graphics, {p1.x + (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + BUTTON_HEIGHT} + 20}, 
                     {p2.x - (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + 2*BUTTON_HEIGHT} + 20}, 
                     feedback, m_hint_color, m_btn_background_color);
    m_password_field.draw();
}

void LoginPanel::inputFieldsClear()
{

    UsernameFieldMsg("Username");
    PasswordFieldMsg("Password");
}

void LoginPanel::clear(){

    m_username_input = "";
    m_password_input = "";
    m_login_status = 0;
    m_login_page = 0;
    m_input_field_chose = 0;
    inputFieldsClear();
}
