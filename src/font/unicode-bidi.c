///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Or,
 *
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 *
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 *
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

/*
** unicode-bidi.c:
**
** A general implementation of UNICODE BIDIRECTIONAL ALGORITHM for MiniGUI
**
**  https://www.unicode.org/reports/tr9/
**
** Created at 2019/03/08
**
** This implementation is copied from LGPL'd FriBidi and revised for MiniGUI
** By Vincent Wei.
**
**  https://github.com/fribidi/fribidi
**
** Authors of FriBidi:
**   Behdad Esfahbod, 2001, 2002, 2004
**   Dov Grobgeld, 1999, 2000
**
** Copyright (C) 2004 Sharif FarsiWeb, Inc
** Copyright (C) 2001,2002 Behdad Esfahbod
** Copyright (C) 1999,2000 Dov Grobgeld
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"

#ifdef _MGCHARSET_UNICODE

#include "minigui.h"
#include "gdi.h"

#include "devfont.h"
#include "bidi.h"
#include "unicode-bidi.h"

#ifdef _DEBUG

#include <assert.h>

static void validate_run_list(BidiRun *run_list)
{
    BidiRun *q;

    assert (run_list);
    assert (run_list->next);
    assert (run_list->next->prev == run_list);
    assert (run_list->type == BIDI_TYPE_SENTINEL);
    for_run_list (q, run_list) {
        assert (q->next);
        assert (q->next->prev == q);
    }
    assert (q == run_list);
}

#else

static inline void validate_run_list(BidiRun *run_list)
{
    // do nothing.
}

#endif /* !_DEBUG */

static BidiRun* new_run(void)
{
    BidiRun *run;

    run = malloc (sizeof (BidiRun));

    if (run) {
        run->len = run->pos = run->level = run->isolate_level = 0;
        run->next = run->prev = run->prev_isolate = run->next_isolate = NULL;
    }

    return run;
}

static BidiRun* new_run_list(void)
{
    BidiRun *run;

    run = new_run ();

    if (run) {
        run->type = BIDI_TYPE_SENTINEL;
        run->level = BIDI_SENTINEL;
        run->pos = BIDI_SENTINEL;
        run->len = BIDI_SENTINEL;
        run->next = run->prev = run;
    }

    return run;
}

static void free_run_list(BidiRun *run_list)
{
    if (!run_list)
        return;

    validate_run_list (run_list);

    {
        BidiRun *pp;

        pp = run_list;
        pp->prev->next = NULL;
        while (pp)
        {
            BidiRun *p;

            p = pp;
            pp = pp->next;
            free (p);
        };
    }
}


static BidiRun* run_list_encode_bidi_types (const BidiType *bidi_types,
    const BidiBracketType *bracket_types, int len)
{
    BidiRun *list, *last;
    BidiRun *run = NULL;
    int i;

    /* Create the list sentinel */
    list = new_run_list ();
    if (!list) return NULL;
    last = list;

    /* Scan over the character types */
    for (i = 0; i < len; i++)
    {
        BidiType char_type = bidi_types[i];
        BidiBracketType bracket_type = BIDI_BRACKET_NONE;
        if (bracket_types)
            bracket_type = bracket_types[i];

        if (char_type != last->type
                /* Always separate bracket into single char runs! */
                || bracket_type != BIDI_BRACKET_NONE
                || last->bracket_type != BIDI_BRACKET_NONE
                || BIDI_IS_ISOLATE(char_type)) {
            run = new_run ();
            if (!run) break;
            run->type = char_type;
            run->pos = i;
            last->len = run->pos - last->pos;
            last->next = run;
            run->prev = last;
            run->bracket_type = bracket_type;
            last = run;
        }
    }

    /* Close the circle */
    last->len = len - last->pos;
    last->next = list;
    list->prev = last;

    if (!run) {
        /* Memory allocation failed */
        free_run_list (list);
        return NULL;
    }

    validate_run_list (list);

    return list;
}

/* override the run list 'base', with the runs in the list 'over', to
   reinsert the previously-removed explicit codes (at X9) from
   'explicits_list' back into 'type_rl_list' for example. This is used at the
   end of I2 to restore the explicit marks, and also to reset the character
   types of characters at L1.

   it is assumed that the 'pos' of the first element in 'base' list is not
   more than the 'pos' of the first element of the 'over' list, and the
   'pos' of the last element of the 'base' list is not less than the 'pos'
   of the last element of the 'over' list. these two conditions are always
   satisfied for the two usages mentioned above.

   Note:
     frees the over list.

   Todo:
     use some explanatory names instead of p, q, ...
     rewrite comment above to remove references to special usage.
*/
static BOOL shadow_run_list(BidiRun *base, BidiRun *over, BOOL preserve_length)
{
    BidiRun *p = base, *q, *r, *s, *t;
    int pos = 0, pos2;
    BOOL status = FALSE;

    validate_run_list (base);
    validate_run_list (over);

    for_run_list (q, over)
    {
        if (!q->len || q->pos < pos) continue;
        pos = q->pos;
        while (p->next->type != BIDI_TYPE_SENTINEL && p->next->pos <= pos)
            p = p->next;
        /* now p is the element that q must be inserted 'in'. */
        pos2 = pos + q->len;
        r = p;
        while (r->next->type != BIDI_TYPE_SENTINEL && r->next->pos < pos2)
            r = r->next;
        if (preserve_length)
            r->len += q->len;
        /* now r is the last element that q affects. */
        if (p == r)
            {
                /* split p into at most 3 intervals, and insert q in the place of
                   the second interval, set r to be the third part. */
                /* third part needed? */
                if (p->pos + p->len > pos2)
                {
                    r = new_run ();
                    if (!r) goto out;
                    p->next->prev = r;
                    r->next = p->next;
                    r->level = p->level;
                    r->isolate_level = p->isolate_level;
                    r->type = p->type;
                    r->len = p->pos + p->len - pos2;
                    r->pos = pos2;
                }
                else
                    r = r->next;

                if (p->pos + p->len >= pos)
                    {
                        /* first part needed? */
                        if (p->pos < pos)
                            /* cut the end of p. */
                            p->len = pos - p->pos;
                        else
                        {
                            t = p;
                            p = p->prev;
                            free (t);
                        }
                    }
            }
        else
        {
            if (p->pos + p->len >= pos)
                {
                    /* p needed? */
                    if (p->pos < pos)
                        /* cut the end of p. */
                        p->len = pos - p->pos;
                    else
                        p = p->prev;
                }

            /* r needed? */
            if (r->pos + r->len > pos2)
            {
                /* cut the beginning of r. */
                r->len = r->pos + r->len - pos2;
                r->pos = pos2;
            }
            else
                r = r->next;

            /* remove the elements between p and r. */
            for (s = p->next; s != r;)
            {
                t = s;
                s = s->next;
                free (t);
            }
        }
        /* before updating the next and prev runs to point to the inserted q,
           we must remember the next element of q in the 'over' list.
         */
        t = q;
        q = q->prev;
        delete_node (t);
        p->next = t;
        t->prev = p;
        t->next = r;
        r->prev = t;
    }
    status = TRUE;

    validate_run_list (base);

out:
    free_run_list (over);

    return status;
}

/* Some convenience macros */
#define RL_TYPE(list) ((list)->type)
#define RL_LEN(list) ((list)->len)
#define RL_LEVEL(list) ((list)->level)

