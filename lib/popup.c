#include "../include/core.h"
#include "../include/graph.h"
#include "../include/popup.h"

static WINDOW* win_body;
static WINDOW* win_form;
static WINDOW* win_menu;
static bool is_on_button;
static MENU* popup_menu;
static ITEM** popup_items;

extern FORM* popup_form;
extern FIELD** popup_fields;
extern struct actions** button_actions;

void popup_new(int rows, int cols, int posy, int posx, int nb_buttons, char** requests, int nb_fields, char* title) {
	WINDOW *inner;
	int i, cury = 0, curx = 1, tmp;

	win_body = newwin(rows, cols, posy, posx);
	assert(win_body != NULL);
	box(win_body, 0, 0);

	mvwprintw(win_body, 0, cols/2 - strlen(title)/2, title);

	popup_items = malloc(sizeof(ITEM *) * (nb_buttons + 1));
	assert(popup_items);

	for (i = 0; i < nb_buttons; i++) {
		popup_items[i] = new_item(button_actions[i]->key, "");
		assert(popup_items[i] != NULL);
	}

	popup_items[i] = NULL;
	popup_menu = new_menu(popup_items);

	win_menu = derwin(win_body, 3, cols-2, rows-4, 1);
	assert(popup_menu != NULL && win_menu != NULL);
	box(win_menu, 0, 0);

	set_menu_win(popup_menu, win_menu);
	set_menu_format(popup_menu, 1, nb_buttons);
	tmp = popup_menu->fcols * (popup_menu->namelen + popup_menu->spc_rows);
	tmp--;

	inner = derwin(win_menu, 1, tmp, 1, (cols-3-tmp)/2);
	assert(inner != NULL);

	set_menu_sub(popup_menu, inner);
	set_menu_mark(popup_menu, "");

	popup_fields = malloc(sizeof(FIELD *) * (nb_fields+1));
	assert(popup_fields);

	for (i = 0; i < nb_fields; i++) {
		if (i % 2 == 1) {
			popup_fields[i] = new_field(1, cols - 6, cury, curx, 0, 0);
			assert(popup_fields[i] != NULL);
			set_field_buffer(popup_fields[i], 0, requests[i]);

			field_opts_on(popup_fields[i], O_ACTIVE);
			field_opts_on(popup_fields[i], O_EDIT);
			set_field_back(popup_fields[i], A_UNDERLINE);
		}
		else {
			popup_fields[i] = new_field(1, strlen(requests[i]), cury, curx, 0, 0);
                	assert(popup_fields[i] != NULL);
                	set_field_buffer(popup_fields[i], 0, requests[i]);

			field_opts_off(popup_fields[i], O_ACTIVE);
			field_opts_off(popup_fields[i], O_EDIT);
		}

		cury = cury + 2;
	}

	popup_fields[i] = NULL;
	popup_form = new_form(popup_fields);
	assert(popup_form != NULL);

	win_form = derwin(win_body, rows-5, cols-2, 1, 1);
	box(win_form, 0, 0);
	assert(popup_form != NULL && win_form != NULL);

	set_form_win(popup_form, win_form);
	inner = derwin(win_form, popup_form->rows+1, popup_form->cols+1, 1, 1);
	assert(inner != NULL);
	set_form_sub(popup_form, inner);

	assert(post_form(popup_form) == E_OK);
	assert(post_menu(popup_menu) == E_OK);
	is_on_button = true;
	pos_menu_cursor(popup_menu);
}

void popup_delete(void) {
	int i;

	unpost_form(popup_form);
	unpost_menu(popup_menu);

	for (i = 0; popup_fields[i] != NULL; i++) {
		free_field(popup_fields[i]);
	}

	for (i = 0; popup_items[i] != NULL; i++) {
		free_item(popup_items[i]);
	}

	free_menu(popup_menu);
	free_form(popup_form);
	delwin(win_form);
	delwin(win_menu);
	delwin(win_body);
	win_body = NULL;
	button_actions = NULL;
}

static void driver_buttons(ITEM *item, COPY_FILE_INFO* cfi) {
	const char *name = item_name(item);
	int i = 0;

	while (button_actions[i] && strcmp(button_actions[i]->key, name) != 0)
		i++;

	if (button_actions[i])
		button_actions[i]->func(cfi);
}

static void switch_to_buttons(void) {
	form_driver(popup_form, REQ_PREV_FIELD);
	form_driver(popup_form, REQ_NEXT_FIELD);

	menu_driver(popup_menu, REQ_FIRST_ITEM);
	is_on_button = true;
	set_menu_fore(popup_menu, A_REVERSE);
}

void popup_driver(COPY_FILE_INFO* cfi) {
	int ch = 0;

	while((ch = getch()) != KEY_F(1)) {
		switch (ch) {
			case KEY_DOWN:
				if (is_on_button)
					break;

				if (popup_form->current == popup_fields[popup_form->maxfield-1])
					switch_to_buttons();
				else
					form_driver(popup_form, REQ_NEXT_FIELD);
				break;

			case KEY_UP:
				if (is_on_button) {
					is_on_button = false;
					set_menu_fore(popup_menu, A_NORMAL);
				} else
					form_driver(popup_form, REQ_PREV_FIELD);
				break;

			case KEY_LEFT:
				if (is_on_button)
					menu_driver(popup_menu, REQ_LEFT_ITEM);
				else
					form_driver(popup_form, REQ_PREV_CHAR);
				break;

			case KEY_RIGHT:
				if (is_on_button)
					menu_driver(popup_menu, REQ_RIGHT_ITEM);
				else
					form_driver(popup_form, REQ_NEXT_CHAR);
				break;

			case KEY_ENTER:
				if (!is_on_button)
					switch_to_buttons();
				else {
					driver_buttons(current_item(popup_menu), cfi);
					return;
				}
				break;

			case KEY_BACKSPACE:
				form_driver(popup_form, REQ_DEL_PREV);

				break;

			default:
				if (!is_on_button)
					form_driver(popup_form, ch);

			break;
		}

	if (is_on_button)
		pos_menu_cursor(popup_menu);
	else
		pos_form_cursor(popup_form);

	wrefresh(win_body);

	}
}

void popup_refresh(void) {
	wrefresh(win_body);
}

bool popup_exists(void) {
	return (win_body != NULL);
}
