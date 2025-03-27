//
// Created by Kirill Tumoian on 26.03.2025.
//
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include "algorithm.h"  // Your ATM logic functions
#include <gtk/gtkentry.h>



// AppData structure to store account details and pointers to UI widgets
typedef struct {
    GtkWidget *stack;            // Stack to switch between screens
    // Screens
    GtkWidget *card_selection_screen;
    GtkWidget *pin_screen;
    GtkWidget *main_menu_screen;
    GtkWidget *change_pin_screen;
    // Card selection screen widgets
    GtkWidget *card1_button;
    GtkWidget *card2_button;
    // PIN screen widgets
    GtkWidget *pin_label;
    GtkWidget *pin_entry;
    GtkWidget *pin_submit;
    // Main menu widgets
    GtkWidget *balance_label;
    GtkWidget *withdraw_button;
    GtkWidget *deposit_button;
    GtkWidget *see_balance_button;
    GtkWidget *change_pin_button;
    GtkWidget *back_button;  // back to card selection
    GtkWidget *quit_button;
    GtkWidget *amount_entry; // for deposit/withdrawal
    // Change PIN screen widgets
    GtkWidget *new_pin_entry1;
    GtkWidget *new_pin_entry2;
    GtkWidget *submit_new_pin;
    GtkWidget *back_from_change;

    // Account data (for simplicity, we assume two hardcoded accounts)
    struct BankAccount account1;
    struct BankAccount account2;

    // Pointer to the currently active account
    struct BankAccount *active_account;
} AppData;

// Utility function to update the balance label text.
void update_balance_label(AppData *app_data) {
    char buf[64];
    snprintf(buf, sizeof(buf), "Balance: £%.2f", app_data->active_account->balance);
    gtk_label_set_text(GTK_LABEL(app_data->balance_label), buf);
}

// Switch screen helper
static void switch_screen(AppData *app_data, const char *screen_name) {
    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), screen_name);
}

// --- Callbacks ---
// Card selection callback: select a card and move to PIN entry
static void on_card_selected(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const gchar *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    // For this example, we assume Card 1 and Card 2 correspond to account1 and account2.
    if (g_strcmp0(button_label, "Card 1") == 0) {
        app_data->active_account = &app_data->account1;
    } else {
        app_data->active_account = &app_data->account2;
    }
    // Reset PIN entry and label
    gtk_editable_set_text(GTK_EDITABLE(app_data->pin_entry), "");
    gtk_label_set_text(GTK_LABEL(app_data->pin_label), "Enter PIN:");
    switch_screen(app_data, "pin");
}

// PIN submission callback
static void on_pin_submit(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const char *pin_text = gtk_editable_get_text(GTK_EDITABLE(app_data->pin_entry));
    int enteredPin = atoi(pin_text);

    // Check PIN using your algorithm function
    if (checkPin(app_data->active_account, enteredPin)) {
        // If correct, show main menu screen and update balance
        update_balance_label(app_data);
        switch_screen(app_data, "main_menu");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data->pin_label), "Incorrect PIN! Try again:");
    }
}

// See balance callback
static void on_see_balance(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    // Instead of setting a precomputed balance, start with an empty message.
    app_data->balance_label = gtk_label_new("Press 'See Balance' to view your balance");

    // Update the label and do nothing else
    update_balance_label(app_data);

}

// Deposit callback
static void on_deposit(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;
    const char *amount_str = gtk_editable_get_text(GTK_EDITABLE(app_data->amount_entry));
    double amount = atof(amount_str);
    g_print("Deposit requested: %f\n", amount);
    dep(app_data->active_account, amount);
    update_balance_label(app_data);
}


// Withdraw callback
static void on_withdraw(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const char *amount_str = gtk_editable_get_text(GTK_EDITABLE(app_data->amount_entry));
    double amount = atof(amount_str);
    const char *result = withdraw(app_data->active_account, amount);
    // Optionally, you can display result on the balance_label
    gtk_label_set_text(GTK_LABEL(app_data->balance_label), result);
    update_balance_label(app_data);
}