/* "Within this scope, bidirectional types EN and AN are treated as R" */
#define RL_TYPE_AN_EN_AS_RTL(list) ( \
    (((list)->type == BIDI_TYPE_AN) || ((list)->type == BIDI_TYPE_EN) | \
        ((list)->type == BIDI_TYPE_RTL)) ? BIDI_TYPE_RTL : (list)->type)
#define RL_BRACKET_TYPE(list) ((list)->bracket_type)
#define RL_ISOLATE_LEVEL(list) ((list)->isolate_level)

#define LOCAL_BRACKET_SIZE 16

/* Pairing nodes are used for holding a pair of open/close brackets as
   described in BD16. */
typedef struct _BidiPairingNode {
    BidiRun *open;
    BidiRun *close;
    struct _BidiPairingNode *next;
} BidiPairingNode;

static BidiRun* merge_with_prev(BidiRun *second)
{
    BidiRun *first;

    first = second->prev;

    first->next = second->next;
    first->next->prev = first;
    RL_LEN (first) += RL_LEN (second);

    if (second->next_isolate)
        second->next_isolate->prev_isolate = first;
    first->next_isolate = second->next_isolate;

    free(second);
    return first;
}

static void compact_list(BidiRun *list)
{
    if (list->next) {
        for_run_list (list, list) {
            if (RL_TYPE (list->prev) == RL_TYPE (list)
                    && RL_LEVEL (list->prev) == RL_LEVEL (list)
                    /* Don't join brackets! */
                    && RL_BRACKET_TYPE(list) == BIDI_BRACKET_NONE
                    && RL_BRACKET_TYPE(list->prev) == BIDI_BRACKET_NONE) {
                list = merge_with_prev (list);
            }
        }
    }
}

static void compact_neutrals(BidiRun *list)
{
    if (list->next) {
        for_run_list (list, list) {
            if (RL_LEVEL (list->prev) == RL_LEVEL (list)
                    &&
                    ((RL_TYPE (list->prev) == RL_TYPE (list)
                      || (BIDI_IS_NEUTRAL (RL_TYPE (list->prev))
                          && BIDI_IS_NEUTRAL (RL_TYPE (list)))))
                    /* Don't join brackets! */
                    && RL_BRACKET_TYPE(list) == BIDI_BRACKET_NONE
                    && RL_BRACKET_TYPE(list->prev) == BIDI_BRACKET_NONE) {
                list = merge_with_prev (list);
            }
        }
    }
}

/*
 * Search for an adjacent run in the forward or backward direction.
 * It uses the next_isolate and prev_isolate run for short circuited searching.
 */

/* The static sentinel is used to signal the end of an isolating sequence */
static BidiRun sentinel = {
    NULL, NULL, NULL, NULL, 0, 0, BIDI_TYPE_SENTINEL, -1, -1,
    BIDI_BRACKET_NONE,
};

static BidiRun *get_adjacent_run(BidiRun *list,
        BOOL forward, BOOL skip_neutral)
{
    BidiRun *ppp = forward ? list->next_isolate : list->prev_isolate;
    if (!ppp)
        return &sentinel;

    while (ppp) {
        BidiType ppp_type = RL_TYPE (ppp);

        if (ppp_type == BIDI_TYPE_SENTINEL)
            break;

        /* Note that when sweeping forward we continue one run
           beyond the PDI to see what lies behind. When looking
           backwards, this is not necessary as the leading isolate
           run has already been assigned the resolved level. */
        if (ppp->isolate_level > list->isolate_level
                || (forward && ppp_type == BIDI_TYPE_PDI)
                || (skip_neutral && !BIDI_IS_STRONG(ppp_type))) {
            ppp = forward ? ppp->next_isolate : ppp->prev_isolate;
            if (!ppp)
                ppp = &sentinel;

            continue;
        }

        break;
    }

    return ppp;
}

#ifdef _DEBUG
/*======================================================================
 *  For debugging, define some functions for printing the types and the
 *  levels.
 *----------------------------------------------------------------------*/
static const char char_from_level_array[] = {
    /* -1 == BIDI_SENTINEL, indicating start or end of string. */
    '$',
    /* 0-61 == 0-9,a-z,A-Z are the the only valid levels before resolving
     * implicits.  after that the level @ may be appear too. */
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z',

    /* TBD - insert another 125-64 levels */

    /* 62 == only must appear after resolving implicits. */
    '@',

    /* 63 == BIDI_LEVEL_INVALID, internal error,
       this level shouldn't be seen. */
    '!',

    /* >= 64 == overflows, this levels and higher
     * levels show a real bug!. */
    '*', '*', '*', '*', '*'
};

#define unibidi_char_from_level(level) char_from_level_array[(level) + 1]

static const char *unibidi_get_bidi_type_name(BidiType t)
{
    switch ((int)t) {
#define _UNIBIDI_ADD_TYPE(TYPE,SYMBOL) case BIDI_TYPE_##TYPE: return #TYPE;
#define _UNIBIDI_ALL_TYPES
#include "unicode-bidi-types-list.inc"
#undef _UNIBIDI_ALL_TYPES
#undef _UNIBIDI_ADD_TYPE
    default:
        return "?";
    }
}

static void print_types_re(const BidiRun *pp)
{
    _DBG_PRINTF ("  Run types  : ");
    for_run_list (pp, pp) {
        _DBG_PRINTF ("%d:%d(%s)[%d,%d] ",
                pp->pos, pp->len, unibidi_get_bidi_type_name (pp->type), pp->level, pp->isolate_level);
    }
    _DBG_PRINTF ("\n");
}

static void print_resolved_levels(const BidiRun *pp)
{
    _DBG_PRINTF ("  Res. levels: ");
    for_run_list (pp, pp) {
        int i;
        for (i = RL_LEN (pp); i; i--)
            _DBG_PRINTF ("%c", unibidi_char_from_level (RL_LEVEL (pp)));
    }
    _DBG_PRINTF ("\n");
}

static void print_resolved_types(const BidiRun *pp)
{
    _DBG_PRINTF ("  Res. types : ");
    for_run_list (pp, pp) {
        int i;
        for (i = RL_LEN (pp); i; i--)
            _DBG_PRINTF ("%s ", unibidi_get_bidi_type_name (pp->type));
    }
    _DBG_PRINTF ("\n");
}

static void print_bidi_string(const BidiType *bidi_types, int len)
{
    int i;

    _DBG_PRINTF ("  Org. types : ");
    for (i = 0; i < len; i++)
        _DBG_PRINTF ("%s ", unibidi_get_bidi_type_name (bidi_types[i]));
    _DBG_PRINTF ("\n");
}

static void print_pairing_nodes(BidiPairingNode *nodes)
{
    _DBG_PRINTF ("Pairs: ");
    while (nodes) {
        _DBG_PRINTF ("(%d, %d) ", nodes->open->pos, nodes->close->pos);
        nodes = nodes->next;
    }
    _DBG_PRINTF ("\n");
}

#endif /* _DEBUG */

/*=========================================================================
 * define macros for push and pop the status in to / out of the stack
 *-------------------------------------------------------------------------*/

/* There are a few little points in pushing into and poping from the status
   stack:
   1. when the embedding level is not valid (more than
   BIDI_MAX_EXPLICIT_LEVEL=125), you must reject it, and not to push
   into the stack, but when you see a PDF, you must find the matching code,
   and if it was pushed in the stack, pop it, it means you must pop if and
   only if you have pushed the matching code, the over_pushed var counts the
   number of rejected codes so far.

   2. there's a more confusing point too, when the embedding level is exactly
   BIDI_MAX_EXPLICIT_LEVEL-1=124, an LRO, LRE, or LRI is rejected
   because the new level would be BIDI_MAX_EXPLICIT_LEVEL+1=126, that
   is invalid; but an RLO, RLE, or RLI is accepted because the new level is
   BIDI_MAX_EXPLICIT_LEVEL=125, that is valid, so the rejected codes
   may be not continuous in the logical order, in fact there are at most two
   continuous intervals of codes, with an RLO, RLE, or RLI between them.  To
   support this case, the first_interval var counts the number of rejected
   codes in the first interval, when it is 0, means that there is only one
   interval.
*/

