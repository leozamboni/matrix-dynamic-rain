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
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define GREEN "\e[0;92m"
#define WHITE "\e[0;97m"
#define GOTOTOP printf("\033[0;0H");

typedef struct matrix Matrix;

struct matrix
{
    u_int8_t **v;
    size_t row;
    size_t col;
    size_t *head;
    size_t *end;
    size_t *size;
};

Matrix *
create (size_t row, size_t col)
{
    Matrix *m = malloc (sizeof (Matrix));
    if (!m) exit (EXIT_FAILURE);
    m->head = malloc (col * sizeof (size_t));
    if (!m->head) exit (EXIT_FAILURE);
    m->size = malloc (col * sizeof (size_t));
    if (!m->size) exit (EXIT_FAILURE);
    m->end = malloc (col * sizeof (size_t));
    if (!m->size) exit (EXIT_FAILURE);
    m->row = row;
    m->col = col;
    m->v = malloc (row * sizeof (u_int8_t *));
    for (size_t i = 0; i < row; ++i)
        {
            m->v[i] = malloc (col * sizeof (u_int8_t));
        }
    return m;
}

void
output (Matrix *m, char *str)
{
    for (size_t i = 0; i < m->row; ++i)
        {
            for (size_t j = 0; j < m->col; ++j)
                {
                    char *c = (i < (size_t)m->row - 1 && m->v[i + 1][j] == 0) ? WHITE
                              : GREEN;
                    printf (
                        "%s%c", c,
                        (char)(m->v[i][j] == 0 ? ' ' : *str + rand () % strlen (str)));
                    sleep (0);
                }
            puts ("");
        }
}

_Bool
empty (Matrix *m)
{
    for (size_t i = 0; i < m->row; ++i)
       {
            if (m->v[i][i])
                return 0;
        }
    return 1;
}

void
set (Matrix **m, size_t i)
{
    u_int16_t row = (*m)->row;
    u_int16_t col = (*m)->col;

    for (size_t j = 0; j < col; ++j)
        {
            for (size_t y = 0; y < row; ++y)
                {
                    if (y == (*m)->head[j])
                        break;
                    if (y <= (*m)->end[j])
                        {
                            (*m)->v[y][j] = 0;
                            continue;
                        }
                    (*m)->v[y][j] = 1;
                }
            (*m)->head[j]++;
        }

    for (size_t j = 0; j < col; ++j)
        {
            if (i >= (*m)->size[j])
                (*m)->end[j]++;
        }
}

void
init (Matrix **m)
{
    for (size_t i = 0; i < (*m)->col; ++i)
        {
            (*m)->head[i] = rand () % 10, (*m)->size[i] = rand () % 10, (*m)->end[i] = 0;
        }

    for (size_t i = 0; i < (*m)->row; ++i)
        {
            for (size_t j = 0; j < (*m)->col; ++j)
                {
                    (*m)->v[i][j] = 0;
                }
        }
}

int
main (void)
{
    system ("clear");
    srand (time (NULL));

    struct winsize w;
    ioctl (0, TIOCGWINSZ, &w);

    Matrix *m = create (w.ws_row - 1, w.ws_col);
    init (&m);

    char *str = "<>!@#$%^&*()>?/1234567890qwertyuiopasdfghjklzxcvbnm";

    size_t i = 0;
    while (1)
        {
            set (&m, i++);
            output (m, str);
            GOTOTOP
            if (empty (m))
                {
                    i = 0;
                    init (&m);
                }
        }

    return 0;
}
