/* Thread creation from provided L4 thread.
   Copyright (C) 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <pt-internal.h>

/* Create a thread with attributes given by ATTR, executing
   START_ROUTINE with argument ARG.  TID is the provided L4
   kernel thread.  */
int
pthread_create_from_l4_tid_np (pthread_t *thread, 
			       const pthread_attr_t *attr,
			       l4_thread_id_t tid, 
			       void *(*start_routine)(void *), void *arg)
{
  int err;
  struct __pthread *pthread;

  err = __pthread_create_internal (&pthread, attr, (void *) &tid, 
				   start_routine, arg);
  if (! err)
    *thread = pthread->thread;

  return err;
}