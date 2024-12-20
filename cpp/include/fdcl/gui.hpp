/** \file gui.hpp
*  \brief GUI construction and update related functions (undocumented)
*/

#ifndef FDCL_GUI_HPP
#define FDCL_GUI_HPP

#include "fdcl/clock.hpp"
#include "fdcl/types/vectors.hpp"

#include <gtkmm.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>


namespace fdcl {

class Gui {
public:
    Gui(void);
    ~Gui(void);
    void init(void);
    void run(void);
    void exit(void);

private:
    fdcl::Clock clock;
    
    bool is_init = false;

    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create();
    Gtk::Window *window;

    Gtk::Label *lbl_t, *lbl_freq_wifi, *lbl_freq_imu, *lbl_freq_gps;
    Gtk::Label *lbl_freq_ane1, *lbl_freq_ane2;

    Gtk::Button *btn_close;

    Gtk::Label *lbl_ypr[3], *lbl_a[3], *lbl_W[3];
    Gtk::Label *lbl_rtk[3], *lbl_gps[3], *lbl_stats[3];
    Gtk::Label *lbl_top_sats, *lbl_top_stat;

    Gtk::Label *lbl_ane1[3], *lbl_ane2[3];

    Gtk::Label *lbl_base_ypr[3], *lbl_base_a[3], *lbl_base_W[3];
    Gtk::Label *lbl_base_ane[3], *lbl_base_gps[3], *lbl_base_stats[3];

    bool update(void);
    bool on_key_press(GdkEventKey *event);

    void update_freq_label(Gtk::Label *lbl, const std::string &freq_name, 
        const int data
    );
    void update_vbox(Gtk::Label *lbl[], const vector3 &data);
    void update_grid(Gtk::Label *lbl[], const matrix3 &data);

    void on_btn_close_clicked(void);

    std::string get_mode(const int mode);
    
    std::string log_file_name;
    int freq_update_counter = 20;
    int num_avg = 100;
    float avg_IMU = 0.0, avg_WIFI = 0.0, avg_VICN = 0.0, avg_CTRL = 0.0,
          avg_GPS = 0.0;
};

}  // end of namespace fdcl

#endif
