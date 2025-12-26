#include "../include/common.h"
#include <string.h>
#include <stdlib.h>

static int	count_words(char const *s, char c)
{
	int	words;
	int	i;

	words = 0;
	i = 0;
	while (s[i] != '\0')
	{
		if ((i == 0) && (s[i] != c))
			words++;
		if ((s[i] != c) && (i > 0) && (s[i - 1] == c))
			words++;
		i++;
	}
	return (words);
}

static char	**free_array(char **array)
{
	int	i;

	i = 0;
	while (array[i] != NULL)
	{
		free(array[i]);
		array[i] = NULL;
		i++;
	}
	free(array);
	return (NULL);
}

static char	**allocate_array(size_t size)
{
	char		**array;
	size_t		i;

	array = calloc(size + 1, sizeof(char *));
	if (!array)
		return (NULL);
	i = 0;
	while (i < size)
	{
		array[i] = calloc(size + 1, sizeof(char));
		if (!array[i])
		{
			array = free_array(array);
			return (NULL);
		}
		i++;
	}
	return (array);
}

static char	**fill_array(char **array, char const *s, char c)
{
	int	i;
	int	x;
	int	y;

	i = 0;
	x = 0;
	y = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c)
		{
			array[x][y++] = s[i];
			if (s[i + 1] == '\0')
				array[x][y] = '\0';
		}
		if ((s[i] == c) && (i > 0) && (s[i - 1] != c))
		{
			array[x][y] = '\0';
			x++;
			y = 0;
		}
		i++;
	}
	return (array);
}

char	**split(char const *s, char c)
{
	char	**array;
	int		words;

	if (!s)
	{
		array = allocate_array(sizeof(char) * 1);
		if (!array)
			return (NULL);
		array[0] = NULL;
		return (array);
	}
	array = allocate_array(strlen(s));
	if (!array)
		return (NULL);
	array = fill_array(array, s, c);
	words = count_words(s, c);
	array[words] = NULL;
	return (array);
}
