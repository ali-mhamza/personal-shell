#include <string.h>

size_t	strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	dst_len;
	size_t	src_len;
	size_t	i;
	size_t	j;

	src_len = strlen(src);
	if (dstsize == 0)
		return (src_len);
	dst_len = strlen(dst);
	i = dst_len;
	j = 0;
	if (dstsize <= dst_len)
		return (dstsize + src_len);
	else
	{
		while (src[j] != '\0' && i < dstsize - 1)
			dst[i++] = src[j++];
		dst[i] = '\0';
	}
	return (dst_len + src_len);
}