/* a. If this new level would be valid, then this embedding code is valid.
   Remember (push) the current embedding level and override status.
   Reset current level to this new level, and reset the override status to
   new_override.
   b. If the new level would not be valid, then this code is invalid. Don't
   change the current level or override status.
*/

#define PUSH_STATUS \
    do { \
      if (over_pushed == 0 \
                && isolate_overflow == 0 \
                && new_level <= BIDI_MAX_EXPLICIT_LEVEL)   \
        { \
          if (level == BIDI_MAX_EXPLICIT_LEVEL - 1) \
            first_interval = over_pushed; \
          status_stack[stack_size].level = level; \
          status_stack[stack_size].isolate_level = isolate_level; \
          status_stack[stack_size].isolate = isolate; \
          status_stack[stack_size].override = override; \
          stack_size++; \
          level = new_level; \
          override = new_override; \
        } else if (isolate_overflow == 0) \
          over_pushed++; \
    } while(0)

/* If there was a valid matching code, restore (pop) the last remembered
   (pushed) embedding level and directional override.
*/
#define POP_STATUS \
    do { \
      if (stack_size) \
      { \
        if (over_pushed > first_interval) \
          over_pushed--; \
        else \
          { \
            if (over_pushed == first_interval) \
              first_interval = 0; \
            stack_size--; \
            level = status_stack[stack_size].level; \
            override = status_stack[stack_size].override; \
            isolate = status_stack[stack_size].isolate; \
            isolate_level = status_stack[stack_size].isolate_level; \
          } \
      } \
    } while(0)


/* Return the type of previous run or the SOR, if already at the start of
   a level run. */
#define PREV_TYPE_OR_SOR(pp) \
    ( \
      RL_LEVEL(pp->prev) == RL_LEVEL(pp) ? \
        RL_TYPE(pp->prev) : \
        BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(pp->prev), RL_LEVEL(pp))) \
    )

/* Return the type of next run or the EOR, if already at the end of
   a level run. */
#define NEXT_TYPE_OR_EOR(pp) \
    ( \
      RL_LEVEL(pp->next) == RL_LEVEL(pp) ? \
        RL_TYPE(pp->next) : \
        BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(pp->next), RL_LEVEL(pp))) \
    )


/* Return the embedding direction of a link. */
#define BIDI_EMBEDDING_DIRECTION(link) \
    BIDI_LEVEL_TO_DIR(RL_LEVEL(link))

BidiType UBidiGetParagraphDir(const BidiType *bidi_types, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (BIDI_IS_LETTER(bidi_types[i]))
            return BIDI_IS_RTL(bidi_types[i]) ? BIDI_PGDIR_RTL : BIDI_PGDIR_LTR;
    }

    return BIDI_PGDIR_ON;
}

/* Push a new entry to the pairing linked list */
static BidiPairingNode * pairing_nodes_push(BidiPairingNode *nodes,
        BidiRun *open, BidiRun *close)
{
    BidiPairingNode *node = malloc(sizeof(BidiPairingNode));
    node->open = open;
    node->close = close;
    node->next = nodes;
    nodes = node;
    return nodes;
}

/* Sort by merge sort */
static void pairing_nodes_front_back_split(BidiPairingNode *source,
        BidiPairingNode **front, BidiPairingNode **back)
{
    BidiPairingNode *pfast, *pslow;
    if (!source || !source->next) {
        *front = source;
        *back = NULL;
    }
    else {
        pslow = source;
        pfast = source->next;
        while (pfast) {
            pfast= pfast->next;
            if (pfast) {
                pfast = pfast->next;
                pslow = pslow->next;
            }
        }

        *front = source;
        *back = pslow->next;
        pslow->next = NULL;
    }
}

static BidiPairingNode * pairing_nodes_sorted_merge(BidiPairingNode *nodes1,
        BidiPairingNode *nodes2)
{
    BidiPairingNode *res = NULL;
    if (!nodes1)
        return nodes2;
    if (!nodes2)
        return nodes1;

    if (nodes1->open->pos < nodes2->open->pos) {
        res = nodes1;
        res->next = pairing_nodes_sorted_merge(nodes1->next, nodes2);
    }
    else {
        res = nodes2;
        res->next = pairing_nodes_sorted_merge(nodes1, nodes2->next);
    }

    return res;
}

static void sort_pairing_nodes(BidiPairingNode **nodes)
{
    BidiPairingNode *front, *back;

    /* 0 or 1 node case */
    if (!*nodes || !(*nodes)->next)
        return;

    pairing_nodes_front_back_split(*nodes, &front, &back);
    sort_pairing_nodes(&front);
    sort_pairing_nodes(&back);
    *nodes = pairing_nodes_sorted_merge(front, back);
}

static void free_pairing_nodes(BidiPairingNode *nodes)
{
    while (nodes)
    {
        BidiPairingNode *p = nodes;
        nodes = nodes->next;
        free(p);
    }
}

