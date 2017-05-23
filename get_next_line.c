/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmatthys <vmatthys@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/11 11:26:41 by vmatthys          #+#    #+#             */
/*   Updated: 2016/12/11 19:29:56 by vmatthys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*
** ft_linewidth(t_fd *cur, t_list *end, void *end_of_line)
** Takes the current fd associated structure, the end list containing the
** the end of the line, and a pointeur on the end of the line.
** Stores the adress of the beginning ot the new line in to_del which will be
** deleted during the line's extraction. Update the rest of the line as the new
** residual of the fd associated structure, starting right after the end of line
** and containing :
** || end->content_size - ((char*)end_of_line - (char*)end->content) - 1)
** || characters
** where :
** ----------- (char*)end_of_line - (char*)end->content - 1
** ----------- length of the end of the line, minus 1 for the spacer.
** Start counting from the length of the end of the line.
** Exploring all content stored since the previous line's extraction.
** Returns the length of the line, as called by ft_extract_line.
*/

static size_t	ft_linewidth(t_fd *cur, t_list *end, void *end_of_line)
{
	t_list	*to_del;
	size_t	width;

	to_del = cur->residual;
	cur->residual = ft_lstnew((void *)((char*)end_of_line + 1),
	end->content_size - (size_t)((char*)end_of_line - (char*)end->content) - 1);
	cur->state =
	cur->state - (int)((char*)end_of_line - (char*)end->content) - 1;
	width = (size_t)((char*)end_of_line - (char*)end->content);
	while (to_del != end)
	{
		width += to_del->content_size;
		to_del = to_del->next;
	}
	return (width);
}

/*
** ft_extract_line(t_fd *cur, t_list *end, char **line, void *end_of_line)
** Takes the current fd associated structure, the end list containing the
** the end of the line, the adress of the line which will be returned
** and a pointeur on the end of the line.
** Returns 0 if there is no end_of_line signal (such as EOF or '\n')
** Allocates new line in cur->residual and the space to contains the line in
** *line. Returns 1 if there is an allocation problem.
** Explores every content of the line starting from cur->residual to end end,
** and copies the content of the list until a '\n' is found, or the end the file
** is reached.
** Transforms the final \n of the line except for the final line.
** Increments the number of line the fd associated structure contains.
** Returns 1.
*/

static int		ft_extract_line(t_fd *cur, t_list *end, char **line,
	void *end_of_line)
{
	t_list	*to_del;
	t_list	*tmp;
	size_t	len;
	size_t	i;

	if ((i = 0) || cur->state <= 0 || !end_of_line)
		return (0);
	to_del = cur->residual;
	len = ft_linewidth(cur, end, end_of_line);
	if (!(cur->residual) || !(*line = ft_strnew(len + 1)))
	{
		ft_putendl_fd("Error : could not allocate memory", 2);
		return (-1);
	}
	while ((tmp = to_del))
	{
		ft_memccpy(*line + i, to_del->content, '\n', to_del->content_size);
		i += to_del->content_size;
		to_del = to_del->next;
		free(tmp->content);
		free(tmp);
	}
	(*line)[len] = ((*line)[len] == '\n') ? 0 : (*line)[len];
	cur->nbr_lines += 1;
	return (1);
}

/*
** ft_read_next_lines(t_fd *cur, char **line, void *buff)
** Takes the current fd associated structure, the adress of the line which will
** be returned, and the buffer allocated in get_next_line.
** Reads the file by BUFF_SIZE bytes until the end of file is reached or an
** error.
** Updates the cur->state with the number of bytes read.
** Allocates a new node pushed back to the previous one and returns -1 if
** the allocation is not possible.
** Tries to extract for every group of BUFF_SIZE bytes a line.
** When successful, return 1 and stores the line. Otherwise, keep reading.
** Updates the cur->state with -1 if the reading ends prematurely, or with
** the previous cur->state when reaching the end of the file.
** Returns 1 in case of reading error.
** Returns the result of ft_extract_line.
*/

static int		ft_read_next_lines(t_fd *cur, char **line, void *buff)
{
	t_list	*running;
	int		extract;

	running = cur->residual;
	while ((extract = (int)read(cur->fd, buff, BUFF_SIZE)) > 0)
	{
		cur->state = extract;
		if (!(running->next = ft_lstnew(buff, (size_t)cur->state)))
		{
			ft_putendl_fd("Error : could not allocate memory", 2);
			return (-1);
		}
		running = running->next;
		if ((extract = ft_extract_line(cur, running, line,
			ft_memchr(running->content, '\n', running->content_size))))
			return (extract);
	}
	cur->state = (extract) ? extract : cur->state;
	if (cur->state == -1)
	{
		ft_putendl_fd("Error : could not read the file", 2);
		return (-1);
	}
	return (ft_extract_line(cur, running, line,
		(void *)((char*)running->content + running->content_size - 1)));
}

/*
** *ft_dispatch(int const fd, t_fd **fd_list)
** Takes the fd and the adress of the head of the fd associated list.
** Explores the list to search for an identical fd associated node already
** created.
** Returns the node corresponding to fd if existing.
** Otherwise returns a new node.
** Returns null on an allocation error.
*/

static t_fd		*ft_dispatch(int const fd, t_fd **fd_list)
{
	t_fd	*new;
	t_fd	*cpy;

	cpy = *fd_list;
	if (cpy)
	{
		while (cpy->next && cpy->fd != fd)
			cpy = cpy->next;
		if (cpy->fd == fd)
			return (cpy);
	}
	if (!(new = (t_fd *)malloc(sizeof(t_fd) * 1)))
		return (NULL);
	if (*fd_list)
		cpy->next = new;
	else
		*fd_list = new;
	new->fd = fd;
	new->state = 0;
	new->next = NULL;
	new->nbr_lines = 0;
	if (!(new->residual = ft_lstnew(NULL, 0)))
		return (NULL);
	return (new);
}

/*
** get_next_line(int const fd, char **line)
** Takes the fd and the adress of the line which have to be returned.
** Returns -1 if fd is not a correct fd, or if line is null, or if the node
** hasn't not be allocated.
** Initialise *line.
** Tries to return an already stored line in the last node stored by
** ft_read_next_lines (which stops creating node the moment a node contains a
** end of line marker).
** On failure, there is no end of line marker already read, meaning the file
** has to be read again.
** Allocates the buffer for ft_read_next_lines, returns -1 on error.
** Returns the result of ft_read_next_lines after freeing the buffer.
*/

int				get_next_line(int const fd, char **line)
{
	static t_fd	*fd_list = NULL;
	t_fd		*cur;
	int			res;
	void		*buff;

	if (fd < 0 || !line || !(cur = ft_dispatch(fd, &fd_list)))
	{
		ft_putendl_fd("Error : could not allocate memory, or line is NULL, "
		"or the file is not readable", 2);
		return (-1);
	}
	*line = NULL;
	if ((res = ft_extract_line(cur, cur->residual, line,
	ft_memchr(cur->residual->content, '\n', cur->residual->content_size)) != 0))
		return (res);
	if (!(buff = ft_memalloc(BUFF_SIZE)))
	{
		ft_putendl_fd("Error : could not allocate memory", 2);
		return (-1);
	}
	res = ft_read_next_lines(cur, line, buff);
	ft_memdel(&buff);
	return (res);
}
