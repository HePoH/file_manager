#include "../include/core.h"
#include "../include/graph.h"
#include "../include/popup.h"

FORM* popup_form;
FIELD** popup_fields;
struct actions** button_actions;

void init_graph() {
        if (initscr() == NULL) {
                perror("initscr");
                exit(EXIT_FAILURE);
        }

        noecho();
        curs_set(0);
        keypad(stdscr, true);
        cbreak();
        start_color();
        refresh();

	init_pair(1, COLOR_WHITE, COLOR_CYAN);
        init_pair(2, COLOR_WHITE, COLOR_BLUE);

	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_BLACK, COLOR_WHITE);

	init_pair(5, COLOR_BLACK, COLOR_WHITE);

}

void init_workspace(DIR_INFO** ld, DIR_INFO** rd){
	/*
		x - COLS
		y - LINES
	*/

	(*ld)->p_wnd = newwin(LINES, COLS/2, 0, 0);
        /*box((*ld)->p_wnd, '|', '-');*/
        box((*ld)->p_wnd, 0, 0);

	(*ld)->fn_wnd = derwin((*ld)->p_wnd, LINES-2, COLS/2 - 48, 1, 2);
	(*ld)->ow_wnd = derwin((*ld)->p_wnd, LINES-2, 13, 1, COLS/2 - 44);
	(*ld)->s_wnd = derwin((*ld)->p_wnd, LINES-2, 9, 1, COLS/2 - 31);
	(*ld)->mt_wnd = derwin((*ld)->p_wnd, LINES-2, 13, 1, COLS/2 - 20);
	(*ld)->pr_wnd = derwin((*ld)->p_wnd, LINES-2, 4, 1, COLS/2 - 5);

        (*rd)->p_wnd = newwin(LINES, COLS/2, 0, COLS/2);
        /*box((*rd)->p_wnd, '|', '-');*/
        box((*rd)->p_wnd, 0, 0);

	(*rd)->fn_wnd = derwin((*rd)->p_wnd, LINES-2, COLS/2 - 48, 1, 2);
	(*rd)->ow_wnd = derwin((*rd)->p_wnd, LINES-2, 13, 1, COLS/2 - 44);
	(*rd)->s_wnd = derwin((*rd)->p_wnd, LINES-2, 9, 1, COLS/2 - 31);
	(*rd)->mt_wnd = derwin((*rd)->p_wnd, LINES-2, 13, 1, COLS/2 - 20);
	(*rd)->pr_wnd = derwin((*rd)->p_wnd, LINES-2, 4, 1, COLS/2 - 5);

	wnoutrefresh((*ld)->p_wnd);
	wnoutrefresh((*rd)->p_wnd);

	doupdate();
}