BidiLevel UBidiGetParagraphEmbeddingLevels(const BidiType *bidi_types,
      const BidiBracketType *bracket_types, int len,
      BidiType *paragraph_dir, BidiLevel *embedding_levels)
{
    BidiLevel base_level_per_iso_level[BIDI_MAX_EXPLICIT_LEVEL];
    BidiLevel base_level, max_level = 0;
    BidiType base_dir;
    BidiRun *main_run_list = NULL, *explicits_list = NULL, *pp;
    BOOL status = FALSE;
    int max_iso_level = 0;

    if (!len) {
        status = TRUE;
        goto out;
    }

    /* Determinate character types */
    {
        /* Get run-length encoded character types */
        main_run_list = run_list_encode_bidi_types (bidi_types, bracket_types, len);
        if (!main_run_list) goto out;
    }

    /* Find base level */
    /* If no strong base_dir was found, resort to the weak direction
       that was passed on input. */
    base_level = BIDI_DIR_TO_LEVEL (*paragraph_dir);
    if (!BIDI_IS_STRONG (*paragraph_dir))
        /* P2. P3. Search for first strong character and use its direction as
           base direction */
    {
        int valid_isolate_count = 0;
        for_run_list (pp, main_run_list)
        {
            if (RL_TYPE(pp) == BIDI_TYPE_PDI)
            {
                /* Ignore if there is no matching isolate */
                if (valid_isolate_count>0)
                    valid_isolate_count--;
            }
            else if (BIDI_IS_ISOLATE(RL_TYPE(pp)))
                valid_isolate_count++;
            else if (valid_isolate_count==0 && BIDI_IS_LETTER (RL_TYPE (pp)))
            {
                base_level = BIDI_DIR_TO_LEVEL (RL_TYPE (pp));
                //*paragraph_dir = BIDI_LEVEL_TO_DIR (base_level);
                break;
            }
        }
    }
    *paragraph_dir = base_dir = BIDI_LEVEL_TO_DIR (base_level);
    _DBG_PRINTF ("  base level : %c\n", unibidi_char_from_level (base_level));
    _DBG_PRINTF ("  base dir   : %s\n", unibidi_get_bidi_type_name (base_dir));

    base_level_per_iso_level[0] = base_level;

#ifdef _DEBUG
    print_types_re (main_run_list);
#endif        /* _DEBUG */

    /* Explicit Levels and Directions */
    _DBG_PRINTF ("explicit levels and directions\n");

    {
        BidiLevel level, new_level = 0;
        int isolate_level = 0;
        BidiType override, new_override;
        int i;
        int stack_size, over_pushed, first_interval;
        int valid_isolate_count = 0;
        int isolate_overflow = 0;
        int isolate = 0; /* The isolate status flag */
        struct
        {
            BidiType override;        /* only LTR, RTL and ON are valid */
            BidiLevel level;
            int isolate;
            int isolate_level;
        } status_stack[BIDI_MAX_RESOLVED_LEVELS];
        BidiRun temp_link;
        BidiRun *run_per_isolate_level[BIDI_MAX_RESOLVED_LEVELS];

        memset(run_per_isolate_level, 0, sizeof(run_per_isolate_level[0])
                * BIDI_MAX_RESOLVED_LEVELS);

        /* explicits_list is a list like main_run_list, that holds the explicit
           codes that are removed from main_run_list, to reinsert them later by
           calling the shadow_run_list.
         */
        explicits_list = new_run_list ();
        if (!explicits_list) goto out;

        /* X1. Begin by setting the current embedding level to the paragraph
           embedding level. Set the directional override status to neutral,
           and directional isolate status to FALSE.

           Process each character iteratively, applying rules X2 through X8.
           Only embedding levels from 0 to 123 are valid in this phase. */

        level = base_level;
        override = BIDI_TYPE_ON;
        /* stack */
        stack_size = 0;
        over_pushed = 0;
        first_interval = 0;
        valid_isolate_count = 0;
        isolate_overflow = 0;

        for_run_list (pp, main_run_list)
        {
            BidiType this_type = RL_TYPE (pp);
            RL_ISOLATE_LEVEL (pp) = isolate_level;

            if (BIDI_IS_EXPLICIT_OR_BN (this_type))
            {
                if (BIDI_IS_STRONG (this_type))
                {                        /* LRE, RLE, LRO, RLO */
                    /* 1. Explicit Embeddings */
                    /*   X2. With each RLE, compute the least greater odd
                         embedding level. */
                    /*   X3. With each LRE, compute the least greater even
                         embedding level. */
                    /* 2. Explicit Overrides */
                    /*   X4. With each RLO, compute the least greater odd
                         embedding level. */
                    /*   X5. With each LRO, compute the least greater even
                         embedding level. */
                    new_override = BIDI_EXPLICIT_TO_OVERRIDE_DIR (this_type);
                    for (i = RL_LEN (pp); i; i--)
                    {
                        new_level =
                            ((level + BIDI_DIR_TO_LEVEL (this_type) + 2) & ~1) -
                            BIDI_DIR_TO_LEVEL (this_type);
                        isolate = 0;
                        PUSH_STATUS;
                    }
                }
                else if (this_type == BIDI_TYPE_PDF)
                {
                    /* 3. Terminating Embeddings and overrides */
                    /*   X7. With each PDF, determine the matching embedding or
                         override code. */
                    for (i = RL_LEN (pp); i; i--)
                    {
                        if (stack_size && status_stack[stack_size-1].isolate != 0)
                            break;
                        POP_STATUS;
                    }
                }

                /* X9. Remove all RLE, LRE, RLO, LRO, PDF, and BN codes. */
                /* Remove element and add it to explicits_list */
                RL_LEVEL (pp) = BIDI_SENTINEL;
                temp_link.next = pp->next;
                move_node_before (pp, explicits_list);
                pp = &temp_link;
            }
            else if (this_type == BIDI_TYPE_PDI)
                /* X6a. pop the direction of the stack */
            {
                for (i = RL_LEN (pp); i; i--)
                {
                    if (isolate_overflow > 0)
                    {
                        isolate_overflow--;
                        RL_LEVEL (pp) = level;
                    }

                    else if (valid_isolate_count > 0)
                    {
                        /* Pop away all LRE,RLE,LRO, RLO levels
                           from the stack, as these are implicitly
                           terminated by the PDI */
                        while (stack_size && !status_stack[stack_size-1].isolate)
                            POP_STATUS;
                        over_pushed = 0; /* The PDI resets the overpushed! */
                        POP_STATUS;
                        isolate_level-- ;
                        valid_isolate_count--;
                        RL_LEVEL (pp) = level;
                        RL_ISOLATE_LEVEL (pp) = isolate_level;
                    }
                    else
                    {
                        /* Ignore isolated PDI's by turning them into ON's */
                        RL_TYPE (pp) = BIDI_TYPE_ON;
                        RL_LEVEL (pp) = level;
                    }
                }
            }
            else if (BIDI_IS_ISOLATE(this_type))
            {
                /* TBD support RL_LEN > 1 */
                new_override = BIDI_TYPE_ON;
                isolate = 1;
                if (this_type == BIDI_TYPE_LRI)
                    new_level = level + 2 - (level%2);
                else if (this_type == BIDI_TYPE_RLI)
                    new_level = level + 1 + (level%2);
                else if (this_type == BIDI_TYPE_FSI)
                {
                    /* Search for a local strong character until we
                       meet the corresponding PDI or the end of the
                       paragraph */
                    BidiRun *fsi_pp;
                    int isolate_count = 0;
                    int fsi_base_level = 0;
                    for_run_list (fsi_pp, pp)
                    {
                        if (RL_TYPE(fsi_pp) == BIDI_TYPE_PDI)
                        {
                            isolate_count--;
                            if (valid_isolate_count < 0)
                                break;
                        }
                        else if (BIDI_IS_ISOLATE(RL_TYPE(fsi_pp)))
                            isolate_count++;
                        else if (isolate_count==0 && BIDI_IS_LETTER (RL_TYPE (fsi_pp)))
                        {
                            fsi_base_level = BIDI_DIR_TO_LEVEL (RL_TYPE (fsi_pp));
                            break;
                        }
                    }

                    /* Same behavior like RLI and LRI above */
                    if (BIDI_LEVEL_IS_RTL (fsi_base_level))
                        new_level = level + 1 + (level%2);
                    else
                        new_level = level + 2 - (level%2);
                }

                RL_LEVEL (pp) = level;
                RL_ISOLATE_LEVEL (pp) = isolate_level++;
                base_level_per_iso_level[isolate_level] = new_level;

                if (!BIDI_IS_NEUTRAL (override))
                    RL_TYPE (pp) = override;

                if (new_level <= BIDI_MAX_EXPLICIT_LEVEL)
                {
                    valid_isolate_count++;
                    PUSH_STATUS;
                    level = new_level;
                }
                else
                    isolate_overflow += 1;
            }
            else if (this_type == BIDI_TYPE_BS)
            {
                /* X8. All explicit directional embeddings and overrides are
                   completely terminated at the end of each paragraph. Paragraph
                   separators are not included in the embedding. */
                break;
            }
            else
            {
                /* X6. For all types besides RLE, LRE, RLO, LRO, and PDF:
                   a. Set the level of the current character to the current
                   embedding level.
                   b. Whenever the directional override status is not neutral,
                   reset the current character type to the directional override
                   status. */
                RL_LEVEL (pp) = level;
                if (!BIDI_IS_NEUTRAL (override))
                    RL_TYPE (pp) = override;
            }
        }

        /* Build the isolate_level connections */
        for_run_list (pp, main_run_list)
        {
            int isolate_level = RL_ISOLATE_LEVEL (pp);
            if (run_per_isolate_level[isolate_level])
            {
                run_per_isolate_level[isolate_level]->next_isolate = pp;
                pp->prev_isolate = run_per_isolate_level[isolate_level];
            }
            run_per_isolate_level[isolate_level] = pp;
        }

        /* Implementing X8. It has no effect on a single paragraph! */
        level = base_level;
        override = BIDI_TYPE_ON;
        stack_size = 0;
        over_pushed = 0;
    }
    /* X10. The remaining rules are applied to each run of characters at the
       same level. For each run, determine the start-of-level-run (sor) and
       end-of-level-run (eor) type, either L or R. This depends on the
       higher of the two levels on either side of the boundary (at the start
       or end of the paragraph, the level of the 'other' run is the base
       embedding level). If the higher level is odd, the type is R, otherwise
       it is L. */
    /* Resolving Implicit Levels can be done out of X10 loop, so only change
       of Resolving Weak Types and Resolving Neutral Types is needed. */

    compact_list (main_run_list);

#ifdef _DEBUG
    print_types_re (main_run_list);
    print_bidi_string (bidi_types, len);
    print_resolved_levels (main_run_list);
    print_resolved_types (main_run_list);
#endif /* _DEBUG */

    /* 4. Resolving weak types. Also calculate the maximum isolate level */
    max_iso_level = 0;
    _DBG_PRINTF ("resolving weak types\n");
    {
        int last_strong_stack[BIDI_MAX_RESOLVED_LEVELS];
        BidiType prev_type_orig;
        BOOL w4;

        last_strong_stack[0] = base_dir;

        for_run_list (pp, main_run_list)
        {
            BidiType prev_type, this_type, next_type;
            BidiRun *ppp_prev, *ppp_next;
            int iso_level;

            ppp_prev = get_adjacent_run(pp, FALSE, FALSE);
            ppp_next = get_adjacent_run(pp, TRUE, FALSE);

            this_type = RL_TYPE (pp);
            iso_level = RL_ISOLATE_LEVEL(pp);

            if (iso_level > max_iso_level)
                max_iso_level = iso_level;

            if (RL_LEVEL(ppp_prev) == RL_LEVEL(pp))
                prev_type = RL_TYPE(ppp_prev);
            else
                prev_type = BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_prev), RL_LEVEL(pp)));

            if (RL_LEVEL(ppp_next) == RL_LEVEL(pp))
                next_type = RL_TYPE(ppp_next);
            else
                next_type = BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_next), RL_LEVEL(pp)));

            if (BIDI_IS_STRONG (prev_type))
                last_strong_stack[iso_level] = prev_type;

            /* W1. NSM
               Examine each non-spacing mark (NSM) in the level run, and change the
               type of the NSM to the type of the previous character. If the NSM
               is at the start of the level run, it will get the type of sor. */
            /* Implementation note: it is important that if the previous character
               is not sor, then we should merge this run with the previous,
               because of rules like W5, that we assume all of a sequence of
               adjacent ETs are in one BidiRun. */
            if (this_type == BIDI_TYPE_NSM)
            {
                /* New rule in Unicode 6.3 */
                if (BIDI_IS_ISOLATE (RL_TYPE (pp->prev)))
                    RL_TYPE(pp) = BIDI_TYPE_ON;

                if (RL_LEVEL (ppp_prev) == RL_LEVEL (pp))
                {
                    if (ppp_prev == pp->prev)
                        pp = merge_with_prev (pp);
                }
                else
                    RL_TYPE (pp) = prev_type;

                if (prev_type == next_type && RL_LEVEL (pp) == RL_LEVEL (pp->next))
                {
                    if (ppp_next == pp->next)
                        pp = merge_with_prev (pp->next);
                }
                continue;                /* As we know the next condition cannot be TRUE. */
            }

            /* W2: European numbers. */
            if (this_type == BIDI_TYPE_EN && last_strong_stack[iso_level] == BIDI_TYPE_AL)
            {
                RL_TYPE (pp) = BIDI_TYPE_AN;

                /* Resolving dependency of loops for rules W1 and W2, so we
                   can merge them in one loop. */
                if (next_type == BIDI_TYPE_NSM)
                    RL_TYPE (ppp_next) = BIDI_TYPE_AN;
            }
        }


        last_strong_stack[0] = base_dir;

        /* Resolving dependency of loops for rules W4 and W5, W5 may
           want to prevent W4 to take effect in the next turn, do this
           through "w4". */
        w4 = TRUE;
        /* Resolving dependency of loops for rules W4 and W5 with W7,
           W7 may change an EN to L but it sets the prev_type_orig if needed,
           so W4 and W5 in next turn can still do their works. */
        prev_type_orig = BIDI_TYPE_ON;

        /* Each isolate level has its own memory of the last strong character */
        for_run_list (pp, main_run_list)
        {
            BidiType prev_type, this_type, next_type;
            int iso_level;
            BidiRun *ppp_prev, *ppp_next;

            this_type = RL_TYPE (pp);
            iso_level = RL_ISOLATE_LEVEL(pp);

            ppp_prev = get_adjacent_run(pp, FALSE, FALSE);
            ppp_next = get_adjacent_run(pp, TRUE, FALSE);

            if (RL_LEVEL(ppp_prev) == RL_LEVEL(pp))
                prev_type = RL_TYPE(ppp_prev);
            else
                prev_type = BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_prev), RL_LEVEL(pp)));

            if (RL_LEVEL(ppp_next) == RL_LEVEL(pp))
                next_type = RL_TYPE(ppp_next);
            else
                next_type = BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_next), RL_LEVEL(pp)));

            if (BIDI_IS_STRONG (prev_type))
                last_strong_stack[iso_level] = prev_type;

            /* W2 ??? */

            /* W3: Change ALs to R. */
            if (this_type == BIDI_TYPE_AL)
            {
                RL_TYPE (pp) = BIDI_TYPE_RTL;
                w4 = TRUE;
                prev_type_orig = BIDI_TYPE_ON;
                continue;
            }

            /* W4. A single european separator changes to a european number.
               A single common separator between two numbers of the same type
               changes to that type. */
            if (w4
                    && RL_LEN (pp) == 1 && BIDI_IS_ES_OR_CS (this_type)
                    && BIDI_IS_NUMBER (prev_type_orig)
                    && prev_type_orig == next_type
                    && (prev_type_orig == BIDI_TYPE_EN
                        || this_type == BIDI_TYPE_CS))
            {
                RL_TYPE (pp) = prev_type;
                this_type = RL_TYPE (pp);
            }
            w4 = TRUE;

            /* W5. A sequence of European terminators adjacent to European
               numbers changes to All European numbers. */
            if (this_type == BIDI_TYPE_ET
                    && (prev_type_orig == BIDI_TYPE_EN
                        || next_type == BIDI_TYPE_EN))
            {
                RL_TYPE (pp) = BIDI_TYPE_EN;
                w4 = FALSE;
                this_type = RL_TYPE (pp);
            }

            /* W6. Otherwise change separators and terminators to other neutral. */
            if (BIDI_IS_NUMBER_SEPARATOR_OR_TERMINATOR (this_type))
                RL_TYPE (pp) = BIDI_TYPE_ON;

            /* W7. Change european numbers to L. */
            if (this_type == BIDI_TYPE_EN && last_strong_stack[iso_level] == BIDI_TYPE_LTR)
            {
                RL_TYPE (pp) = BIDI_TYPE_LTR;
                prev_type_orig = (RL_LEVEL (pp) == RL_LEVEL (pp->next) ?
                        BIDI_TYPE_EN : BIDI_TYPE_ON);
            }
            else
                prev_type_orig = PREV_TYPE_OR_SOR (pp->next);
        }
    }

    compact_neutrals (main_run_list);

