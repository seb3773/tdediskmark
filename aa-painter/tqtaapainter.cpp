#include "tqtaapainter.h"

#include <ntqmemarray.h>

#include <math.h>

static inline int tqt_aa_iabs_(int x) { return (x < 0) ? -x : x; }

static inline int tqt_aa_imin_(int a, int b) { return (a < b) ? a : b; }
static inline int tqt_aa_imax_(int a, int b) { return (a > b) ? a : b; }

static inline float tqt_aa_fclamp01_(float x)
{
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

static void tqt_aa_wu_line_(TQImage& img, int x0, int y0, int x1, int y1, const TQColor& fg, const TQColor& bg);
static inline void tqt_aa_plot_dst_(TQImage& img, int x, int y, const TQColor& fg, float a);

static inline float tqt_aa_dist2_point_seg_(float px, float py,
                                            float ax, float ay,
                                            float bx, float by)
{
    const float abx = bx - ax;
    const float aby = by - ay;
    const float apx = px - ax;
    const float apy = py - ay;

    const float denom = abx * abx + aby * aby;
    float t = 0.0f;
    if (denom > 0.0f) t = (apx * abx + apy * aby) / denom;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    const float cx = ax + t * abx;
    const float cy = ay + t * aby;
    const float dx = px - cx;
    const float dy = py - cy;
    return dx * dx + dy * dy;
}

static void tqt_aa_thick_line_(TQImage& img,
                               int x0, int y0,
                               int x1, int y1,
                               const TQColor& fg,
                               int width)
{
    if (width <= 1) {
        tqt_aa_wu_line_(img, x0, y0, x1, y1, fg, TQColor());
        return;
    }

    const float hw = 0.5f * (float)width;
    const int pad = (int)ceilf(hw + 1.0f);

    int minx = tqt_aa_imin_(x0, x1) - pad;
    int maxx = tqt_aa_imax_(x0, x1) + pad;
    int miny = tqt_aa_imin_(y0, y1) - pad;
    int maxy = tqt_aa_imax_(y0, y1) + pad;

    if (minx < 0) minx = 0;
    if (miny < 0) miny = 0;
    if (maxx >= img.width()) maxx = img.width() - 1;
    if (maxy >= img.height()) maxy = img.height() - 1;

    const float ax = (float)x0 + 0.5f;
    const float ay = (float)y0 + 0.5f;
    const float bx = (float)x1 + 0.5f;
    const float by = (float)y1 + 0.5f;

    const float edge = hw + 0.5f;
    const float edge2 = edge * edge;

    int y;
    for (y = miny; y <= maxy; ++y) {
        const float py = (float)y + 0.5f;
        int x;
        for (x = minx; x <= maxx; ++x) {
            const float px = (float)x + 0.5f;
            const float d2 = tqt_aa_dist2_point_seg_(px, py, ax, ay, bx, by);
            if (d2 >= edge2) continue;

            const float d = sqrtf(d2);
            const float a = tqt_aa_fclamp01_(edge - d);
            if (a > 0.0f) tqt_aa_plot_dst_(img, x, y, fg, a);
        }
    }
}

static void tqt_aa_draw_polyline_into_w_(TQImage& img, const TQPoint* pts, int n, const TQColor& fg, int width);

void TQtAAPainter::fillPolygonAA(TQImage* img,
                                 const TQPoint* pts,
                                 int n,
                                 const TQColor& fg)
{
    fillPolygonAA(img, pts, n, fg, 1);
}

void TQtAAPainter::fillPolygonAA(TQImage* img,
                                 const TQPoint* pts,
                                 int n,
                                 const TQColor& fg,
                                 int outlineWidth)
{
    if (!img) return;
    if (img->isNull()) return;
    if (!pts) return;
    if (n < 3) return;

    int minx = pts[0].x();
    int maxx = pts[0].x();
    int miny = pts[0].y();
    int maxy = pts[0].y();

    int i;
    for (i = 1; i < n; ++i) {
        const int x = pts[i].x();
        const int y = pts[i].y();
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
    }

    if (minx < 0) minx = 0;
    if (miny < 0) miny = 0;
    if (maxx >= img->width()) maxx = img->width() - 1;
    if (maxy >= img->height()) maxy = img->height() - 1;
    if (minx > maxx || miny > maxy) return;

    TQMemArray<float> xs;
    xs.resize(n);

    int y;
    for (y = miny; y <= maxy; ++y) {
        const float sy = (float)y + 0.5f;
        int m = 0;

        int j;
        for (j = 0; j < n; ++j) {
            const int k = (j + 1 == n) ? 0 : (j + 1);
            const float y0 = (float)pts[j].y();
            const float y1 = (float)pts[k].y();
            const float x0 = (float)pts[j].x();
            const float x1 = (float)pts[k].x();

            // Half-open rule to avoid double counting at vertices.
            if ((sy >= y0 && sy < y1) || (sy >= y1 && sy < y0)) {
                const float t = (sy - y0) / (y1 - y0);
                xs[m++] = x0 + t * (x1 - x0);
            }
        }

        if (m < 2) continue;

        // Insertion sort (m is small).
        int a;
        for (a = 1; a < m; ++a) {
            const float v = xs[a];
            int b = a - 1;
            while (b >= 0 && xs[b] > v) {
                xs[b + 1] = xs[b];
                --b;
            }
            xs[b + 1] = v;
        }

        for (a = 0; a + 1 < m; a += 2) {
            float x0 = xs[a];
            float x1 = xs[a + 1];
            if (x0 > x1) {
                const float tmp = x0;
                x0 = x1;
                x1 = tmp;
            }

            int ix0 = (int)ceilf(x0 - 0.5f);
            int ix1 = (int)floorf(x1 - 0.5f);
            if (ix0 < minx) ix0 = minx;
            if (ix1 > maxx) ix1 = maxx;
            if (ix0 > ix1) continue;

            int x;
            for (x = ix0; x <= ix1; ++x) {
                tqt_aa_plot_dst_(*img, x, y, fg, 1.0f);
            }
        }
    }

    if (outlineWidth > 0) {
        TQMemArray<TQPoint> pp;
        pp.resize(n + 1);
        for (i = 0; i < n; ++i) pp[i] = pts[i];
        pp[n] = pts[0];
        tqt_aa_draw_polyline_into_w_(*img, pp.data(), n + 1, fg, outlineWidth);
    }
}

void TQtAAPainter::fillAnnularArcAA(TQImage* img,
                                    int cx, int cy,
                                    int rOuter,
                                    int rInner,
                                    double startDeg,
                                    double spanDeg,
                                    const TQColor& fg,
                                    int segments)
{
    if (!img) return;
    if (img->isNull()) return;
    if (rOuter <= 0) return;
    if (rInner < 0) rInner = 0;
    if (rInner >= rOuter) return;
    if (spanDeg == 0.0) return;

    int seg = segments;
    if (seg <= 0) {
        const double aspan = (spanDeg < 0.0) ? -spanDeg : spanDeg;
        seg = (int)(aspan * 1.2);
        if (seg < 12) seg = 12;
        if (seg > 720) seg = 720;
    }

    if (rInner == 0) {
        TQMemArray<TQPoint> pts;
        pts.resize(seg + 2);

        int i;
        for (i = 0; i <= seg; ++i) {
            const double t = (double)i / (double)seg;
            const double a = startDeg + t * spanDeg;
            const double rad = a * (3.14159265358979323846 / 180.0);

            const int x = cx + (int)floor((double)rOuter * cos(rad) + 0.5);
            const int y = cy - (int)floor((double)rOuter * sin(rad) + 0.5);
            pts[i] = TQPoint(x, y);
        }

        pts[seg + 1] = TQPoint(cx, cy);
        fillPolygonAA(img, pts.data(), seg + 2, fg, 0);
        return;
    }

    TQMemArray<TQPoint> pts;
    pts.resize((seg + 1) * 2);

    int i;
    for (i = 0; i <= seg; ++i) {
        const double t = (double)i / (double)seg;
        const double a = startDeg + t * spanDeg;
        const double rad = a * (3.14159265358979323846 / 180.0);

        const int x = cx + (int)floor((double)rOuter * cos(rad) + 0.5);
        const int y = cy - (int)floor((double)rOuter * sin(rad) + 0.5);
        pts[i] = TQPoint(x, y);
    }

    for (i = 0; i <= seg; ++i) {
        const double t = (double)(seg - i) / (double)seg;
        const double a = startDeg + t * spanDeg;
        const double rad = a * (3.14159265358979323846 / 180.0);

        const int x = cx + (int)floor((double)rInner * cos(rad) + 0.5);
        const int y = cy - (int)floor((double)rInner * sin(rad) + 0.5);
        pts[seg + 1 + i] = TQPoint(x, y);
    }

    fillPolygonAA(img, pts.data(), (seg + 1) * 2, fg, 0);
}


static void tqt_aa_draw_polyline_img_(TQPainter* p, const TQPoint* pts, int n, const TQColor& fg, const TQColor& bg)
{
    if (!p) return;
    if (!pts) return;
    if (n < 2) return;

    int minx = pts[0].x();
    int maxx = pts[0].x();
    int miny = pts[0].y();
    int maxy = pts[0].y();

    int i;
    for (i = 1; i < n; ++i) {
        const int x = pts[i].x();
        const int y = pts[i].y();
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
    }

    minx -= 2;
    miny -= 2;
    maxx += 2;
    maxy += 2;

    const int w = maxx - minx + 1;
    const int h = maxy - miny + 1;
    if (w <= 0 || h <= 0) return;

    TQImage img(w, h, 32);
    img.fill(bg.rgb());

    for (i = 0; i < n - 1; ++i) {
        tqt_aa_wu_line_(img,
                        pts[i].x() - minx, pts[i].y() - miny,
                        pts[i + 1].x() - minx, pts[i + 1].y() - miny,
                        fg, bg);
    }

    p->drawImage(minx, miny, img);
}

static inline int tqt_aa_clampi_(int x, int a, int b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

static inline float tqt_aa_frac_(float x)
{
    return x - (float)((int)x);
}

static inline float tqt_aa_rfrac_(float x)
{
    return 1.0f - tqt_aa_frac_(x);
}

static inline unsigned int tqt_aa_pack_rgb_(int r, int g, int b)
{
    return 0xff000000u | ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b;
}

static inline void tqt_aa_plot_dst_(TQImage& img, int x, int y, const TQColor& fg, float a)
{
    if ((unsigned int)x >= (unsigned int)img.width()) return;
    if ((unsigned int)y >= (unsigned int)img.height()) return;

    if (a <= 0.0f) return;
    if (a >= 1.0f) {
        img.setPixel(x, y, fg.rgb());
        return;
    }

    const unsigned int dst = (unsigned int)img.pixel(x, y);
    const int dr = (int)((dst >> 16) & 0xff);
    const int dg = (int)((dst >> 8) & 0xff);
    const int db = (int)(dst & 0xff);

    const float ia = 1.0f - a;

    const int r = (int)((float)fg.red() * a + (float)dr * ia);
    const int g = (int)((float)fg.green() * a + (float)dg * ia);
    const int b = (int)((float)fg.blue() * a + (float)db * ia);

    img.setPixel(x, y, tqt_aa_pack_rgb_(tqt_aa_clampi_(r, 0, 255), tqt_aa_clampi_(g, 0, 255), tqt_aa_clampi_(b, 0, 255)));
}

static void tqt_aa_wu_line_(TQImage& img, int x0, int y0, int x1, int y1, const TQColor& fg, const TQColor& bg)
{
    int steep = (tqt_aa_iabs_(y1 - y0) > tqt_aa_iabs_(x1 - x0));
    if (steep) {
        int t;
        t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }

    if (x0 > x1) {
        int t;
        t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }

    const float dx = (float)(x1 - x0);
    const float dy = (float)(y1 - y0);
    const float gradient = (dx != 0.0f) ? (dy / dx) : 0.0f;

    float y = (float)y0;

    int x;
    for (x = x0; x <= x1; ++x) {
        const int iy = (int)y;
        const float f = tqt_aa_frac_(y);

        if (steep) {
            tqt_aa_plot_dst_(img, iy, x, fg, 1.0f - f);
            tqt_aa_plot_dst_(img, iy + 1, x, fg, f);
        } else {
            tqt_aa_plot_dst_(img, x, iy, fg, 1.0f - f);
            tqt_aa_plot_dst_(img, x, iy + 1, fg, f);
        }

        y += gradient;
    }
}

static void tqt_aa_draw_polyline_into_(TQImage& img, const TQPoint* pts, int n, const TQColor& fg)
{
    if (!pts) return;
    if (n < 2) return;

    int i;
    for (i = 0; i < n - 1; ++i) {
        tqt_aa_wu_line_(img, pts[i].x(), pts[i].y(), pts[i + 1].x(), pts[i + 1].y(), fg, TQColor());
    }
}

static void tqt_aa_draw_polyline_into_w_(TQImage& img, const TQPoint* pts, int n, const TQColor& fg, int width)
{
    if (!pts) return;
    if (n < 2) return;
    if (width <= 1) {
        tqt_aa_draw_polyline_into_(img, pts, n, fg);
        return;
    }

    int i;
    for (i = 0; i < n - 1; ++i) {
        tqt_aa_thick_line_(img,
                           pts[i].x(), pts[i].y(),
                           pts[i + 1].x(), pts[i + 1].y(),
                           fg, width);
    }
}

void TQtAAPainter::drawLineAA(TQImage* img,
                              int x0, int y0,
                              int x1, int y1,
                              const TQColor& fg,
                              int width)
{
    if (!img) return;
    if (img->isNull()) return;

    if (width <= 0) width = 1;
    tqt_aa_thick_line_(*img, x0, y0, x1, y1, fg, width);
}

void TQtAAPainter::drawPolylineAA(TQImage* img,
                                  const TQPoint* pts,
                                  int n,
                                  const TQColor& fg,
                                  int width)
{
    if (!img) return;
    if (img->isNull()) return;

    if (width <= 0) width = 1;
    tqt_aa_draw_polyline_into_w_(*img, pts, n, fg, width);
}

void TQtAAPainter::drawEllipseAA(TQImage* img,
                                 int cx, int cy,
                                 int rx, int ry,
                                 const TQColor& fg,
                                 int segments)
{
    if (!img) return;
    if (img->isNull()) return;
    if (rx <= 0 || ry <= 0) return;

    // Analytic AA outline for segments == 0.
    // Complexity: O((rx*ry)) over the bounding box. Depends: libm (sqrt).
    // Assumes: output is opaque RGB, blended against the current destination pixel.
    if (segments == 0) {
        const int pad = 2;
        const int minx = cx - rx - pad;
        const int maxx = cx + rx + pad;
        const int miny = cy - ry - pad;
        const int maxy = cy + ry + pad;

        const double invRx = 1.0 / (double)rx;
        const double invRy = 1.0 / (double)ry;
        const double scale = 0.5 * ((double)rx + (double)ry);

        int y;
        for (y = miny; y <= maxy; ++y) {
            const double py = ((double)y + 0.5) - (double)cy;
            const double fy = (py * invRy);
            const double fy2 = fy * fy;

            int x;
            for (x = minx; x <= maxx; ++x) {
                const double px = ((double)x + 0.5) - (double)cx;
                const double fx = (px * invRx);
                const double f = fx * fx + fy2;

                // Distance in normalized ellipse-space.
                const double d = fabs(sqrt(f) - 1.0);
                const double distPx = d * scale;

                if (distPx < 1.0) {
                    const float a = (float)(1.0 - distPx);
                    tqt_aa_plot_dst_(*img, x, y, fg, a);
                }
            }
        }
        return;
    }

    int seg = segments;
    if (seg <= 0) {
        const int m = (rx > ry) ? rx : ry;
        seg = m * 4;
        if (seg < 24) seg = 24;
        if (seg > 512) seg = 512;
    }

    TQMemArray<TQPoint> pts;
    pts.resize(seg + 1);

    int i;
    for (i = 0; i <= seg; ++i) {
        const double a = (double)i * (2.0 * 3.14159265358979323846) / (double)seg;
        const int x = cx + (int)floor(cos(a) * (double)rx + 0.5);
        const int y = cy + (int)floor(sin(a) * (double)ry + 0.5);
        pts[i] = TQPoint(x, y);
    }

    tqt_aa_draw_polyline_into_(*img, pts.data(), seg + 1, fg);
}

void TQtAAPainter::drawCircleAA(TQImage* img,
                                int cx, int cy,
                                int r,
                                const TQColor& fg,
                                int segments)
{
    drawEllipseAA(img, cx, cy, r, r, fg, segments);
}

void TQtAAPainter::fillCircleAA(TQImage* img,
                                int cx, int cy,
                                int r,
                                const TQColor& fg)
{
    if (!img) return;
    if (img->isNull()) return;
    if (r <= 0) return;

    int minx = cx - r - 1;
    int maxx = cx + r + 1;
    int miny = cy - r - 1;
    int maxy = cy + r + 1;

    if (minx < 0) minx = 0;
    if (miny < 0) miny = 0;
    if (maxx >= img->width()) maxx = img->width() - 1;
    if (maxy >= img->height()) maxy = img->height() - 1;

    const float ccx = (float)cx + 0.5f;
    const float ccy = (float)cy + 0.5f;
    const float rr = (float)r;
    const float inner = rr - 0.5f;
    const float outer = rr + 0.5f;
    const float inner2 = inner * inner;
    const float outer2 = outer * outer;

    int y;
    for (y = miny; y <= maxy; ++y) {
        const float py = (float)y + 0.5f;
        const float dy = py - ccy;
        int x;
        for (x = minx; x <= maxx; ++x) {
            const float px = (float)x + 0.5f;
            const float dx = px - ccx;
            const float d2 = dx * dx + dy * dy;
            if (d2 >= outer2) continue;
            if (d2 <= inner2) {
                tqt_aa_plot_dst_(*img, x, y, fg, 1.0f);
                continue;
            }

            const float d = sqrtf(d2);
            const float a = tqt_aa_fclamp01_(outer - d);
            if (a > 0.0f) tqt_aa_plot_dst_(*img, x, y, fg, a);
        }
    }
}

void TQtAAPainter::drawArcAA(TQImage* img,
                             int cx, int cy,
                             int r,
                             double startDeg,
                             double spanDeg,
                             const TQColor& fg,
                             int segments,
                             int width)
{
    if (!img) return;
    if (img->isNull()) return;
    if (r <= 0) return;
    if (spanDeg == 0.0) return;

    if (width <= 0) width = 1;

    int seg = segments;
    if (seg <= 0) {
        const double aspan = (spanDeg < 0.0) ? -spanDeg : spanDeg;
        seg = (int)(aspan * 1.2);
        if (seg < 12) seg = 12;
        if (seg > 720) seg = 720;
    }

    TQMemArray<TQPoint> pts;
    pts.resize(seg + 1);

    int i;
    for (i = 0; i <= seg; ++i) {
        const double t = (double)i / (double)seg;
        const double a = startDeg + t * spanDeg;

        // Same convention as fillAnnularArcAA (Qt-like):
        // - 0° at 3 o'clock
        // - CCW positive
        // - screen y axis downward (therefore y uses -sin)
        const double rad = a * (3.14159265358979323846 / 180.0);

        const int x = cx + (int)floor((double)r * cos(rad) + 0.5);
        const int y = cy - (int)floor((double)r * sin(rad) + 0.5);
        pts[i] = TQPoint(x, y);
    }

    tqt_aa_draw_polyline_into_w_(*img, pts.data(), seg + 1, fg, width);
}

void TQtAAPainter::drawLineAA(TQPainter* p,
                              int x0, int y0,
                              int x1, int y1,
                              const TQColor& fg,
                              const TQColor& bg,
                              int width)
{
    if (!p) return;

    if (width <= 0) width = 1;

    int minx = tqt_aa_imin_(x0, x1);
    int maxx = tqt_aa_imax_(x0, x1);
    int miny = tqt_aa_imin_(y0, y1);
    int maxy = tqt_aa_imax_(y0, y1);

    const float hw = 0.5f * (float)width;
    const int pad = (int)ceilf(hw + 1.0f);
    minx -= pad;
    miny -= pad;
    maxx += pad;
    maxy += pad;

    const int w = maxx - minx + 1;
    const int h = maxy - miny + 1;
    if (w <= 0 || h <= 0) return;

    TQImage img(w, h, 32);
    img.fill(bg.rgb());

    tqt_aa_thick_line_(img,
                       x0 - minx, y0 - miny,
                       x1 - minx, y1 - miny,
                       fg, width);

    p->drawImage(minx, miny, img);
}

void TQtAAPainter::drawPolylineAA(TQPainter* p,
                                  const TQPoint* pts,
                                  int n,
                                  const TQColor& fg,
                                  const TQColor& bg,
                                  int width)
{
    if (!p) return;
    if (!pts) return;
    if (n < 2) return;

    if (width <= 0) width = 1;

    int minx = pts[0].x();
    int maxx = pts[0].x();
    int miny = pts[0].y();
    int maxy = pts[0].y();

    int i;
    for (i = 1; i < n; ++i) {
        const int x = pts[i].x();
        const int y = pts[i].y();
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
    }

    const float hw = 0.5f * (float)width;
    const int pad = (int)ceilf(hw + 1.0f);
    minx -= pad;
    miny -= pad;
    maxx += pad;
    maxy += pad;

    const int w = maxx - minx + 1;
    const int h = maxy - miny + 1;
    if (w <= 0 || h <= 0) return;

    TQImage img(w, h, 32);
    img.fill(bg.rgb());

    for (i = 0; i < n - 1; ++i) {
        tqt_aa_thick_line_(img,
                           pts[i].x() - minx, pts[i].y() - miny,
                           pts[i + 1].x() - minx, pts[i + 1].y() - miny,
                           fg, width);
    }

    p->drawImage(minx, miny, img);
}

void TQtAAPainter::drawEllipseAA(TQPainter* p,
                                 int cx, int cy,
                                 int rx, int ry,
                                 const TQColor& fg,
                                 const TQColor& bg,
                                 int segments)
{
    if (!p) return;
    if (rx <= 0 || ry <= 0) return;

    int seg = segments;
    if (seg <= 0) {
        const int m = (rx > ry) ? rx : ry;
        seg = m * 4;
        if (seg < 24) seg = 24;
        if (seg > 512) seg = 512;
    }

    // Approximate ellipse with a closed polyline.
    // Complexity: O(segments). Depends: libm (sin/cos).
    TQMemArray<TQPoint> pts;
    pts.resize(seg + 1);

    int i;
    for (i = 0; i <= seg; ++i) {
        const double a = (double)i * (2.0 * 3.14159265358979323846) / (double)seg;
        const int x = cx + (int)floor(cos(a) * (double)rx + 0.5);
        const int y = cy + (int)floor(sin(a) * (double)ry + 0.5);
        pts[i] = TQPoint(x, y);
    }

    tqt_aa_draw_polyline_img_(p, pts.data(), seg + 1, fg, bg);
}

void TQtAAPainter::drawCircleAA(TQPainter* p,
                                int cx, int cy,
                                int r,
                                const TQColor& fg,
                                const TQColor& bg,
                                int segments)
{
    drawEllipseAA(p, cx, cy, r, r, fg, bg, segments);
}

void TQtAAPainter::fillCircleAA(TQPainter* p,
                                int cx, int cy,
                                int r,
                                const TQColor& fg)
{
    if (!p) return;
    if (r <= 0) return;

    p->setPen(fg);
    p->setBrush(fg);
    p->drawEllipse(cx - r, cy - r, r * 2, r * 2);
}
