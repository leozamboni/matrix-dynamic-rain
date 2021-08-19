/*
 * Matrix rain code in terminal 
 * Copyright © 2021 Leonardo Zamboni
 *
 * this program is free software: you can redistribute it and/or modify
 * it under the terms of the gnu general public license as published by
 * the free software foundation, either version 3 of the license, or
 * (at your option) any later version.
 * 
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * gnu general public license for more details.
 * 
 * you should have received a copy of the gnu general public license
 * along with this program.  if not, see <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define ROW 30
#define COL 60

#define USLEEP 100

#define GREEN "\e[0;92m"
#define WHITE "\e[0;97m"

typedef struct matrix Matrix;

struct matrix {
	uint16_t row;
	uint16_t col;
	uint8_t **v;
	Matrix *n;
};

typedef struct queue Queue;

struct queue {
	Matrix *f;
	Matrix *r;
};

Matrix *create(uint16_t row, uint16_t col);
uint8_t pull(Matrix *m, uint16_t i, uint16_t j);
void push(Matrix *m, uint16_t i, uint16_t j, uint8_t v);

Queue *create_queue(void) {
	Queue *q = (Queue *) malloc(sizeof(Queue));
	q->f = q->r = NULL;
	return q;
}

void push_queue(Queue *q, Matrix *m) {
	uint16_t row = m->row;
	uint16_t col = m->col;

	Matrix *matrix = create(row, col);
			
	for (size_t i = 0; i < row; ++i)
		for (size_t j = 0; j < col; ++j)
			push(matrix, i, j, pull(m, i, j));

	matrix->n = NULL;
	if (!q->f) q->f = matrix;
	else q->r->n = matrix;
	q->r = matrix;
}

void output_queue(Matrix *m, char *str) {
	if (!m) return;

	uint16_t row = m->row;
	uint16_t col = m->col;

	for (size_t i = 0; i < row; ++i) {
		for (size_t j = 0; j < col; ++j) {
			char *c = GREEN;
			char ch = *str + rand() % strlen(str);
			if (i < row - 1 && pull(m, i+1, j) == 0) c = WHITE;
			if (!pull(m, i, j)) ch = ' ';
			printf("%s%c ", c, ch);
			usleep(USLEEP);
		}
		puts("");
	}

	printf("%c[0;0f",0x1B);
	output_queue(m->n, str);
}

Matrix *create(uint16_t row, uint16_t col) {
	Matrix *m = (Matrix *) malloc(sizeof(Matrix));
	if (!m) exit(EXIT_FAILURE);
	m->row = row;
	m->col = col;
	m->n = NULL;
	m->v = (uint8_t **) malloc(row * sizeof(uint8_t *));
	for (size_t i = 0; i < row; ++i)
		m->v[i] = (uint8_t *) malloc(col * sizeof(uint8_t));
	return m;
} 

uint8_t pull(Matrix *m, uint16_t i, uint16_t j) {
	if (i > m->row || j > m->col) exit(EXIT_FAILURE);
	return m->v[i][j];
}

void push(Matrix *m, uint16_t i, uint16_t j, uint8_t v) {
	if (i > m->row || j > m->col) exit(EXIT_FAILURE);
	m->v[i][j] = v;
}

void output(Matrix *m) {
	for (size_t i = 0; i < m->row; ++i) {
		for (size_t j = 0; j < m->col; ++j) {
			printf("%d ", pull(m, i, j));
		}
		puts("");
	}
	puts("");
}

Matrix *set(Matrix *m, uint16_t *head, uint16_t *end, uint16_t *size, size_t index) {
	uint16_t row = m->row;
	uint16_t col = m->col;

	Matrix *matrix = create(row, col);

	for (size_t i = 0; i < col; ++i) {
		for (size_t j = 0; j < row; ++j) {
			if (j == head[i]) break;
			if (j <= end[i]) { 
				push(matrix, j, i, 0); 
				continue;
			}
			push(matrix, j, i, 1);
		}
		head[i]++;
	}
		
	for (size_t i = 0; i < col; ++i) {
		if (index >= size[i]) end[i]++;
	}


	return matrix;
}

int main(void) {
	system("clear");
	srand(time(NULL));

	char *str = "<>!@#$%^&*()>?/1234567890qwertyuiopasdfghjklzxcvbnm";		

	uint16_t row, col;
	row = ROW, col = COL;

	Matrix *m = create(row, col);	

	uint16_t head[col], end[col], size[col];
	for (size_t i = 0; i < col; ++i)
		head[i] = rand() % (row - 10), size[i] = rand() % 10, end[i] = 0;

	for (size_t i = 0; i < row; ++i) 
		for (size_t j = 0; j < col; ++j)
			push(m, i, j, 0);

	Queue *q = create_queue();
	for (size_t j = 0; j < (row + 10); ++j) 
		push_queue(q, set(m, head, end, size, j));

	output_queue(q->f, str);	

	free(q);
	return 0;
}