#ifdef _DEBUG
    print_resolved_levels (main_run_list);
    print_resolved_types (main_run_list);
#endif /* _DEBUG */

    /* 5. Resolving Neutral Types */

    _DBG_PRINTF ("resolving neutral types - N0\n");

    {
        /*  BD16 - Build list of all pairs*/
        int num_iso_levels = max_iso_level + 1;
        BidiPairingNode *pairing_nodes = NULL;
        BidiRun *local_bracket_stack[BIDI_MAX_EXPLICIT_LEVEL][LOCAL_BRACKET_SIZE];
        BidiRun **bracket_stack[BIDI_MAX_EXPLICIT_LEVEL];
        int bracket_stack_size[BIDI_MAX_EXPLICIT_LEVEL];
        int last_level = RL_LEVEL(main_run_list);
        int last_iso_level = 0;

        memset(bracket_stack, 0, sizeof(bracket_stack[0])*num_iso_levels);
        memset(bracket_stack_size, 0, sizeof(bracket_stack_size[0])*num_iso_levels);

        /* populate the bracket_size. The first LOCAL_BRACKET_SIZE entries
           of the stack are one the stack. Allocate the rest of the entries.
         */
        {
            int iso_level;
            for (iso_level=0; iso_level < LOCAL_BRACKET_SIZE; iso_level++)
                bracket_stack[iso_level] = local_bracket_stack[iso_level];

            for (iso_level=LOCAL_BRACKET_SIZE; iso_level < num_iso_levels; iso_level++)
                bracket_stack[iso_level] = malloc (sizeof (bracket_stack[0])
                        * BIDI_MAX_NESTED_BRACKET_PAIRS);
        }

        /* Build the bd16 pair stack. */
        for_run_list (pp, main_run_list)
        {
            int level = RL_LEVEL(pp);
            int iso_level = RL_ISOLATE_LEVEL(pp);
            BidiBracketType brack_prop = RL_BRACKET_TYPE(pp);

            /* Interpret the isolating run sequence as such that they
               end at a change in the level, unless the iso_level has been
               raised. */
            if (level != last_level && last_iso_level == iso_level)
                bracket_stack_size[last_iso_level] = 0;

            if (brack_prop!= BIDI_BRACKET_NONE
                    && RL_TYPE(pp)==BIDI_TYPE_ON) {
                if (BIDI_IS_BRACKET_OPEN(brack_prop)) {
                    if (bracket_stack_size[iso_level] ==
                            BIDI_MAX_NESTED_BRACKET_PAIRS)
                        break;

                    /* push onto the pair stack */
                    bracket_stack[iso_level][bracket_stack_size[iso_level]++] = pp;
                }
                else {
                    int stack_idx = bracket_stack_size[iso_level] - 1;
                    while (stack_idx >= 0) {
                        BidiBracketType se_brack_prop =
                        RL_BRACKET_TYPE(bracket_stack[iso_level][stack_idx]);
                        if (BIDI_BRACKET_CHAR(se_brack_prop) ==
                                BIDI_BRACKET_CHAR(brack_prop)) {
                            bracket_stack_size[iso_level] = stack_idx;

                            pairing_nodes = pairing_nodes_push(pairing_nodes,
                                    bracket_stack[iso_level][stack_idx],
                                    pp);
                            break;
                        }
                        stack_idx--;
                    }
                }
            }
            last_level = level;
            last_iso_level = iso_level;
        }

        /* The list must now be sorted for the next algo to work! */
        sort_pairing_nodes(&pairing_nodes);

#ifdef _DEBUG
        print_pairing_nodes (pairing_nodes);
#endif /* _DEBUG */

        /* Start the N0 */
        {
            BidiPairingNode *ppairs = pairing_nodes;
            while (ppairs)
            {
                int iso_level = ppairs->open->isolate_level;
                int embedding_level = base_level_per_iso_level[iso_level];

                /* Find matching strong. */
                BOOL found = FALSE;
                BidiRun *ppn;
                for (ppn = ppairs->open; ppn!= ppairs->close; ppn = ppn->next)
                {
                    BidiType this_type = RL_TYPE_AN_EN_AS_RTL(ppn);

                    /* Calculate level like in resolve implicit levels below to prevent
                       embedded levels not to match the base_level */
                    int this_level = RL_LEVEL (ppn) +
                        (BIDI_LEVEL_IS_RTL (RL_LEVEL(ppn)) ^ BIDI_DIR_TO_LEVEL (this_type));

                    /* N0b */
                    if (BIDI_IS_STRONG (this_type) && this_level == embedding_level)
                    {
                        RL_TYPE(ppairs->open) = RL_TYPE(ppairs->close) = this_level%2 ? BIDI_TYPE_RTL : BIDI_TYPE_LTR;
                        found = TRUE;
                        break;
                    }
                }

                /* N0c */
                /* Search for any strong type preceding and within the bracket pair */
                if (!found)
                {
                    /* Search for a preceding strong */
                    int prec_strong_level = embedding_level; /* TBDov! Extract from Isolate level in effect */
                    int iso_level = RL_ISOLATE_LEVEL(ppairs->open);
                    for (ppn = ppairs->open->prev; ppn->type != BIDI_TYPE_SENTINEL; ppn=ppn->prev)
                    {
                        BidiType this_type = RL_TYPE_AN_EN_AS_RTL(ppn);
                        if (BIDI_IS_STRONG (this_type) && RL_ISOLATE_LEVEL(ppn) == iso_level)
                        {
                            prec_strong_level = RL_LEVEL (ppn) +
                                (BIDI_LEVEL_IS_RTL (RL_LEVEL(ppn)) ^ BIDI_DIR_TO_LEVEL (this_type));

                            break;
                        }
                    }

                    for (ppn = ppairs->open; ppn!= ppairs->close; ppn = ppn->next)
                    {
                        BidiType this_type = RL_TYPE_AN_EN_AS_RTL(ppn);
                        if (BIDI_IS_STRONG (this_type) && RL_ISOLATE_LEVEL(ppn) == iso_level)
                        {
                            /* By constraint this is opposite the embedding direction,
                               since we did not match the N0b rule. We must now
                               compare with the preceding strong to establish whether
                               to apply N0c1 (opposite) or N0c2 embedding */
                            RL_TYPE(ppairs->open) = RL_TYPE(ppairs->close) = prec_strong_level % 2 ? BIDI_TYPE_RTL : BIDI_TYPE_LTR;
                            RL_LEVEL(ppairs->open) = RL_LEVEL(ppairs->close) = prec_strong_level;
                            found = TRUE;
                            break;
                        }
                    }
                }

                ppairs = ppairs->next;
            }

            free_pairing_nodes(pairing_nodes);

            if (num_iso_levels >= LOCAL_BRACKET_SIZE)
            {
                int i;
                /* Only need to free the non static members */
                for (i=LOCAL_BRACKET_SIZE; i<num_iso_levels; i++)
                    free(bracket_stack[i]);
            }

            /* Remove the bracket property and re-compact */
            {
                const BidiBracketType NoBracket = BIDI_BRACKET_NONE;
                for_run_list (pp, main_run_list)
                    pp->bracket_type = NoBracket;
                compact_neutrals (main_run_list);
            }
        }

#ifdef _DEBUG
        print_resolved_levels (main_run_list);
        print_resolved_types (main_run_list);
#endif /* _DEBUG */
    }

    _DBG_PRINTF ("resolving neutral types - N1+N2\n");
    {
        for_run_list (pp, main_run_list)
        {
            BidiType prev_type, this_type, next_type;
            BidiRun *ppp_prev, *ppp_next;

            ppp_prev = get_adjacent_run(pp, FALSE, FALSE);
            ppp_next = get_adjacent_run(pp, TRUE, FALSE);

            /* "European and Arabic numbers are treated as though they were R"
               BIDI_CHANGE_NUMBER_TO_RTL does this. */
            this_type = BIDI_CHANGE_NUMBER_TO_RTL (RL_TYPE (pp));

            if (RL_LEVEL(ppp_prev) == RL_LEVEL(pp))
                prev_type = BIDI_CHANGE_NUMBER_TO_RTL (RL_TYPE(ppp_prev));
            else
                prev_type = BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_prev), RL_LEVEL(pp)));

            if (RL_LEVEL(ppp_next) == RL_LEVEL(pp))
                next_type = BIDI_CHANGE_NUMBER_TO_RTL (RL_TYPE(ppp_next));
            else
                next_type = BIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_next), RL_LEVEL(pp)));

            if (BIDI_IS_NEUTRAL (this_type))
                RL_TYPE (pp) = (prev_type == next_type) ?
                    /* N1. */ prev_type :
                    /* N2. */ BIDI_EMBEDDING_DIRECTION (pp);
        }
    }

    compact_list (main_run_list);

