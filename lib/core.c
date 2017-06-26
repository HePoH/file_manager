#include "../include/core.h"

void init_core(DIR_INFO** ld, DIR_INFO** rd, DIR_INFO** cd) {
	*ld = (DIR_INFO*) malloc(sizeof(DIR_INFO));
	*rd = (DIR_INFO*) malloc(sizeof(DIR_INFO));

	if (chdir(getenv("PWD")) == -1) {
		perror("chdir");
		return;
	}

	get_dir_info(getenv("PWD"), ld);

	if (chdir(getenv("HOME")) == -1) {
		perror("chdir");
		return;
	}

	get_dir_info(getenv("HOME"), rd);

	*cd = *ld;
}

void free_fil(DIR_INFO** di) {
	FILE_INFO_LIST *p = NULL, *q = NULL;
        p = (*di)->head_file;

        while(p) {
		q = p;
                p = p->next;
		free(q);
        }

	(*di)->head_file = NULL;
	(*di)->current_file = NULL;
	(*di)->tail_file = NULL;
}

void get_dir_info(char* path, DIR_INFO** di) {
	struct dirent **fnl;
	int fc;

	fc = scandir(path, &fnl, 0, alphasort);

	if (fc < 0)
		perror("scandir");
	else {
		int i = 1;

		if (getcwd((*di)->path, BUF_SIZE) == NULL)
			strncpy((*di)->path, "-", BUF_SIZE - 1);

		(*di)->file_count = fc - 2;

		if (fc > 1) {
			FILE_INFO_LIST* head = NULL;

			head = malloc(sizeof(FILE_INFO_LIST));
			if (head == NULL) {
				perror("malloc");
				return;
			}

			strncpy(head->value.name, fnl[i]->d_name, BUF_SIZE - 1);
			strncpy(head->value.type, "-", BUF_SIZE - 1);

			head->value.size = 0;
			head->value.mod_date = 0;

			head->next = NULL;
			head->prev = NULL;

			(*di)->head_file = head;
			(*di)->current_file = head;

			i++;
		}

		for (; i < fc; i++) {
			FILE_INFO_LIST* fil = NULL;

			fil = (FILE_INFO_LIST*) malloc(sizeof(FILE_INFO_LIST));
			if (fil == NULL) {
				perror("malloc");
				return;
                        }

			strncpy(fil->value.name, fnl[i]->d_name, BUF_SIZE - 1);
			strncpy(fil->value.type, "-", BUF_SIZE - 1);

			fil->value.size = 0;
			fil->value.mod_date = 0;

			fil->next = NULL;
			fil->prev = (*di)->current_file;

			(*di)->current_file->next = fil;
			(*di)->current_file = fil;

			free(fnl[i]);
		}

		(*di)->tail_file = (*di)->current_file;
		(*di)->current_file = (*di)->head_file;

		free(fnl);
	}
}
