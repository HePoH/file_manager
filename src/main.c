#include "../include/core.h"
#include "../include/graph.h"
#include "../include/popup.h"

int main(){
	int ch = 0, ret = 0;
	pid_t pid = 0;
	pthread_t ctid = 0, sctid = 0;
	DIR_INFO *ld = NULL, *rd = NULL, *cd = NULL;

	init_core(&ld, &rd, &cd);
	init_graph();
	init_workspace(&ld, &rd);

	print_dir_static(cd);
	print_dir_static(rd);

	print_dir_dynamic(ld, 1);
	print_dir_dynamic(rd, 2);

	while((ch = getch()) != 'q') {
		switch(ch) {
			case KEY_UP:
				if (cd->current_file != cd->head_file) {
					cd->current_file = cd->current_file->prev;
					print_dir_dynamic(cd, 1);
				}
				break;

			case KEY_DOWN:
				if (cd->current_file->next != NULL) {
					cd->current_file = cd->current_file->next;
					print_dir_dynamic(cd, 1);
				}
				break;

			case KEY_LEFT:
				if (cd == rd) {
					print_dir_dynamic(cd, 2);
					cd = ld;

					if (chdir(cd->dir_path) == -1) {
						perror("chdir");
						return 0;
					}

					print_dir_dynamic(cd, 1);
				}
				break;

			case KEY_RIGHT:
				if (cd == ld) {
                                        print_dir_dynamic(cd, 2);
                                        cd = rd;

					if (chdir(cd->dir_path) == -1) {
						perror("chdir");
						return 0;
					}

                                        print_dir_dynamic(cd, 1);
                                }
                                break;

			case KEY_TAB:
				print_dir_dynamic(cd, 2);

				if (cd == ld)
					cd = rd;
				else
					cd = ld;

				if (chdir(cd->dir_path) == -1) {
					perror("chdir");
					return 0;
				}

				print_dir_dynamic(cd, 1);

				break;

			case KEY_HOME:
				cd->current_file = cd->head_file;
                                print_dir_dynamic(cd, 1);

                                break;

                        case KEY_END:
				cd->current_file = cd->tail_file;
                                print_dir_dynamic(cd, 1);

				break;

			case KEY_ENTER:
				if (!chdir(cd->current_file->file_name)) {
					free_fil(&cd);
					get_dir_info(".", &cd);

					print_dir_static(cd);
					print_dir_dynamic(cd, 1);
				}

				if (S_ISREG(cd->current_file->file_stat.st_mode))
					if (cd->current_file->file_stat.st_mode & S_IXUSR ||
					    cd->current_file->file_stat.st_mode & S_IXGRP ||
					    cd->current_file->file_stat.st_mode & S_IXOTH) {
						close_graph();

						pid = fork();
						if (pid == 0) {
							execl(cd->current_file->file_name, cd->current_file->file_name, (char *) 0);
						}

						wait(0);

						init_graph();
						init_workspace(&ld, &rd);

						print_dir_dynamic(ld, 1);
						print_dir_dynamic(rd, 2);
					}

				break;

			case KEY_F(5):
				if (S_ISREG(cd->current_file->file_stat.st_mode)) {
					char* file_path = NULL, cur_dir[BUF_SIZE];
					COPY_FILE_INFO* cfi = NULL;

					cfi = malloc(sizeof(COPY_FILE_INFO));
					if (cfi == NULL) {
						perror("malloc");
						exit(EXIT_FAILURE);
					}

					file_path = get_file_path(cd->current_file->file_name);
					getcwd(cur_dir, BUF_SIZE);

					strncpy(cfi->fn_src, file_path, BUF_SIZE - 1);
					strncpy(cfi->fn_dst, cur_dir, BUF_SIZE - 1);

					display_copy_form(cfi);
					curs_set(0);

					ret = pthread_create(&ctid, NULL, copy_file, (void*)cfi);
					if(ret) {
						perror("pthread_create");
						exit(EXIT_FAILURE);
					}

					usleep(1000);

					ret = pthread_create(&sctid, NULL, display_copy_status, (void*)cfi);
					if(ret) {
						perror("pthread_create");
						exit(EXIT_FAILURE);
					}

					pthread_join(ctid, NULL);
					pthread_join(sctid, NULL);

					wclear(ld->p_wnd);
					wclear(rd->p_wnd);

					box(ld->p_wnd, 0, 0);
					box(rd->p_wnd, 0, 0);

					wnoutrefresh(ld->p_wnd);
					wnoutrefresh(rd->p_wnd);

					doupdate();

					print_dir_static(ld);
					print_dir_static(rd);

					if (cd == ld) {
						print_dir_dynamic(ld, 1);
						print_dir_dynamic(rd, 2);
					}
					else {
						print_dir_dynamic(ld, 2);
						print_dir_dynamic(rd, 1);
					}

				}

				break;
		}
	}

	close_graph();

	free(ld);
	free(rd);
	cd = NULL;

	exit(EXIT_SUCCESS);
}