// Change PIN button: open change PIN screen
static void on_open_change_pin(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    // Clear previous entries
    gtk_editable_set_text(GTK_EDITABLE(app_data->pin_entry), "");
    gtk_editable_set_text(GTK_EDITABLE(app_data->pin_entry), "");
    switch_screen(app_data, "change_pin");
}

// Submit new PIN callback
static void on_submit_new_pin(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const char *new_pin1_str = gtk_editable_get_text(GTK_EDITABLE(app_data->new_pin_entry1));
    const char *new_pin2_str = gtk_editable_get_text(GTK_EDITABLE(app_data->new_pin_entry2));
    int new_pin1 = atoi(new_pin1_str);
    int new_pin2 = atoi(new_pin2_str);
    const char *result = changePin(app_data->active_account, new_pin1, new_pin2);
    gtk_label_set_text(GTK_LABEL(app_data->balance_label), result);
    // After changing the pin, go back to main menu
    switch_screen(app_data, "main_menu");
}

// Back button from change PIN screen: return to main menu
static void on_back_from_change(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    switch_screen(app_data, "main_menu");
}

// Back button from main menu: return to card selection screen
static void on_back_to_card_selection(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    switch_screen(app_data, "card_selection");
}

// Quit callback
static void on_quit(GtkWidget *widget, gpointer user_data) {
    g_application_quit(G_APPLICATION(user_data));
}

