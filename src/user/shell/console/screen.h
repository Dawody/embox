/**
 * \file screen.h
 * \date 28.02.2009
 * \author Eldar Abusalimov
 */
#ifndef SCREEN_H_
#define SCREEN_H_

#include "types.h"
#include "cmdline.h"
#include "drivers/terminal.h"

struct _SCREEN;

typedef struct _SCREEN_CALLBACK {
	void (*on_char)(struct _SCREEN_CALLBACK *, struct _SCREEN *view, char ch);
	void (*on_cursor_up)(struct _SCREEN_CALLBACK *, struct _SCREEN *view,
			int by);
	void (*on_cursor_down)(struct _SCREEN_CALLBACK *, struct _SCREEN *view,
			int by);
	void (*on_cursor_left)(struct _SCREEN_CALLBACK *, struct _SCREEN *view,
			int by);
	void (*on_cursor_right)(struct _SCREEN_CALLBACK *, struct _SCREEN *view,
			int by);
	void (*on_new_line)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_backspace)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_delete)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_insert)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_home)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_end)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_tab)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_etx)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_eot)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_dc2)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void (*on_dc4)(struct _SCREEN_CALLBACK *, struct _SCREEN *view);
	void *outer;
} SCREEN_CALLBACK;

typedef TERMINAL_IO SCREEN_IO;

typedef struct _SCREEN {
	SCREEN_CALLBACK *callback;
	TERMINAL terminal[1];
	char string[CMDLINE_MAX_LENGTH + 1]; /**< line itself */
	//int length; /**< string length */
	int cursor;   /**< cursor current position */
	const char *prompt;
	//int prompt_len;
	bool running;
} SCREEN;

SCREEN * screen_init(SCREEN *, SCREEN_IO *io);

void screen_out_show_prompt(SCREEN *, const char *prompt);
void screen_out_puts(SCREEN *, char *str);
void screen_out_update(SCREEN *, CMDLINE *cmdline);
void screen_in_start(SCREEN *, SCREEN_CALLBACK *cb);
void screen_in_stop(SCREEN *);

#endif /* SCREEN_H_ */
