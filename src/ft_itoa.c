#include <stddef.h>
#include <stdlib.h>

static int	digit_count(int n)
{
	int			size;
	long int	m;

	if (n == 0)
		return (1);
	m = n;
	if (n < 0)
		m *= -1;
	size = 0;
	while (m > 0)
	{
		m = m / 10;
		size++;
	}
	return (size);
}

static char	*fill_string(char *str, int num, int digits)
{
	long int	m;
	int			stop;

	m = num;
	stop = 0;
	if (num < 0)
	{
		str[0] = '-';
		m *= -1;
		stop = 1;
	}
	str[digits + stop] = '\0';
	while (digits > 0)
	{
		str[digits + stop - 1] = '0' + (m % 10);
		m = m / 10;
		digits--;
	}
	return (str);
}

char	*itoa(int n)
{
	int			digits;
	char		*str;
	int			stop;

	digits = digit_count(n);
	stop = 0;
	if (n < 0)
		stop = 1;
	str = malloc(digits + stop + 1);
	if (!str)
		return (NULL);
	return (fill_string(str, n, digits));
}
