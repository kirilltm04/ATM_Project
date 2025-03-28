//
// Created by Kirill Tumoian on 28.03.2025.
//
//
// Created by Kirill Tumoian on 26.03.2025.
//
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "algorithm.h"  // Your ATM functions: checkPin, dep, withdraw, changePin

// Structure to hold account data and pointers to UI widgets.
typedef struct {
    GtkWidget *main_window;      // Pointer to the main application window
    GtkWidget *stack;            // Stack to switch between screens
    // Init screens
    GtkWidget *card_selection_screen;
    GtkWidget *pin_screen;
    GtkWidget *main_menu_screen;
    GtkWidget *change_pin_screen;
    GtkWidget *receipt_menu_screen;
    GtkWidget *error_screen;

    // Card selection screen widgets
    GtkWidget *card1_button;
    GtkWidget *card2_button;

    // PIN screen widgets
    GtkWidget *pin_label;
    GtkWidget *pin_entry;
    GtkWidget *pin_submit;

    // Main menu widgets
    GtkWidget *balance_label;    // Shows balance (only when "See Balance" is pressed)
    GtkWidget *amount_entry;     // For deposit/withdrawal amount
    GtkWidget *confirm_deposit_button;
    GtkWidget *confirm_withdraw_button;
    GtkWidget *see_balance_button;
    GtkWidget *deposit_button;
    GtkWidget *withdraw_button;
    GtkWidget *change_pin_button;
    GtkWidget *back_button;      // Back to card selection
    GtkWidget *quit_button;

    GtkWidget *withdraw_entry;


    // Change PIN screen widgets
    GtkWidget *new_pin_entry1;
    GtkWidget *new_pin_entry2;
    GtkWidget *submit_new_pin;
    GtkWidget *back_from_change;

    // Receipt menu widgets
    GtkWidget *receipt_label;      // (optional) display receipt info
    GtkWidget *receipt_yes_button;
    GtkWidget *receipt_no_button;

    // Error screen widgets
    GtkWidget *error_label;
    GtkWidget *error_back_button;

    // Two hardcoded accounts.
    struct BankAccount account1;
    struct BankAccount account2;

    // Pointer to the currently active account.
    struct BankAccount *active_account;

    // Fields to store the original balance before a transaction,
    // and the type of transaction ("Deposit" or "Withdrawal").
    double original_balance;
    char transaction_type[50];

    // Field to track the number of incorrect PIN attempts.
    int pin_attempts;
} AppData;

// Utility: update balance label to display the actual balance.
void update_balance_label(AppData *app_data) {
    char buf[64];
    snprintf(buf, sizeof(buf), "Balance: £%.2f", app_data->active_account->balance);
    gtk_label_set_text(GTK_LABEL(app_data->balance_label), buf);
}

// Utility: clear (hide) the balance display.
void clear_balance_label(AppData *app_data) {
    gtk_label_set_text(GTK_LABEL(app_data->balance_label), "");
}

// Helper to switch the visible screen in the stack.
static void switch_screen(AppData *app_data, const char *screen_name) {
    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), screen_name);
}

// --- Callbacks ---
// Card selection: set active account based on which card is chosen.
// If the card is blocked, display a modal error dialog.
static void on_card_selected(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const gchar *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    if (g_strcmp0(button_label, "Card 1") == 0)
        app_data->active_account = &app_data->account1;
    else
        app_data->active_account = &app_data->account2;

    // If the selected card is blocked, show an error dialog.
    if (app_data->active_account->blocked) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app_data->main_window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "This card was retained. Please call the bank.");
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(app_data->main_window));
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
        gtk_window_present(GTK_WINDOW(dialog));
        return;  // Do not proceed to PIN screen.
    }
    // Reset incorrect PIN attempt counter.
    app_data->pin_attempts = 0;
    gtk_editable_set_text(GTK_EDITABLE(app_data->pin_entry), "");
    gtk_label_set_text(GTK_LABEL(app_data->pin_label), "Enter PIN:");
    switch_screen(app_data, "pin");
}

