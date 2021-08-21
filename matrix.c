/*
 *  Matrix rain code in terminal 
 *  Copyright (c) 2021 Leonardo Zamboni
 * 
 *  this program is free software: you can redistribute it and/or modify
 *  it under the terms of the gnu general public license as published by
 *  the free software foundation, either version 3 of the license, or
 *  (at your option) any later version.
 *  
 *  this program is distributed in the hope that it will be useful,
 *  but without any warranty; without even the implied warranty of
 *  merchantability or fitness for a particular purpose.  see the
 *  gnu general public license for more details.
 *  
 *  you should have received a copy of the gnu general public license
 *  along with this program.  if not, see <http://www.gnu.org/licenses/>.
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define USLEEP 0

#define GREEN "\e[0;92m"
#define WHITE "\e[0;97m"

typedef struct matrix Matrix;

struct matrix {
	uint16_t row;
	uint16_t col;
	uint8_t **v;
};

Matrix *create(uint16_t row, uint16_t col) {
	Matrix *m = (Matrix *) malloc(sizeof(Matrix));
	if (!m) exit(EXIT_FAILURE);
	m->row = row;
	m->col = col;
	m->v = (uint8_t **) malloc(row * sizeof(uint8_t *));
	for (size_t i = 0; i < row; ++i)
		m->v[i] = (uint8_t *) malloc(col * sizeof(uint8_t));
	return m;
}

void output(Matrix *m, char *str) {
	for (size_t i = 0; i < m->row; ++i) {
		for (size_t j = 0; j < m->col; ++j) {
			char *c = (i < (size_t)  m->row - 1 && m->v[i+1][j] == 0) ? WHITE : GREEN;
			printf("%s%c", c, (char) (m->v[i][j] == 0 ? ' ' : *str + rand() % strlen(str)));
			usleep(USLEEP);
		}
		puts("");
	}
}

uint8_t empty(Matrix *m) {
	for (size_t i = 0; i < m->row; ++i)
		for (size_t j = 0; j < m->col; ++j)
			if (m->v[i][j] != 0) return 1;
	return 0; 
}

void set(Matrix **m, uint16_t **head, uint16_t **end, uint16_t **size, size_t i) {
	uint16_t row = (*m)->row;
	uint16_t col = (*m)->col;
		
	for (size_t j = 0; j < col; ++j) {
		for (size_t y = 0; y < row; ++y) {
			if (y == (*head)[j]) break;
			if (y <= (*end)[j]) { (*m)->v[y][j] = 0; continue; }
			(*m)->v[y][j] = 1;
		}
		(*head)[j]++;
	}

	for (size_t j = 0; j < col; ++j) 
		if (i >= (*size)[j]) (*end)[j]++;
}

void init(Matrix **m, uint16_t **head, uint16_t **end, uint16_t **size) {
	uint16_t row = (*m)->row;
	uint16_t col = (*m)->col;

	*head = (uint16_t *) malloc(col * sizeof(uint16_t));
	*end = (uint16_t *) malloc(col * sizeof(uint16_t));
	*size = (uint16_t *) malloc(col * sizeof(uint16_t));

	for (size_t i = 0; i < col; ++i)
		(*head)[i] = rand() % 10, (*size)[i] = rand() % 10, (*end)[i] = 0;

	for (size_t i = 0; i < row; ++i)
		for (size_t j = 0; j < col; ++j)
			(*m)->v[i][j] = 0;
}

int main(void) {
	system("clear");
	srand(time(NULL));

	struct winsize w;
    	ioctl(0, TIOCGWINSZ, &w);

	char *str = "<>!@#$%^&*()>?/1234567890qwertyuiopasdfghjklzxcvbnm";		

	uint16_t row, col;
	row = w.ws_row-1, col = w.ws_col;
	
	Matrix *m = create(row, col);
	
	uint16_t *head, *end, *size;
	init(&m, &head, &end, &size);

	size_t i = 0;
	while (1) {
		set(&m, &head, &end, &size, i++);
		output(m, str);
		printf("%c[0;0f",0x1B);
		if (!empty(m)) { i = 0; init(&m, &head, &end, &size); }
	} 

	return 0;
}
