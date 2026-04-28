/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: damagda <<marvin@42.fr>>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 00:00:00 by damagda           #+#    #+#             */
/*   Updated: 2026/04/14 18:09:23 by damagda          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen_gnl(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}

char	*ft_strchr_gnl(char *s, int c)
{
	size_t	i;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i++;
	}
	if ((char)c == '\0')
		return ((char *)&s[i]);
	return (NULL);
}

static void	ft_fill(char *res, char *s1, char *s2, size_t len1)
{
	size_t	i;

	i = 0;
	if (s1)
	{
		while (i < len1)
		{
			res[i] = s1[i];
			i++;
		}
	}
	i = 0;
	while (s2[i])
	{
		res[len1 + i] = s2[i];
		i++;
	}
}

char	*ft_strjoin_gnl(char *s1, char *s2)
{
	char	*res;
	size_t	len1;
	size_t	len2;

	len1 = ft_strlen_gnl(s1);
	len2 = ft_strlen_gnl(s2);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		free(s1);
		return (NULL);
	}
	ft_fill(res, s1, s2, len1);
	res[len1 + len2] = '\0';
	free(s1);
	return (res);
}

char	*ft_substr_gnl(char *s, unsigned int start, size_t len)
{
	char	*res;
	size_t	i;
	size_t	slen;

	if (!s)
		return (NULL);
	slen = ft_strlen_gnl(s);
	if (start >= (unsigned int)slen)
		return (ft_strjoin_gnl(NULL, ""));
	if (len > slen - start)
		len = slen - start;
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (i < len)
	{
		res[i] = s[start + i];
		i++;
	}
	res[i] = '\0';
	return (res);
}