// PIN submission: check entered PIN and go to main menu if correct.
// If incorrect, increment attempt count and block card after 3 attempts.
static void on_pin_submit(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const char *pin_text = gtk_editable_get_text(GTK_EDITABLE(app_data->pin_entry));
    int enteredPin = atoi(pin_text);

    if (checkPin(app_data->active_account, enteredPin)) {
        gtk_label_set_text(GTK_LABEL(app_data->balance_label), "Press 'See Balance' to view your balance");
        switch_screen(app_data, "main_menu");
    } else {
        app_data->pin_attempts++;
        if (app_data->pin_attempts >= 3) {
            app_data->active_account->blocked = true;
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app_data->main_window),
                                                       GTK_DIALOG_MODAL,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Card has been retained due to too many incorrect attempts. Please call the bank.");
            gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(app_data->main_window));
            g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
            gtk_window_present(GTK_WINDOW(dialog));
            switch_screen(app_data, "card_selection");
        } else {
            gtk_label_set_text(GTK_LABEL(app_data->pin_label), "Incorrect PIN! Try again:");
        }
    }
}

static void on_back_from_change(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    switch_screen(app_data, "main_menu");
}

static void on_submit_new_pin(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const char *new_pin1_str = gtk_editable_get_text(GTK_EDITABLE(app_data->new_pin_entry1));
    const char *new_pin2_str = gtk_editable_get_text(GTK_EDITABLE(app_data->new_pin_entry2));
    int new_pin1 = atoi(new_pin1_str);
    int new_pin2 = atoi(new_pin2_str);
    const char *result = changePin(app_data->active_account, new_pin1, new_pin2);

    /* Clear all children from the change PIN screen */
    GtkWidget *child = gtk_widget_get_first_child(app_data->change_pin_screen);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    /* Prepare a result message. If successful, include the new PIN. */
    char message[128];
    if (strstr(result, "successfully") != NULL) {
        snprintf(message, sizeof(message), "%s New PIN: %d", result, new_pin1);
    } else {
        snprintf(message, sizeof(message), "%s", result);
    }

    /* Create a label to show the result message and a "Back to Menu" button. */
    GtkWidget *result_label = gtk_label_new(message);
    GtkWidget *back_button = gtk_button_new_with_label("Back to Menu");
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_from_change), app_data);

    /* Add the new widgets to the change PIN screen container. */
    gtk_box_append(GTK_BOX(app_data->change_pin_screen), result_label);
    gtk_box_append(GTK_BOX(app_data->change_pin_screen), back_button);

    /* Show the updated change PIN screen */
    gtk_widget_show(app_data->change_pin_screen);
}

static void on_open_change_pin(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;

    // Clear existing children from change_pin_screen
    GtkWidget *child = gtk_widget_get_first_child(app_data->change_pin_screen);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }

    // Rebuild the original Change PIN UI
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

    // Connect the signals again
    g_signal_connect(app_data->submit_new_pin, "clicked", G_CALLBACK(on_submit_new_pin), app_data);
    g_signal_connect(app_data->back_from_change, "clicked", G_CALLBACK(on_back_from_change), app_data);

    clear_balance_label(app_data);
    switch_screen(app_data, "change_pin");
}



// Back from main menu to card selection.
static void on_back_to_card_selection(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    switch_screen(app_data, "card_selection");
}

// Quit callback.
static void on_quit(GtkWidget *widget, gpointer user_data) {
    g_application_quit(G_APPLICATION(user_data));
}

// Receipt menu: "Yes" button callback.
// This creates a separate window showing the receipt.
static void on_receipt_yes(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    char receipt_text[512];
    snprintf(receipt_text, sizeof(receipt_text),
             "---- Transaction Receipt ----\n"
             "Transaction: %s\n"
             "Original Balance: £%.2f\n"
             "New Balance: £%.2f\n"
             "Thank you, %s\n"
             "------------------------------",
             app_data->transaction_type,
             app_data->original_balance,
             app_data->active_account->balance,
             app_data->active_account->accountHolder);

    GtkWidget *receipt_window = gtk_application_window_new(
            GTK_APPLICATION(gtk_window_get_application(GTK_WINDOW(app_data->main_window))));
    gtk_window_set_title(GTK_WINDOW(receipt_window), "Transaction Receipt");
    gtk_window_set_default_size(GTK_WINDOW(receipt_window), 300, 200);

    GtkWidget *receipt_label = gtk_label_new(receipt_text);
    gtk_window_set_child(GTK_WINDOW(receipt_window), receipt_label);
    gtk_window_present(GTK_WINDOW(receipt_window));

    switch_screen(app_data, "main_menu");
}

// Receipt menu: "No" button callback.
static void on_receipt_no(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    switch_screen(app_data, "main_menu");
}

// Error screen: back button callback.
static void on_error_back(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    switch_screen(app_data, "main_menu");
}

