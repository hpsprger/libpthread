/* __pthread_destory_specific.  Hurd version.
   Copyright (C) 2002 Free Software Foundation, Inc.
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

#include <pthread.h>
#include <stdlib.h>
#include <hurd/ihash.h>

#include <pt-internal.h>

void
__pthread_destroy_specific (struct __pthread *thread)
{
  error_t err;
  int i;
  int seen_one;

  /* Check if there is any thread specific data.  */
  if (! thread->thread_specifics)
    return;

  __pthread_key_lock_ready ();

  /* Iterate and call the destructors on any thread specific data.  */
  for (;;)
    {
      seen_one = 0;

      __pthread_mutex_lock (&__pthread_key_lock);

      for (i = 0; i < __pthread_key_count; i ++)
	{
	  void *value;

	  if (__pthread_key_destructors[i] == PTHREAD_KEY_INVALID)
	    continue;

	  value = hurd_ihash_find (thread->thread_specifics, i);
	  if (value)
	    {
	      err = hurd_ihash_remove (thread->thread_specifics, i);
	      assert (err == 1);

	      if (__pthread_key_destructors[i])
		{
		  seen_one = 1;
		  __pthread_key_destructors[i] (value);
		}
	    }
	}

      __pthread_mutex_unlock (&__pthread_key_lock);

      if (! seen_one)
	break;

      /* This may take a very long time.  Let those blocking on
	 pthread_key_create or pthread_key_delete make progress.  */
      sched_yield ();
    }

  hurd_ihash_free (thread->thread_specifics);
  thread->thread_specifics = 0;
}
