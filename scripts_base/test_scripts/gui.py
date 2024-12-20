import threading
import time

from gi.repository import GLib, Gtk, GObject
from data import data


def app_main():
    builder = Gtk.Builder()
    builder.add_from_file("../gui.glade")

    window = builder.get_object('window_main')
    window.set_title('Real-Time Data Monitoring')
    window.connect('destroy', Gtk.main_quit)
    window.show_all()


    def update_progess(i):
        # progress.pulse()
        # progress.set_text(str(i))
        return False

    def example_target():
        for i in range(50):
            GLib.idle_add(update_progess, i)
            time.sleep(0.2)

    thread = threading.Thread(target=example_target)
    thread.daemon = True
    thread.start()


if __name__ == "__main__":
    app_main()
    Gtk.main()