void print_dir_static(DIR_INFO* di) {
	FILE_INFO_LIST* p = NULL;
	struct tm* timeinfo = NULL;
	char mt_buffer[BUF_SIZE];

	p = di->head_file;

	/*mvwhline(di->p_wnd, 0, 1, '-', cols/2 - 2);
	mvwhline(di->p_wnd, rows - 1, 1, '-', cols/2 - 2);*/

	mvwhline(di->p_wnd, 0, 1, 0, COLS/2 - 2);
	mvwhline(di->p_wnd, LINES - 1, 1, 0, COLS/2 - 2);

	/*wattron(di->p_wnd, A_BOLD);*/
	wattron(di->p_wnd, COLOR_PAIR(4));

	wmove(di->p_wnd, 0, COLS/4 - strlen(di->dir_path)/2 - 3);
	wprintw(di->p_wnd, "[ %s ]", di->dir_path);

	wmove(di->p_wnd, LINES - 1, ((COLS/2 - 46) / 2) - 6);
	wprintw(di->p_wnd, "[ Total: %d ]", di->file_count);

	wattroff(di->p_wnd, COLOR_PAIR(4));
	/*wattroff(di->p_wnd, A_BOLD);*/

	wclear(di->fn_wnd);
	wclear(di->ow_wnd);
	wclear(di->s_wnd);
	wclear(di->mt_wnd);
	wclear(di->pr_wnd);

    	wattron(di->fn_wnd, A_BOLD);
    	wattron(di->ow_wnd, A_BOLD);
    	wattron(di->s_wnd, A_BOLD);
    	wattron(di->mt_wnd, A_BOLD);
    	wattron(di->pr_wnd, A_BOLD);

	wattron(di->fn_wnd, COLOR_PAIR(3));
	wattron(di->ow_wnd, COLOR_PAIR(3));
	wattron(di->s_wnd, COLOR_PAIR(3));
	wattron(di->mt_wnd, COLOR_PAIR(3));
	wattron(di->pr_wnd, COLOR_PAIR(3));

	wprintw(di->fn_wnd, "File name\n\n");
	wprintw(di->ow_wnd, "Owner\n\n");
	wprintw(di->s_wnd, "Size\n\n");
	wprintw(di->mt_wnd, "Modify time\n\n");
	wprintw(di->pr_wnd, "Prm\n\n");

	while(p) {
		struct passwd* pwuser;
		timeinfo = localtime(&p->file_stat.st_mtim.tv_sec);
		strftime (mt_buffer, BUF_SIZE, "%b %e %R%n", timeinfo);

		pwuser = getpwuid(p->file_stat.st_uid);
		if (pwuser == NULL){
			perror("getpwuid()");
			exit(EXIT_FAILURE);
		}

		wprintw(di->ow_wnd, "%s\n", pwuser->pw_name);
		wprintw(di->s_wnd, "%ld\n", p->file_stat.st_size);
		wprintw(di->mt_wnd, "%s", mt_buffer);
		wprintw(di->pr_wnd, "%o\n", p->file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

		p = p->next;
        }

	wattroff(di->fn_wnd, COLOR_PAIR(3));
	wattroff(di->ow_wnd, COLOR_PAIR(3));
	wattroff(di->s_wnd, COLOR_PAIR(3));
	wattroff(di->mt_wnd, COLOR_PAIR(3));
	wattroff(di->pr_wnd, COLOR_PAIR(3));

	wattroff(di->fn_wnd, A_BOLD);
	wattroff(di->ow_wnd, A_BOLD);
	wattroff(di->s_wnd, A_BOLD);
	wattroff(di->mt_wnd, A_BOLD);
	wattroff(di->pr_wnd, A_BOLD);

	/*mvwvline(di->p_wnd, 1, cols/2 - 46, '|', rows-2);
	mvwvline(di->p_wnd, 1, cols/2 - 33, '|', rows-2);
	mvwvline(di->p_wnd, 1, cols/2 - 22, '|', rows-2);
	mvwvline(di->p_wnd, 1, cols/2 - 7, '|', rows-2);

	mvwhline(di->p_wnd, 2, 1, '-', cols/2 - 2);*/

	mvwvline(di->p_wnd, 1, COLS/2 - 46, 0, LINES-2);
	mvwvline(di->p_wnd, 1, COLS/2 - 33, 0, LINES-2);
	mvwvline(di->p_wnd, 1, COLS/2 - 22, 0, LINES-2);
	mvwvline(di->p_wnd, 1, COLS/2 - 7, 0, LINES-2);

	mvwhline(di->p_wnd, 2, 1, 0, COLS/2 - 2);

	wnoutrefresh(di->p_wnd);
	wnoutrefresh(di->fn_wnd);
	wnoutrefresh(di->ow_wnd);
	wnoutrefresh(di->s_wnd);
	wnoutrefresh(di->mt_wnd);
	wnoutrefresh(di->pr_wnd);

	doupdate();
}

void print_dir_dynamic(DIR_INFO* di, int bg_c) {
	FILE_INFO_LIST* p = NULL;
	p = di->head_file;

	wmove(di->fn_wnd, 2, 0);
    	wattron(di->fn_wnd, A_BOLD);

	while(p) {
		if (p == di->current_file) {
			wattron(di->fn_wnd, COLOR_PAIR(bg_c));
			wprintw(di->fn_wnd, "%s\n", p->file_name);
			wattroff(di->fn_wnd, COLOR_PAIR(bg_c));
		}
		else {
			wattron(di->fn_wnd, COLOR_PAIR(3));
			wprintw(di->fn_wnd, "%s\n", p->file_name);
			wattroff(di->fn_wnd, COLOR_PAIR(3));
		}

		p = p->next;
        }

	wattroff(di->fn_wnd, A_BOLD);
	wrefresh(di->fn_wnd);
}

void copy_action_btn_ok(void) {
        int i;

        mvprintw(LINES-2, 1, "[*] OK: ");

        for (i = 0; i < popup_form->maxfield; i++) {
                printw("%s", trim(field_buffer(popup_fields[i], 0)));

                if (field_opts(popup_fields[i]) & O_ACTIVE)
                        printw("\t");
        }

        refresh();
}

void copy_action_btn_quit(void) {
        mvprintw(LINES-2, 1, "[*] QUIT: F1 to quit");
        refresh();
}


void display_copy_form() {
	char* requests[] = { "From:", "/home/2017/d_medvedev/work/file_manager/file.bin", "To:", "/home/2017/d_medvedev/work/file_manager.bin" };
	int ch;
	struct actions act_ok, act_quit;

	act_ok.key = "OK";
	act_ok.func = copy_action_btn_ok;
	act_quit.key = "QUIT";
	act_quit.func = copy_action_btn_quit;

	button_actions = malloc(sizeof(struct action *) * 3);
	button_actions[0] = &act_ok;
	button_actions[1] = &act_quit;
	button_actions[2] = NULL;

	popup_new(15, COLS/2, (LINES-13) / 2, COLS/4, 2, requests, 4, "[ Copying ]");

	refresh();
	popup_refresh();

	while((ch = getch()) != KEY_F(1))
		popup_driver(ch);

	/*popup_delete();

	free(button_actions[0]);
	free(button_actions[1]);
	free(button_actions);*/
}

void close_graph(){
        echo();
        endwin();
}
