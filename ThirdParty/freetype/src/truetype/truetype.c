/****************************************************************************
 *
 * truetype.c
 *
 *   FreeType TrueType driver component (body only).
 *
 * Copyright (C) 1996-2023 by
 * David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 *
 */


#define FT_MAKE_OPTION_SINGLE_OBJECT

#include "source/ttdriver.c"   /* driver interface    */
#include "source/ttgload.c"    /* glyph loader        */
#include "source/ttgxvar.c"    /* gx distortable font */
#include "source/ttinterp.c"
#include "source/ttobjs.c"     /* object manager      */
#include "source/ttpload.c"    /* tables loader       */
#include "source/ttsubpix.c"


/* END */