// --- New Callback Functions for Navigation ---
static void on_deposit_button(GtkWidget *widget, gpointer data) {
    AppData *a = (AppData *)data;
    gtk_editable_set_text(GTK_EDITABLE(a->amount_entry), "");
    switch_screen(a, "deposit");
}

static void on_withdraw_button(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    gtk_editable_set_text(GTK_EDITABLE(app_data->withdraw_entry), "");
    switch_screen(app_data, "withdraw");
}


static void on_deposit_back(GtkWidget *widget, gpointer data) {
    AppData *a = (AppData *)data;
    switch_screen(a, "main_menu");
}

static void on_withdraw_back(GtkWidget *widget, gpointer data) {
    AppData *a = (AppData *)data;
    switch_screen(a, "main_menu");
}

static void on_balance_back(GtkWidget *widget, gpointer data) {
    AppData *a = (AppData *)data;
    switch_screen(a, "main_menu");
}

static void on_deposit_confirm(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const char *amount_str = gtk_editable_get_text(GTK_EDITABLE(app_data->amount_entry));
    double amount = atof(amount_str);
    app_data->original_balance = app_data->active_account->balance;
    strcpy(app_data->transaction_type, "Deposit");
    const char *result = deposit(app_data->active_account, amount);
    if (strstr(result, "successful") != NULL) {
        switch_screen(app_data, "receipt_menu");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data->error_label), result);
        switch_screen(app_data, "error");
    }
}

static void on_withdraw_confirm(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    const char *amount_str = gtk_editable_get_text(GTK_EDITABLE(app_data->withdraw_entry));
    double amount = atof(amount_str);
    app_data->original_balance = app_data->active_account->balance;
    strcpy(app_data->transaction_type, "Withdrawal");
    const char *result = withdraw(app_data->active_account, amount);
    if (strstr(result, "successful") != NULL) {
        switch_screen(app_data, "receipt_menu");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data->error_label), result);
        switch_screen(app_data, "error");
    }
}

static void on_show_balance_full(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    char buf[128];
    snprintf(buf, sizeof(buf), "Your Balance:\n£%.2f", app_data->active_account->balance);
    gtk_label_set_text(GTK_LABEL(app_data->balance_label), buf);
    switch_screen(app_data, "balance_full");
}