#ifdef _DEBUG
    print_resolved_levels (main_run_list);
    print_resolved_types (main_run_list);
#endif /* _DEBUG */

    /* 6. Resolving implicit levels */
    _DBG_PRINTF ("resolving implicit levels\n");
    {
        max_level = base_level;

        for_run_list (pp, main_run_list)
        {
            BidiType this_type;
            int level;

            this_type = RL_TYPE (pp);
            level = RL_LEVEL (pp);

            /* I1. Even */
            /* I2. Odd */
            if (BIDI_IS_NUMBER (this_type))
                RL_LEVEL (pp) = (level + 2) & ~1;
            else
                RL_LEVEL (pp) =
                    level +
                    (BIDI_LEVEL_IS_RTL (level) ^ BIDI_DIR_TO_LEVEL (this_type));

            if (RL_LEVEL (pp) > max_level)
                max_level = RL_LEVEL (pp);
        }
    }

    compact_list (main_run_list);

#ifdef _DEBUG
    print_bidi_string (bidi_types, len);
    print_resolved_levels (main_run_list);
    print_resolved_types (main_run_list);
#endif /* _DEBUG */

    /* Reinsert the explicit codes & BN's that are already removed, from the
       explicits_list to main_run_list. */
    _DBG_PRINTF ("reinserting explicit codes\n");
    if (explicits_list->next != explicits_list) {
            BidiRun *p;
            BOOL stat =
                shadow_run_list (main_run_list, explicits_list, TRUE);
            explicits_list = NULL;
            if (!stat) goto out;

            /* Set level of inserted explicit chars to that of their previous
             * char, such that they do not affect reordering. */
            p = main_run_list->next;
            if (p != main_run_list && p->level == BIDI_SENTINEL)
                p->level = base_level;
            for_run_list (p, main_run_list) if (p->level == BIDI_SENTINEL)
                p->level = p->prev->level;
        }

