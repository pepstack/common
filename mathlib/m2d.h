/**
 * m2d.h
 *   2D Graphics and Math API.
 */
#ifndef MATHLIB_M2D_H_
#define MATHLIB_M2D_H_

#if defined(__cplusplus)
extern "C" {
#endif


#include <stdint.h>

#define m2dMIN(a, b)    ((a) < (b)? (a) : ((a) > (b)? (b) : (a)))

#define m2dMAX(a, b)    ((a) > (b)? (a) : ((a) < (b)? (b) : (a)))

#define m2dSGN(v)       ((v) > 0? 1 : ((v) < 0? (-1) : 0))

#define m2dABS(v)       ((v) > 0? (v) : ((v) < 0? (-(v)) : 0))

/**
 *   1: a > b
 *   0: a == b
 *  -1: a < b
 */
#define m2dCMP(a, b)    ((a) > (b) ? 1 : ((a) < (b) ? (-1) : 0))


/**
 * 2d point data type
 */
typedef struct M2D_POINT_F
{
    float x, y;
} m2dPointF;


typedef struct M2D_POINT_D
{
    double x, y;
} m2dPointD;


typedef struct M2D_POINT
{
    int32_t x, y;
} m2dPoint;


typedef struct M2D_POINT_L
{
    int64_t x, y;
} m2dPointL;


/**
 * rectangle data type
 */
typedef struct M2D_RECT_F
{
    float left, top, right, bottom;
} m2dRectF;


typedef struct M2D_RECT_D
{
    double left, top, right, bottom;
} m2dRectD;


typedef struct M2D_RECT
{
    int32_t left, top, right, bottom;
} m2dRect;


typedef struct M2D_RECT_L
{
    int64_t left, top, right, bottom;
} m2dRectL;


#define m2dRectSet(rc, type, l, t, r, b)  (rc).left = (type)(l); (rc).top = (type)(t); (rc).right = (type)(r); (rc).bottom = (type)(b)

#define m2dRectIsEmpty(rc, epsilon)  ((rc).right - (rc).left < (epsilon) || (rc).right - (rc).top < (epsilon))

#define m2dRectNotIntersect(a, b)  ((a).right < (b).left || (a).bottom < (b).top || (b).right < (a).left || (b).bottom < (a).top)

#define m2dRectHasNeg(rc)   ((rc).right < 0 || (rc).left < 0 || (rc).bottom < 0 || (rc).top < 0)

#define m2dRectWidth(rc)    ((rc).right - (rc).left)

#define m2dRectHeight(rc)   ((rc).bottom - (rc).top)

#define m2dRectArea(rc)     (m2dRectWidth(rc) * m2dRectHeight(rc))

#define m2dRectUnion(a, b, U)  do {                  \
        (U).left   = m2dMIN((a).left,   (b).left);   \
        (U).top    = m2dMIN((a).top,    (b).top);    \
        (U).right  = m2dMAX((a).right,  (b).right);  \
        (U).bottom = m2dMAX((a).bottom, (b).bottom); \
    } while(0)

#define m2dRectIntersect(a, b, R)  do {              \
        (R).left   = m2dMAX((a).left,   (b).left);   \
        (R).top    = m2dMAX((a).top,    (b).top);    \
        (R).right  = m2dMIN((a).right,  (b).right);  \
        (R).bottom = m2dMIN((a).bottom, (b).bottom); \
    } while(0)

#define m2dRectContain(outer, inner)   ((outer).left <= (inner).left && (outer).top <= (inner).top && (outer).right >= (inner).right && (outer).bottom >= (inner).bottom)

#define m2dRectEqual(a, b)   (m2dRectContain(a, b) && m2dRectContain(b, a))

#if defined(__cplusplus)
}
#endif
#endif /* MATHLIB_M2D_H_ */