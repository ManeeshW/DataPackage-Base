#include "fdcl/gui.hpp"
#include "fdcl/system.hpp"


fdcl::Gui::Gui(void) {
    std::cout << "GUI: starting" << std::endl;
    init();
}


fdcl::Gui::~Gui(void) {
    exit();
    std::cout << "GUI: closed" << std::endl;
}


void fdcl::Gui::init(void) {
    int width = 600;
    int height = 200;

    int line_height = 10;
    
    int box_width = 120;
    int box_height = 80;

    int button_width = 80;

    auto builder = Gtk::Builder::create_from_file("../gui.glade");
    builder->get_widget<Gtk::Window>("window_main", window);
    window->set_default_size(width, height);
    window->set_title("ONR Data Package");

    builder->get_widget<Gtk::Button>("btn_close", btn_close);
    btn_close->set_tooltip_text("Exit the program.");
    btn_close->signal_clicked().connect(sigc::mem_fun(*this,
        &Gui::on_btn_close_clicked));

    builder->get_widget<Gtk::Label>("lbl_t", lbl_t);
    lbl_t->set_text("t = 0.0 s");
    lbl_t->set_xalign(0.0);

    builder->get_widget<Gtk::Label>("lbl_t", lbl_freq_wifi);
    lbl_freq_wifi->set_text("WIFI: 0 Hz");
    lbl_freq_wifi->set_xalign(0.0);

    builder->get_widget<Gtk::Label>("lbl_freq_imu", lbl_freq_imu);
    lbl_freq_imu->set_text("IMU: 0 Hz");
    lbl_freq_imu->set_xalign(0.0);

    builder->get_widget<Gtk::Label>("lbl_freq_gps", lbl_freq_gps);
    lbl_freq_gps->set_text("GPS: 0 Hz");
    lbl_freq_gps->set_xalign(0.0);

    builder->get_widget<Gtk::Label>("lbl_freq_ane1", lbl_freq_ane1);
    lbl_freq_ane1->set_text("ANE1: 0 Hz");
    lbl_freq_ane1->set_xalign(0.0);

    builder->get_widget<Gtk::Label>("lbl_freq_ane2", lbl_freq_ane2);
    lbl_freq_ane2->set_text("ANE2: 0 Hz");
    lbl_freq_ane2->set_xalign(0.0);


    builder->get_widget<Gtk::Label>("lbl_sats", lbl_top_sats);
    builder->get_widget<Gtk::Label>("lbl_rtk_status", lbl_top_stat);

    builder->get_widget<Gtk::Label>("lbl_ypr_1", lbl_ypr[0]);
    builder->get_widget<Gtk::Label>("lbl_ypr_2", lbl_ypr[1]);
    builder->get_widget<Gtk::Label>("lbl_ypr_3", lbl_ypr[2]);

    builder->get_widget<Gtk::Label>("lbl_a_1", lbl_a[0]);
    builder->get_widget<Gtk::Label>("lbl_a_2", lbl_a[1]);
    builder->get_widget<Gtk::Label>("lbl_a_3", lbl_a[2]);

    builder->get_widget<Gtk::Label>("lbl_W_1", lbl_W[0]);
    builder->get_widget<Gtk::Label>("lbl_W_2", lbl_W[1]);
    builder->get_widget<Gtk::Label>("lbl_W_3", lbl_W[2]);

    builder->get_widget<Gtk::Label>("lbl_rtk_1", lbl_rtk[0]);
    builder->get_widget<Gtk::Label>("lbl_rtk_2", lbl_rtk[1]);
    builder->get_widget<Gtk::Label>("lbl_rtk_3", lbl_rtk[2]);

    builder->get_widget<Gtk::Label>("lbl_gps_1", lbl_gps[0]);
    builder->get_widget<Gtk::Label>("lbl_gps_2", lbl_gps[1]);
    builder->get_widget<Gtk::Label>("lbl_gps_3", lbl_gps[2]);

    builder->get_widget<Gtk::Label>("lbl_rover_stat", lbl_stats[0]);
    builder->get_widget<Gtk::Label>("lbl_rover_sats", lbl_stats[1]);
    builder->get_widget<Gtk::Label>("lbl_rover_utc", lbl_stats[2]);

    builder->get_widget<Gtk::Label>("lbl_ane1_1", lbl_ane1[0]);
    builder->get_widget<Gtk::Label>("lbl_ane1_2", lbl_ane1[1]);
    builder->get_widget<Gtk::Label>("lbl_ane1_3", lbl_ane1[2]);

    builder->get_widget<Gtk::Label>("lbl_ane2_1", lbl_ane2[0]);
    builder->get_widget<Gtk::Label>("lbl_ane2_2", lbl_ane2[1]);
    builder->get_widget<Gtk::Label>("lbl_ane2_3", lbl_ane2[2]);


    builder->get_widget<Gtk::Label>("lbl_base_ypr_1", lbl_base_ypr[0]);
    builder->get_widget<Gtk::Label>("lbl_base_ypr_2", lbl_base_ypr[1]);
    builder->get_widget<Gtk::Label>("lbl_base_ypr_3", lbl_base_ypr[2]);

    builder->get_widget<Gtk::Label>("lbl_base_a_1", lbl_base_a[0]);
    builder->get_widget<Gtk::Label>("lbl_base_a_2", lbl_base_a[1]);
    builder->get_widget<Gtk::Label>("lbl_base_a_3", lbl_base_a[2]);

    builder->get_widget<Gtk::Label>("lbl_base_W_1", lbl_base_W[0]);
    builder->get_widget<Gtk::Label>("lbl_base_W_2", lbl_base_W[1]);
    builder->get_widget<Gtk::Label>("lbl_base_W_3", lbl_base_W[2]);

    builder->get_widget<Gtk::Label>("lbl_base_gps_1", lbl_base_gps[0]);
    builder->get_widget<Gtk::Label>("lbl_base_gps_2", lbl_base_gps[1]);
    builder->get_widget<Gtk::Label>("lbl_base_gps_3", lbl_base_gps[2]);

    builder->get_widget<Gtk::Label>("lbl_base_gps_stat", lbl_base_stats[0]);
    builder->get_widget<Gtk::Label>("lbl_base_gps_sats", lbl_base_stats[1]);
    builder->get_widget<Gtk::Label>("lbl_base_gps_utc", lbl_base_stats[2]);

    builder->get_widget<Gtk::Label>("lbl_base_ane_1", lbl_base_ane[0]);
    builder->get_widget<Gtk::Label>("lbl_base_ane_2", lbl_base_ane[1]);
    builder->get_widget<Gtk::Label>("lbl_base_ane_3", lbl_base_ane[2]);

    window->set_focus_child(*btn_close);
    window->show_all();

    Glib::signal_idle().connect(sigc::mem_fun(*this, &fdcl::Gui::update));
    // window.signal_key_press_event().connect(sigc::mem_fun(*this, 
    //     &Gui::on_key_press), false);

    is_init = true;
}


