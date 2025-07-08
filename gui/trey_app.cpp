#include <gtk/gtk.h>
#include <libappindicator3-0.1/libappindicator/app-indicator.h>
#include "../include/db.hpp"
#include "../include/clipboard_mgmt.hpp"
#include <X11/Xlib.h>
#include <vector>
#include <string>

static void on_copy_clicked(GtkButton *button, gpointer user_data) {
    const char *text = (const char *)user_data;
    Display *display = XOpenDisplay(nullptr);
    if (display) {
        set_clipboard(display, std::string(text));
        XCloseDisplay(display);
    }
}

static void show_history_window(GtkWidget *, gpointer) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Clipboard History");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrolled), vbox);

    std::vector<std::string> history = load_clipboard_blocks();
    for (const auto &entry : history) {
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        GtkWidget *label = gtk_label_new(entry.substr(0, 60).c_str());
        gtk_label_set_xalign(GTK_LABEL(label), 0.0);
        GtkWidget *button = gtk_button_new_with_label("Copy");
        g_signal_connect(button, "clicked", G_CALLBACK(on_copy_clicked), (gpointer)entry.c_str());
        gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    }
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    AppIndicator *indicator = app_indicator_new(
        "clipboard-manager",
        "system-run-symbolic",
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS
    );
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);

    GtkWidget *menu = gtk_menu_new();
    GtkWidget *item_show = gtk_menu_item_new_with_label("Show Clipboard History");
    g_signal_connect(item_show, "activate", G_CALLBACK(show_history_window), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_show);

    GtkWidget *item_quit = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(item_quit, "activate", G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item_quit);

    gtk_widget_show_all(menu);
    app_indicator_set_menu(indicator, GTK_MENU(menu));

    gtk_main();
    return 0;
}

