#include <ntqapplication.h>
#include <ntqwidget.h>

#include <ntqpainter.h>
#include <ntqpointarray.h>

#include "tqtaapainter.h"

class AADemo : public TQWidget {
public:
    AADemo(TQWidget* parent = 0, const char* name = 0)
        : TQWidget(parent, name)
    {
        setMinimumSize(700, 360);
    }

private:
    void drawScene_(TQPainter* p, int ox, int oy, int w, int h,
                    int useAA,
                    const TQColor& fg,
                    const TQColor& fg2,
                    const TQColor& bg)
    {
        const int x0 = ox + 18;
        const int y0 = oy + 32;
        const int x1 = ox + w - 18;
        const int y1 = oy + h - 22;

        if (!useAA) {
            p->setPen(TQPen(fg, 1));
            p->drawLine(x0, y0, x1, y1);
            p->drawLine(x0, y1, x1, y0);

            p->setPen(TQPen(fg, 1));
            p->drawRect(ox + 18, oy + 68, w - 36, 96);

            {
                TQPointArray pts(5);
                pts.setPoint(0, ox + 18, oy + 190);
                pts.setPoint(1, ox + w - 18, oy + 200);
                pts.setPoint(2, ox + 78, oy + 230);
                pts.setPoint(3, ox + w - 98, oy + 270);
                pts.setPoint(4, ox + 18, oy + 290);
                p->setPen(TQPen(fg2, 1));
                p->drawPolyline(pts);
            }

            p->setPen(TQPen(fg, 1));
            p->drawEllipse(ox + w / 2 - 70, oy + 170, 140, 90);
        }
        else {
            // Legacy rect is kept as reference; AA helper currently provides lines/polylines.
            p->setPen(TQPen(fg, 1));
            p->drawRect(ox + 18, oy + 68, w - 36, 96);

            {
                TQPoint pts[5];
                pts[0] = TQPoint(ox + 18, oy + 190);
                pts[1] = TQPoint(ox + w - 18, oy + 200);
                pts[2] = TQPoint(ox + 78, oy + 230);
                pts[3] = TQPoint(ox + w - 98, oy + 270);
                pts[4] = TQPoint(ox + 18, oy + 290);
                TQtAAPainter::drawPolylineAA(p, pts, 5, fg2, bg, 1);
            }

            // Ellipse AA (outline)
            TQtAAPainter::drawEllipseAA(p, ox + w / 2, oy + 215, 70, 45, fg, bg, 0);

            // Draw diagonals last: AA helpers draw bg-filled images and would otherwise overwrite.
            TQtAAPainter::drawLineAA(p, x0, y0, x1, y1, fg, bg, 1);
            TQtAAPainter::drawLineAA(p, x0, y1, x1, y0, fg, bg, 1);
        }
    }

protected:
    void paintEvent(TQPaintEvent*)
    {
        const TQColor bg(18, 18, 18);
        const TQColor fg(240, 240, 240);
        const TQColor fg2(80, 160, 255);

        TQPainter p(this);
        p.fillRect(rect(), bg);

        const int pad = 20;
        const int mid = width() / 2;

        const int panelW = mid - pad * 2;
        const int panelH = height() - 2 * pad;

        const int lox = pad;
        const int loy = pad;
        const int rox = mid + pad;
        const int roy = pad;

        p.setPen(TQPen(fg, 1));
        p.drawText(lox, 16, "Legacy (no AA)");
        p.drawText(rox, 16, "TQtAAPainter (Wu)");

        drawScene_(&p, lox, loy, panelW, panelH, 0, fg, fg2, bg);

        // AA side: render into one image, then blit once (prevents overwrite artifacts).
        {
            TQImage aa(panelW, panelH, 32);
            aa.fill(bg.rgb());

            // Rect reference
            {
                const int x0 = 18;
                const int y0 = 68;
                const int x1 = panelW - 18;
                const int y1 = 68 + 96;

                TQtAAPainter::drawLineAA(&aa, x0, y0, x1, y0, fg, 1);
                TQtAAPainter::drawLineAA(&aa, x1, y0, x1, y1, fg, 1);
                TQtAAPainter::drawLineAA(&aa, x1, y1, x0, y1, fg, 1);
                TQtAAPainter::drawLineAA(&aa, x0, y1, x0, y0, fg, 1);
            }

            // Polyline
            {
                TQPoint pts[5];
                pts[0] = TQPoint(18, 190);
                pts[1] = TQPoint(panelW - 18, 200);
                pts[2] = TQPoint(78, 230);
                pts[3] = TQPoint(panelW - 98, 270);
                pts[4] = TQPoint(18, 290);
                TQtAAPainter::drawPolylineAA(&aa, pts, 5, fg2, 1);
            }

            // Ellipse
            TQtAAPainter::drawEllipseAA(&aa, panelW / 2, 215, 70, 45, fg, 0);

            // Diagonals
            TQtAAPainter::drawLineAA(&aa, 18, 32, panelW - 18, panelH - 22, fg, 1);
            TQtAAPainter::drawLineAA(&aa, 18, panelH - 22, panelW - 18, 32, fg, 1);

            p.drawImage(rox, roy, aa);
        }
    }
};

int main(int argc, char** argv)
{
    TQApplication app(argc, argv);

    AADemo w;
    w.setCaption("aa-painter (TQt3)" );

    app.setMainWidget(&w);
    TQObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    w.show();

    return app.exec();
}
