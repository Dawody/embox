/**
 * \file screen_in.c
 * \date 28.02.2009
 * \author Eldar Abusalimov
 */
#include "screen.h"
#include "assert.h"
#include "common.h"
#include "console.h"
#include "kernel/sys.h"

#define FIRE_CALLBACK(cb, func, view, args...)	((cb->func != NULL) ? cb->func(cb, view, ##args) : 0)

extern CONSOLE *cur_console;

static void handle_char_token(SCREEN *this, TERMINAL_TOKEN ch) {
	SCREEN_CALLBACK *cb = this->callback;
	if (cb == NULL) {
		return;
	}

	FIRE_CALLBACK(cb, on_char, this, ch);
}

static void handle_ctrl_token(SCREEN *this, TERMINAL_TOKEN token,
		TERMINAL_TOKEN_PARAMS *params) {
	SCREEN_CALLBACK *cb = this->callback;
	if (cb == NULL) {
		return;
	}

	static TERMINAL_TOKEN prev_token = TERMINAL_TOKEN_EMPTY;
	switch (token) {
	case TERMINAL_TOKEN_CURSOR_LEFT:
		FIRE_CALLBACK(cb, on_cursor_left, this, 1);
		break;
	case TERMINAL_TOKEN_CURSOR_RIGHT:
		FIRE_CALLBACK(cb, on_cursor_right, this, 1);
		break;
	case TERMINAL_TOKEN_CURSOR_UP:
		FIRE_CALLBACK(cb, on_cursor_up, this, 1);
		break;
	case TERMINAL_TOKEN_CURSOR_DOWN:
		FIRE_CALLBACK(cb, on_cursor_down, this, 1);
		break;
	case TERMINAL_TOKEN_BS:
		FIRE_CALLBACK(cb, on_backspace, this);
		break;
	case TERMINAL_TOKEN_DEL:
		FIRE_CALLBACK(cb, on_delete, this);
		break;
	case TERMINAL_TOKEN_END:
		/* TODO: strange char 'F' */
		FIRE_CALLBACK(cb, on_end, this);
		break;
	case TERMINAL_TOKEN_ETX:
		FIRE_CALLBACK(cb, on_etx, this);
		break;
	case TERMINAL_TOKEN_EOT:
	        FIRE_CALLBACK(cb, on_eot, this);
	        break;
	case TERMINAL_TOKEN_DC2:
		FIRE_CALLBACK(cb, on_dc2, this);
		break;
	case TERMINAL_TOKEN_DC4:
	        FIRE_CALLBACK(cb, on_dc4, this);
	        break;
	case TERMINAL_TOKEN_LF:
		if (prev_token == TERMINAL_TOKEN_CR) {
			break;
		}
	case TERMINAL_TOKEN_CR:
		FIRE_CALLBACK(cb, on_new_line, this);
		break;
	case TERMINAL_TOKEN_PRIVATE:
		if (params->length == 0) {
			break;
		}
		switch (params->data[0]) {
		case TERMINAL_TOKEN_PARAM_PRIVATE_DELETE:
			FIRE_CALLBACK(cb, on_delete, this);
			break;
		//case TERMINAL_TOKEN_PARAM_PRIVATE_END:
		//	FIRE_CALLBACK(cb, on_end, this);
		//	break;
		case TERMINAL_TOKEN_PARAM_PRIVATE_HOME:
			FIRE_CALLBACK(cb, on_home, this);
			break;
		case TERMINAL_TOKEN_PARAM_PRIVATE_INSERT:
			FIRE_CALLBACK(cb, on_insert, this);
			break;
		default:
			break;
		}
		break;
	case TERMINAL_TOKEN_HT:
		FIRE_CALLBACK(cb, on_tab, this);
		break;
	default:
		break;
	}

	prev_token = token;
}

void uart_irq_handler() {
	if (!sys_exec_is_started())
		return;
	static TERMINAL_TOKEN token;
	static TERMINAL_TOKEN_PARAMS params[1];
	SCREEN *this = cur_console->view;
	terminal_receive(this->terminal, &token, params);
	char ch = token & 0xFF;
	//TODO:
	if (ch == token) {
	        handle_char_token(this, token);
	} else {
	        handle_ctrl_token(this, token, params);
	}
}

void screen_in_start(SCREEN *this, SCREEN_CALLBACK *cb) {
	if (this == NULL) {
		return;
	}

	static TERMINAL_TOKEN token;
	static TERMINAL_TOKEN_PARAMS params[1];

	if (this->running) {
		return;
	}
	this->running = true;

	this->callback = cb;
	uart_set_irq_handler(uart_irq_handler);
	while (this->callback != NULL && terminal_receive(this->terminal, &token,
			params)) {
		char ch = token & 0xFF;
		if (ch == token) {
			handle_char_token(this, token);
		} else {
			handle_ctrl_token(this, token, params);
		}
	}
	assert(this->callback == NULL);
	this->running = false;

}

void screen_in_stop(SCREEN *this) {
	if (this == NULL) {
		return;
	}
	uart_remove_irq_handler();
	this->callback = NULL;
}

