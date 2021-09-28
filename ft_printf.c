/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohan <sohan@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 21:17:40 by sohan             #+#    #+#             */
/*   Updated: 2021/09/28 21:17:44 by sohan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "ft_printf.h"
#include "heap.h"

int	print_untyped_character(const char *fmt, int ret)
{
	write(1, fmt, 1);
	ret++;
	return (ret);
}

void	print_padding_by_length(int length, int *ret)
{
	*ret += length;
	while (length--)
		write(1, "0", 1);
}

void	print_space_by_length(int length, int *ret)
{
	*ret += length;
	while (length--)
		write(1, " ", 1);
}

void	print_one_space(int *ret)
{
	write(1, " ", 1);
	*ret += 1;
}

void	print_character(int c, int *ret)
{
	write(1, &c, 1);
	*ret += 1;
}

void	print_plus(int num, int *ret)
{
	if (num >= 0)
	{
		write(1, "+", 1);
		*ret += 1;
	}
}

void	print_argument_by_length(char *str, int length, int *ret)
{
	write(1, str, length);
	*ret += length;
}

void	print_prefix_pointer(int *ret)
{
	write(1, "0x", 2);
	*ret += 2;
}

void	print_prefix_sharp_up(int *ret)
{
	write(1, "0X", 2);
	*ret += 2;
}

void	print_prefix_sharp_low(int *ret)
{
	write(1, "0x", 2);
	*ret += 2;
}

void	move_fmt_by_length(const char **fmt, int length)
{
	(*fmt) += length;
}

int	is_flag(char c)
{
	if (c == '0' || c == '-' || c == '+' || c == ' ' || c == '#')
		return (1);
	return (0);
}

void	set_flag(const char **fmt, int *flag)
{
	while (is_flag(**fmt))
	{
		if (**fmt == '0')
			*flag |= zero;
		else if (**fmt == '-')
			*flag |= minus;
		else if (**fmt == '+')
			*flag |= plus;
		else if (**fmt == ' ')
			*flag |= space;
		else if (**fmt == '#')
			*flag |= sharp;
		move_fmt_by_length(fmt, 1);
	}
}

int	find_digit(int num)
{
	int digit;

	digit = 0;
	if (!num)
		digit = 1;
	while (num)
	{
		num /= 10;
		digit++;
	}
	return (digit);
}

void	set_width(const char **fmt, int *width)
{
	if (ft_isdigit(**fmt) && **fmt != '0')
	{
		*width = ft_atoi(*fmt);
		move_fmt_by_length(fmt, find_digit(*width));
	}
}

void	set_precision(const char **fmt, t_component *cmp)
{
	if (**fmt == '.')
	{
		move_fmt_by_length(fmt, 1);
		cmp->width_precision = ft_atoi(*fmt);
		move_fmt_by_length(fmt, find_digit(cmp->width_precision));
		cmp->bit |= precision;
	}
}

void	set_type(const char **fmt, int *type)
{
	if (**fmt == 'c')
		*type |= character;
	else if (**fmt == 's')
		*type |= string;
	else if (**fmt == 'p')
		*type |= pointer;
	else if (**fmt == 'd')
		*type |= integer;
	else if (**fmt == 'i')
		*type |= integer;
	else if (**fmt == 'u')
		*type |= u_integer;
	else if (**fmt == 'x')
		*type |= hex_low;
	else if (**fmt == 'X')
		*type |= hex_up;
	else if (**fmt == '%')
		*type |= percent;
	move_fmt_by_length(fmt, 1);
}

void	set_minimum_field_width(const char **fmt, t_component *cmp)
{
	set_flag(fmt, &cmp->bit);
	set_width(fmt, &cmp->width_total);
	set_precision(fmt, cmp);
	set_type(fmt, &cmp->bit);
}

void	set_width_bit(t_component	*cmp)
{
	if ((cmp->bit & plus) || (cmp->bit &space))
		cmp->width_flag++;
	if (cmp->bit & sharp)
		cmp->width_flag += 2;
}

void	set_print_length(t_component *cmp)
{
	set_width_bit(cmp);
	cmp->width_str = ft_strlen(cmp->str);
	if (cmp->width_precision && cmp->width_str > cmp->width_precision)
		cmp->width_str = cmp->width_precision;
	cmp->width_padding = cmp->width_precision - cmp->width_str;
	if (cmp->width_padding < 0)
		cmp->width_padding = 0;
	cmp->width_space = cmp->width_total - cmp->width_flag - cmp->width_str;
	if (!(cmp->bit & string))
		cmp->width_space -= cmp->width_padding;
	if (cmp->bit & character)
		cmp->width_space--;
	if (cmp->bit & pointer)
		cmp->width_space -= 2;
	if (cmp->width_space < 0)
		cmp->width_space = 0;
}

void	get_argument_value(va_list arg_ptr, t_component *cmp)
{
	if (cmp->bit & int_mask)
		cmp->_int = va_arg(arg_ptr, int);
	else if (cmp->bit & string)
		cmp->str = va_arg(arg_ptr, char*);
	else if (cmp->bit & pointer)
		cmp->_pointer = va_arg(arg_ptr, unsigned long long);
	else if (cmp->bit & u_int_mask)
		cmp->_uint = va_arg(arg_ptr, unsigned int);
	else if (cmp->bit & percent)
		cmp->str = "%";
}

int	integer_to_string(t_component *cmp)
{
	char	*base;

	base = 0;
	if (cmp->bit & int_base_mask)
		base = "0123456789";
	else if (cmp->bit & x_base_mask)
		base = "0123456789abcdef";
	else if (cmp->bit & hex_up)
		base = "0123456789ABCDEF";
	if (!cmp->str && !(cmp->bit & character))
	{
		if (cmp->bit & int_mask)
			cmp->str = ft_itoa(cmp->_int, base);
		else if (cmp->bit & u_int_mask)
			cmp->str = ft_utoa(cmp->_uint, base);
		else if (!base)
			cmp->str = "(null)";
		else
			cmp->str = ft_ptoa(cmp->_pointer, base);
		if (!cmp->str)
			return (-1);
	}
	return (1);
}

void	set_zero(t_fp function[])
{
	function[ZERO].usage = USED;
	function[SPACE].usage = NOT_USED;
	function[ZERO].print_padding = print_padding_by_length;
}

void	set_pointer(t_fp function[])
{
	function[POINTER].usage = USED;
	function[POINTER].print = print_prefix_pointer;
}

void	set_plus(t_fp function[])
{
	function[PLUS].usage = USED;
	function[PLUS].print_plus = print_plus;
}

void	set_prec(t_fp function[])
{
	function[PRECISION].usage = USED;
	function[PRECISION].print_precision = print_padding_by_length;
}

void	set_space(t_fp function[])
{
	function[ONESPACE].usage = USED;
	function[ONESPACE].print = print_one_space;
}

void	set_sharp(t_fp function[], int bit)
{
	if (bit & hex_up)
	{
		function[SHARPUP].usage = USED;
		function[SHARPUP].print = print_prefix_sharp_up;
	}
	else
	{
		function[SHARPLOW].usage = USED;
		function[SHARPLOW].print = print_prefix_sharp_low;
	}
}

void	set_character(t_fp function[])
{
	function[CHAR].usage = USED;
	function[CHAR].print_char = print_character;
	function[ARGUMENT].usage = NOT_USED;
}

void	set_string(t_fp function[])
{
	function[ZERO].usage = NOT_USED;
	function[PRECISION].usage = NOT_USED;
}

void	set_function_usage(int bit, t_fp function[])
{
		function[ARGUMENT].usage = USED;
		function[ARGUMENT].print_string = print_argument_by_length;
		function[SPACE].usage = USED;
		function[SPACE].print_space = print_space_by_length;

		if (bit & zero)
			set_zero(function);
		if (bit & pointer)
			set_pointer(function);
		if (bit & plus)
			set_plus(function);
		if (bit & space)
			set_space(function);
		if (bit & sharp)
			set_sharp(function, bit);
		if (bit & character)
			set_character(function);
		if (bit & precision)
			set_prec(function);
		if (bit & string)
			set_string(function);
}

int	get_used_number(t_fp function[])
{
	int index;
	int	num;

	index = MAX_SIZE;
	num = 0;
	while (index--)
	{
		if (function[index].usage == USED)
			num++;
	}
	return (num);
}

void	set_priority(t_fp function[], int bit)
{
	function[ARGUMENT].priority = get_used_number(function);
	function[CHAR].priority = get_used_number(function);
	if (bit & minus)
		function[SPACE].priority = function[ARGUMENT].priority + 1;
	if (bit & zero)
		function[ZERO].priority = function[SPACE].priority + 1;
	if (bit & precision)
		function[PRECISION].priority = function[ARGUMENT].priority - 1;
	if (bit & pointer)
		function[POINTER].priority = function[ARGUMENT].priority - 1;
	if (bit & plus)
	{
		function[PLUS].priority = function[ARGUMENT].priority - 1;
		function[PRECISION].priority = function[PLUS].priority + 1;
	}
	if (bit & space)
		function[ONESPACE].priority = FIRST;
	if (bit & sharp)
	{
		function[SHARPUP].priority = function[PRECISION].priority - 1;
		function[SHARPLOW].priority = function[PRECISION].priority - 1;
	}
}

void	insert_to_heap(t_fp function[], t_heap *heap)
{
	int	index;

	index = MAX_SIZE;
	while (index--)
	{
		if (function[index].usage == USED)
			heap_insert(heap, function[index]);
	}
}

void	print_by_priority(t_heap *heap, t_component *cmp, int *ret)
{
	t_fp *fp;

	while (heap->size)
	{
		fp = heap_delete(heap);
		if (fp->print)
			fp->print(ret);
		else if (fp->print_space)
			fp->print_space(cmp->width_space, ret);
		else if (fp->print_padding)
			fp->print_padding(cmp->width_space, ret);
		else if (fp->print_precision)
			fp->print_precision(cmp->width_padding, ret);
		else if (fp->print_string)
			fp->print_string(cmp->str, cmp->width_str, ret);
		else if (fp->print_char)
			fp->print_char(cmp->_int, ret);
		else if (fp->print_plus)
			fp->print_plus(cmp->_int, ret);
		free(fp);
	}
}

void	init_component(t_component *cmp)
{
	cmp->_int = 0;
	cmp->_uint = 0;
	cmp->_pointer = 0;
	cmp->bit = 0;
	cmp->str = 0;
	cmp->width_flag = 0;
	cmp->width_padding = 0;
	cmp->width_precision = 0;
	cmp->width_space = 0;
	cmp->width_total = 0;
	cmp->width_str = 0;
}

void	init_function(t_fp fp[])
{
	int	i;

	i = 0;
	while (i < MAX_SIZE)
	{
		fp[i].print = 0;
		fp[i].print_space = 0;
		fp[i].print_padding = 0;
		fp[i].print_precision = 0;
		fp[i].print_string = 0;
		fp[i].print_char = 0;
		fp[i].print_plus = 0;
		fp[i].priority = 0;
		fp[i].usage = NOT_USED;
		i++;
	}
}

int	do_print(const char **fmt, va_list arg_ptr, int ret)
{
	t_component	cmp;
	t_fp	fp[MAX_SIZE];
	t_heap	heap;
	int	malloc_error;

	init_component(&cmp);
	heap_initialize(&heap);
	init_function(fp);
	move_fmt_by_length(fmt, 1);
	set_minimum_field_width(fmt, &cmp);
	get_argument_value(arg_ptr, &cmp);
	malloc_error = integer_to_string(&cmp);
	if (malloc_error == -1)
		return (-1);
	set_print_length(&cmp);
	set_function_usage(cmp.bit, fp);
	set_priority(fp, cmp.bit);
	insert_to_heap(fp, &heap);
	print_by_priority(&heap, &cmp, &ret);
	if (cmp.bit & malloc_free)
		free(cmp.str);
	return (ret);
}

int	ft_vprintf(const char **fmt, va_list arg_ptr, int ret)
{

	while(**fmt)
	{
		if (**fmt == '%')
		{
			ret = do_print(fmt, arg_ptr, ret);
			if (ret == -1)
				return (-1);
		}
		if (**fmt == '\0')
			break;
		if (**fmt != '%')
		{
			ret = print_untyped_character(*fmt, ret);
			move_fmt_by_length(fmt, 1);
		}
	}
	return (ret);
}

int	ft_printf(const char *fmt, ...)
{
	int	ret;
	va_list	arg_ptr;

	ret = 0;
	va_start(arg_ptr, fmt);
	ret = ft_vprintf(&fmt, arg_ptr, ret);
	if (ret == -1)
	{
		write(2, "Memory Allocation Failed!\n", 26);
		return (-1);
	}
	va_end(arg_ptr);
	return (ret);
}