#ifdef _DEBUG
    print_types_re (main_run_list);
    print_resolved_levels (main_run_list);
    print_resolved_types (main_run_list);
#endif /* _DEBUG */

    _DBG_PRINTF ("reset the embedding levels, 1, 2, 3.\n");
    {
        int j, state, pos;
        BidiType char_type;
        BidiRun *p, *q, *list;

        /* L1. Reset the embedding levels of some chars:
           1. segment separators,
           2. paragraph separators,
           3. any sequence of whitespace characters preceding a segment
           separator or paragraph separator, and
           4. any sequence of whitespace characters and/or isolate formatting
           characters at the end of the line.
           ... (to be continued in UBidiReorderLine()). */
        list = new_run_list ();
        if (!list) goto out;
        q = list;
        state = 1;
        pos = len - 1;
        for (j = len - 1; j >= -1; j--)
        {
            /* close up the open link at the end */
            if (j >= 0)
                char_type = bidi_types[j];
            else
                char_type = BIDI_TYPE_ON;
            if (!state && BIDI_IS_SEPARATOR (char_type))
            {
                state = 1;
                pos = j;
            }
            else if (state &&
                    !(BIDI_IS_EXPLICIT_OR_SEPARATOR_OR_BN_OR_WS(char_type)
                        || BIDI_IS_ISOLATE(char_type)))
            {
                state = 0;
                p = new_run ();
                if (!p) {
                    free_run_list (list);
                    goto out;
                }
                p->pos = j + 1;
                p->len = pos - j;
                p->type = base_dir;
                p->level = base_level;
                move_node_before (p, q);
                q = p;
            }
        }
        if (!shadow_run_list (main_run_list, list, FALSE)) goto out;
    }

#ifdef _DEBUG
    print_types_re (main_run_list);
    print_resolved_levels (main_run_list);
    print_resolved_types (main_run_list);
#endif /* _DEBUG */

    {
        int pos = 0;
        for_run_list (pp, main_run_list)
        {
            int l;
            BidiLevel level = pp->level;
            for (l = pp->len; l; l--)
                embedding_levels[pos++] = level;
        }
    }

    status = TRUE;

out:
    if (main_run_list)
        free_run_list (main_run_list);
    if (explicits_list) free_run_list (explicits_list);

    return status ? max_level + 1 : 0;
}