bool fdcl::Gui::update(void) {
    if (!is_init) {
        return true;
    }

    double t = clock.get_seconds();
    std::stringstream t_stream;
    t_stream << "t: " 
        << std::setw(9) << std::fixed << std::setprecision(1) << t << " s";
    lbl_t->set_text(t_stream.str());

    update_freq_label(lbl_freq_imu, "IMU", rover.freq_imu);
    update_freq_label(lbl_freq_gps, "GPS", rover.freq_gps);
    update_freq_label(lbl_freq_ane1, "ANE1", rover.freq_ane1);
    update_freq_label(lbl_freq_ane2, "ANE2", rover.freq_ane2);

    vector3 ypr, a, W;
    rover.get_imu(ypr, a, W);

    update_vbox(lbl_ypr, ypr);
    update_vbox(lbl_a, a);
    update_vbox(lbl_W, W);

    piksi_t piksi;
    vector3 x_g, v_g, llh;

    rover.get_gps(piksi);
    x_g << piksi.n, piksi.e, piksi.d;
    llh << piksi.lat, piksi.lon, piksi.h;

    update_vbox(lbl_rtk, x_g);
    update_vbox(lbl_gps, llh);

    std::stringstream stream;
    stream << "Mode: " << get_mode(piksi.status);
    lbl_stats[0]->set_text(stream.str());
    stream.str(std::string());
    stream << get_mode(piksi.status);
    lbl_top_stat->set_text(stream.str());

    stream.str(std::string());
    stream << "Sats: " << std::setw(3) << std::setprecision(0) << piksi.sats;
    lbl_stats[1]->set_text(stream.str());
    stream.str(std::string());
    stream << std::setw(3) << std::setprecision(0) << piksi.sats;
    lbl_top_sats->set_text(stream.str());

    stream.str(std::string());
    stream << std::setw(7) << std::setfill('0') <<
        std::setprecision(2) << std::fixed << piksi.utc;
    lbl_stats[2]->set_text(stream.str());

    vector4 ane1, ane2;
    rover.get_ane1(ane1);
    rover.get_ane2(ane2);

    vector3 ane1_uvw, ane2_uvw;
    ane1_uvw << ane1(0), ane1(1), ane1(2);
    ane2_uvw << ane2(0), ane2(1), ane2(2);

    update_vbox(lbl_ane1, ane1_uvw);
    update_vbox(lbl_ane2, ane2_uvw);

    base.get_imu(ypr, a, W);

    update_vbox(lbl_base_ypr, ypr);
    update_vbox(lbl_base_a, a);
    update_vbox(lbl_base_W, W);

    base.get_gps(piksi);
    llh << piksi.lat, piksi.lon, piksi.h;

    update_vbox(lbl_base_gps, llh);

    stream.str(std::string());
    stream << "Mode: " << get_mode(piksi.status);
    lbl_base_stats[0]->set_text(stream.str());

    stream.str(std::string());
    stream << "Sats: " << std::setw(3) << std::setprecision(0) << piksi.sats;
    lbl_base_stats[1]->set_text(stream.str());

    stream.str(std::string());
    stream << std::setw(7) << std::setfill('0') <<
        std::setprecision(2) << std::fixed << piksi.utc;
    lbl_base_stats[2]->set_text(stream.str());

    base.get_ane1(ane1);
    ane1_uvw << ane1(0), ane1(1), ane1(2);

    update_vbox(lbl_base_ane, ane1_uvw);

    return true;
}


