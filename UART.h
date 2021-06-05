/*
 * UART.h
 *
 *  Created on: May 12, 2021
 *      Author: austi
 */

#ifndef UART_H_
#define UART_H_

void UART_init(void);
void UART_write_string(const char* string);
void UART_esc_code(const char* esc_code);
void UART_write_int(unsigned int num);
void UART_write_char(unsigned char c);


#endif /* UART_H_ */
