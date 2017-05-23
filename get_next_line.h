/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmatthys <vmatthys@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/07 23:24:42 by vmatthys          #+#    #+#             */
/*   Updated: 2016/12/11 19:42:52 by vmatthys         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# include <stdlib.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <unistd.h>
# include "libft.h"
# define BUFF_SIZE 666

typedef struct		s_fd
{
	int				fd;
	int				state;
	size_t			nbr_lines;
	t_list			*residual;
	struct s_fd		*next;
}					t_fd;

int					get_next_line(int const fd, char **line);
#endif

/*
** typedef struct		s_list
** {
**	void			*content;
**	size_t			content_size;
**	struct s_list	*next;
**}					t_list;
*/
