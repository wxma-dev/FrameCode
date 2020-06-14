#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "bas/bas_global.h"

BAS_EXPORT int	giOpterr = 1;		/* if error message should be printed */
BAS_EXPORT int  giOptind = 1;		/* index into parent argv vector */
BAS_EXPORT int  giOptopt;			/* character checked for validity */
BAS_EXPORT int  giOptreset;		/* reset getopt */
BAS_EXPORT char	*gpcOptarg;		/* argument associated with option */

#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""

/*
 * getopt --
 *	Parse argc/argv argument vector.
 */
BAS_EXPORT int basGetopt(nargc, nargv, ostr)
	int nargc;
	char * const nargv[];
	const char *ostr;
{
	static char *place = EMSG;		/* option letter processing */
	char *oli;				/* option letter list index */

	if (giOptreset || *place == 0) {		/* update scanning pointer */
		giOptreset = 0;
		place = nargv[giOptind];

		if (giOptind >= nargc || *place++ != '-') {
		    /* Find candidate option */
    		while ((giOptind < nargc) && (*place != '-')) {
    		    place = nargv[++giOptind];
    		}
    		if (giOptind >= nargc || *place++ != '-') {
    			/* Argument is absent or is not an option */
    			place = EMSG;
    			return (-1);
		    }
		}
		giOptopt = *place++;
		if (giOptopt == '-' && *place == 0) {
			/* "--" => end of options */
			++giOptind;
			place = EMSG;
			return (-1);
		}
		if (giOptopt == 0) {
			/* Solitary '-', treat as a '-' option
			   if the program (eg su) is looking for it. */
			place = EMSG;
			if (strchr(ostr, '-') == NULL)
				return (-1);
			giOptopt = '-';
		}
	} else
		giOptopt = *place++;

	/* See if option letter is one the caller wanted... */
	if (giOptopt == ':' || (oli = strchr(ostr, giOptopt)) == NULL) {
		if (*place == 0)
			++giOptind;
		if (giOpterr && *ostr != ':')
			(void)fprintf(stderr,
			    "%s: illegal option -- %c\n", basename(nargv[0]),
			    giOptopt);
		return (BADCH);
	}

	/* Does this option need an argument? */
	if (oli[1] != ':') {
		/* don't need argument */
		gpcOptarg = NULL;
		if (*place == 0)
			++giOptind;
	} else {
		/* Option-argument is either the rest of this argument or the
		   entire next argument. */
		if (*place)
			gpcOptarg = place;
		else if (nargc > ++giOptind)
			gpcOptarg = nargv[giOptind];
		else {
			/* option-argument absent */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (giOpterr)
				(void)fprintf(stderr,
				    "%s: option requires an argument -- %c\n",
				    basename(nargv[0]), giOptopt);
			return (BADCH);
		}
		place = EMSG;
		++giOptind;
	}
	return (giOptopt);			/* return option letter */
}
