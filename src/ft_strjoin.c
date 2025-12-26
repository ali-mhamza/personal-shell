#include "../include/common.h"
#include <string.h>
#include <stdlib.h>

char	*strjoin(char const *s1, char const *s2)
{
	size_t	s1_size;
	size_t	s2_size;
	char	*new_str;

	s1_size = strlen(s1);
	s2_size = strlen(s2);
	new_str = calloc(s1_size + s2_size + 1, sizeof(char));
	if (!new_str)
		return (NULL);
	p_strlcat(new_str, s1, s1_size + 1);
	p_strlcat(new_str + s1_size, s2, s2_size + 1);
	return (new_str);
}