bool fdcl::Gui::on_key_press(GdkEventKey* event) {
    switch (event->keyval) {
        case GDK_KEY_M:
        case GDK_KEY_m:
            break;
    }

    return false;
}


void fdcl::Gui::run(void) {
    std::cout << "GUI: drawing window" << std::endl;
    app->run(*window);
}


void fdcl::Gui::exit(void) {
    std::cout << "GUI: closing window" << std::endl;
    base.turn_off();

    if (is_init)
    {
        app->quit();
    }
}


void fdcl::Gui::on_btn_close_clicked(void) {
    exit();
}


void fdcl::Gui::update_freq_label(Gtk::Label *lbl, const std::string &freq_name, 
    const int data) {
    std::stringstream stream;
    stream << freq_name << ": "
        << std::setw(9) << std::fixed << std::setprecision(0) << data
        << " Hz";
    lbl->set_text(stream.str());
}


void fdcl::Gui::update_vbox(Gtk::Label *lbl[], const vector3 &data) {

    for (int i = 0; i < 3; i++) {
        std::stringstream stream;
        stream << std::setw(9) << std::fixed << std::setprecision(3) << data(i);
        lbl[i]->set_text(stream.str());
    }
}


void fdcl::Gui::update_grid(Gtk::Label *lbl[], const matrix3 &data) {
    int k = 0;
    for (int i = 1; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            k++;
            std::stringstream stream;
            stream << std::setw(9) << std::fixed << std::setprecision(3) 
                << data(i-1, j);
            lbl[k]->set_text(stream.str());
        }
    }
}


std::string fdcl::Gui::get_mode(const int mode) {
    switch(mode) {
        case 0:
            return "Invalid";
        case 1:
            return "SPP";
        case 2:
            return "DGPS";
        case 3:
            return "Float";
        case 4:
            return "Fixed";
        case 6:
            return "SBAS";
        default:
            return "Undefined";
    }
}