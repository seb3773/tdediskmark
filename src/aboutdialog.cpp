#include "aboutdialog.h"
#include <tqlabel.h>
#include <tqpushbutton.h>
#include <tqpixmap.h>
#include <tqfont.h>

#include "about_tdediskmark_png.h"
#include "tdedisksw_png.h"
extern unsigned char ___icons_tdediskmark_png[];
extern unsigned int ___icons_tdediskmark_png_len;

AboutDialog::AboutDialog(TQWidget* parent) : TQDialog(parent, "AboutDialog", true)
{
    setCaption(tr("About tdeDiskMark"));
    setFixedSize(600, 270); // Widened by 100px as requested
    setPaletteBackgroundColor(TQColor("#000000"));

    TQPixmap appIcon;
    appIcon.loadFromData(___icons_tdediskmark_png, ___icons_tdediskmark_png_len);
    setIcon(appIcon);

    // --- LEFT IMAGE (Dragon) ---
    TQPixmap dragonPix;
    dragonPix.loadFromData(___icons_about_tdediskmark_png, ___icons_about_tdediskmark_png_len);
    TQLabel *dragonLabel = new TQLabel(this);
    dragonLabel->setPixmap(dragonPix);
    dragonLabel->setGeometry(10, 10, dragonPix.width(), dragonPix.height());

    // "by seb3773" under the dragon
    TQLabel *authorLabel = new TQLabel("by seb3773", this);
    authorLabel->setPaletteForegroundColor(TQColor("#FFFFFF"));
    TQFont f = authorLabel->font();
    f.setItalic(true);
    authorLabel->setFont(f);
    authorLabel->setGeometry(118, 245, 120, 20); // Aligned near dragon's foot
    
    // --- RIGHT TOP IMAGE (tdeDiskMark logo) ---
    TQPixmap logoPix;
    logoPix.loadFromData(___icons_tdedisksw_png, ___icons_tdedisksw_png_len);
    TQLabel *logoLabel = new TQLabel(this);
    logoLabel->setPixmap(logoPix);
    logoLabel->setGeometry(280, 15, logoPix.width(), logoPix.height());

    // --- TEXT UNDER LOGO (Centered under logo) ---
    TQLabel *descLabel = new TQLabel("HDD and SSD benchmark tool\nfor Trinity Desktop", this);
    descLabel->setPaletteForegroundColor(TQColor("#FFFFFF"));
    descLabel->setAlignment(TQt::AlignHCenter | TQt::AlignTop);
    f = descLabel->font();
    f.setPointSize(11);
    descLabel->setFont(f);
    descLabel->setGeometry(240, 58, 320, 50);

    // --- SOURCE ATTRIBUTION (Left-aligned under logo block) ---
    TQLabel *baseLabel = new TQLabel("based on KDiskmark\n( https://github.com/JonMagon/KDiskMark )", this);
    baseLabel->setPaletteForegroundColor(TQColor("#FFFFFF"));
    baseLabel->setAlignment(TQt::AlignLeft | TQt::AlignTop);
    f = baseLabel->font();
    f.setPointSize(10);
    baseLabel->setFont(f);
    baseLabel->setGeometry(268, 145, 330, 50);

    // --- STANDARD CLOSE BUTTON ---
    TQPushButton *okBtn = new TQPushButton(tr("Close"), this);
    okBtn->setGeometry(370, 220, 100, 30); // Center-right standard layout

    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));
}
