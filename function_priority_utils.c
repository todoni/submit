#include "ft_printf.h"
#include "heap.h"

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