// --- The activate() Callback with Updated Layout and Callbacks ---
static void activate(GtkApplication *app, gpointer user_data) {
    g_setenv("GSK_RENDERER", "cairo", TRUE);
    g_setenv("LIBGL_ALWAYS_SOFTWARE", "1", TRUE);
    g_setenv("GTK_A11Y", "none", TRUE);
    gtk_init();

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ATM Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 800);

    // Load CSS using gtk_css_provider_load_from_string (GTK4)
    GtkCssProvider *provider = gtk_css_provider_new();
    const gchar *css =
            "window { "
            "   background-color: #E6E6E6; "  // Light grey background for a formal look.
            "   padding: 20px; "
            "} "
            "button { "
            "   background-color: #004080; "  // A formal, deep navy blue.
            "   color: white; "
            "   padding: 10px 20px; "
            "   border-radius: 8px; "
            "   font-size: 18px; "
            "   min-width: 150px; "
            "   min-height: 50px; "
            "   border: none; "
            "   box-shadow: 2px 2px 5px rgba(0, 0, 0, 0.2); "
            "} "
            "button:hover { "
            "   background-color: #003060; "  // Slightly darker on hover.
            "} "
            "label { "
            "   color: #333333; "
            "   font-size: 20px; "
            "   font-family: 'Helvetica', sans-serif; "
            "} "
            "entry { "
            "   padding: 5px; "
            "   border: 1px solid #CCCCCC; "
            "   border-radius: 5px; "
            "   font-size: 18px; "
            "   background-color: #FFFFFF; "
            "   color: #333333; "
            "} ";

    gtk_css_provider_load_from_string(provider, css);
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    AppData *app_data = g_new0(AppData, 1);
    app_data->main_window = window;
    app_data->stack = gtk_stack_new();
    gtk_widget_set_vexpand(app_data->stack, TRUE);
    gtk_widget_set_hexpand(app_data->stack, TRUE);


    /* ------------------ Card Selection Screen ------------------ */
    app_data->card_selection_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(app_data->card_selection_screen, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(app_data->card_selection_screen, GTK_ALIGN_CENTER);
    GtkWidget *card_label = gtk_label_new("Select your card:");
    gtk_box_append(GTK_BOX(app_data->card_selection_screen), card_label);
    app_data->card1_button = gtk_button_new_with_label("Card 1");
    app_data->card2_button = gtk_button_new_with_label("Card 2");
    gtk_box_append(GTK_BOX(app_data->card_selection_screen), app_data->card1_button);
    gtk_box_append(GTK_BOX(app_data->card_selection_screen), app_data->card2_button);
    g_signal_connect(app_data->card1_button, "clicked", G_CALLBACK(on_card_selected), app_data);
    g_signal_connect(app_data->card2_button, "clicked", G_CALLBACK(on_card_selected), app_data);
    // Add Quit button here.
    GtkWidget *start_quit_button = gtk_button_new_with_label("Quit");
    gtk_box_append(GTK_BOX(app_data->card_selection_screen), start_quit_button);
    g_signal_connect(start_quit_button, "clicked", G_CALLBACK(on_quit), app);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->card_selection_screen, "card_selection");


    /* ------------------ PIN Entry Screen ------------------ */
    app_data->pin_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(app_data->pin_screen, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(app_data->pin_screen, GTK_ALIGN_CENTER);
    app_data->pin_label = gtk_label_new("Enter PIN:");
    app_data->pin_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(app_data->pin_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->pin_entry), "4-digit PIN");
    g_signal_connect(app_data->pin_entry, "activate", G_CALLBACK(on_pin_submit), app_data);
    app_data->pin_submit = gtk_button_new_with_label("Submit PIN");
    gtk_box_append(GTK_BOX(app_data->pin_screen), app_data->pin_label);
    gtk_box_append(GTK_BOX(app_data->pin_screen), app_data->pin_entry);
    gtk_box_append(GTK_BOX(app_data->pin_screen), app_data->pin_submit);
    g_signal_connect(app_data->pin_submit, "clicked", G_CALLBACK(on_pin_submit), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->pin_screen, "pin");

    /* ------------------ Main Menu Screen ------------------ */
    app_data->main_menu_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
    gtk_widget_set_halign(app_data->main_menu_screen, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(app_data->main_menu_screen, GTK_ALIGN_CENTER);

    GtkWidget *menu_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(menu_grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(menu_grid), 20);
    gtk_widget_set_halign(menu_grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(menu_grid, GTK_ALIGN_CENTER);



    // Main menu title.
    GtkWidget *menu_title = gtk_label_new("ATM Main Menu");
    gtk_grid_attach(GTK_GRID(menu_grid), menu_title, 0, 0, 2, 1);

    // Buttons arranged in grid.
    app_data->see_balance_button = gtk_button_new_with_label("Show Balance");
    app_data->deposit_button = gtk_button_new_with_label("Deposit Money");
    app_data->withdraw_button = gtk_button_new_with_label("Withdraw Money");
    app_data->change_pin_button = gtk_button_new_with_label("Change PIN");
    app_data->back_button = gtk_button_new_with_label("Back to Card Selection");
    app_data->quit_button = gtk_button_new_with_label("Quit");

    gtk_grid_attach(GTK_GRID(menu_grid), app_data->see_balance_button, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(menu_grid), app_data->deposit_button, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(menu_grid), app_data->withdraw_button, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(menu_grid), app_data->change_pin_button, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(menu_grid), app_data->back_button, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(menu_grid), app_data->quit_button, 1, 3, 1, 1);

    g_signal_connect(app_data->see_balance_button, "clicked", G_CALLBACK(on_show_balance_full), app_data);
    g_signal_connect(app_data->deposit_button, "clicked", G_CALLBACK(on_deposit_button), app_data);
    g_signal_connect(app_data->withdraw_button, "clicked", G_CALLBACK(on_withdraw_button), app_data);
    g_signal_connect(app_data->change_pin_button, "clicked", G_CALLBACK(on_open_change_pin), app_data);
    g_signal_connect(app_data->back_button, "clicked", G_CALLBACK(on_back_to_card_selection), app_data);
    g_signal_connect(app_data->quit_button, "clicked", G_CALLBACK(on_quit), app);
    gtk_box_append(GTK_BOX(app_data->main_menu_screen), menu_grid);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->main_menu_screen, "main_menu");



    /* ------------------ Deposit Screen ------------------ */
    GtkWidget *deposit_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);

    gtk_widget_set_halign(deposit_screen, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(deposit_screen, GTK_ALIGN_CENTER);
    GtkWidget *deposit_label = gtk_label_new("Enter Deposit Amount:");
    // Re-use the same amount_entry widget.
    app_data->amount_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->amount_entry), "Amount to deposit");
    g_signal_connect(app_data->amount_entry, "activate", G_CALLBACK(on_deposit_confirm), app_data);
    GtkWidget *confirm_deposit_button = gtk_button_new_with_label("Confirm Deposit");
    GtkWidget *deposit_back = gtk_button_new_with_label("Back");
    gtk_box_append(GTK_BOX(deposit_screen), deposit_label);
    gtk_box_append(GTK_BOX(deposit_screen), app_data->amount_entry);
    gtk_box_append(GTK_BOX(deposit_screen), confirm_deposit_button);
    gtk_box_append(GTK_BOX(deposit_screen), deposit_back);
    g_signal_connect(confirm_deposit_button, "clicked", G_CALLBACK(on_deposit_confirm), app_data);
    g_signal_connect(deposit_back, "clicked", G_CALLBACK(on_deposit_back), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), deposit_screen, "deposit");

    /* ------------------ Withdraw Screen ------------------ */