// --- UI Setup in the activate callback ---
static void activate(GtkApplication *app, gpointer user_data) {
    // Set environment variables to force Cairo rendering
    g_setenv("GSK_RENDERER", "cairo", TRUE);
    g_setenv("LIBGL_ALWAYS_SOFTWARE", "1", TRUE);
    g_setenv("GTK_A11Y", "none", TRUE);
    gtk_init();
    // Create main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ATM Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 800);

    // Create a stack for different screens
    AppData *app_data = g_new0(AppData, 1);
    app_data->stack = gtk_stack_new();
    gtk_widget_set_vexpand(app_data->stack, TRUE);
    gtk_widget_set_hexpand(app_data->stack, TRUE);

    // --- Card Selection Screen ---
    app_data->card_selection_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *card_label = gtk_label_new("Select your card:");
    gtk_box_append(GTK_BOX(app_data->card_selection_screen), card_label);
    app_data->card1_button = gtk_button_new_with_label("Card 1");
    app_data->card2_button = gtk_button_new_with_label("Card 2");
    gtk_box_append(GTK_BOX(app_data->card_selection_screen), app_data->card1_button);
    gtk_box_append(GTK_BOX(app_data->card_selection_screen), app_data->card2_button);
    g_signal_connect(app_data->card1_button, "clicked", G_CALLBACK(on_card_selected), app_data);
    g_signal_connect(app_data->card2_button, "clicked", G_CALLBACK(on_card_selected), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->card_selection_screen, "card_selection");

    g_signal_connect(app_data->deposit_button, "clicked", G_CALLBACK(on_deposit), app_data);
    g_signal_connect(app_data->withdraw_button, "clicked", G_CALLBACK(on_withdraw), app_data);


    // --- PIN Entry Screen ---
    app_data->pin_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    app_data->pin_label = gtk_label_new("Enter PIN:");
    app_data->pin_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(app_data->pin_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->pin_entry), "4-digit PIN");
    app_data->pin_submit = gtk_button_new_with_label("Submit PIN");
    gtk_box_append(GTK_BOX(app_data->pin_screen), app_data->pin_label);
    gtk_box_append(GTK_BOX(app_data->pin_screen), app_data->pin_entry);
    gtk_box_append(GTK_BOX(app_data->pin_screen), app_data->pin_submit);
    g_signal_connect(app_data->pin_submit, "clicked", G_CALLBACK(on_pin_submit), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->pin_screen, "pin");

    // --- Main Menu Screen ---
    app_data->main_menu_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    // Balance label
    app_data->balance_label = gtk_label_new("Balance: ");
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->balance_label);
    // Entry for deposit/withdrawal amount
    app_data->amount_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->amount_entry), "Enter amount");
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->amount_entry);
    // Menu buttons
    app_data->see_balance_button = gtk_button_new_with_label("See Balance");
    app_data->deposit_button = gtk_button_new_with_label("Deposit Money");
    app_data->withdraw_button = gtk_button_new_with_label("Withdraw Money");
    app_data->change_pin_button = gtk_button_new_with_label("Change PIN");
    app_data->back_button = gtk_button_new_with_label("Back to Card Selection");
    app_data->quit_button = gtk_button_new_with_label("Quit");
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->see_balance_button);
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->deposit_button);
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->withdraw_button);
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->change_pin_button);
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->back_button);
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), app_data->quit_button);
    g_signal_connect(app_data->see_balance_button, "clicked", G_CALLBACK(on_see_balance), app_data);
    g_signal_connect(app_data->deposit_button, "clicked", G_CALLBACK(on_deposit), app_data);
    g_signal_connect(app_data->withdraw_button, "clicked", G_CALLBACK(on_withdraw), app_data);
    g_signal_connect(app_data->change_pin_button, "clicked", G_CALLBACK(on_open_change_pin), app_data);
    g_signal_connect(app_data->back_button, "clicked", G_CALLBACK(on_back_to_card_selection), app_data);
    g_signal_connect(app_data->quit_button, "clicked", G_CALLBACK(on_quit), app);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->main_menu_screen, "main_menu");

    // --- Change PIN Screen ---
    app_data->change_pin_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *change_label = gtk_label_new("Enter new PIN:");
    app_data->new_pin_entry1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->new_pin_entry1), "New PIN");
    app_data->new_pin_entry2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->new_pin_entry2), "Confirm New PIN");
    app_data->submit_new_pin = gtk_button_new_with_label("Submit New PIN");
    app_data->back_from_change = gtk_button_new_with_label("Back to Menu");
    gtk_box_append(GTK_BOX(app_data->change_pin_screen), change_label);
    gtk_box_append(GTK_BOX(app_data->change_pin_screen), app_data->new_pin_entry1);
    gtk_box_append(GTK_BOX(app_data->change_pin_screen), app_data->new_pin_entry2);
    gtk_box_append(GTK_BOX(app_data->change_pin_screen), app_data->submit_new_pin);
    gtk_box_append(GTK_BOX(app_data->change_pin_screen), app_data->back_from_change);
    g_signal_connect(app_data->submit_new_pin, "clicked", G_CALLBACK(on_submit_new_pin), app_data);
    g_signal_connect(app_data->back_from_change, "clicked", G_CALLBACK(on_back_from_change), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->change_pin_screen, "change_pin");

    // Initialize hardcoded account values
    app_data->account1.accountNumber = 1;
    snprintf(app_data->account1.accountHolder, sizeof(app_data->account1.accountHolder), "Kirill Tumoian");
    app_data->account1.balance = 1234.60;
    app_data->account1.pinCode = 1234;
    app_data->account1.blocked = false;

    app_data->account2.accountNumber = 2;
    snprintf(app_data->account2.accountHolder, sizeof(app_data->account2.accountHolder), "Andrew Bradley");
    app_data->account2.balance = 848.50;
    app_data->account2.pinCode = 5678;
    app_data->account2.blocked = false;

    // Set the initial visible screen
    switch_screen(app_data, "card_selection");

    // Add the stack to the window and show the window
    gtk_window_set_child(GTK_WINDOW(window), app_data->stack);
    gtk_widget_show(window);
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.ATM", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
