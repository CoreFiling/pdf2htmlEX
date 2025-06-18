/*
 * Preprocessor.cc
 *
 * Check used codes for each font
 *
 * by WangLu
 * 2012.09.07
 */

#include <cstring>
#include <iostream>
#include <algorithm>

#include <GfxState.h>
#include <GfxFont.h>

#include "Preprocessor.h"
#include "util/misc.h"
#include "util/const.h"
#include "util/unicode.h"

namespace pdf2htmlEX {

using std::cerr;
using std::endl;
using std::flush;
using std::max;
using std::vector;

Preprocessor::Preprocessor(const Param & param)
    : OutputDev()
    , param(param)
    , max_width(0)
    , max_height(0)
    , cur_font_id(0)
    , cur_codepoint_usages(nullptr)
    , cur_codepoint_map(nullptr)
{ }

Preprocessor::~Preprocessor(void)
{ }

void Preprocessor::process(PDFDoc * doc)
{
    int page_count = (param.last_page - param.first_page + 1);
    for(int i = param.first_page; i <= param.last_page ; ++i) 
    {
        if(param.quiet == 0)
            cerr << "Preprocessing: " << (i - param.first_page) << "/" << page_count << '\r' << flush;

        doc->displayPage(this, i, DEFAULT_DPI, DEFAULT_DPI,
                0, 
                (!(param.use_cropbox)),
                true,  // crop
                false, // printing
                nullptr, nullptr, nullptr, nullptr);
    }
    if(page_count >= 0 && param.quiet == 0)
        cerr << "Preprocessing: " << page_count << "/" << page_count;

    if(param.quiet == 0)
        cerr << endl;
}

void Preprocessor::drawChar(GfxState *state, double x, double y,
      double dx, double dy,
      double originX, double originY,
      CharCode code, int nBytes, const Unicode *u, int uLen)
{
    GfxFont * font = state->getFont();
    if(!font) return;

    long long fn_id = hash_ref(font->getID());

    if(fn_id != cur_font_id)
    {
        int len = font->isCIDFont() ? 0x10000 : 0x100;
        cur_font_id = fn_id;
        if (codepoint_usages.count(cur_font_id) == 0)
        {
            codepoint_usages.insert(std::make_pair(cur_font_id, vector<int>(len, 0)));
            codepoint_maps.insert(std::make_pair(cur_font_id, vector<Unicode>(len, -1)));
        }

        cur_codepoint_usages = &codepoint_usages.find(cur_font_id)->second;
        cur_codepoint_map = &codepoint_maps.find(cur_font_id)->second;
    }

    (*cur_codepoint_usages)[code]++;
    (*cur_codepoint_map)[code] = check_unicode(u, uLen, code, font);
}

void Preprocessor::startPage(int pageNum, GfxState *state)
{
    startPage(pageNum, state, nullptr);
}

void Preprocessor::startPage(int pageNum, GfxState *state, XRef * xref)
{
    max_width = max<double>(max_width, state->getPageWidth());
    max_height = max<double>(max_height, state->getPageHeight());
}

const vector<int> & Preprocessor::get_codepoint_usages (long long font_id) const
{
    return codepoint_usages.find(font_id)->second;
}

const vector<Unicode> & Preprocessor::get_codepoint_map (long long font_id) const
{
    return codepoint_maps.find(font_id)->second;
}

const vector<long long> Preprocessor::get_used_font_ids() const
{
    vector<long long> ids;
    ids.reserve(codepoint_usages.size());
    for (const auto & p : codepoint_usages)
        ids.push_back(p.first);
    return ids;
}

} // namespace pdf2htmlEX