static void bidi_string_reverse(Uchar32 *str, int len)
{
    int i;

    for (i = 0; i < len / 2; i++) {
        Uchar32 tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

static void index_array_reverse (int *arr, int len)
{
    int i;

    for (i = 0; i < len / 2; i++) {
        int tmp = arr[i];
        arr[i] = arr[len - 1 - i];
        arr[len - 1 - i] = tmp;
    }
}

BidiLevel UBidiReorderLine(Uint32 flags,
        const BidiType *bidi_types, int len, int off,
        BidiType paragraph_dir, BidiLevel *embedding_levels,
        Uchar32 *visual_str, int *map,
        void* extra, CB_REVERSE_ARRAY cb_reverse_extra)
{
    BOOL status = FALSE;
    BidiLevel max_level = 0;

    if (len == 0) {
        status = TRUE;
        goto out;
    }

    {
        int i;

        /* L1. Reset the embedding levels of some chars:
           4. any sequence of white space characters at the end of the line. */
        _DBG_PRINTF ("%s: L1. Reset the embedding levels of some chars:\n"
                "4. any sequence of white space characters at the end of the line.\n",
                __FUNCTION__);

        for (i = off + len - 1; i >= off &&
                BIDI_IS_EXPLICIT_OR_BN_OR_WS (bidi_types[i]); i--)
            embedding_levels[i] = BIDI_DIR_TO_LEVEL (paragraph_dir);
    }

    /* 7. Reordering resolved levels */
    {
        BidiLevel level;
        int i;

        /* Reorder both the outstring and the order array */
        {
            if (BIDI_TEST_BITS (flags, BIDI_FLAG_REORDER_NSM)) {
                /* L3. Reorder NSMs. */
                for (i = off + len - 1; i >= off; i--) {
                    if (BIDI_LEVEL_IS_RTL (embedding_levels[i])
                            && bidi_types[i] == BIDI_TYPE_NSM)
                    {
                        int seq_end = i;
                        level = embedding_levels[i];

                        for (i--; i >= off &&
                                BIDI_IS_EXPLICIT_OR_BN_OR_NSM (bidi_types[i])
                                && embedding_levels[i] == level; i--)
                            ;

                        if (i < off || embedding_levels[i] != level) {
                            i++;
                            _DBG_PRINTF ("%s, warning: NSM(s) at the beginning of level run\n",
                                __FUNCTION__);
                        }

                        if (visual_str) {
                            bidi_string_reverse (visual_str + i, seq_end - i + 1);
                        }

                        if (map) {
                            index_array_reverse (map + i, seq_end - i + 1);
                        }

                        if (extra && cb_reverse_extra) {
                            cb_reverse_extra(extra, seq_end - i + 1, i);
                        }
                    }
                }
            }

            /* Find max_level of the line.  We don't reuse the paragraph
             * max_level, both for a cleaner API, and that the line max_level
             * may be far less than paragraph max_level. */
            for (i = off + len - 1; i >= off; i--)
                if (embedding_levels[i] > max_level)
                    max_level = embedding_levels[i];

            /* L2. Reorder. */
            for (level = max_level; level > 0; level--) {
                for (i = off + len - 1; i >= off; i--) {
                    if (embedding_levels[i] >= level) {
                        /* Find all stretches that are >= level_idx */
                        int seq_end = i;
                        for (i--; i >= off && embedding_levels[i] >= level; i--)
                            ;

                        if (visual_str)
                            bidi_string_reverse (visual_str + i + 1, seq_end - i);

                        if (map)
                            index_array_reverse (map + i + 1, seq_end - i);

                        if (extra && cb_reverse_extra) {
                            cb_reverse_extra(extra, seq_end - i + 1, i);
                        }
                    }
                }
            }
        }
    }

    status = TRUE;

out:
    return status ? max_level + 1 : 0;
}

/* Local array size, used for stack-based local arrays */
#if SIZEOF_PTR == 8
#   define LOCAL_ARRAY_SIZE 256
#else
#   define LOCAL_ARRAY_SIZE 128
#endif

BidiLevel GUIAPI UBidiGetParagraphEmbeddingLevelsAlt(
        const Uchar32* ucs, int nr_ucs,
        ParagraphDir *paragraph_dir, BidiLevel *els)
{
    int i;
    ParagraphDir base_dir = *paragraph_dir;
    BidiLevel max_level = 0;

    BidiType local_bidi_ts[LOCAL_ARRAY_SIZE];
    BidiType *bidi_ts = NULL;
    BidiType ored_types = 0;
    BidiType anded_strongs = BIDI_TYPE_RLE;

    BidiBracketType local_brk_ts[LOCAL_ARRAY_SIZE];
    BidiBracketType *brk_ts = NULL;

    /* NOTE:
     * BIDI_IS_ISSOLATE, BIDI_IS_NUMBER, and BIDI_IS_LETTER
     * can not be applied to ored_types */
    BOOL have_isolate = FALSE;
    BOOL have_number = FALSE;
    BOOL have_letter = FALSE;

    if (!els) {
        _DBG_PRINTF("%s: Embedding levels is NULL.\n",
            __FUNCTION__);
        goto out;
    }

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        bidi_ts = local_bidi_ts;
    else
        bidi_ts = malloc(nr_ucs * sizeof(BidiType));

    if (!bidi_ts) {
        _DBG_PRINTF("%s: failed to allocate space for bidi types.\n",
            __FUNCTION__);
        goto out;
    }

    if (nr_ucs < LOCAL_ARRAY_SIZE)
        brk_ts = local_brk_ts;
    else
        brk_ts = (BidiBracketType*)malloc (nr_ucs * sizeof(BidiBracketType));

    if (!brk_ts) {
        _DBG_PRINTF("%s: failed to allocate space for bracket types.\n",
            __FUNCTION__);
        goto out;
    }

    for (i = 0; i < nr_ucs; i++) {
        BidiType bidi_type;
        bidi_ts[i] = bidi_type = UCharGetBidiType(ucs[i]);
        ored_types |= bidi_type;

        if (BIDI_IS_ISOLATE(bidi_type) || BIDI_IS_EXPLICIT(bidi_type))
            have_isolate = TRUE;
        if (BIDI_IS_NUMBER(bidi_type))
            have_number = TRUE;
        if (BIDI_IS_LETTER(bidi_type))
            have_letter = TRUE;

        if (BIDI_IS_STRONG (bidi_type))
            anded_strongs &= bidi_type;

        if (bidi_ts[i] == BIDI_TYPE_ON)
            brk_ts[i] = UCharGetBracketType(ucs[i]);
        else
            brk_ts[i] = BIDI_BRACKET_NONE;
    }

    /* Short-circuit (malloc-expensive) Bidi call for unidirectional text. */

    /* The case that all resolved levels will be ltr.
     * No isolates, all strongs be LTR, there should be no Arabic numbers
     * (or letters for that matter), and one of the following:
     *
     * o base_dir doesn't have an RTL taste.
     * o there are letters, and base_dir is weak.
     */
    if (!have_isolate /* BIDI_IS_ISOLATE (ored_types) */ &&
            !BIDI_IS_RTL (ored_types) &&
            !BIDI_IS_ARABIC (ored_types) &&
            (!BIDI_IS_RTL (base_dir) ||
             (BIDI_IS_WEAK (base_dir) &&
              have_letter /* BIDI_IS_LETTER (ored_types) */)
            ))
    {
        _DBG_PRINTF("%s: have_isolate: %s, ored_types, base_dir: ");

        /* all LTR */
        base_dir = BIDI_PGDIR_LTR;
        memset (els, 0, nr_ucs);
        max_level = 1;
    }
    /* The case that all resolved levels will be RTL is much more complex.
     * No isolates, no numbers, all strongs are RTL, and one of
     * the following:
     *
     * o base_dir has an RTL taste (may be weak).
     * o there are letters, and base_dir is weak.
     */
    else if (!have_isolate /* BIDI_IS_ISOLATE (ored_types) */ &&
            !have_number /* BIDI_IS_NUMBER (ored_types) */ &&
            BIDI_IS_RTL (anded_strongs) &&
            (BIDI_IS_RTL (base_dir) ||
             (BIDI_IS_WEAK (base_dir) &&
              have_letter /* BIDI_IS_LETTER (ored_types) */)
            ))
    {
        /* all RTL */
        base_dir = BIDI_PGDIR_RTL;
        memset (els, 1, nr_ucs);
        max_level = 2;
    }
    else {
        max_level = UBidiGetParagraphEmbeddingLevels(bidi_ts, brk_ts, nr_ucs,
                &base_dir, els);
        if (max_level == 0) {
            _DBG_PRINTF("%s: failed to get paragraph embedding levels.\n",
                __FUNCTION__);
            memset (els, 0, nr_ucs);
        }
    }

    *paragraph_dir = (base_dir == BIDI_PGDIR_LTR) ? BIDI_PGDIR_LTR : BIDI_PGDIR_RTL;

out:
    if (bidi_ts && bidi_ts != local_bidi_ts)
        free (bidi_ts);

    if (brk_ts && brk_ts != local_brk_ts)
        free (brk_ts);

    return max_level;
}

#endif /* _MGCHARSET_UNICODE */