/* ------------------ Withdraw Screen ------------------ */
    GtkWidget *withdraw_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(withdraw_screen, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(withdraw_screen, GTK_ALIGN_CENTER);
    GtkWidget *withdraw_label = gtk_label_new("Enter Withdrawal Amount:");
// Create a new entry widget for withdrawal.
    app_data->withdraw_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->withdraw_entry), "Amount to withdraw");
    g_signal_connect(app_data->withdraw_entry, "activate", G_CALLBACK(on_withdraw_confirm), app_data);
    GtkWidget *confirm_withdraw_button = gtk_button_new_with_label("Confirm Withdraw");
    GtkWidget *withdraw_back = gtk_button_new_with_label("Back");
    gtk_box_append(GTK_BOX(withdraw_screen), withdraw_label);
    gtk_box_append(GTK_BOX(withdraw_screen), app_data->withdraw_entry);
    gtk_box_append(GTK_BOX(withdraw_screen), confirm_withdraw_button);
    gtk_box_append(GTK_BOX(withdraw_screen), withdraw_back);
    g_signal_connect(confirm_withdraw_button, "clicked", G_CALLBACK(on_withdraw_confirm), app_data);
    g_signal_connect(withdraw_back, "clicked", G_CALLBACK(on_withdraw_back), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), withdraw_screen, "withdraw");


    /* ------------------ Full Screen Balance ------------------ */
    GtkWidget *balance_full_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(balance_full_screen, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(balance_full_screen, GTK_ALIGN_CENTER);
    app_data->balance_label = gtk_label_new("");
    gtk_box_append(GTK_BOX(balance_full_screen), app_data->balance_label);
    GtkWidget *balance_back = gtk_button_new_with_label("Back");
    gtk_box_append(GTK_BOX(balance_full_screen), balance_back);
    g_signal_connect(balance_back, "clicked", G_CALLBACK(on_balance_back), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), balance_full_screen, "balance_full");

    /* ------------------ Change PIN, Receipt, and Error Screens ------------------ */
    app_data->change_pin_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
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

    app_data->receipt_menu_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    app_data->receipt_label = gtk_label_new("Print receipt?");
    app_data->receipt_yes_button = gtk_button_new_with_label("Yes");
    app_data->receipt_no_button = gtk_button_new_with_label("No");
    gtk_box_append(GTK_BOX(app_data->receipt_menu_screen), app_data->receipt_label);
    gtk_box_append(GTK_BOX(app_data->receipt_menu_screen), app_data->receipt_yes_button);
    gtk_box_append(GTK_BOX(app_data->receipt_menu_screen), app_data->receipt_no_button);
    g_signal_connect(app_data->receipt_yes_button, "clicked", G_CALLBACK(on_receipt_yes), app_data);
    g_signal_connect(app_data->receipt_no_button, "clicked", G_CALLBACK(on_receipt_no), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->receipt_menu_screen, "receipt_menu");



    app_data->error_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    app_data->error_label = gtk_label_new("");
    app_data->error_back_button = gtk_button_new_with_label("Back to Menu");
    gtk_box_append(GTK_BOX(app_data->error_screen), app_data->error_label);
    gtk_box_append(GTK_BOX(app_data->error_screen), app_data->error_back_button);
    g_signal_connect(app_data->error_back_button, "clicked", G_CALLBACK(on_error_back), app_data);
    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->error_screen, "error");



    /* ------------------ Initialize Hardcoded Accounts ------------------ */
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

    switch_screen(app_data, "card_selection");
    gtk_window_set_child(GTK_WINDOW(window), app_data->stack);
    gtk_window_present(GTK_WINDOW(window));
}


int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.ATM", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
