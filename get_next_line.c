/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: damagda <<marvin@42.fr>>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 00:00:00 by damagda           #+#    #+#             */
/*   Updated: 2026/04/29 16:39:47 by damagda          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*extract_line(char *stash)
{
	char	*line;
	size_t	len;
	char	*nl;

	if (!stash || !stash[0])
		return (NULL);
	nl = ft_strchr_gnl(stash, '\n');
	if (nl)
		len = (size_t)(nl - stash) + 1;
	else
		len = ft_strlen_gnl(stash);
	line = ft_substr_gnl(stash, 0, len);
	return (line);
}

static char	*update_stash(char *stash)
{
	char	*nl;
	char	*new_stash;

	nl = ft_strchr_gnl(stash, '\n');
	if (!nl || !nl[1])
	{
		free(stash);
		return (NULL);
	}
	new_stash = ft_substr_gnl(stash,
			(unsigned int)(nl - stash + 1),
			ft_strlen_gnl(nl + 1));
	free(stash);
	return (new_stash);
}

static char	*read_and_stash(int fd, char *stash)
{
	char	*buf;
	char	*tmp;
	ssize_t	bytes;

	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
	{
		free(stash);
		return (NULL);
	}
	bytes = 1;
	while (!ft_strchr_gnl(stash, '\n') && bytes > 0)
	{
		bytes = read(fd, buf, BUFFER_SIZE);
		if (bytes < 0)
		{
			free(buf);
			free(stash);
			return (NULL);
		}
		buf[bytes] = '\0';
		tmp = ft_strjoin_gnl(stash, buf);
		if (!tmp)
		{
			free(buf);
			return (NULL);
		}
		stash = tmp;
	}
	free(buf);
	return (stash);
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	stash = read_and_stash(fd, stash);
	if (!stash)
		return (NULL);
	line = extract_line(stash);
	stash = update_stash(stash);
	return (line);
}