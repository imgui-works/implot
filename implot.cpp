// MIT License

// Copyright (c) 2020 Evan Pezent

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ImPlot v0.8 WIP

/*

API BREAKING CHANGES
====================
Occasionally introducing changes that are breaking the API. We try to make the breakage minor and easy to fix.
Below is a change-log of API breaking changes only. If you are using one of the functions listed, expect to have to fix some code.
When you are not sure about a old symbol or function name, try using the Search/Find function of your IDE to look for comments or references in all implot files.
You can read releases logs https://github.com/epezent/implot/releases for more details.

- 2020/09/06 (0.7) - Several flags under ImPlotFlags and ImPlotAxisFlags were inverted (e.g. ImPlotFlags_Legend -> ImPlotFlags_NoLegend) so that the default flagset
                     is simply 0. This more closely matches ImGui's style and makes it easier to enable non-default but commonly used flags (e.g. ImPlotAxisFlags_Time).
- 2020/08/28 (0.5) - ImPlotMarker_ can no longer be combined with bitwise OR, |. This features caused unecessary slow-down, and almost no one used it.
- 2020/08/25 (0.5) - ImPlotAxisFlags_Scientific was removed. Logarithmic axes automatically uses scientific notation.
- 2020/08/17 (0.5) - PlotText was changed so that text is centered horizontally and vertically about the desired point.
- 2020/08/16 (0.5) - An ImPlotContext must be explicitly created and destroyed now with `CreateContext` and `DestroyContext`. Previously, the context was statically initialized in this source file.
- 2020/06/13 (0.4) - The flags `ImPlotAxisFlag_Adaptive` and `ImPlotFlags_Cull` were removed. Both are now done internally by default.
- 2020/06/03 (0.3) - The signature and behavior of PlotPieChart was changed so that data with sum less than 1 can optionally be normalized. The label format can now be specified as well.
- 2020/06/01 (0.3) - SetPalette was changed to `SetColormap` for consistency with other plotting libraries. `RestorePalette` was removed. Use `SetColormap(ImPlotColormap_Default)`.
- 2020/05/31 (0.3) - Plot functions taking custom ImVec2* getters were removed. Use the ImPlotPoint* getter versions instead.
- 2020/05/29 (0.3) - The signature of ImPlotLimits::Contains was changed to take two doubles instead of ImVec2
- 2020/05/16 (0.2) - All plotting functions were reverted to being prefixed with "Plot" to maintain a consistent VerbNoun style. `Plot` was split into `PlotLine`
                     and `PlotScatter` (however, `PlotLine` can still be used to plot scatter points as `Plot` did before.). `Bar` is not `PlotBars`, to indicate
                     that multiple bars will be plotted.
- 2020/05/13 (0.2) - `ImMarker` was change to `ImPlotMarker` and `ImAxisFlags` was changed to `ImPlotAxisFlags`.
- 2020/05/11 (0.2) - `ImPlotFlags_Selection` was changed to `ImPlotFlags_BoxSelect`
- 2020/05/11 (0.2) - The namespace ImGui:: was replaced with ImPlot::. As a result, the following additional changes were made:
                     - Functions that were prefixed or decorated with the word "Plot" have been truncated. E.g., `ImGui::PlotBars` is now just `ImPlot::Bar`.
                       It should be fairly obvious what was what.
                     - Some functions have been given names that would have otherwise collided with the ImGui namespace. This has been done to maintain a consistent
                       style with ImGui. E.g., 'ImGui::PushPlotStyleVar` is now 'ImPlot::PushStyleVar'.
- 2020/05/10 (0.2) - The following function/struct names were changes:
                    - ImPlotRange       -> ImPlotLimits
                    - GetPlotRange()    -> GetPlotLimits()
                    - SetNextPlotRange  -> SetNextPlotLimits
                    - SetNextPlotRangeX -> SetNextPlotLimitsX
                    - SetNextPlotRangeY -> SetNextPlotLimitsY
- 2020/05/10 (0.2) - Plot queries are pixel based by default. Query rects that maintain relative plot position have been removed. This was done to support multi-y-axis.

*/

#include "implot.h"
#include "implot_internal.h"

#ifdef _MSC_VER
#define sprintf sprintf_s
#endif

// Global plot context
ImPlotContext* GImPlot = NULL;

//-----------------------------------------------------------------------------
// Struct Implementations
//-----------------------------------------------------------------------------

ImPlotRange::ImPlotRange() {
    Min = NAN;
    Max = NAN;
}

ImPlotInputMap::ImPlotInputMap() {
    PanButton             = ImGuiMouseButton_Left;
    PanMod                = ImGuiKeyModFlags_None;
    FitButton             = ImGuiMouseButton_Left;
    ContextMenuButton     = ImGuiMouseButton_Right;
    BoxSelectButton       = ImGuiMouseButton_Right;
    BoxSelectMod          = ImGuiKeyModFlags_None;
    BoxSelectCancelButton = ImGuiMouseButton_Left;
    QueryButton           = ImGuiMouseButton_Middle;
    QueryMod              = ImGuiKeyModFlags_None;
    QueryToggleMod        = ImGuiKeyModFlags_Ctrl;
    HorizontalMod         = ImGuiKeyModFlags_Alt;
    VerticalMod           = ImGuiKeyModFlags_Shift;
}

ImPlotStyle::ImPlotStyle() {

    LineWeight       = 1;
    Marker           = ImPlotMarker_None;
    MarkerSize       = 4;
    MarkerWeight     = 1;
    FillAlpha        = 1;
    ErrorBarSize     = 5;
    ErrorBarWeight   = 1.5f;
    DigitalBitHeight = 8;
    DigitalBitGap    = 4;

    PlotBorderSize   = 1;
    MinorAlpha       = 0.25f;
    MajorTickLen     = ImVec2(10,10);
    MinorTickLen     = ImVec2(5,5);
    MajorTickSize    = ImVec2(1,1);
    MinorTickSize    = ImVec2(1,1);
    MajorGridSize    = ImVec2(1,1);
    MinorGridSize    = ImVec2(1,1);
    PlotPadding      = ImVec2(8,8);
    LabelPadding     = ImVec2(5,5);
    LegendPadding    = ImVec2(10,10);
    InfoPadding      = ImVec2(10,10);
    PlotMinSize      = ImVec2(300,225);

    ImPlot::StyleColorsAuto(this);

    AntiAliasedLines = false;
    UseLocalTime     = false;
}

namespace ImPlot {

const char* GetStyleColorName(ImPlotCol col) {
    static const char* col_names[] = {
        "Line",
        "Fill",
        "MarkerOutline",
        "MarkerFill",
        "ErrorBar",
        "FrameBg",
        "PlotBg",
        "PlotBorder",
        "LegendBg",
        "LegendBorder",
        "LegendText",
        "TitleText",
        "InlayText",
        "XAxis",
        "XAxisGrid",
        "YAxis",
        "YAxisGrid",
        "YAxis2",
        "YAxisGrid2",
        "YAxis3",
        "YAxisGrid3",
        "Selection",
        "Query",
        "Crosshairs"
    };
    return col_names[col];
}

const char* GetMarkerName(ImPlotMarker marker) {
    switch (marker) {
        case ImPlotMarker_None:     return "None";
        case ImPlotMarker_Circle:   return "Circle";
        case ImPlotMarker_Square:   return "Square";
        case ImPlotMarker_Diamond:  return "Diamond";
        case ImPlotMarker_Up:       return "Up";
        case ImPlotMarker_Down:     return "Down";
        case ImPlotMarker_Left:     return "Left";
        case ImPlotMarker_Right:    return "Right";
        case ImPlotMarker_Cross:    return "Cross";
        case ImPlotMarker_Plus:     return "Plus";
        case ImPlotMarker_Asterisk: return "Asterisk";
        default:                    return "";
    }
}

ImVec4 GetAutoColor(ImPlotCol idx) {
    ImVec4 col(0,0,0,1);
    switch(idx) {
        case ImPlotCol_Line:          return col; // these are plot dependent!
        case ImPlotCol_Fill:          return col; // these are plot dependent!
        case ImPlotCol_MarkerOutline: return col; // these are plot dependent!
        case ImPlotCol_MarkerFill:    return col; // these are plot dependent!
        case ImPlotCol_ErrorBar:      return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_FrameBg:       return ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
        case ImPlotCol_PlotBg:        return ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        case ImPlotCol_PlotBorder:    return ImGui::GetStyleColorVec4(ImGuiCol_Border);
        case ImPlotCol_LegendBg:      return ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
        case ImPlotCol_LegendBorder:  return GetStyleColorVec4(ImPlotCol_PlotBorder);
        case ImPlotCol_LegendText:    return GetStyleColorVec4(ImPlotCol_InlayText);
        case ImPlotCol_TitleText:     return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_InlayText:     return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_XAxis:         return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_XAxisGrid:     return GetStyleColorVec4(ImPlotCol_XAxis) * ImVec4(1,1,1,0.25f);
        case ImPlotCol_YAxis:         return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_YAxisGrid:     return GetStyleColorVec4(ImPlotCol_YAxis) * ImVec4(1,1,1,0.25f);
        case ImPlotCol_YAxis2:        return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_YAxisGrid2:    return GetStyleColorVec4(ImPlotCol_YAxis2) * ImVec4(1,1,1,0.25f);
        case ImPlotCol_YAxis3:        return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_YAxisGrid3:    return GetStyleColorVec4(ImPlotCol_YAxis3) * ImVec4(1,1,1,0.25f);
        case ImPlotCol_Selection:     return ImVec4(1,1,0,1);
        case ImPlotCol_Query:         return ImVec4(0,1,0,1);
        case ImPlotCol_Crosshairs:    return GetStyleColorVec4(ImPlotCol_PlotBorder);
        default: return col;
    }
}

struct ImPlotStyleVarInfo {
    ImGuiDataType   Type;
    ImU32           Count;
    ImU32           Offset;
    void*           GetVarPtr(ImPlotStyle* style) const { return (void*)((unsigned char*)style + Offset); }
};

static const ImPlotStyleVarInfo GPlotStyleVarInfo[] =
{
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, LineWeight)       }, // ImPlotStyleVar_LineWeight
    { ImGuiDataType_S32,   1, (ImU32)IM_OFFSETOF(ImPlotStyle, Marker)           }, // ImPlotStyleVar_Marker
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, MarkerSize)       }, // ImPlotStyleVar_MarkerSize
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, MarkerWeight)     }, // ImPlotStyleVar_MarkerWeight
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, FillAlpha)        }, // ImPlotStyleVar_FillAlpha
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, ErrorBarSize)     }, // ImPlotStyleVar_ErrorBarSize
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, ErrorBarWeight)   }, // ImPlotStyleVar_ErrorBarWeight
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, DigitalBitHeight) }, // ImPlotStyleVar_DigitalBitHeight
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, DigitalBitGap)    }, // ImPlotStyleVar_DigitalBitGap

    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, PlotBorderSize)   }, // ImPlotStyleVar_PlotBorderSize
    { ImGuiDataType_Float, 1, (ImU32)IM_OFFSETOF(ImPlotStyle, MinorAlpha)       }, // ImPlotStyleVar_MinorAlpha
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, MajorTickLen)     }, // ImPlotStyleVar_MajorTickLen
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, MinorTickLen)     }, // ImPlotStyleVar_MinorTickLen
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, MajorTickSize)    }, // ImPlotStyleVar_MajorTickSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, MinorTickSize)    }, // ImPlotStyleVar_MinorTickSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, MajorGridSize)    }, // ImPlotStyleVar_MajorGridSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, MinorGridSize)    }, // ImPlotStyleVar_MinorGridSize
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, PlotPadding)      }, // ImPlotStyleVar_PlotPadding
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, LabelPadding)     }, // ImPlotStyleVar_LabelPaddine
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, LegendPadding)    }, // ImPlotStyleVar_LegendPadding
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, InfoPadding)      }, // ImPlotStyleVar_InfoPadding
    { ImGuiDataType_Float, 2, (ImU32)IM_OFFSETOF(ImPlotStyle, PlotMinSize)      }  // ImPlotStyleVar_PlotMinSize
};

static const ImPlotStyleVarInfo* GetPlotStyleVarInfo(ImPlotStyleVar idx) {
    IM_ASSERT(idx >= 0 && idx < ImPlotStyleVar_COUNT);
    IM_ASSERT(IM_ARRAYSIZE(GPlotStyleVarInfo) == ImPlotStyleVar_COUNT);
    return &GPlotStyleVarInfo[idx];
}

//-----------------------------------------------------------------------------
// Generic Helpers
//-----------------------------------------------------------------------------

void AddTextVertical(ImDrawList *DrawList, ImVec2 pos, ImU32 col, const char *text_begin, const char* text_end) {
    if (!text_end)
        text_end = text_begin + strlen(text_begin);
    ImGuiContext& g = *GImGui;
    ImFont* font = g.Font;
    pos.x = IM_FLOOR(pos.x + font->DisplayOffset.y);
    pos.y = IM_FLOOR(pos.y + font->DisplayOffset.x);
    const char* s = text_begin;
    const int vtx_count = (int)(text_end - s) * 4;
    const int idx_count = (int)(text_end - s) * 6;
    DrawList->PrimReserve(idx_count, vtx_count);
    const float scale = g.FontSize / font->FontSize;
    while (s < text_end) {
        unsigned int c = (unsigned int)*s;
        if (c < 0x80) {
            s += 1;
        }
        else {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }
        const ImFontGlyph * glyph = font->FindGlyph((ImWchar)c);
        if (glyph == NULL)
            continue;
        DrawList->PrimQuadUV(pos + ImVec2(glyph->Y0, -glyph->X0) * scale, pos + ImVec2(glyph->Y0, -glyph->X1) * scale,
                             pos + ImVec2(glyph->Y1, -glyph->X1) * scale, pos + ImVec2(glyph->Y1, -glyph->X0) * scale,
                             ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V0),
                             ImVec2(glyph->U1, glyph->V1), ImVec2(glyph->U0, glyph->V1),
                             col);
        pos.y -= glyph->AdvanceX * scale;
    }
}

double NiceNum(double x, bool round) {
    double f;  /* fractional part of x */
    double nf; /* nice, rounded fraction */
    int expv = (int)floor(ImLog10(x));
    f = x / ImPow(10.0, (double)expv); /* between 1 and 10 */
    if (round)
        if (f < 1.5)
            nf = 1;
        else if (f < 3)
            nf = 2;
        else if (f < 7)
            nf = 5;
        else
            nf = 10;
    else if (f <= 1)
        nf = 1;
    else if (f <= 2)
        nf = 2;
    else if (f <= 5)
        nf = 5;
    else
        nf = 10;
    return nf * ImPow(10.0, expv);
}

//-----------------------------------------------------------------------------
// Context Utils
//-----------------------------------------------------------------------------

ImPlotContext* CreateContext() {
    ImPlotContext* ctx = IM_NEW(ImPlotContext)();
    Initialize(ctx);
    if (GImPlot == NULL)
        SetCurrentContext(ctx);
    return ctx;
}

void DestroyContext(ImPlotContext* ctx) {
    if (ctx == NULL)
        ctx = GImPlot;
    if (GImPlot == ctx)
        SetCurrentContext(NULL);
    IM_DELETE(ctx);
}

ImPlotContext* GetCurrentContext() {
    return GImPlot;
}

void SetCurrentContext(ImPlotContext* ctx) {
    GImPlot = ctx;
}

void Initialize(ImPlotContext* ctx) {
    Reset(ctx);
    ctx->Colormap = GetColormap(ImPlotColormap_Default, &ctx->ColormapSize);
}

void Reset(ImPlotContext* ctx) {
    // end child window if it was made
    if (ctx->ChildWindowMade)
        ImGui::EndChild();
    ctx->ChildWindowMade = false;
    // reset the next plot/item data
    ctx->NextPlotData = ImPlotNextPlotData();
    ctx->NextItemStyle = ImPlotItemStyle();
    // reset items count
    ctx->VisibleItemCount = 0;
    // reset legend items
    ctx->LegendIndices.shrink(0);
    ctx->LegendLabels.Buf.shrink(0);
    // reset ticks/labels
    ctx->XTicks.Reset();
    for (int i = 0; i < 3; ++i) {
        ctx->YTicks[i].Reset();
    }
    // reset extents/fit
    ctx->FitThisFrame = false;
    ctx->FitX = false;
    ctx->ExtentsX.Min = HUGE_VAL;
    ctx->ExtentsX.Max = -HUGE_VAL;
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        ctx->ExtentsY[i].Min = HUGE_VAL;
        ctx->ExtentsY[i].Max = -HUGE_VAL;
        ctx->FitY[i] = false;
    }
    // reset digital plot items count
    ctx->DigitalPlotItemCnt = 0;
    ctx->DigitalPlotOffset = 0;
    // nullify plot
    ctx->CurrentPlot = NULL;
    ctx->CurrentItem = NULL;
}

//-----------------------------------------------------------------------------
// Plot Utils
//-----------------------------------------------------------------------------

ImPlotState* GetPlot(const char* title) {
    ImGuiWindow*   Window = GImGui->CurrentWindow;
    const ImGuiID  ID     = Window->GetID(title);
    return GImPlot->Plots.GetByKey(ID);
}

ImPlotState* GetCurrentPlot() {
    return GImPlot->CurrentPlot;
}

void BustPlotCache() {
    GImPlot->Plots.Clear();
}

void FitPoint(const ImPlotPoint& p) {
    ImPlotContext& gp = *GImPlot;
    const int y_axis  = gp.CurrentPlot->CurrentYAxis;
    ImPlotRange& ex_x = gp.ExtentsX;
    ImPlotRange& ex_y = gp.ExtentsY[y_axis];
    const bool log_x  = ImHasFlag(gp.CurrentPlot->XAxis.Flags, ImPlotAxisFlags_LogScale);
    const bool log_y  = ImHasFlag(gp.CurrentPlot->YAxis[y_axis].Flags, ImPlotAxisFlags_LogScale);
    if (!ImNanOrInf(p.x) && !(log_x && p.x <= 0)) {
        ex_x.Min = p.x < ex_x.Min ? p.x : ex_x.Min;
        ex_x.Max = p.x > ex_x.Max ? p.x : ex_x.Max;
    }
    if (!ImNanOrInf(p.y) && !(log_y && p.y <= 0)) {
        ex_y.Min = p.y < ex_y.Min ? p.y : ex_y.Min;
        ex_y.Max = p.y > ex_y.Max ? p.y : ex_y.Max;
    }
}

void PushLinkedAxis(ImPlotAxis& axis) {
    if (axis.LinkedMin) { *axis.LinkedMin = axis.Range.Min; }
    if (axis.LinkedMax) { *axis.LinkedMax = axis.Range.Max; }
}

void PullLinkedAxis(ImPlotAxis& axis) {
    if (axis.LinkedMin) { axis.SetMin(*axis.LinkedMin); }
    if (axis.LinkedMax) { axis.SetMax(*axis.LinkedMax); }
}

//-----------------------------------------------------------------------------
// Coordinate Utils
//-----------------------------------------------------------------------------

void UpdateTransformCache() {
    ImPlotContext& gp = *GImPlot;
    // get pixels for transforms
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        gp.PixelRange[i] = ImRect(ImHasFlag(gp.CurrentPlot->XAxis.Flags, ImPlotAxisFlags_Invert) ? gp.BB_Plot.Max.x : gp.BB_Plot.Min.x,
                                  ImHasFlag(gp.CurrentPlot->YAxis[i].Flags, ImPlotAxisFlags_Invert) ? gp.BB_Plot.Min.y : gp.BB_Plot.Max.y,
                                  ImHasFlag(gp.CurrentPlot->XAxis.Flags, ImPlotAxisFlags_Invert) ? gp.BB_Plot.Min.x : gp.BB_Plot.Max.x,
                                  ImHasFlag(gp.CurrentPlot->YAxis[i].Flags, ImPlotAxisFlags_Invert) ? gp.BB_Plot.Max.y : gp.BB_Plot.Min.y);
        gp.My[i] = (gp.PixelRange[i].Max.y - gp.PixelRange[i].Min.y) / gp.CurrentPlot->YAxis[i].Range.Size();
    }
    gp.LogDenX = ImLog10(gp.CurrentPlot->XAxis.Range.Max / gp.CurrentPlot->XAxis.Range.Min);
    for (int i = 0; i < IMPLOT_Y_AXES; i++)
        gp.LogDenY[i] = ImLog10(gp.CurrentPlot->YAxis[i].Range.Max / gp.CurrentPlot->YAxis[i].Range.Min);
    gp.Mx = (gp.PixelRange[0].Max.x - gp.PixelRange[0].Min.x) / gp.CurrentPlot->XAxis.Range.Size();
}

ImPlotPoint PixelsToPlot(float x, float y, int y_axis_in) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "PixelsToPlot() needs to be called between BeginPlot() and EndPlot()!");
    const int y_axis = y_axis_in >= 0 ? y_axis_in : gp.CurrentPlot->CurrentYAxis;
    ImPlotPoint plt;
    plt.x = (x - gp.PixelRange[y_axis].Min.x) / gp.Mx + gp.CurrentPlot->XAxis.Range.Min;
    plt.y = (y - gp.PixelRange[y_axis].Min.y) / gp.My[y_axis] + gp.CurrentPlot->YAxis[y_axis].Range.Min;
    if (ImHasFlag(gp.CurrentPlot->XAxis.Flags, ImPlotAxisFlags_LogScale)) {
        double t = (plt.x - gp.CurrentPlot->XAxis.Range.Min) / gp.CurrentPlot->XAxis.Range.Size();
        plt.x = ImPow(10, t * gp.LogDenX) * gp.CurrentPlot->XAxis.Range.Min;
    }
    if (ImHasFlag(gp.CurrentPlot->YAxis[y_axis].Flags, ImPlotAxisFlags_LogScale)) {
        double t = (plt.y - gp.CurrentPlot->YAxis[y_axis].Range.Min) / gp.CurrentPlot->YAxis[y_axis].Range.Size();
        plt.y = ImPow(10, t * gp.LogDenY[y_axis]) * gp.CurrentPlot->YAxis[y_axis].Range.Min;
    }
    return plt;
}

ImPlotPoint PixelsToPlot(const ImVec2& pix, int y_axis) {
    return PixelsToPlot(pix.x, pix.y, y_axis);
}

// This function is convenient but should not be used to process a high volume of points. Use the Transformer structs below instead.
ImVec2 PlotToPixels(double x, double y, int y_axis_in) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "PlotToPixels() needs to be called between BeginPlot() and EndPlot()!");
    const int y_axis = y_axis_in >= 0 ? y_axis_in : gp.CurrentPlot->CurrentYAxis;
    ImVec2 pix;
    if (ImHasFlag(gp.CurrentPlot->XAxis.Flags, ImPlotAxisFlags_LogScale)) {
        double t = ImLog10(x / gp.CurrentPlot->XAxis.Range.Min) / gp.LogDenX;
        x       = ImLerp(gp.CurrentPlot->XAxis.Range.Min, gp.CurrentPlot->XAxis.Range.Max, (float)t);
    }
    if (ImHasFlag(gp.CurrentPlot->YAxis[y_axis].Flags, ImPlotAxisFlags_LogScale)) {
        double t = ImLog10(y / gp.CurrentPlot->YAxis[y_axis].Range.Min) / gp.LogDenY[y_axis];
        y       = ImLerp(gp.CurrentPlot->YAxis[y_axis].Range.Min, gp.CurrentPlot->YAxis[y_axis].Range.Max, (float)t);
    }
    pix.x = (float)(gp.PixelRange[y_axis].Min.x + gp.Mx * (x - gp.CurrentPlot->XAxis.Range.Min));
    pix.y = (float)(gp.PixelRange[y_axis].Min.y + gp.My[y_axis] * (y - gp.CurrentPlot->YAxis[y_axis].Range.Min));
    return pix;
}

// This function is convenient but should not be used to process a high volume of points. Use the Transformer structs below instead.
ImVec2 PlotToPixels(const ImPlotPoint& plt, int y_axis) {
    return PlotToPixels(plt.x, plt.y, y_axis);
}

//-----------------------------------------------------------------------------
// Legend Utils
//-----------------------------------------------------------------------------

int GetLegendCount() {
    ImPlotContext& gp = *GImPlot;
    return gp.LegendIndices.size();
}

const char* GetLegendLabel(int i) {
    ImPlotContext& gp = *GImPlot;
    ImPlotItem* item  = gp.CurrentPlot->Items.GetByIndex(gp.LegendIndices[i]);
    IM_ASSERT(item->NameOffset != -1 && item->NameOffset < gp.LegendLabels.Buf.Size);
    return gp.LegendLabels.Buf.Data + item->NameOffset;
}

//-----------------------------------------------------------------------------
// Tick Utils
//-----------------------------------------------------------------------------

void LabelTickDefault(ImPlotTick& tick, ImGuiTextBuffer& buffer) {
    char temp[32];
    if (tick.ShowLabel) {
        tick.BufferOffset = buffer.size();
        snprintf(temp, 32, "%.10g", tick.PlotPos);
        buffer.append(temp, temp + strlen(temp) + 1);
        tick.LabelSize = ImGui::CalcTextSize(buffer.Buf.Data + tick.BufferOffset);
    }
}

void LabelTickScientific(ImPlotTick& tick, ImGuiTextBuffer& buffer) {
    char temp[32];
    if (tick.ShowLabel) {
        tick.BufferOffset = buffer.size();
        snprintf(temp, 32, "%.0E", tick.PlotPos);
        buffer.append(temp, temp + strlen(temp) + 1);
        tick.LabelSize = ImGui::CalcTextSize(buffer.Buf.Data + tick.BufferOffset);
    }
}

void AddTicksDefault(const ImPlotRange& range, int nMajor, int nMinor, ImPlotTickCollection& ticks) {
    const double nice_range = NiceNum(range.Size() * 0.99, false);
    const double interval   = NiceNum(nice_range / (nMajor - 1), true);
    const double graphmin   = floor(range.Min / interval) * interval;
    const double graphmax   = ceil(range.Max / interval) * interval;
    for (double major = graphmin; major < graphmax + 0.5 * interval; major += interval) {
        if (range.Contains(major))
            ticks.AddTick(major, true, true, LabelTickDefault);
        for (int i = 1; i < nMinor; ++i) {
            double minor = major + i * interval / nMinor;
            if (range.Contains(minor))
                ticks.AddTick(minor, false, true, LabelTickDefault);
        }
    }
}

void AddTicksLogarithmic(const ImPlotRange& range, int nMajor, ImPlotTickCollection& ticks) {
    if (range.Min <= 0 || range.Max <= 0)
        return;
    double log_min = ImLog10(range.Min);
    double log_max = ImLog10(range.Max);
    int exp_step  = ImMax(1,(int)(log_max - log_min) / nMajor);
    int exp_min   = (int)log_min;
    int exp_max   = (int)log_max;
    if (exp_step != 1) {
        while(exp_step % 3 != 0)       exp_step++; // make step size multiple of three
        while(exp_min % exp_step != 0) exp_min--;  // decrease exp_min until exp_min + N * exp_step will be 0
    }
    for (int e = exp_min - exp_step; e < (exp_max + exp_step); e += exp_step) {
        double major1 = ImPow(10, (double)(e));
        double major2 = ImPow(10, (double)(e + 1));
        double interval = (major2 - major1) / 9;
        if (major1 >= (range.Min - DBL_EPSILON) && major1 <= (range.Max + DBL_EPSILON))
            ticks.AddTick(major1, true, true, LabelTickScientific);
        for (int j = 0; j < exp_step; ++j) {
            major1 = ImPow(10, (double)(e+j));
            major2 = ImPow(10, (double)(e+j+1));
            interval = (major2 - major1) / 9;
            for (int i = 1; i < (9 + (int)(j < (exp_step - 1))); ++i) {
                double minor = major1 + i * interval;
                if (minor >= (range.Min - DBL_EPSILON) && minor <= (range.Max + DBL_EPSILON))
                    ticks.AddTick(minor, false, false, LabelTickScientific);

            }
        }
    }
}

void AddTicksCustom(const double* values, const char** labels, int n, ImPlotTickCollection& ticks) {
    for (int i = 0; i < n; ++i) {
        ImPlotTick tick(values[i], false, true);
        if (labels != NULL) {
            tick.BufferOffset = ticks.Labels.size();
            ticks.Labels.append(labels[i], labels[i] + strlen(labels[i]) + 1);
            tick.LabelSize = ImGui::CalcTextSize(labels[i]);
        }
        else {
            LabelTickDefault(tick, ticks.Labels);
        }
        ticks.AddTick(tick);
    }
}

//-----------------------------------------------------------------------------
// Time Ticks and Utils
//-----------------------------------------------------------------------------

// this may not be thread safe?
static const double TimeUnitSpans[ImPlotTimeUnit_COUNT] = {
    0.000001,
    0.001,
    1,
    60,
    3600,
    86400,
    2629800,
    31557600
};

inline ImPlotTimeUnit GetUnitForRange(double range) {
    static double cutoffs[ImPlotTimeUnit_COUNT] = {0.001, 1, 60, 3600, 86400, 2629800, 31557600, IMPLOT_MAX_TIME};
    for (int i = 0; i < ImPlotTimeUnit_COUNT; ++i) {
        if (range <= cutoffs[i])
            return (ImPlotTimeUnit)i;
    }
    return ImPlotTimeUnit_Yr;
}

inline int LowerBoundStep(int max_divs, const int* divs, const int* step, int size) {
    if (max_divs < divs[0])
        return 0;
    for (int i = 1; i < size; ++i) {
        if (max_divs < divs[i])
            return step[i-1];
    }
    return step[size-1];
}

inline int GetTimeStep(int max_divs, ImPlotTimeUnit unit) {
    if (unit == ImPlotTimeUnit_Ms || unit == ImPlotTimeUnit_Us) {
        static const int step[] = {500,250,200,100,50,25,20,10,5,2,1};
        static const int divs[] = {2,4,5,10,20,40,50,100,200,500,1000};
        return LowerBoundStep(max_divs, divs, step, 11);
    }
    if (unit == ImPlotTimeUnit_S || unit == ImPlotTimeUnit_Min) {
        static const int step[] = {30,15,10,5,1};
        static const int divs[] = {2,4,6,12,60};
        return LowerBoundStep(max_divs, divs, step, 5);
    }
    else if (unit == ImPlotTimeUnit_Hr) {
        static const int step[] = {12,6,3,2,1};
        static const int divs[] = {2,4,8,12,24};
        return LowerBoundStep(max_divs, divs, step, 5);
    }
    else if (unit == ImPlotTimeUnit_Day) {
        static const int step[] = {14,7,2,1};
        static const int divs[] = {2,4,14,28};
        return LowerBoundStep(max_divs, divs, step, 4);
    }
    else if (unit == ImPlotTimeUnit_Mo) {
        static const int step[] = {6,3,2,1};
        static const int divs[] = {2,4,6,12};
        return LowerBoundStep(max_divs, divs, step, 4);
    }
    return 0;
}

ImPlotTime MkGmtTime(struct tm *ptm) {
    time_t secs = 0;
    int year = ptm->tm_year + 1900;
    for (int y = 1970; y < year; ++y)
        secs += (IsLeapYear(y)? 366: 365) * 86400;
    for (int m = 0; m < ptm->tm_mon; ++m)
        secs += GetDaysInMonth(year, m) * 86400;
    secs += (ptm->tm_mday - 1) * 86400;
    secs += ptm->tm_hour       * 3600;
    secs += ptm->tm_min        * 60;
    secs += ptm->tm_sec;
    return ImPlotTime(secs,0);
}

tm* GetGmtTime(const ImPlotTime& t, tm* ptm)
{
#ifdef _WIN32
  if (gmtime_s(ptm, &t.S) == 0)
    return ptm;
  else
    return NULL;
#else
  return gmtime_r(&t.S, ptm);
#endif
}

ImPlotTime MkLocTime(struct tm *ptm) {
    ImPlotTime t;
    t.S = mktime(ptm);
    return t;
}

tm* GetLocTime(const ImPlotTime& t, tm* ptm) {
#ifdef _WIN32
  if (localtime_s(ptm, &t.S) == 0)
    return ptm;
  else
    return NULL;
#else
    return localtime_r(&t.S, ptm);
#endif
}

inline ImPlotTime MkTime(struct tm *ptm) {
    if (GetStyle().UseLocalTime)
        return MkLocTime(ptm);
    else
        return MkGmtTime(ptm);
}

inline tm* GetTime(const ImPlotTime& t, tm* ptm) {
    if (GetStyle().UseLocalTime)
        return GetLocTime(t,ptm);
    else
        return GetGmtTime(t,ptm);
}

ImPlotTime AddTime(const ImPlotTime& t, ImPlotTimeUnit unit, int count) {
    tm& Tm = GImPlot->Tm;
    ImPlotTime t_out = t;
    switch(unit) {
        case ImPlotTimeUnit_Us:  t_out.Us += count;         break;
        case ImPlotTimeUnit_Ms:  t_out.Us += count * 1000;  break;
        case ImPlotTimeUnit_S:   t_out.S  += count;         break;
        case ImPlotTimeUnit_Min: t_out.S  += count * 60;    break;
        case ImPlotTimeUnit_Hr:  t_out.S  += count * 3600;  break;
        case ImPlotTimeUnit_Day: t_out.S  += count * 86400; break;
        case ImPlotTimeUnit_Mo:  for (int i = 0; i < count; ++i) {    // this might have a bug
                                     GetTime(t_out, &Tm);
                                     t_out.S += 86400 * GetDaysInMonth(Tm.tm_year + 1900, Tm.tm_mon);
                                 }
                                 break;
        case ImPlotTimeUnit_Yr:  for (int i = 0; i < count; ++i) {
                                    if (IsLeapYear(GetYear(t_out)))
                                        t_out.S += 366 * 86400;
                                    else
                                        t_out.S += 365 * 86400;
                                 }
                                 break;
        default:                 break;
    }
    t_out.RollOver();
    return t_out;
}

ImPlotTime FloorTime(const ImPlotTime& t, ImPlotTimeUnit unit) {
    GetTime(t, &GImPlot->Tm);
    switch (unit) {
        case ImPlotTimeUnit_S:   return ImPlotTime(t.S, 0);
        case ImPlotTimeUnit_Ms:  return ImPlotTime(t.S, (t.Us / 1000) * 1000);
        case ImPlotTimeUnit_Us:  return t;
        case ImPlotTimeUnit_Yr:  GImPlot->Tm.tm_mon  = 0; // fall-through
        case ImPlotTimeUnit_Mo:  GImPlot->Tm.tm_mday = 1; // fall-through
        case ImPlotTimeUnit_Day: GImPlot->Tm.tm_hour = 0; // fall-through
        case ImPlotTimeUnit_Hr:  GImPlot->Tm.tm_min  = 0; // fall-through
        case ImPlotTimeUnit_Min: GImPlot->Tm.tm_sec  = 0; break;
        default:                 return t;
    }
    return MkTime(&GImPlot->Tm);
}

ImPlotTime CeilTime(const ImPlotTime& t, ImPlotTimeUnit unit) {
    return AddTime(FloorTime(t, unit), unit, 1);
}

ImPlotTime RoundTime(const ImPlotTime& t, ImPlotTimeUnit unit) {
    ImPlotTime t1 = FloorTime(t, unit);
    ImPlotTime t2 = AddTime(t1,unit,1);
    if (t1.S == t2.S)
        return t.Us - t1.Us < t2.Us - t.Us ? t1 : t2;
    return t.S - t1.S < t2.S - t.S ? t1 : t2;
}

int GetYear(const ImPlotTime& t) {
    tm& Tm = GImPlot->Tm;
    GetTime(t, &Tm);
    return Tm.tm_year + 1900;
}

ImPlotTime MakeYear(int year) {
    int yr = year - 1900;
    if (yr < 0)
        yr = 0;
    tm& Tm = GImPlot->Tm;
    Tm.tm_sec  = 0;
    Tm.tm_min  = 0;
    Tm.tm_hour = 0;
    Tm.tm_mday = 1;
    Tm.tm_mon  = 0;
    Tm.tm_year = yr;
    Tm.tm_sec  = 0;
    return MkTime(&Tm);
}

int FormatTime(const ImPlotTime& t, char* buffer, int size, ImPlotTimeFmt fmt) {
    tm& Tm = GImPlot->Tm;
    GetTime(t, &Tm);

    const char* ap = Tm.tm_hour < 12 ? "am" : "pm";
    const int us   = t.Us % 1000;
    const int ms   = t.Us / 1000;
    const int sec  = Tm.tm_sec;
    const int min  = Tm.tm_min;
    const int hr   = (Tm.tm_hour == 0 || Tm.tm_hour == 12) ? 12 : Tm.tm_hour % 12;
    const int day  = Tm.tm_mday;
    const int mon  = Tm.tm_mon + 1;
    const int year = Tm.tm_year + 1900;
    const int yr   = year % 100;

    static const char mnames[12][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

    switch(fmt) {
        case ImPlotTimeFmt_Us:            return snprintf(buffer, size, ".%03d %03d", ms, us);
        case ImPlotTimeFmt_SUs:           return snprintf(buffer, size, ":%02d.%03d %03d", sec, ms, us);
        case ImPlotTimeFmt_SMs:           return snprintf(buffer, size, ":%02d.%03d", sec, ms);
        case ImPlotTimeFmt_S:             return snprintf(buffer, size, ":%02d", sec);
        case ImPlotTimeFmt_HrMinS:        return snprintf(buffer, size, "%d:%02d:%02d%s", hr, min, sec, ap);
        case ImPlotTimeFmt_HrMin:         return snprintf(buffer, size, "%d:%02d%s", hr, min, ap);
        case ImPlotTimeFmt_Hr:            return snprintf(buffer, size, "%d%s", hr, ap);
        case ImPlotTimeFmt_DayMo:         return snprintf(buffer, size, "%d/%d", mon, day);
        case ImPlotTimeFmt_DayMoHr:       return snprintf(buffer, size, "%d/%d %d%s", mon, day, hr, ap);
        case ImPlotTimeFmt_DayMoHrMin:    return snprintf(buffer, size, "%d/%d %d:%02d%s", mon, day, hr, min, ap);
        case ImPlotTimeFmt_DayMoYr:       return snprintf(buffer, size, "%d/%d/%02d", mon, day, yr);
        case ImPlotTimeFmt_DayMoYrHrMin:  return snprintf(buffer, size, "%d/%d/%02d %d:%02d%s", mon, day, yr, hr, min, ap);
        case ImPlotTimeFmt_DayMoYrHrMinS: return snprintf(buffer, size, "%d/%d/%02d %d:%02d:%02d%s", mon, day, yr, hr, min, sec, ap);
        case ImPlotTimeFmt_MoYr:          return snprintf(buffer, size, "%s %d", mnames[Tm.tm_mon], year);
        case ImPlotTimeFmt_Mo:            return snprintf(buffer, size, "%s", mnames[Tm.tm_mon]);
        case ImPlotTimeFmt_Yr:            return snprintf(buffer, size, "%d", year);
        default:                          return 0;
    }
}

void PrintTime(const ImPlotTime& t, ImPlotTimeFmt fmt) {
    static char buff[32];
    FormatTime(t, buff, 32, fmt);
    printf("%s\n",buff);
}

// Returns the nominally largest possible width for a time format
inline float GetTimeLabelWidth(ImPlotTimeFmt fmt) {
    switch (fmt) {
        case ImPlotTimeFmt_Us:            return ImGui::CalcTextSize(".888 888").x;             // .428 552
        case ImPlotTimeFmt_SUs:           return ImGui::CalcTextSize(":88.888 888").x;          // :29.428 552
        case ImPlotTimeFmt_SMs:           return ImGui::CalcTextSize(":88.888").x;             // :29.428
        case ImPlotTimeFmt_S:             return ImGui::CalcTextSize(":88").x;                 // :29
        case ImPlotTimeFmt_HrMinS:        return ImGui::CalcTextSize("88:88:88pm").x;          // 7:21:29pm
        case ImPlotTimeFmt_HrMin:         return ImGui::CalcTextSize("88:88pm").x;             // 7:21pm
        case ImPlotTimeFmt_Hr:            return ImGui::CalcTextSize("88pm").x;                // 7pm
        case ImPlotTimeFmt_DayMo:         return ImGui::CalcTextSize("88/88").x;               // 10/3
        case ImPlotTimeFmt_DayMoHr:       return ImGui::CalcTextSize("88/88 88pm").x;          // 10/3 7:21pm
        case ImPlotTimeFmt_DayMoHrMin:    return ImGui::CalcTextSize("88/88 88:88pm").x;       // 10/3 7:21pm
        case ImPlotTimeFmt_DayMoYr:       return ImGui::CalcTextSize("88/88/88").x;            // 10/3/1991
        case ImPlotTimeFmt_DayMoYrHrMin:  return ImGui::CalcTextSize("88/88/88 88:88pm").x;    // 10/3/91 7:21pm
        case ImPlotTimeFmt_DayMoYrHrMinS: return ImGui::CalcTextSize("88/88/88 88:88:88pm").x; // 10/3/91 7:21:29pm
        case ImPlotTimeFmt_MoYr:          return ImGui::CalcTextSize("MMM 8888").x;            // Oct 1991
        case ImPlotTimeFmt_Mo:            return ImGui::CalcTextSize("MMM").x;                 // Oct
        case ImPlotTimeFmt_Yr:            return ImGui::CalcTextSize("8888").x;                // 1991
        default:                          return 0;
    }
}

inline void LabelTickTime(ImPlotTick& tick, ImGuiTextBuffer& buffer, const ImPlotTime& t, ImPlotTimeFmt fmt) {
    char temp[32];
    if (tick.ShowLabel) {
        tick.BufferOffset = buffer.size();
        FormatTime(t, temp, 32, fmt);
        buffer.append(temp, temp + strlen(temp) + 1);
        tick.LabelSize = ImGui::CalcTextSize(buffer.Buf.Data + tick.BufferOffset);
    }
}

inline bool TimeLabelSame(const char* l1, const char* l2) {
    size_t len1 = strlen(l1);
    size_t len2 = strlen(l2);
    size_t n  = len1 < len2 ? len1 : len2;
    return strcmp(l1 + len1 - n, l2 + len2 - n) == 0;
}

static const ImPlotTimeFmt TimeFormatLevel0[ImPlotTimeUnit_COUNT] = {
    ImPlotTimeFmt_Us,
    ImPlotTimeFmt_SMs,
    ImPlotTimeFmt_S,
    ImPlotTimeFmt_HrMin,
    ImPlotTimeFmt_Hr,
    ImPlotTimeFmt_DayMo,
    ImPlotTimeFmt_Mo,
    ImPlotTimeFmt_Yr
};

static const ImPlotTimeFmt TimeFormatLevel1[ImPlotTimeUnit_COUNT] = {
    ImPlotTimeFmt_HrMin,
    ImPlotTimeFmt_HrMinS,
    ImPlotTimeFmt_HrMin,
    ImPlotTimeFmt_HrMin,
    ImPlotTimeFmt_DayMoYr,
    ImPlotTimeFmt_DayMoYr,
    ImPlotTimeFmt_Yr,
    ImPlotTimeFmt_Yr
};

static const ImPlotTimeFmt TimeFormatLevel1First[ImPlotTimeUnit_COUNT] = {
    ImPlotTimeFmt_DayMoYrHrMinS,
    ImPlotTimeFmt_DayMoYrHrMinS,
    ImPlotTimeFmt_DayMoYrHrMin,
    ImPlotTimeFmt_DayMoYrHrMin,
    ImPlotTimeFmt_DayMoYr,
    ImPlotTimeFmt_DayMoYr,
    ImPlotTimeFmt_Yr,
    ImPlotTimeFmt_Yr
};

static const ImPlotTimeFmt TimeFormatMouseCursor[ImPlotTimeUnit_COUNT] = {
    ImPlotTimeFmt_Us,
    ImPlotTimeFmt_SUs,
    ImPlotTimeFmt_SMs,
    ImPlotTimeFmt_HrMinS,
    ImPlotTimeFmt_HrMin,
    ImPlotTimeFmt_DayMoHr,
    ImPlotTimeFmt_DayMoYr,
    ImPlotTimeFmt_MoYr
};

void AddTicksTime(const ImPlotRange& range, float plot_width, ImPlotTickCollection& ticks) {
    // get units for level 0 and level 1 labels
    const ImPlotTimeUnit unit0 = GetUnitForRange(range.Size() / (plot_width / 100)); // level = 0 (top)
    const ImPlotTimeUnit unit1 = unit0 + 1;                                          // level = 1 (bottom)
    // get time format specs
    const ImPlotTimeFmt fmt0 = TimeFormatLevel0[unit0];
    const ImPlotTimeFmt fmt1 = TimeFormatLevel1[unit1];
    const ImPlotTimeFmt fmtf = TimeFormatLevel1First[unit1];
    // min max times
    const ImPlotTime t_min = ImPlotTime::FromDouble(range.Min);
    const ImPlotTime t_max = ImPlotTime::FromDouble(range.Max);
    // maximum allowable density of labels
    const float max_density = 0.5f;
    // book keeping
    const char* last_major  = NULL;
    if (unit0 != ImPlotTimeUnit_Yr) {
        // pixels per major (level 1) division
        const float pix_per_major_div = plot_width / (float)(range.Size() / TimeUnitSpans[unit1]);
        // nominal pixels taken up by labels
        const float fmt0_width = GetTimeLabelWidth(fmt0);
        const float fmt1_width = GetTimeLabelWidth(fmt1);
        const float fmtf_width = GetTimeLabelWidth(fmtf);
        // the maximum number of minor (level 0) labels that can fit between major (level 1) divisions
        const int   minor_per_major   = (int)(max_density * pix_per_major_div / fmt0_width);
        // the minor step size (level 0)
        const int step = GetTimeStep(minor_per_major, unit0);
        // generate ticks
        ImPlotTime t1 = FloorTime(ImPlotTime::FromDouble(range.Min), unit1);
        while (t1 < t_max) {
            // get next major
            const ImPlotTime t2 = AddTime(t1, unit1, 1);
            // add major tick
            if (t1 >= t_min && t1 <= t_max) {
                // minor level 0 tick
                ImPlotTick tick_min(t1.ToDouble(),true,true);
                tick_min.Level = 0;
                LabelTickTime(tick_min,ticks.Labels,t1,fmt0);
                ticks.AddTick(tick_min);
                // major level 1 tick
                ImPlotTick tick_maj(t1.ToDouble(),true,true);
                tick_maj.Level = 1;
                LabelTickTime(tick_maj,ticks.Labels,t1, last_major == NULL ? fmtf : fmt1);
                const char* this_major = ticks.Labels.Buf.Data + tick_maj.BufferOffset;
                if (last_major && TimeLabelSame(last_major,this_major))
                    tick_maj.ShowLabel = false;
                last_major = this_major;
                ticks.AddTick(tick_maj);
            }
            // add minor ticks up until next major
            if (minor_per_major > 1 && (t_min <= t2 && t1 <= t_max)) {
                ImPlotTime t12 = AddTime(t1, unit0, step);
                while (t12 < t2) {
                    float px_to_t2 = (float)((t2 - t12).ToDouble()/range.Size()) * plot_width;
                    if (t12 >= t_min && t12 <= t_max) {
                        ImPlotTick tick(t12.ToDouble(),false,px_to_t2 >= fmt0_width);
                        tick.Level =  0;
                        LabelTickTime(tick,ticks.Labels,t12,fmt0);
                        ticks.AddTick(tick);
                        if (last_major == NULL && px_to_t2 >= fmt0_width && px_to_t2 >= (fmt1_width + fmtf_width) / 2) {
                            ImPlotTick tick_maj(t12.ToDouble(),true,true);
                            tick_maj.Level = 1;
                            LabelTickTime(tick_maj,ticks.Labels,t12,fmtf);
                            last_major = ticks.Labels.Buf.Data + tick_maj.BufferOffset;
                            ticks.AddTick(tick_maj);
                        }
                    }
                    t12 = AddTime(t12, unit0, step);
                }
            }
            t1 = t2;
        }
    }
    else {
        const float label_width = GetTimeLabelWidth(TimeFormatLevel0[ImPlotTimeUnit_Yr]);
        const int   max_labels  = (int)(max_density * plot_width / label_width);
        const int year_min      = GetYear(t_min);
        const int year_max      = GetYear(CeilTime(t_max, ImPlotTimeUnit_Yr));
        const double nice_range = NiceNum((year_max - year_min)*0.99,false);
        const double interval   = NiceNum(nice_range / (max_labels - 1), true);
        const int graphmin      = (int)(floor(year_min / interval) * interval);
        const int graphmax      = (int)(ceil(year_max  / interval) * interval);
        const int step          = (int)interval <= 0 ? 1 : (int)interval;

        for (int y = graphmin; y < graphmax; y += step) {
            ImPlotTime t = MakeYear(y);
            if (t >= t_min && t <= t_max) {
                ImPlotTick tick(t.ToDouble(), true, true);
                tick.Level = 0;
                LabelTickTime(tick, ticks.Labels, t, TimeFormatLevel0[ImPlotTimeUnit_Yr]);
                ticks.AddTick(tick);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Axis Utils
//-----------------------------------------------------------------------------

void UpdateAxisColors(int axis_flag, ImPlotAxisColor* col) {
    const ImVec4 col_label = GetStyleColorVec4(axis_flag);
    const ImVec4 col_grid  = GetStyleColorVec4(axis_flag + 1);
    col->Major  = ImGui::GetColorU32(col_grid);
    col->Minor  = ImGui::GetColorU32(col_grid*ImVec4(1,1,1,GImPlot->Style.MinorAlpha));
    col->MajTxt = ImGui::GetColorU32(col_label);
    col->MinTxt = ImGui::GetColorU32(col_label);
}


//-----------------------------------------------------------------------------
// BeginPlot()
//-----------------------------------------------------------------------------

bool BeginPlot(const char* title, const char* x_label, const char* y_label, const ImVec2& size,
               ImPlotFlags flags, ImPlotAxisFlags x_flags, ImPlotAxisFlags y_flags, ImPlotAxisFlags y2_flags, ImPlotAxisFlags y3_flags)
{
    IM_ASSERT_USER_ERROR(GImPlot != NULL, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == NULL, "Mismatched BeginPlot()/EndPlot()!");
    IM_ASSERT_USER_ERROR(!(ImHasFlag(x_flags, ImPlotAxisFlags_Time) && ImHasFlag(x_flags, ImPlotAxisFlags_LogScale)), "ImPlotAxisFlags_Time and ImPlotAxisFlags_LogScale cannot be enabled at the same time!");
    IM_ASSERT_USER_ERROR(!ImHasFlag(y_flags, ImPlotAxisFlags_Time), "Y axes cannot display time formatted labels!");

    // FRONT MATTER  -----------------------------------------------------------

    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems) {
        Reset(GImPlot);
        return false;
    }

    const ImGuiID     ID       = Window->GetID(title);
    const ImGuiStyle &Style    = G.Style;
    const ImGuiIO &   IO       = ImGui::GetIO();

    bool just_created = gp.Plots.GetByKey(ID) == NULL;
    gp.CurrentPlot    = gp.Plots.GetOrAddByKey(ID);
    ImPlotState &plot = *gp.CurrentPlot;

    plot.CurrentYAxis = 0;

    if (just_created) {
        plot.Flags          = flags;
        plot.XAxis.Flags    = x_flags;
        plot.YAxis[0].Flags = y_flags;
        plot.YAxis[1].Flags = y2_flags;
        plot.YAxis[2].Flags = y3_flags;
    }
    else {
        // TODO: Check which individual flags changed, and only reset those!
        // There's probably an easy bit mask trick I'm not aware of.
        if (flags != plot.PreviousFlags)
            plot.Flags = flags;
        if (x_flags != plot.XAxis.PreviousFlags)
            plot.XAxis.Flags = x_flags;
        if (y_flags != plot.YAxis[0].PreviousFlags)
            plot.YAxis[0].Flags = y_flags;
        if (y2_flags != plot.YAxis[1].PreviousFlags)
            plot.YAxis[1].Flags = y2_flags;
        if (y3_flags != plot.YAxis[2].PreviousFlags)
            plot.YAxis[2].Flags = y3_flags;
    }

    plot.PreviousFlags          = flags;
    plot.XAxis.PreviousFlags    = x_flags;
    plot.YAxis[0].PreviousFlags = y_flags;
    plot.YAxis[1].PreviousFlags = y2_flags;
    plot.YAxis[2].PreviousFlags = y3_flags;

    // capture scroll with a child region
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoChild)) {
        ImGui::BeginChild(title, ImVec2(size.x == 0 ? IMPLOT_DEFAULT_W : size.x, size.y == 0 ? IMPLOT_DEFAULT_H : size.y));
        Window = ImGui::GetCurrentWindow();
        Window->ScrollMax.y = 1.0f;
        gp.ChildWindowMade = true;
    }
    else {
        gp.ChildWindowMade = false;
    }

    ImDrawList &DrawList = *Window->DrawList;

    // NextPlotData -----------------------------------------------------------

    // linked axes
    plot.XAxis.LinkedMin = gp.NextPlotData.LinkedXmin;
    plot.XAxis.LinkedMax = gp.NextPlotData.LinkedXmax;
    PullLinkedAxis(plot.XAxis);
    for (int i = 0; i < IMPLOT_Y_AXES; ++i) {
        plot.YAxis[i].LinkedMin = gp.NextPlotData.LinkedYmin[i];
        plot.YAxis[i].LinkedMax = gp.NextPlotData.LinkedYmax[i];
        PullLinkedAxis(plot.YAxis[i]);
    }

    if (gp.NextPlotData.HasXRange) {
        if (just_created || gp.NextPlotData.XRangeCond == ImGuiCond_Always)
            plot.XAxis.SetRange(gp.NextPlotData.X);
    }

    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (gp.NextPlotData.HasYRange[i]) {
            if (just_created || gp.NextPlotData.YRangeCond[i] == ImGuiCond_Always)
                plot.YAxis[i].SetRange(gp.NextPlotData.Y[i]);
        }
    }

    // AXIS STATES ------------------------------------------------------------
    gp.X    = ImPlotAxisState(&plot.XAxis,    gp.NextPlotData.HasXRange,    gp.NextPlotData.XRangeCond,    true);
    gp.Y[0] = ImPlotAxisState(&plot.YAxis[0], gp.NextPlotData.HasYRange[0], gp.NextPlotData.YRangeCond[0], true);
    gp.Y[1] = ImPlotAxisState(&plot.YAxis[1], gp.NextPlotData.HasYRange[1], gp.NextPlotData.YRangeCond[1], ImHasFlag(plot.Flags, ImPlotFlags_YAxis2));
    gp.Y[2] = ImPlotAxisState(&plot.YAxis[2], gp.NextPlotData.HasYRange[2], gp.NextPlotData.YRangeCond[2], ImHasFlag(plot.Flags, ImPlotFlags_YAxis3));

    gp.LockPlot = gp.X.Lock && gp.Y[0].Lock && gp.Y[1].Lock && gp.Y[2].Lock;

    for (int i = 0; i < IMPLOT_Y_AXES; ++i) {
        if (!ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LogScale) && !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_LogScale))
            gp.Scales[i] = ImPlotScale_LinLin;
        else if (ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LogScale) && !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_LogScale))
            gp.Scales[i] = ImPlotScale_LogLin;
        else if (!ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LogScale) && ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_LogScale))
            gp.Scales[i] = ImPlotScale_LinLog;
        else if (ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LogScale) && ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_LogScale))
            gp.Scales[i] = ImPlotScale_LogLog;
    }

    // constraints
    plot.XAxis.Constrain();
    for (int i = 0; i < IMPLOT_Y_AXES; ++i)
        plot.YAxis[i].Constrain();


    // AXIS COLORS -----------------------------------------------------------------

    UpdateAxisColors(ImPlotCol_XAxis,  &gp.Col_X);
    UpdateAxisColors(ImPlotCol_YAxis,  &gp.Col_Y[0]);
    UpdateAxisColors(ImPlotCol_YAxis2, &gp.Col_Y[1]);
    UpdateAxisColors(ImPlotCol_YAxis3, &gp.Col_Y[2]);

    // BB, PADDING, HOVER -----------------------------------------------------------

    // frame
    ImVec2 frame_size = ImGui::CalcItemSize(size, IMPLOT_DEFAULT_W, IMPLOT_DEFAULT_H);
    if (frame_size.x < gp.Style.PlotMinSize.x && size.x < 0.0f)
        frame_size.x = gp.Style.PlotMinSize.x;
    if (frame_size.y < gp.Style.PlotMinSize.y && size.y < 0.0f)
        frame_size.y = gp.Style.PlotMinSize.y;
    gp.BB_Frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ImGui::ItemSize(gp.BB_Frame);
    if (!ImGui::ItemAdd(gp.BB_Frame, 0, &gp.BB_Frame)) {
        Reset(GImPlot);
        return false;
    }
    gp.Hov_Frame = ImGui::ItemHoverable(gp.BB_Frame, ID);
    ImGui::RenderFrame(gp.BB_Frame.Min, gp.BB_Frame.Max, GetStyleColorU32(ImPlotCol_FrameBg), true, Style.FrameRounding);

    // canvas bb
    gp.BB_Canvas = ImRect(gp.BB_Frame.Min + gp.Style.PlotPadding, gp.BB_Frame.Max - gp.Style.PlotPadding);

    gp.RenderX = (!ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_NoGridLines) ||
                  !ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_NoTickMarks) ||
                  !ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_NoTickLabels));
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        gp.RenderY[i] = gp.Y[i].Present &&
                        (!ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_NoGridLines) ||
                         !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_NoTickMarks) ||
                         !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_NoTickLabels));
    }

    // plot bb

    // (1) calc top/bot padding and plot height
    const ImVec2 title_size = ImGui::CalcTextSize(title, NULL, true);
    const float txt_height  = ImGui::GetTextLineHeight();

    const float pad_top = title_size.x > 0.0f ? txt_height + gp.Style.LabelPadding.y : 0;
    const float pad_bot = (gp.X.HasLabels ? txt_height + gp.Style.LabelPadding.y + (gp.X.IsTime ? txt_height + gp.Style.LabelPadding.y : 0) : 0)
                        + (x_label ? txt_height + gp.Style.LabelPadding.y : 0);

    const float plot_height = gp.BB_Canvas.GetHeight() - pad_top - pad_bot;

    // (2) get y tick labels (needed for left/right pad)
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (gp.RenderY[i] && gp.NextPlotData.ShowDefaultTicksY[i]) {
            if (ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_LogScale))
                AddTicksLogarithmic(plot.YAxis[i].Range, ImMax(2, (int)IM_ROUND(plot_height * 0.02f)) ,gp.YTicks[i]);
            else
                AddTicksDefault(plot.YAxis[i].Range, ImMax(2, (int)IM_ROUND(0.0025 * plot_height)), IMPLOT_SUB_DIV, gp.YTicks[i]);
        }
    }

    // (3) calc left/right pad
    const float pad_left    = (y_label ? txt_height + gp.Style.LabelPadding.x : 0)
                            + (gp.Y[0].HasLabels ? gp.YTicks[0].MaxWidth + gp.Style.LabelPadding.x : 0);
    const float pad_right   = ((gp.Y[1].Present && gp.Y[1].HasLabels) ? gp.YTicks[1].MaxWidth + gp.Style.LabelPadding.x : 0)
                            + ((gp.Y[1].Present && gp.Y[2].Present)   ? gp.Style.LabelPadding.x + gp.Style.MinorTickLen.y : 0)
                            + ((gp.Y[2].Present && gp.Y[2].HasLabels) ? gp.YTicks[2].MaxWidth + gp.Style.LabelPadding.x : 0);

    const float plot_width = gp.BB_Canvas.GetWidth() - pad_left - pad_right;

    // (4) get x ticks
    if (gp.RenderX && gp.NextPlotData.ShowDefaultTicksX) {
        if (gp.X.IsTime)
            AddTicksTime(plot.XAxis.Range, plot_width, gp.XTicks);
        else if (ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LogScale))
            AddTicksLogarithmic(plot.XAxis.Range, (int)IM_ROUND(plot_width * 0.01f), gp.XTicks);
        else
            AddTicksDefault(plot.XAxis.Range, ImMax(2, (int)IM_ROUND(0.0025 * plot_width)), IMPLOT_SUB_DIV, gp.XTicks);
    }

    // (5) calc plot bb
    gp.BB_Plot  = ImRect(gp.BB_Canvas.Min + ImVec2(pad_left, pad_top), gp.BB_Canvas.Max - ImVec2(pad_right, pad_bot));
    gp.Hov_Plot = gp.BB_Plot.Contains(IO.MousePos);

    // x axis region bb and hover
    const ImRect xAxisRegion_bb(gp.BB_Plot.GetBL(), ImVec2(gp.BB_Plot.Max.x, gp.BB_Frame.Max.y));
    plot.XAxis.HoveredExt = xAxisRegion_bb.Contains(IO.MousePos);
    plot.XAxis.HoveredTot = plot.XAxis.HoveredExt || gp.Hov_Plot;

    // axis label reference
    gp.YAxisReference[0] = gp.BB_Plot.Min.x;
    gp.YAxisReference[1] = gp.BB_Plot.Max.x;
    gp.YAxisReference[2] = !gp.Y[1].Present ? gp.BB_Plot.Max.x : (gp.YAxisReference[1] + (gp.Y[1].HasLabels ? gp.Style.LabelPadding.x + gp.YTicks[1].MaxWidth : 0) + gp.Style.LabelPadding.x + gp.Style.MinorTickLen.y);

    // y axis regions bb and hover
    ImRect yAxisRegion_bb[IMPLOT_Y_AXES];
    yAxisRegion_bb[0] = ImRect(ImVec2(gp.BB_Frame.Min.x, gp.BB_Plot.Min.y), ImVec2(gp.BB_Plot.Min.x, gp.BB_Plot.Max.y));
    yAxisRegion_bb[1] = gp.Y[2].Present
                      ? ImRect(gp.BB_Plot.GetTR(), ImVec2(gp.YAxisReference[2], gp.BB_Plot.Max.y))
                      : ImRect(gp.BB_Plot.GetTR(), ImVec2(gp.BB_Frame.Max.x, gp.BB_Plot.Max.y));

    yAxisRegion_bb[2] = ImRect(ImVec2(gp.YAxisReference[2], gp.BB_Plot.Min.y), ImVec2(gp.BB_Frame.Max.x, gp.BB_Plot.Max.y));

    for (int i = 0; i < IMPLOT_Y_AXES; ++i) {
        plot.YAxis[i].HoveredExt = gp.Y[i].Present && yAxisRegion_bb[i].Contains(IO.MousePos);
        plot.YAxis[i].HoveredTot = plot.YAxis[i].HoveredExt || gp.Hov_Plot;
    }

#if 0
    ImGui::GetForegroundDrawList()->AddRect(gp.BB_Canvas.Min, gp.BB_Canvas.Max, IM_COL32_WHITE);
    ImGui::GetForegroundDrawList()->AddRectFilled(xAxisRegion_bb.Min, xAxisRegion_bb.Max, IM_COL32(255,0,0,plot.XAxis.HoveredTot ? 128 : 64));
    ImGui::GetForegroundDrawList()->AddRectFilled(yAxisRegion_bb[0].Min, yAxisRegion_bb[0].Max, IM_COL32(255,255,0,plot.YAxis[0].HoveredTot ? 128 : 64));
    if (gp.Y[1].Present)
        ImGui::GetForegroundDrawList()->AddRectFilled(yAxisRegion_bb[1].Min, yAxisRegion_bb[1].Max, IM_COL32(0,255,0,plot.YAxis[1].HoveredTot ? 128 : 64));
    if (gp.Y[2].Present)
        ImGui::GetForegroundDrawList()->AddRectFilled(yAxisRegion_bb[2].Min, yAxisRegion_bb[2].Max, IM_COL32(0,0,255,plot.YAxis[2].HoveredTot ? 128 : 64));
#endif

    const bool any_hov_y_axis_region = plot.YAxis[0].HoveredTot || plot.YAxis[1].HoveredTot || plot.YAxis[2].HoveredTot;

    // legend hovered from last frame
    const bool hov_legend = !ImHasFlag(plot.Flags, ImPlotFlags_NoLegend) ? gp.Hov_Frame && plot.BB_Legend.Contains(IO.MousePos) : false;

    bool hov_query = false;
    if (gp.Hov_Frame && gp.Hov_Plot && plot.Queried && !plot.Querying) {
        ImRect bb_query = plot.QueryRect;

        bb_query.Min += gp.BB_Plot.Min;
        bb_query.Max += gp.BB_Plot.Min;

        hov_query = bb_query.Contains(IO.MousePos);
    }

    // QUERY DRAG -------------------------------------------------------------
    if (plot.DraggingQuery && (IO.MouseReleased[gp.InputMap.PanButton] || !IO.MouseDown[gp.InputMap.PanButton])) {
        plot.DraggingQuery = false;
    }
    if (plot.DraggingQuery) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        plot.QueryRect.Min += IO.MouseDelta;
        plot.QueryRect.Max += IO.MouseDelta;
    }
    if (gp.Hov_Frame && gp.Hov_Plot && hov_query && !plot.DraggingQuery && !plot.Selecting && !hov_legend) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        const bool any_y_dragging = plot.YAxis[0].Dragging || plot.YAxis[1].Dragging || plot.YAxis[2].Dragging;
        if (IO.MouseDown[gp.InputMap.PanButton] && !plot.XAxis.Dragging && !any_y_dragging) {
            plot.DraggingQuery = true;
        }
    }

    // DRAG INPUT -------------------------------------------------------------

    // end drags
    if (plot.XAxis.Dragging && (IO.MouseReleased[gp.InputMap.PanButton] || !IO.MouseDown[gp.InputMap.PanButton])) {
        plot.XAxis.Dragging             = false;
        G.IO.MouseDragMaxDistanceSqr[0] = 0;
    }
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (plot.YAxis[i].Dragging && (IO.MouseReleased[gp.InputMap.PanButton] || !IO.MouseDown[gp.InputMap.PanButton])) {
            plot.YAxis[i].Dragging          = false;
            G.IO.MouseDragMaxDistanceSqr[0] = 0;
        }
    }
    const bool any_y_dragging = plot.YAxis[0].Dragging || plot.YAxis[1].Dragging || plot.YAxis[2].Dragging;
    bool drag_in_progress = plot.XAxis.Dragging || any_y_dragging;
    // do drag
    if (drag_in_progress) {
        UpdateTransformCache();
        if (!gp.X.Lock && plot.XAxis.Dragging) {
            ImPlotPoint plot_tl = PixelsToPlot(gp.BB_Plot.Min - IO.MouseDelta, 0);
            ImPlotPoint plot_br = PixelsToPlot(gp.BB_Plot.Max - IO.MouseDelta, 0);
            if (!gp.X.LockMin)
                plot.XAxis.SetMin(gp.X.Invert ? plot_br.x : plot_tl.x);
            if (!gp.X.LockMax)
                plot.XAxis.SetMax(gp.X.Invert ? plot_tl.x : plot_br.x);
        }
        for (int i = 0; i < IMPLOT_Y_AXES; i++) {
            if (!gp.Y[i].Lock && plot.YAxis[i].Dragging) {
                ImPlotPoint plot_tl = PixelsToPlot(gp.BB_Plot.Min - IO.MouseDelta, i);
                ImPlotPoint plot_br = PixelsToPlot(gp.BB_Plot.Max - IO.MouseDelta, i);
                if (!gp.Y[i].LockMin)
                    plot.YAxis[i].SetMin(gp.Y[i].Invert ? plot_tl.y : plot_br.y);
                if (!gp.Y[i].LockMax)
                    plot.YAxis[i].SetMax(gp.Y[i].Invert ? plot_br.y : plot_tl.y);
            }
        }
        // Set the mouse cursor based on which axes are moving.
        int direction = 0;
        if (!gp.X.Lock && plot.XAxis.Dragging) {
            direction |= (1 << 1);
        }
        for (int i = 0; i < IMPLOT_Y_AXES; i++) {
            if (!gp.Y[i].Present) { continue; }
            if (!gp.Y[i].Lock && plot.YAxis[i].Dragging) {
                direction |= (1 << 2);
                break;
            }
        }
        if (IO.MouseDragMaxDistanceSqr[0] > 5) {
            if (direction == 0)
                ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
            else if (direction == (1 << 1))
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            else if (direction == (1 << 2))
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            else
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        }
    }
    // start drag
    if (!drag_in_progress && gp.Hov_Frame && IO.MouseClicked[gp.InputMap.PanButton] && ImHasFlag(IO.KeyMods, gp.InputMap.PanMod) && !plot.Selecting && !hov_legend && !hov_query && !plot.DraggingQuery) {
        if (plot.XAxis.HoveredTot) {
            plot.XAxis.Dragging = true;
        }
        for (int i = 0; i < IMPLOT_Y_AXES; i++) {
            if (plot.YAxis[i].HoveredTot) {
                plot.YAxis[i].Dragging = true;
            }
        }
    }

    // SCROLL INPUT -----------------------------------------------------------

    if (gp.Hov_Frame && (plot.XAxis.HoveredTot || any_hov_y_axis_region) && IO.MouseWheel != 0) {
        UpdateTransformCache();
        float zoom_rate = IMPLOT_ZOOM_RATE;
        if (IO.MouseWheel > 0)
            zoom_rate = (-zoom_rate) / (1.0f + (2.0f * zoom_rate));
        float tx = ImRemap(IO.MousePos.x, gp.BB_Plot.Min.x, gp.BB_Plot.Max.x, 0.0f, 1.0f);
        float ty = ImRemap(IO.MousePos.y, gp.BB_Plot.Min.y, gp.BB_Plot.Max.y, 0.0f, 1.0f);
        if (plot.XAxis.HoveredTot && !gp.X.Lock) {
            ImPlotAxisScale axis_scale(0, tx, ty, zoom_rate);
            const ImPlotPoint& plot_tl = axis_scale.Min;
            const ImPlotPoint& plot_br = axis_scale.Max;
            if (!gp.X.LockMin)
                plot.XAxis.SetMin(gp.X.Invert ? plot_br.x : plot_tl.x);
            if (!gp.X.LockMax)
                plot.XAxis.SetMax(gp.X.Invert ? plot_tl.x : plot_br.x);
        }
        for (int i = 0; i < IMPLOT_Y_AXES; i++) {
            if (plot.YAxis[i].HoveredTot && !gp.Y[i].Lock) {
                ImPlotAxisScale axis_scale(i, tx, ty, zoom_rate);
                const ImPlotPoint& plot_tl = axis_scale.Min;
                const ImPlotPoint& plot_br = axis_scale.Max;
                if (!gp.Y[i].LockMin)
                    plot.YAxis[i].SetMin(gp.Y[i].Invert ? plot_tl.y : plot_br.y);
                if (!gp.Y[i].LockMax)
                    plot.YAxis[i].SetMax(gp.Y[i].Invert ? plot_br.y : plot_tl.y);
            }
        }
    }

    // BOX-SELECTION AND QUERY ------------------------------------------------

    // confirm selection
    if (plot.Selecting && (IO.MouseReleased[gp.InputMap.BoxSelectButton] || !IO.MouseDown[gp.InputMap.BoxSelectButton])) {
        UpdateTransformCache();
        ImVec2 select_size = plot.SelectStart - IO.MousePos;
        if (!ImHasFlag(plot.Flags, ImPlotFlags_NoBoxSelect)) {
            ImPlotPoint p1 = PixelsToPlot(plot.SelectStart);
            ImPlotPoint p2 = PixelsToPlot(IO.MousePos);
            const bool x_can_change = !ImHasFlag(IO.KeyMods,gp.InputMap.HorizontalMod) && ImFabs(select_size.x) > 2;
            const bool y_can_change = !ImHasFlag(IO.KeyMods,gp.InputMap.VerticalMod)   && ImFabs(select_size.y) > 2;
            if (!gp.X.LockMin && x_can_change)
                plot.XAxis.SetMin(ImMin(p1.x, p2.x));
            if (!gp.X.LockMax && x_can_change)
                plot.XAxis.SetMax(ImMax(p1.x, p2.x));
            for (int i = 0; i < IMPLOT_Y_AXES; i++) {
                p1 = PixelsToPlot(plot.SelectStart, i);
                p2 = PixelsToPlot(IO.MousePos, i);
                if (!gp.Y[i].LockMin && y_can_change)
                    plot.YAxis[i].SetMin(ImMin(p1.y, p2.y));
                if (!gp.Y[i].LockMax && y_can_change)
                    plot.YAxis[i].SetMax(ImMax(p1.y, p2.y));
            }
        }
        plot.Selecting = false;
    }
    // bad selection
    if (plot.Selecting && (ImHasFlag(plot.Flags, ImPlotFlags_NoBoxSelect) || gp.LockPlot) && ImLengthSqr(plot.SelectStart - IO.MousePos) > 4) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
    }
    // cancel selection
    if (plot.Selecting && (IO.MouseClicked[gp.InputMap.BoxSelectCancelButton] || IO.MouseDown[gp.InputMap.BoxSelectCancelButton])) {
        plot.Selecting = false;
    }
    // begin selection or query
    if (gp.Hov_Frame && gp.Hov_Plot && IO.MouseClicked[gp.InputMap.BoxSelectButton] && ImHasFlag(IO.KeyMods, gp.InputMap.BoxSelectMod)) {
        plot.SelectStart = IO.MousePos;
        plot.Selecting = true;
    }
    // update query
    if (plot.Querying) {
        UpdateTransformCache();
        plot.QueryRect.Min.x = ImHasFlag(IO.KeyMods, gp.InputMap.HorizontalMod) ? gp.BB_Plot.Min.x : ImMin(plot.QueryStart.x, IO.MousePos.x);
        plot.QueryRect.Max.x = ImHasFlag(IO.KeyMods, gp.InputMap.HorizontalMod) ? gp.BB_Plot.Max.x : ImMax(plot.QueryStart.x, IO.MousePos.x);
        plot.QueryRect.Min.y = ImHasFlag(IO.KeyMods, gp.InputMap.VerticalMod) ? gp.BB_Plot.Min.y : ImMin(plot.QueryStart.y, IO.MousePos.y);
        plot.QueryRect.Max.y = ImHasFlag(IO.KeyMods, gp.InputMap.VerticalMod) ? gp.BB_Plot.Max.y : ImMax(plot.QueryStart.y, IO.MousePos.y);

        plot.QueryRect.Min -= gp.BB_Plot.Min;
        plot.QueryRect.Max -= gp.BB_Plot.Min;
    }
    // end query
    if (plot.Querying && (IO.MouseReleased[gp.InputMap.QueryButton] || IO.MouseReleased[gp.InputMap.BoxSelectButton])) {
        plot.Querying = false;
        if (plot.QueryRect.GetWidth() > 2 && plot.QueryRect.GetHeight() > 2)
            plot.Queried = true;
        else
            plot.Queried = false;
    }

    // begin query
    if (ImHasFlag(plot.Flags, ImPlotFlags_Query) && gp.Hov_Frame && gp.Hov_Plot && IO.MouseClicked[gp.InputMap.QueryButton] && ImHasFlag(IO.KeyMods, gp.InputMap.QueryMod)) {
        plot.QueryRect = ImRect(0,0,0,0);
        plot.Querying = true;
        plot.Queried  = true;
        plot.QueryStart = IO.MousePos;
    }
    // toggle between select/query
    if (ImHasFlag(plot.Flags, ImPlotFlags_Query) && plot.Selecting && ImHasFlag(IO.KeyMods,gp.InputMap.QueryToggleMod)) {
        plot.Selecting = false;
        plot.QueryRect = ImRect(0,0,0,0);
        plot.Querying = true;
        plot.Queried  = true;
        plot.QueryStart = plot.SelectStart;
    }
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoBoxSelect) && plot.Querying && !ImHasFlag(IO.KeyMods, gp.InputMap.QueryToggleMod) && !IO.MouseDown[gp.InputMap.QueryButton]) {
        plot.Selecting = true;
        plot.Querying = false;
        plot.Queried = false;
        plot.QueryRect = ImRect(0,0,0,0);
    }
    if (!ImHasFlag(plot.Flags, ImPlotFlags_Query)) {
        plot.Queried  = false;
        plot.Querying = false;
        plot.QueryRect = ImRect(0,0,0,0);
    }

    // FIT -----------------------------------------------------------

    // fit from double click
    if ( IO.MouseDoubleClicked[gp.InputMap.FitButton] && gp.Hov_Frame && (plot.XAxis.HoveredTot || any_hov_y_axis_region) && !hov_legend && !hov_query ) {
        gp.FitThisFrame = true;
        gp.FitX = plot.XAxis.HoveredTot;
        for (int i = 0; i < IMPLOT_Y_AXES; i++)
            gp.FitY[i] = plot.YAxis[i].HoveredTot;
    }
    // fit from FitNextPlotAxes
    if (gp.NextPlotData.FitX) {
        gp.FitThisFrame = true;
        gp.FitX         = true;
    }
    for (int i = 0; i < IMPLOT_Y_AXES; ++i) {
        if (gp.NextPlotData.FitY[i]) {
            gp.FitThisFrame = true;
            gp.FitY[i]      = true;
        }
    }

    // FOCUS ------------------------------------------------------------------

    // focus window
    if ((IO.MouseClicked[0] || IO.MouseClicked[1] || IO.MouseClicked[2]) && gp.Hov_Frame)
        ImGui::FocusWindow(ImGui::GetCurrentWindow());

    UpdateTransformCache();

    // set mouse position
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        gp.MousePos[i] = PixelsToPlot(IO.MousePos, i);
    }

    // RENDER -----------------------------------------------------------------

    // grid bg
    DrawList.AddRectFilled(gp.BB_Plot.Min, gp.BB_Plot.Max, GetStyleColorU32(ImPlotCol_PlotBg));

    // render axes
    PushPlotClipRect();

    // transform ticks (TODO: Move this into ImPlotTickCollection)
    if (gp.RenderX) {
        for (int t = 0; t < gp.XTicks.Size; t++) {
            ImPlotTick *xt = &gp.XTicks.Ticks[t];
            xt->PixelPos = PlotToPixels(xt->PlotPos, 0, 0).x;
        }
    }
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (gp.RenderY[i]) {
            for (int t = 0; t < gp.YTicks[i].Size; t++) {
                ImPlotTick *yt = &gp.YTicks[i].Ticks[t];
                yt->PixelPos = PlotToPixels(0, yt->PlotPos, i).y;
            }
        }
    }

    // render grid
    if (!ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_NoGridLines)) {
        float density   = gp.XTicks.Size / gp.BB_Plot.GetWidth();
        ImVec4 col_min  = ImGui::ColorConvertU32ToFloat4(gp.Col_X.Minor);
        col_min.w      *= ImClamp(ImRemap(density, 0.1f, 0.2f, 1.0f, 0.0f), 0.0f, 1.0f);
        ImU32 col_min32 = ImGui::ColorConvertFloat4ToU32(col_min);
        for (int t = 0; t < gp.XTicks.Size; t++) {
            ImPlotTick& xt = gp.XTicks.Ticks[t];
            if (xt.Level == 0) {
                if (xt.Major)
                    DrawList.AddLine(ImVec2(xt.PixelPos, gp.BB_Plot.Min.y), ImVec2(xt.PixelPos, gp.BB_Plot.Max.y), gp.Col_X.Major, gp.Style.MajorGridSize.x);
                else if (density < 0.2f)
                    DrawList.AddLine(ImVec2(xt.PixelPos, gp.BB_Plot.Min.y), ImVec2(xt.PixelPos, gp.BB_Plot.Max.y), col_min32, gp.Style.MinorGridSize.x);
            }
        }
    }

    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (gp.Y[i].Present && !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_NoGridLines)) {
            float density   = gp.YTicks[i].Size / gp.BB_Plot.GetHeight();
            ImVec4 col_min  = ImGui::ColorConvertU32ToFloat4(gp.Col_Y[i].Minor);
            col_min.w      *= ImClamp(ImRemap(density, 0.1f, 0.2f, 1.0f, 0.0f), 0.0f, 1.0f);
            ImU32 col_min32 = ImGui::ColorConvertFloat4ToU32(col_min);
            for (int t = 0; t < gp.YTicks[i].Size; t++) {
                ImPlotTick& yt = gp.YTicks[i].Ticks[t];
                if (yt.Major)
                    DrawList.AddLine(ImVec2(gp.BB_Plot.Min.x, yt.PixelPos), ImVec2(gp.BB_Plot.Max.x, yt.PixelPos), gp.Col_Y[i].Major, gp.Style.MajorGridSize.y);
                else if (density < 0.2f)
                    DrawList.AddLine(ImVec2(gp.BB_Plot.Min.x, yt.PixelPos), ImVec2(gp.BB_Plot.Max.x, yt.PixelPos), col_min32, gp.Style.MinorGridSize.y);
            }
        }
    }

    PopPlotClipRect();

    // render title
    if (title_size.x > 0.0f) {
        ImU32 col = GetStyleColorU32(ImPlotCol_TitleText);
        const char* title_end = ImGui::FindRenderedTextEnd(title, NULL);
        DrawList.AddText(ImVec2(gp.BB_Canvas.GetCenter().x - title_size.x * 0.5f, gp.BB_Canvas.Min.y),col,title,title_end);
    }

    // render axis labels
    if (x_label) {
        const ImVec2 xLabel_size = ImGui::CalcTextSize(x_label);
        const ImVec2 xLabel_pos(gp.BB_Plot.GetCenter().x - xLabel_size.x * 0.5f, gp.BB_Canvas.Max.y - txt_height);
        DrawList.AddText(xLabel_pos, gp.Col_X.MajTxt, x_label);
    }
    if (y_label) {
        const ImVec2 yLabel_size = CalcTextSizeVertical(y_label);
        const ImVec2 yLabel_pos(gp.BB_Canvas.Min.x, gp.BB_Plot.GetCenter().y + yLabel_size.y * 0.5f);
        AddTextVertical(&DrawList, yLabel_pos, gp.Col_Y[0].MajTxt, y_label);
   }

    // render tick labels
    ImGui::PushClipRect(gp.BB_Frame.Min, gp.BB_Frame.Max, true);
    if (!ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_NoTickLabels)) {
        for (int t = 0; t < gp.XTicks.Size; t++) {
            ImPlotTick *xt = &gp.XTicks.Ticks[t];
            if (xt->ShowLabel && xt->PixelPos >= gp.BB_Plot.Min.x - 1 && xt->PixelPos <= gp.BB_Plot.Max.x + 1)
                DrawList.AddText(ImVec2(xt->PixelPos - xt->LabelSize.x * 0.5f, gp.BB_Plot.Max.y + gp.Style.LabelPadding.y + xt->Level * (txt_height + gp.Style.LabelPadding.y)),
                xt->Major ? gp.Col_X.MajTxt : gp.Col_X.MinTxt, gp.XTicks.GetLabel(t));
        }
    }
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (gp.Y[i].Present && !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_NoTickLabels)) {
            for (int t = 0; t < gp.YTicks[i].Size; t++) {
                const float x_start = gp.YAxisReference[i] + (i == 0 ?  (-gp.Style.LabelPadding.x - gp.YTicks[i].Ticks[t].LabelSize.x) : gp.Style.LabelPadding.x);
                ImPlotTick *yt = &gp.YTicks[i].Ticks[t];
                if (yt->ShowLabel && yt->PixelPos >= gp.BB_Plot.Min.y - 1 && yt->PixelPos <= gp.BB_Plot.Max.y + 1) {
                    ImVec2 start(x_start, yt->PixelPos - 0.5f * yt->LabelSize.y);
                    DrawList.AddText(start, yt->Major ? gp.Col_Y[i].MajTxt : gp.Col_Y[i].MinTxt, gp.YTicks[i].GetLabel(t));
                }
            }
        }
    }
    ImGui::PopClipRect();

    // push plot ID into stack
    ImGui::PushID(ID);
    return true;
}

//-----------------------------------------------------------------------------
// Context Menu
//-----------------------------------------------------------------------------

template <typename F>
bool DragFloat(const char* label, F* v, float v_speed, F v_min, F v_max) {
    return false;
}

template <>
bool DragFloat<double>(const char* label, double* v, float v_speed, double v_min, double v_max) {
    return ImGui::DragScalar(label, ImGuiDataType_Double, v, v_speed, &v_min, &v_max, "%.3f", 1);
}

template <>
bool DragFloat<float>(const char* label, float* v, float v_speed, float v_min, float v_max) {
    return ImGui::DragScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, "%.3f", 1);
}

inline void BeginDisabledControls(bool cond) {
    if (cond) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f);
    }
}

inline void EndDisabledControls(bool cond) {
    if (cond) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

inline void ShowAxisContextMenu(ImPlotAxisState& state, bool time_allowed) {
    ImGui::PushItemWidth(75);
    bool total_lock = state.HasRange && state.RangeCond == ImGuiCond_Always;
    bool logscale  = ImHasFlag(state.Axis->Flags, ImPlotAxisFlags_LogScale);
    bool timescale = ImHasFlag(state.Axis->Flags, ImPlotAxisFlags_Time);
    bool grid      = !ImHasFlag(state.Axis->Flags, ImPlotAxisFlags_NoGridLines);
    bool ticks     = !ImHasFlag(state.Axis->Flags, ImPlotAxisFlags_NoTickMarks);
    bool labels    = !ImHasFlag(state.Axis->Flags, ImPlotAxisFlags_NoTickLabels);
    double drag_speed = (state.Axis->Range.Size() <= DBL_EPSILON) ? DBL_EPSILON * 1.0e+13 : 0.01 * state.Axis->Range.Size(); // recover from almost equal axis limits.

    BeginDisabledControls(total_lock);
    if (ImGui::Checkbox("##LockMin", &state.LockMin))
        ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_LockMin);
    EndDisabledControls(total_lock);

    ImGui::SameLine();
    BeginDisabledControls(state.LockMin);
    double temp_min = state.Axis->Range.Min;
    if (DragFloat("Min", &temp_min, (float)drag_speed, -HUGE_VAL, state.Axis->Range.Max - DBL_EPSILON))
        state.Axis->SetMin(temp_min);
    EndDisabledControls(state.LockMin);

    BeginDisabledControls(total_lock);
    if (ImGui::Checkbox("##LockMax", &state.LockMax))
        ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_LockMax);
    EndDisabledControls(total_lock);

    ImGui::SameLine();
    BeginDisabledControls(state.LockMax);
    double temp_max = state.Axis->Range.Max;
    if (DragFloat("Max", &temp_max, (float)drag_speed, state.Axis->Range.Min + DBL_EPSILON, HUGE_VAL))
        state.Axis->SetMax(temp_max);
    EndDisabledControls(state.LockMax);

    ImGui::Separator();

    if (ImGui::Checkbox("Invert", &state.Invert))
        ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_Invert);
    BeginDisabledControls(timescale && time_allowed);
    if (ImGui::Checkbox("Log Scale", &logscale))
        ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_LogScale);
    EndDisabledControls(timescale && time_allowed);

    if (time_allowed) {
        BeginDisabledControls(logscale);
        if (ImGui::Checkbox("Time", &timescale))
            ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_Time);
        EndDisabledControls(logscale);
    }

    ImGui::Separator();
    if (ImGui::Checkbox("Grid Lines", &grid))
        ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_NoGridLines);
    if (ImGui::Checkbox("Tick Marks", &ticks))
        ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_NoTickMarks);
    if (ImGui::Checkbox("Labels", &labels))
        ImFlipFlag(state.Axis->Flags, ImPlotAxisFlags_NoTickLabels);

}

void ShowPlotContextMenu(ImPlotState& plot) {
    ImPlotContext& gp = *GImPlot;
    if (ImGui::BeginMenu("X-Axis")) {
        ImGui::PushID("X");
        ShowAxisContextMenu(gp.X, true);
        ImGui::PopID();
        ImGui::EndMenu();
    }
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (i == 1 && !ImHasFlag(plot.Flags, ImPlotFlags_YAxis2)) {
            continue;
        }
        if (i == 2 && !ImHasFlag(plot.Flags, ImPlotFlags_YAxis3)) {
            continue;
        }
        char buf[10] = {};
        if (i == 0) {
            snprintf(buf, sizeof(buf) - 1, "Y-Axis");
        } else {
            snprintf(buf, sizeof(buf) - 1, "Y-Axis %d", i + 1);
        }
        if (ImGui::BeginMenu(buf)) {
            ImGui::PushID(i);
            ShowAxisContextMenu(gp.Y[i], false);
            ImGui::PopID();
            ImGui::EndMenu();
        }
    }

    ImGui::Separator();
    if ((ImGui::BeginMenu("Settings"))) {
        if (ImGui::MenuItem("Box Select",NULL,!ImHasFlag(plot.Flags, ImPlotFlags_NoBoxSelect))) {
            ImFlipFlag(plot.Flags, ImPlotFlags_NoBoxSelect);
        }
        if (ImGui::MenuItem("Query",NULL,ImHasFlag(plot.Flags, ImPlotFlags_Query))) {
            ImFlipFlag(plot.Flags, ImPlotFlags_Query);
        }
        if (ImGui::MenuItem("Crosshairs",NULL,ImHasFlag(plot.Flags, ImPlotFlags_Crosshairs))) {
            ImFlipFlag(plot.Flags, ImPlotFlags_Crosshairs);
        }
        if (ImGui::MenuItem("Mouse Position",NULL,!ImHasFlag(plot.Flags, ImPlotFlags_NoMousePos))) {
            ImFlipFlag(plot.Flags, ImPlotFlags_NoMousePos);
        }
        if (ImGui::MenuItem("Anti-Aliased Lines",NULL,ImHasFlag(plot.Flags, ImPlotFlags_AntiAliased))) {
            ImFlipFlag(plot.Flags, ImPlotFlags_AntiAliased);
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Legend",NULL,!ImHasFlag(plot.Flags, ImPlotFlags_NoLegend))) {
        ImFlipFlag(plot.Flags, ImPlotFlags_NoLegend);
    }
#ifdef IMPLOT_DEBUG
    if (ImGui::BeginMenu("Debug")) {
        ImGui::PushItemWidth(50);
        ImGui::LabelText("Plots", "%d", gp.Plots.GetSize());
        ImGui::LabelText("Color Mods", "%d", gp.ColorModifiers.size());
        ImGui::LabelText("Style Mods", "%d", gp.StyleModifiers.size());
        ImGui::TextUnformatted(gp.XTicks.Labels.Buf.Data, gp.XTicks.Labels.Buf.Data + gp.XTicks.Labels.size());
        ImGui::TextUnformatted(gp.YTicks[0].Labels.Buf.Data, gp.YTicks[0].Labels.Buf.Data + gp.YTicks[0].Labels.size());
        // ImGui::TextUnformatted(gp.YTicks[1].Labels.Buf.Data, gp.YTicks[1].Labels.Buf.Data + gp.YTicks[1].Labels.size());
        // ImGui::TextUnformatted(gp.YTicks[2].Labels.Buf.Data, gp.YTicks[2].Labels.Buf.Data + gp.YTicks[2].Labels.size());

        ImGui::PopItemWidth();
        ImGui::EndMenu();
    }
#endif

}

//-----------------------------------------------------------------------------
// EndPlot()
//-----------------------------------------------------------------------------

void EndPlot() {
    IM_ASSERT_USER_ERROR(GImPlot != NULL, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp     = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "Mismatched BeginPlot()/EndPlot()!");
    ImGuiContext &G       = *GImGui;
    ImPlotState &plot     = *gp.CurrentPlot;
    ImGuiWindow * Window  = G.CurrentWindow;
    ImDrawList & DrawList = *Window->DrawList;
    const ImGuiIO &   IO  = ImGui::GetIO();

    // AXIS STATES ------------------------------------------------------------

    const bool any_y_locked   = gp.Y[0].Lock || gp.Y[1].Present ? gp.Y[1].Lock : false || gp.Y[2].Present ? gp.Y[2].Lock : false;
    const bool any_y_dragging = plot.YAxis[0].Dragging || plot.YAxis[1].Dragging || plot.YAxis[2].Dragging;


    // FINAL RENDER -----------------------------------------------------------

    // render ticks
    PushPlotClipRect();
    if (!ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_NoTickMarks)) {
        for (int t = 0; t < gp.XTicks.Size; t++) {
            ImPlotTick *xt = &gp.XTicks.Ticks[t];
            if (xt->Level == 0)
                DrawList.AddLine(ImVec2(xt->PixelPos, gp.BB_Plot.Max.y),
                                ImVec2(xt->PixelPos, gp.BB_Plot.Max.y - (xt->Major ? gp.Style.MajorTickLen.x : gp.Style.MinorTickLen.x)),
                                gp.Col_X.Major,
                                xt->Major ? gp.Style.MajorTickSize.x : gp.Style.MinorTickSize.x);
        }
    }
    PopPlotClipRect();

    ImGui::PushClipRect(gp.BB_Plot.Min, ImVec2(gp.BB_Frame.Max.x, gp.BB_Plot.Max.y), true);
    int axis_count = 0;
    for (int i = 0; i < IMPLOT_Y_AXES; i++) {
        if (!gp.Y[i].Present) { continue; }
        axis_count++;

        float x_start = gp.YAxisReference[i];
        if (!ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_NoTickMarks)) {
            float direction = (i == 0) ? 1.0f : -1.0f;
            bool no_major = axis_count >= 3;
            for (int t = 0; t < gp.YTicks[i].Size; t++) {
                ImPlotTick *yt = &gp.YTicks[i].Ticks[t];
                ImVec2 start = ImVec2(x_start, yt->PixelPos);
                DrawList.AddLine(start,
                                 start + ImVec2(direction * ((!no_major && yt->Major) ? gp.Style.MajorTickLen.y : gp.Style.MinorTickLen.y), 0),
                                 gp.Col_Y[i].Major,
                                 (!no_major && yt->Major) ? gp.Style.MajorTickSize.y : gp.Style.MinorTickSize.y);
            }
        }

        if (axis_count >= 3) {
            // Draw a bar next to the ticks to act as a visual separator.
            DrawList.AddLine(
                    ImVec2(x_start, gp.BB_Plot.Min.y),
                    ImVec2(x_start, gp.BB_Plot.Max.y),
                    GetStyleColorU32(ImPlotCol_YAxisGrid3), 1);
        }
    }
    ImGui::PopClipRect();

    // render y-axis drag/drop hover
    if ((gp.Y[1].Present || gp.Y[2].Present) && ImGui::IsDragDropPayloadBeingAccepted()) {
        for (int i = 0; i < IMPLOT_Y_AXES; ++i) {
            if (plot.YAxis[i].HoveredExt) {
                float x_loc = gp.YAxisReference[i];
                ImVec2 p1(x_loc - 5, gp.BB_Plot.Min.y - 5);
                ImVec2 p2(x_loc + 5, gp.BB_Plot.Max.y + 5);
                DrawList.AddRect(p1, p2, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f,  ImDrawCornerFlags_All, 2.0f);
            }
        }
    }

    PushPlotClipRect();
    // render selection/query
    if (plot.Selecting) {
        const ImRect select_bb(ImMin(IO.MousePos, plot.SelectStart), ImMax(IO.MousePos, plot.SelectStart));
        const bool wide_enough = ImFabs(select_bb.GetWidth())  > 2;
        const bool tall_enough = ImFabs(select_bb.GetHeight()) > 2;
        const bool big_enough  = wide_enough && tall_enough;
        if (plot.Selecting && !gp.LockPlot && !ImHasFlag(plot.Flags, ImPlotFlags_NoBoxSelect)) {
            const ImVec4 col   = GetStyleColorVec4(ImPlotCol_Selection);
            const ImU32 col_bg = ImGui::GetColorU32(col * ImVec4(1,1,1,0.25f));
            const ImU32 col_bd = ImGui::GetColorU32(col);
            if (IO.KeyMods == (gp.InputMap.HorizontalMod | gp.InputMap.VerticalMod) && big_enough) {
                DrawList.AddRectFilled(gp.BB_Plot.Min, gp.BB_Plot.Max, col_bg);
                DrawList.AddRect(      gp.BB_Plot.Min, gp.BB_Plot.Max, col_bd);
            }
            else if ((gp.X.Lock || IO.KeyMods == gp.InputMap.HorizontalMod) && tall_enough) {
                DrawList.AddRectFilled(ImVec2(gp.BB_Plot.Min.x, select_bb.Min.y), ImVec2(gp.BB_Plot.Max.x, select_bb.Max.y), col_bg);
                DrawList.AddRect(      ImVec2(gp.BB_Plot.Min.x, select_bb.Min.y), ImVec2(gp.BB_Plot.Max.x, select_bb.Max.y), col_bd);
            }
            else if ((any_y_locked || IO.KeyMods == gp.InputMap.VerticalMod) && wide_enough) {
                DrawList.AddRectFilled(ImVec2(select_bb.Min.x, gp.BB_Plot.Min.y), ImVec2(select_bb.Max.x, gp.BB_Plot.Max.y), col_bg);
                DrawList.AddRect(      ImVec2(select_bb.Min.x, gp.BB_Plot.Min.y), ImVec2(select_bb.Max.x, gp.BB_Plot.Max.y), col_bd);
            }
            else if (big_enough) {
                DrawList.AddRectFilled(select_bb.Min, select_bb.Max, col_bg);
                DrawList.AddRect(      select_bb.Min, select_bb.Max, col_bd);
            }
        }
    }

    if (ImHasFlag(plot.Flags, ImPlotFlags_Query)) // draw query rect only when query enabled.
    {
        const ImVec4 col   = GetStyleColorVec4(ImPlotCol_Query);
        const ImU32 col_bd =  ImGui::GetColorU32(col * ImVec4(1,1,1,0.25f));
        const ImU32 col_bg =  ImGui::GetColorU32(col);
        if (plot.Querying || plot.Queried) {
            if (plot.QueryRect.GetWidth() > 2 && plot.QueryRect.GetHeight() > 2) {
                DrawList.AddRectFilled(plot.QueryRect.Min + gp.BB_Plot.Min, plot.QueryRect.Max + gp.BB_Plot.Min, col_bd);
                DrawList.AddRect(      plot.QueryRect.Min + gp.BB_Plot.Min, plot.QueryRect.Max + gp.BB_Plot.Min, col_bg);
            }
        }
        else if (plot.Queried) {
            ImRect bb_query = plot.QueryRect;
            bb_query.Min += gp.BB_Plot.Min;
            bb_query.Max += gp.BB_Plot.Min;
            DrawList.AddRectFilled(bb_query.Min, bb_query.Max, col_bd);
            DrawList.AddRect(      bb_query.Min, bb_query.Max, col_bg);
        }
    }

    // render legend
    const float txt_ht = ImGui::GetTextLineHeight();
    const ImVec2 legend_offset = gp.Style.LegendPadding;
    const ImVec2 legend_spacing(5, 5);
    const float  legend_icon_size = txt_ht;
    ImRect legend_content_bb;
    int nItems = GetLegendCount();
    bool hov_legend = false;
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoLegend) && nItems > 0) {
        // get max width
        float max_label_width = 0;
        for (int i = 0; i < nItems; ++i) {
            const char* label = GetLegendLabel(i);
            ImVec2 labelWidth = ImGui::CalcTextSize(label, NULL, true);
            max_label_width   = labelWidth.x > max_label_width ? labelWidth.x : max_label_width;
        }
        legend_content_bb = ImRect(gp.BB_Plot.Min + legend_offset, gp.BB_Plot.Min + legend_offset + ImVec2(max_label_width, nItems * txt_ht));
        plot.BB_Legend    = ImRect(legend_content_bb.Min, legend_content_bb.Max + legend_spacing * 2 + ImVec2(legend_icon_size, 0));
        hov_legend = !ImHasFlag(plot.Flags, ImPlotFlags_NoLegend) ? gp.Hov_Frame && plot.BB_Legend.Contains(IO.MousePos) : false;
        // render legend box
        ImU32  col_bg = GetStyleColorU32(ImPlotCol_LegendBg);
        ImU32  col_bd = GetStyleColorU32(ImPlotCol_LegendBorder);
        ImVec4 col_txt = GetStyleColorVec4(ImPlotCol_LegendText);
        ImU32  col_txt_dis = ImGui::GetColorU32(col_txt * ImVec4(1,1,1,0.25f));
        DrawList.AddRectFilled(plot.BB_Legend.Min, plot.BB_Legend.Max, col_bg);
        DrawList.AddRect(plot.BB_Legend.Min, plot.BB_Legend.Max, col_bd);
        // render each legend item
        for (int i = 0; i < nItems; ++i) {
            ImPlotItem* item = GetItem(i);
            ImRect icon_bb;
            icon_bb.Min = legend_content_bb.Min + legend_spacing + ImVec2(0, i * txt_ht) + ImVec2(2, 2);
            icon_bb.Max = legend_content_bb.Min + legend_spacing + ImVec2(0, i * txt_ht) + ImVec2(legend_icon_size - 2, legend_icon_size - 2);
            ImRect label_bb;
            label_bb.Min = legend_content_bb.Min + legend_spacing + ImVec2(0, i * txt_ht) + ImVec2(2, 2);
            label_bb.Max = legend_content_bb.Min + legend_spacing + ImVec2(0, i * txt_ht) + ImVec2(legend_content_bb.Max.x, legend_icon_size - 2);
            ImU32 col_hl_txt;
            if (hov_legend && (icon_bb.Contains(IO.MousePos) || label_bb.Contains(IO.MousePos))) {
                item->LegendHovered = true;
                col_hl_txt = ImGui::GetColorU32(ImLerp(col_txt, item->Color, 0.25f));
            }
            else
            {
               item->LegendHovered = false;
               col_hl_txt = ImGui::GetColorU32(col_txt);
            }
            ImU32 iconColor;
            ImVec4 item_color = item->Color;
            item_color.w = 1;
            if (hov_legend && icon_bb.Contains(IO.MousePos)) {
                ImVec4 colAlpha = item_color;
                colAlpha.w    = 0.5f;
                iconColor     = item->Show ? ImGui::GetColorU32(colAlpha)
                                          : ImGui::GetColorU32(ImGuiCol_TextDisabled, 0.5f);
                if (IO.MouseClicked[0])
                    item->Show = !item->Show;
            } else {
                iconColor = item->Show ? ImGui::GetColorU32(item_color) : col_txt_dis;
            }
            DrawList.AddRectFilled(icon_bb.Min, icon_bb.Max, iconColor, 1);
            const char* label = GetLegendLabel(i);
            const char* text_display_end = ImGui::FindRenderedTextEnd(label, NULL);
            if (label != text_display_end)
                DrawList.AddText(legend_content_bb.Min + legend_spacing + ImVec2(legend_icon_size, i * txt_ht), item->Show ? col_hl_txt  : col_txt_dis, label, text_display_end);
        }
    }

    // render crosshairs
    if (ImHasFlag(plot.Flags, ImPlotFlags_Crosshairs) && gp.Hov_Plot && gp.Hov_Frame &&
        !(plot.XAxis.Dragging || any_y_dragging) && !plot.Selecting && !plot.Querying && !hov_legend) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        ImVec2 xy = IO.MousePos;
        ImVec2 h1(gp.BB_Plot.Min.x, xy.y);
        ImVec2 h2(xy.x - 5, xy.y);
        ImVec2 h3(xy.x + 5, xy.y);
        ImVec2 h4(gp.BB_Plot.Max.x, xy.y);
        ImVec2 v1(xy.x, gp.BB_Plot.Min.y);
        ImVec2 v2(xy.x, xy.y - 5);
        ImVec2 v3(xy.x, xy.y + 5);
        ImVec2 v4(xy.x, gp.BB_Plot.Max.y);
        ImU32 col = GetStyleColorU32(ImPlotCol_Crosshairs);
        DrawList.AddLine(h1, h2, col);
        DrawList.AddLine(h3, h4, col);
        DrawList.AddLine(v1, v2, col);
        DrawList.AddLine(v3, v4, col);
    }

    // render mouse pos
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoMousePos) && gp.Hov_Plot) {
        char buffer[128] = {};
        ImBufferWriter writer(buffer, sizeof(buffer));

        // x
        if (ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LogScale)) {
            writer.Write("%.3E", gp.MousePos[0].x);
        }
        else if (ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_Time)) {
            ImPlotTimeUnit unit = GetUnitForRange(plot.XAxis.Range.Size() / (gp.BB_Plot.GetWidth() / 100));
            const int written = FormatTime(ImPlotTime::FromDouble(gp.MousePos[0].x), &writer.Buffer[writer.Pos], writer.Size - writer.Pos - 1, TimeFormatMouseCursor[unit]);
            if (written > 0)
                writer.Pos += ImMin(written, writer.Size - writer.Pos - 1);
        }
        else {
            double range_x = gp.XTicks.Size > 1 ? (gp.XTicks.Ticks[1].PlotPos - gp.XTicks.Ticks[0].PlotPos) : plot.XAxis.Range.Size();
            writer.Write("%.*f", Precision(range_x), gp.MousePos[0].x);
        }
        // y1
        if (ImHasFlag(plot.YAxis[0].Flags, ImPlotAxisFlags_LogScale)) {
            writer.Write(",%.3E", gp.MousePos[0].y);
        }
        else {
            double range_y = gp.YTicks[0].Size > 1 ? (gp.YTicks[0].Ticks[1].PlotPos - gp.YTicks[0].Ticks[0].PlotPos) : plot.YAxis[0].Range.Size();
            writer.Write(",%.*f", Precision(range_y), gp.MousePos[0].y);
        }
        // y2
        if (ImHasFlag(plot.Flags, ImPlotFlags_YAxis2)) {
            if (ImHasFlag(plot.YAxis[1].Flags, ImPlotAxisFlags_LogScale)) {
                writer.Write(",(%.3E)", gp.MousePos[1].y);
            }
            else {
                double range_y = gp.YTicks[1].Size > 1 ? (gp.YTicks[1].Ticks[1].PlotPos - gp.YTicks[1].Ticks[0].PlotPos) : plot.YAxis[1].Range.Size();
                writer.Write(",(%.*f)", Precision(range_y), gp.MousePos[1].y);
            }
        }
        // y3
        if (ImHasFlag(plot.Flags, ImPlotFlags_YAxis3)) {
            if (ImHasFlag(plot.YAxis[2].Flags, ImPlotAxisFlags_LogScale)) {
                writer.Write(",(%.3E)", gp.MousePos[2].y);
            }
            else {
                double range_y = gp.YTicks[2].Size > 1 ? (gp.YTicks[2].Ticks[1].PlotPos - gp.YTicks[2].Ticks[0].PlotPos) : plot.YAxis[2].Range.Size();
                writer.Write(",(%.*f)", Precision(range_y), gp.MousePos[2].y);
            }
        }
        ImVec2 size = ImGui::CalcTextSize(buffer);
        ImVec2 pos  = gp.BB_Plot.Max - size - gp.Style.InfoPadding;
        DrawList.AddText(pos, GetStyleColorU32(ImPlotCol_InlayText), buffer);
    }

    PopPlotClipRect();

    // render border
    if (gp.Style.PlotBorderSize > 0)
        DrawList.AddRect(gp.BB_Plot.Min, gp.BB_Plot.Max, GetStyleColorU32(ImPlotCol_PlotBorder), 0, ImDrawCornerFlags_All, gp.Style.PlotBorderSize);

    // FIT DATA --------------------------------------------------------------

    if (gp.FitThisFrame && (gp.VisibleItemCount > 0 || plot.Queried)) {
        if (gp.FitX && !ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMin) && !ImNanOrInf(gp.ExtentsX.Min)) {
            plot.XAxis.Range.Min = (gp.ExtentsX.Min);
        }
        if (gp.FitX && !ImHasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMax) && !ImNanOrInf(gp.ExtentsX.Max)) {
            plot.XAxis.Range.Max = (gp.ExtentsX.Max);
        }
        if ((plot.XAxis.Range.Max - plot.XAxis.Range.Min) <= (2.0 * FLT_EPSILON))  {
            plot.XAxis.Range.Max += FLT_EPSILON;
            plot.XAxis.Range.Min -= FLT_EPSILON;
        }
        plot.XAxis.Constrain();
        for (int i = 0; i < IMPLOT_Y_AXES; i++) {
            if (gp.FitY[i] && !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_LockMin) && !ImNanOrInf(gp.ExtentsY[i].Min)) {
                plot.YAxis[i].Range.Min = (gp.ExtentsY[i].Min);
            }
            if (gp.FitY[i] && !ImHasFlag(plot.YAxis[i].Flags, ImPlotAxisFlags_LockMax) && !ImNanOrInf(gp.ExtentsY[i].Max)) {
                plot.YAxis[i].Range.Max = (gp.ExtentsY[i].Max);
            }
            if ((plot.YAxis[i].Range.Max - plot.YAxis[i].Range.Min) <= (2.0 * FLT_EPSILON))  {
                plot.YAxis[i].Range.Max += FLT_EPSILON;
                plot.YAxis[i].Range.Min -= FLT_EPSILON;
            }
            plot.YAxis[i].Constrain();
        }
    }

    // CONTEXT MENUS -----------------------------------------------------------

    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoMenus) && gp.Hov_Frame && gp.Hov_Plot && IO.MouseDoubleClicked[gp.InputMap.ContextMenuButton] && !hov_legend)
        ImGui::OpenPopup("##PlotContext");
    if (ImGui::BeginPopup("##PlotContext")) {
        ShowPlotContextMenu(plot);
        ImGui::EndPopup();
    }

    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoMenus) && gp.Hov_Frame && plot.XAxis.HoveredExt && IO.MouseDoubleClicked[gp.InputMap.ContextMenuButton] && !hov_legend)
        ImGui::OpenPopup("##XContext");
    if (ImGui::BeginPopup("##XContext")) {
        ImGui::Text("X-Axis"); ImGui::Separator();
        ShowAxisContextMenu(gp.X, true);
        ImGui::EndPopup();
    }

    for (int i = 0; i < IMPLOT_Y_AXES; ++i) {
        ImGui::PushID(i);
        if (!ImHasFlag(plot.Flags, ImPlotFlags_NoMenus) && gp.Hov_Frame && plot.YAxis[i].HoveredExt && IO.MouseDoubleClicked[gp.InputMap.ContextMenuButton] && !hov_legend)
            ImGui::OpenPopup("##YContext");
        if (ImGui::BeginPopup("##YContext")) {
            if (i == 0) {
                ImGui::Text("Y-Axis"); ImGui::Separator();
            }
            else {
                ImGui::Text("Y-Axis %d", i + 1); ImGui::Separator();
            }
            ShowAxisContextMenu(gp.Y[i], false);
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }


    // LINKED AXES ------------------------------------------------------------

    PushLinkedAxis(plot.XAxis);
    for (int i = 0; i < IMPLOT_Y_AXES; ++i)
        PushLinkedAxis(plot.YAxis[i]);

    // CLEANUP ----------------------------------------------------------------

    // reset the plot items for the next frame
    for (int i = 0; i < gp.CurrentPlot->Items.GetSize(); ++i) {
        gp.CurrentPlot->Items.GetByIndex(i)->SeenThisFrame = false;
    }

    // Pop ImGui::PushID at the end of BeginPlot
    ImGui::PopID();
    // Reset context for next plot
    Reset(GImPlot);
}

//-----------------------------------------------------------------------------
// MISC API
//-----------------------------------------------------------------------------

ImPlotInputMap& GetInputMap() {
    return GImPlot->InputMap;
}

void SetNextPlotLimits(double x_min, double x_max, double y_min, double y_max, ImGuiCond cond) {
    IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot == NULL, "SetNextPlotLimits() needs to be called before BeginPlot()!");
    SetNextPlotLimitsX(x_min, x_max, cond);
    SetNextPlotLimitsY(y_min, y_max, cond);
}

void SetNextPlotLimitsX(double x_min, double x_max, ImGuiCond cond) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == NULL, "SetNextPlotLSetNextPlotLimitsXimitsY() needs to be called before BeginPlot()!");
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    gp.NextPlotData.HasXRange = true;
    gp.NextPlotData.XRangeCond = cond;
    gp.NextPlotData.X.Min = x_min;
    gp.NextPlotData.X.Max = x_max;
}

void SetNextPlotLimitsY(double y_min, double y_max, ImGuiCond cond, int y_axis) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == NULL, "SetNextPlotLimitsY() needs to be called before BeginPlot()!");
    IM_ASSERT_USER_ERROR(y_axis >= 0 && y_axis < IMPLOT_Y_AXES, "y_axis needs to be between 0 and IMPLOT_Y_AXES");
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    gp.NextPlotData.HasYRange[y_axis] = true;
    gp.NextPlotData.YRangeCond[y_axis] = cond;
    gp.NextPlotData.Y[y_axis].Min = y_min;
    gp.NextPlotData.Y[y_axis].Max = y_max;
}

void LinkNextPlotLimits(double* xmin, double* xmax, double* ymin, double* ymax, double* ymin2, double* ymax2, double* ymin3, double* ymax3) {
    ImPlotContext& gp = *GImPlot;
    gp.NextPlotData.LinkedXmin    = xmin;
    gp.NextPlotData.LinkedXmax    = xmax;
    gp.NextPlotData.LinkedYmin[0] = ymin;
    gp.NextPlotData.LinkedYmax[0] = ymax;
    gp.NextPlotData.LinkedYmin[1] = ymin2;
    gp.NextPlotData.LinkedYmax[1] = ymax2;
    gp.NextPlotData.LinkedYmin[2] = ymin3;
    gp.NextPlotData.LinkedYmax[2] = ymax3;
}

void FitNextPlotAxes(bool x, bool y, bool y2, bool y3) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == NULL, "FitNextPlotAxes() needs to be called before BeginPlot()!");
    gp.NextPlotData.FitX = x;
    gp.NextPlotData.FitY[0] = y;
    gp.NextPlotData.FitY[1] = y2;
    gp.NextPlotData.FitY[2] = y3;
}

void SetNextPlotTicksX(const double* values, int n_ticks, const char** labels, bool show_default) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == NULL, "SetNextPlotTicksX() needs to be called before BeginPlot()!");
    gp.NextPlotData.ShowDefaultTicksX = show_default;
    AddTicksCustom(values, labels, n_ticks, gp.XTicks);
}

void SetNextPlotTicksX(double x_min, double x_max, int n_ticks, const char** labels, bool show_default) {
    IM_ASSERT_USER_ERROR(n_ticks > 1, "The number of ticks must be greater than 1");
    static ImVector<double> buffer;
    FillRange(buffer, n_ticks, x_min, x_max);
    SetNextPlotTicksX(&buffer[0], n_ticks, labels, show_default);
}

void SetNextPlotTicksY(const double* values, int n_ticks, const char** labels, bool show_default, int y_axis) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == NULL, "SetNextPlotTicksY() needs to be called before BeginPlot()!");
    IM_ASSERT_USER_ERROR(y_axis >= 0 && y_axis < IMPLOT_Y_AXES, "y_axis needs to be between 0 and IMPLOT_Y_AXES");
    gp.NextPlotData.ShowDefaultTicksY[y_axis] = show_default;
    AddTicksCustom(values, labels, n_ticks, gp.YTicks[y_axis]);
}

void SetNextPlotTicksY(double y_min, double y_max, int n_ticks, const char** labels, bool show_default, int y_axis) {
    IM_ASSERT_USER_ERROR(n_ticks > 1, "The number of ticks must be greater than 1");
    static ImVector<double> buffer;
    FillRange(buffer, n_ticks, y_min, y_max);
    SetNextPlotTicksY(&buffer[0], n_ticks, labels, show_default,y_axis);
}

void SetPlotYAxis(int y_axis) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "SetPlotYAxis() needs to be called between BeginPlot() and EndPlot()!");
    IM_ASSERT_USER_ERROR(y_axis >= 0 && y_axis < IMPLOT_Y_AXES, "y_axis needs to be between 0 and IMPLOT_Y_AXES");
    gp.CurrentPlot->CurrentYAxis = y_axis;
}

ImVec2 GetPlotPos() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "GetPlotPos() needs to be called between BeginPlot() and EndPlot()!");
    return gp.BB_Plot.Min;
}

ImVec2 GetPlotSize() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "GetPlotSize() needs to be called between BeginPlot() and EndPlot()!");
    return gp.BB_Plot.GetSize();
}

ImDrawList* GetPlotDrawList() {
    return ImGui::GetWindowDrawList();
}

void PushPlotClipRect() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "PushPlotClipRect() needs to be called between BeginPlot() and EndPlot()!");
    ImGui::PushClipRect(gp.BB_Plot.Min, gp.BB_Plot.Max, true);
}

void PopPlotClipRect() {
    ImGui::PopClipRect();
}

bool IsPlotHovered() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "IsPlotHovered() needs to be called between BeginPlot() and EndPlot()!");
    return gp.Hov_Frame && gp.Hov_Plot;
}

bool IsPlotXAxisHovered() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "IsPlotXAxisHovered() needs to be called between BeginPlot() and EndPlot()!");
    return gp.CurrentPlot->XAxis.HoveredExt;
}

bool IsPlotYAxisHovered(int y_axis_in) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(y_axis_in >= -1 && y_axis_in < IMPLOT_Y_AXES, "y_axis needs to between -1 and IMPLOT_Y_AXES");
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "IsPlotYAxisHovered() needs to be called between BeginPlot() and EndPlot()!");
    const int y_axis = y_axis_in >= 0 ? y_axis_in : gp.CurrentPlot->CurrentYAxis;
    return gp.CurrentPlot->YAxis[y_axis].HoveredExt;
}

ImPlotPoint GetPlotMousePos(int y_axis_in) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(y_axis_in >= -1 && y_axis_in < IMPLOT_Y_AXES, "y_axis needs to between -1 and IMPLOT_Y_AXES");
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "GetPlotMousePos() needs to be called between BeginPlot() and EndPlot()!");
    const int y_axis = y_axis_in >= 0 ? y_axis_in : gp.CurrentPlot->CurrentYAxis;
    return gp.MousePos[y_axis];
}


ImPlotLimits GetPlotLimits(int y_axis_in) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(y_axis_in >= -1 && y_axis_in < IMPLOT_Y_AXES, "y_axis needs to between -1 and IMPLOT_Y_AXES");
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "GetPlotLimits() needs to be called between BeginPlot() and EndPlot()!");
    const int y_axis = y_axis_in >= 0 ? y_axis_in : gp.CurrentPlot->CurrentYAxis;

    ImPlotState& plot = *gp.CurrentPlot;
    ImPlotLimits limits;
    limits.X = plot.XAxis.Range;
    limits.Y = plot.YAxis[y_axis].Range;
    return limits;
}

bool IsPlotQueried() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "IsPlotQueried() needs to be called between BeginPlot() and EndPlot()!");
    return gp.CurrentPlot->Queried;
}

ImPlotLimits GetPlotQuery(int y_axis_in) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(y_axis_in >= -1 && y_axis_in < IMPLOT_Y_AXES, "y_axis needs to between -1 and IMPLOT_Y_AXES");
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "GetPlotQuery() needs to be called between BeginPlot() and EndPlot()!");
    ImPlotState& plot = *gp.CurrentPlot;
    const int y_axis = y_axis_in >= 0 ? y_axis_in : gp.CurrentPlot->CurrentYAxis;

    UpdateTransformCache();
    ImPlotPoint p1 = PixelsToPlot(plot.QueryRect.Min + gp.BB_Plot.Min, y_axis);
    ImPlotPoint p2 = PixelsToPlot(plot.QueryRect.Max + gp.BB_Plot.Min, y_axis);

    ImPlotLimits result;
    result.X.Min = ImMin(p1.x, p2.x);
    result.X.Max = ImMax(p1.x, p2.x);
    result.Y.Min = ImMin(p1.y, p2.y);
    result.Y.Max = ImMax(p1.y, p2.y);
    return result;
}

bool IsLegendEntryHovered(const char* label_id) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "IsPlotItemHighlight() needs to be called between BeginPlot() and EndPlot()!");
    ImGuiID id = ImGui::GetID(label_id);
    ImPlotItem* item = gp.CurrentPlot->Items.GetByKey(id);
    return item && item->LegendHovered;
}

bool BeginLegendDragDropSource(const char* label_id, ImGuiDragDropFlags flags) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "BeginLegendDragDropSource() needs to be called between BeginPlot() and EndPlot()!");
    ImGuiID source_id = ImGui::GetID(label_id);
    ImPlotItem* item = gp.CurrentPlot->Items.GetByKey(source_id);
    bool is_hovered = item && item->LegendHovered;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    ImGuiMouseButton mouse_button = ImGuiMouseButton_Left;

    if (g.IO.MouseDown[mouse_button] == false) {
        if (g.ActiveId == source_id)
            ImGui::ClearActiveID();
        return false;
    }

    if (is_hovered && g.IO.MouseClicked[mouse_button]) {
        ImGui::SetActiveID(source_id, window);
        ImGui::FocusWindow(window);
    }

    if (g.ActiveId != source_id)
        return false;

    // Allow the underlying widget to display/return hovered during the mouse
    // release frame, else we would get a flicker.
    g.ActiveIdAllowOverlap = is_hovered;

    // Disable navigation and key inputs while dragging
    g.ActiveIdUsingNavDirMask = ~(ImU32)0;
    g.ActiveIdUsingNavInputMask = ~(ImU32)0;
    g.ActiveIdUsingKeyInputMask = ~(ImU64)0;

    if (ImGui::IsMouseDragging(mouse_button)) {
        if (!g.DragDropActive) {
            ImGui::ClearDragDrop();
            ImGuiPayload& payload = g.DragDropPayload;
            payload.SourceId = source_id;
            payload.SourceParentId = 0;
            g.DragDropActive = true;
            g.DragDropSourceFlags = 0;
            g.DragDropMouseButton = mouse_button;
        }
        g.DragDropSourceFrameCount = g.FrameCount;
        g.DragDropWithinSource = true;

        if (!(flags & ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
            // Target can request the Source to not display its tooltip (we use a
            // dedicated flag to make this request explicit) We unfortunately can't
            // just modify the source flags and skip the call to BeginTooltip, as
            // caller may be emitting contents.
            ImGui::BeginTooltip();
            if (g.DragDropAcceptIdPrev && (g.DragDropAcceptFlags & ImGuiDragDropFlags_AcceptNoPreviewTooltip)) {
                ImGuiWindow* tooltip_window = g.CurrentWindow;
                tooltip_window->SkipItems = true;
                tooltip_window->HiddenFramesCanSkipItems = 1;
            }
        }
        return true;
    }
    return false;
}

void EndLegendDragDropSource() {
    ImGui::EndDragDropSource();
}

bool BeginLegendPopup(const char* label_id, ImGuiMouseButton mouse_button) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "BeginLegendPopup() needs to be called between BeginPlot() and EndPlot()!");
    ImGuiWindow* window = GImGui->CurrentWindow;
    if (window->SkipItems)
        return false;
    ImGuiID id = ImGui::GetID(label_id);
    if (ImGui::IsMouseReleased(mouse_button)) {
        ImPlotItem* item = gp.CurrentPlot->Items.GetByKey(id);
        if (item && item->LegendHovered)
            ImGui::OpenPopupEx(id);
    }
    return ImGui::BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
}

void EndLegendPopup() {
    ImGui::EndPopup();
}

//-----------------------------------------------------------------------------
// STYLING
//-----------------------------------------------------------------------------

ImPlotStyle& GetStyle() {
    ImPlotContext& gp = *GImPlot;
    return gp.Style;
}

void PushStyleColor(ImPlotCol idx, ImU32 col) {
    ImPlotContext& gp = *GImPlot;
    ImGuiColorMod backup;
    backup.Col = idx;
    backup.BackupValue = gp.Style.Colors[idx];
    gp.ColorModifiers.push_back(backup);
    gp.Style.Colors[idx] = ImGui::ColorConvertU32ToFloat4(col);
}

void PushStyleColor(ImPlotCol idx, const ImVec4& col) {
    ImPlotContext& gp = *GImPlot;
    ImGuiColorMod backup;
    backup.Col = idx;
    backup.BackupValue = gp.Style.Colors[idx];
    gp.ColorModifiers.push_back(backup);
    gp.Style.Colors[idx] = col;
}

void PopStyleColor(int count) {
    ImPlotContext& gp = *GImPlot;
    while (count > 0)
    {
        ImGuiColorMod& backup = gp.ColorModifiers.back();
        gp.Style.Colors[backup.Col] = backup.BackupValue;
        gp.ColorModifiers.pop_back();
        count--;
    }
}

void PushStyleVar(ImPlotStyleVar idx, float val) {
    ImPlotContext& gp = *GImPlot;
    const ImPlotStyleVarInfo* var_info = GetPlotStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 1) {
        float* pvar = (float*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() float variant but variable is not a float!");
}

void PushStyleVar(ImPlotStyleVar idx, int val) {
    ImPlotContext& gp = *GImPlot;
    const ImPlotStyleVarInfo* var_info = GetPlotStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_S32 && var_info->Count == 1) {
        int* pvar = (int*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    else if (var_info->Type == ImGuiDataType_Float && var_info->Count == 1) {
        float* pvar = (float*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = (float)val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() int variant but variable is not a int!");
}

void PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
{
    ImPlotContext& gp = *GImPlot;
    const ImPlotStyleVarInfo* var_info = GetPlotStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 2)
    {
        ImVec2* pvar = (ImVec2*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() ImVec2 variant but variable is not a ImVec2!");
}

void PopStyleVar(int count) {
    ImPlotContext& gp = *GImPlot;
    while (count > 0) {
        ImGuiStyleMod& backup = gp.StyleModifiers.back();
        const ImPlotStyleVarInfo* info = GetPlotStyleVarInfo(backup.VarIdx);
        void* data = info->GetVarPtr(&gp.Style);
        if (info->Type == ImGuiDataType_Float && info->Count == 1) {
            ((float*)data)[0] = backup.BackupFloat[0];
        }
        else if (info->Type == ImGuiDataType_Float && info->Count == 2) {
             ((float*)data)[0] = backup.BackupFloat[0];
             ((float*)data)[1] = backup.BackupFloat[1];
        }
        else if (info->Type == ImGuiDataType_S32 && info->Count == 1) {
            ((int*)data)[0] = backup.BackupInt[0];
        }
        gp.StyleModifiers.pop_back();
        count--;
    }
}

//------------------------------------------------------------------------------
// COLORMAPS
//------------------------------------------------------------------------------


void PushColormap(ImPlotColormap colormap) {
    ImPlotContext& gp = *GImPlot;
    gp.ColormapModifiers.push_back(ImPlotColormapMod(gp.Colormap, gp.ColormapSize));
    gp.Colormap = GetColormap(colormap, &gp.ColormapSize);
}

void PushColormap(const ImVec4* colormap, int size) {
    ImPlotContext& gp = *GImPlot;
    gp.ColormapModifiers.push_back(ImPlotColormapMod(gp.Colormap, gp.ColormapSize));
    gp.Colormap = colormap;
    gp.ColormapSize = size;
}

void PopColormap(int count) {
    ImPlotContext& gp = *GImPlot;
    while (count > 0) {
        const ImPlotColormapMod& backup = gp.ColormapModifiers.back();
        gp.Colormap     = backup.Colormap;
        gp.ColormapSize = backup.ColormapSize;
        gp.ColormapModifiers.pop_back();
        count--;
    }
}

void SetColormap(ImPlotColormap colormap, int samples) {
    ImPlotContext& gp = *GImPlot;
    gp.Colormap = GetColormap(colormap, &gp.ColormapSize);
    if (samples > 1) {
        static ImVector<ImVec4> resampled;
        resampled.resize(samples);
        ResampleColormap(gp.Colormap, gp.ColormapSize, &resampled[0], samples);
        SetColormap(&resampled[0], samples);
    }
    else {
        BustItemCache();
    }
}

void SetColormap(const ImVec4* colors, int size) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(colors != NULL, "You can't set the colors to NULL!");
    IM_ASSERT_USER_ERROR(size > 0, "The number of colors must be greater than 0!");
    static ImVector<ImVec4> user_colormap;
    user_colormap.shrink(0);
    user_colormap.reserve(size);
    for (int i = 0; i < size; ++i)
        user_colormap.push_back(colors[i]);
    gp.Colormap = &user_colormap[0];
    gp.ColormapSize = size;
    BustItemCache();
}

const ImVec4* GetColormap(ImPlotColormap colormap, int* size_out) {
    static const int csizes[ImPlotColormap_COUNT] = {10,10,9,9,12,11,11,11,11,11,11};
    static const ImOffsetCalculator<ImPlotColormap_COUNT> coffs(csizes);
    static ImVec4 cdata[] = {
        // ImPlotColormap_Default                                  // X11 Named Colors
        ImVec4(0.0f, 0.7490196228f, 1.0f, 1.0f),                   // Blues::DeepSkyBlue,
        ImVec4(1.0f, 0.0f, 0.0f, 1.0f),                            // Reds::Red,
        ImVec4(0.4980392158f, 1.0f, 0.0f, 1.0f),                   // Greens::Chartreuse,
        ImVec4(1.0f, 1.0f, 0.0f, 1.0f),                            // Yellows::Yellow,
        ImVec4(0.0f, 1.0f, 1.0f, 1.0f),                            // Cyans::Cyan,
        ImVec4(1.0f, 0.6470588446f, 0.0f, 1.0f),                   // Oranges::Orange,
        ImVec4(1.0f, 0.0f, 1.0f, 1.0f),                            // Purples::Magenta,
        ImVec4(0.5411764979f, 0.1686274558f, 0.8862745166f, 1.0f), // Purples::BlueViolet,
        ImVec4(0.5f, 0.5f, 0.5f, 1.0f),                            // Grays::Gray50,
        ImVec4(0.8235294223f, 0.7058823705f, 0.5490196347f, 1.0f), // Browns::Tan
        // ImPlotColormap_Deep
        ImVec4(0.298f, 0.447f, 0.690f, 1.000f),
        ImVec4(0.867f, 0.518f, 0.322f, 1.000f),
        ImVec4(0.333f, 0.659f, 0.408f, 1.000f),
        ImVec4(0.769f, 0.306f, 0.322f, 1.000f),
        ImVec4(0.506f, 0.446f, 0.702f, 1.000f),
        ImVec4(0.576f, 0.471f, 0.376f, 1.000f),
        ImVec4(0.855f, 0.545f, 0.765f, 1.000f),
        ImVec4(0.549f, 0.549f, 0.549f, 1.000f),
        ImVec4(0.800f, 0.725f, 0.455f, 1.000f),
        ImVec4(0.392f, 0.710f, 0.804f, 1.000f),
        // ImPlotColormap_Dark
        ImVec4(0.894118f, 0.101961f, 0.109804f, 1.0f),
        ImVec4(0.215686f, 0.494118f, 0.721569f, 1.0f),
        ImVec4(0.301961f, 0.686275f, 0.290196f, 1.0f),
        ImVec4(0.596078f, 0.305882f, 0.639216f, 1.0f),
        ImVec4(1.000000f, 0.498039f, 0.000000f, 1.0f),
        ImVec4(1.000000f, 1.000000f, 0.200000f, 1.0f),
        ImVec4(0.650980f, 0.337255f, 0.156863f, 1.0f),
        ImVec4(0.968627f, 0.505882f, 0.749020f, 1.0f),
        ImVec4(0.600000f, 0.600000f, 0.600000f, 1.0f),
        // ImPlotColormap_Pastel
        ImVec4(0.984314f, 0.705882f, 0.682353f, 1.0f),
        ImVec4(0.701961f, 0.803922f, 0.890196f, 1.0f),
        ImVec4(0.800000f, 0.921569f, 0.772549f, 1.0f),
        ImVec4(0.870588f, 0.796078f, 0.894118f, 1.0f),
        ImVec4(0.996078f, 0.850980f, 0.650980f, 1.0f),
        ImVec4(1.000000f, 1.000000f, 0.800000f, 1.0f),
        ImVec4(0.898039f, 0.847059f, 0.741176f, 1.0f),
        ImVec4(0.992157f, 0.854902f, 0.925490f, 1.0f),
        ImVec4(0.949020f, 0.949020f, 0.949020f, 1.0f),
        // ImPlotColormap_Paired
        ImVec4(0.258824f, 0.807843f, 0.890196f, 1.0f),
        ImVec4(0.121569f, 0.470588f, 0.705882f, 1.0f),
        ImVec4(0.698039f, 0.874510f, 0.541176f, 1.0f),
        ImVec4(0.200000f, 0.627451f, 0.172549f, 1.0f),
        ImVec4(0.984314f, 0.603922f, 0.600000f, 1.0f),
        ImVec4(0.890196f, 0.101961f, 0.109804f, 1.0f),
        ImVec4(0.992157f, 0.749020f, 0.435294f, 1.0f),
        ImVec4(1.000000f, 0.498039f, 0.000000f, 1.0f),
        ImVec4(0.792157f, 0.698039f, 0.839216f, 1.0f),
        ImVec4(0.415686f, 0.239216f, 0.603922f, 1.0f),
        ImVec4(1.000000f, 1.000000f, 0.600000f, 1.0f),
        ImVec4(0.694118f, 0.349020f, 0.156863f, 1.0f),
        // ImPlotColormap_Viridis
        ImVec4(0.267004f, 0.004874f, 0.329415f, 1.0f),
        ImVec4(0.282623f, 0.140926f, 0.457517f, 1.0f),
        ImVec4(0.253935f, 0.265254f, 0.529983f, 1.0f),
        ImVec4(0.206756f, 0.371758f, 0.553117f, 1.0f),
        ImVec4(0.163625f, 0.471133f, 0.558148f, 1.0f),
        ImVec4(0.127568f, 0.566949f, 0.550556f, 1.0f),
        ImVec4(0.134692f, 0.658636f, 0.517649f, 1.0f),
        ImVec4(0.266941f, 0.748751f, 0.440573f, 1.0f),
        ImVec4(0.477504f, 0.821444f, 0.318195f, 1.0f),
        ImVec4(0.741388f, 0.873449f, 0.149561f, 1.0f),
        ImVec4(0.993248f, 0.906157f, 0.143936f, 1.0f),
        // ImPlotColormap_Plasma
        ImVec4(5.03830e-02f, 2.98030e-02f, 5.27975e-01f, 1.00000e+00f),
        ImVec4(2.54627e-01f, 1.38820e-02f, 6.15419e-01f, 1.00000e+00f),
        ImVec4(4.17642e-01f, 5.64000e-04f, 6.58390e-01f, 1.00000e+00f),
        ImVec4(5.62738e-01f, 5.15450e-02f, 6.41509e-01f, 1.00000e+00f),
        ImVec4(6.92840e-01f, 1.65141e-01f, 5.64522e-01f, 1.00000e+00f),
        ImVec4(7.98216e-01f, 2.80197e-01f, 4.69538e-01f, 1.00000e+00f),
        ImVec4(8.81443e-01f, 3.92529e-01f, 3.83229e-01f, 1.00000e+00f),
        ImVec4(9.49217e-01f, 5.17763e-01f, 2.95662e-01f, 1.00000e+00f),
        ImVec4(9.88260e-01f, 6.52325e-01f, 2.11364e-01f, 1.00000e+00f),
        ImVec4(9.88648e-01f, 8.09579e-01f, 1.45357e-01f, 1.00000e+00f),
        ImVec4(9.40015e-01f, 9.75158e-01f, 1.31326e-01f, 1.00000e+00f),
        // ImPlotColormap_Hot
        ImVec4(0.2500f,        0.f,        0.f, 1.0f),
        ImVec4(0.5000f,        0.f,        0.f, 1.0f),
        ImVec4(0.7500f,        0.f,        0.f, 1.0f),
        ImVec4(1.0000f,        0.f,        0.f, 1.0f),
        ImVec4(1.0000f,    0.2500f,        0.f, 1.0f),
        ImVec4(1.0000f,    0.5000f,        0.f, 1.0f),
        ImVec4(1.0000f,    0.7500f,        0.f, 1.0f),
        ImVec4(1.0000f,    1.0000f,        0.f, 1.0f),
        ImVec4(1.0000f,    1.0000f,    0.3333f, 1.0f),
        ImVec4(1.0000f,    1.0000f,    0.6667f, 1.0f),
        ImVec4(1.0000f,    1.0000f,    1.0000f, 1.0f),
        // ImPlotColormap_Cool
        ImVec4(    0.f,    1.0000f,    1.0000f, 1.0f),
        ImVec4(0.1000f,    0.9000f,    1.0000f, 1.0f),
        ImVec4(0.2000f,    0.8000f,    1.0000f, 1.0f),
        ImVec4(0.3000f,    0.7000f,    1.0000f, 1.0f),
        ImVec4(0.4000f,    0.6000f,    1.0000f, 1.0f),
        ImVec4(0.5000f,    0.5000f,    1.0000f, 1.0f),
        ImVec4(0.6000f,    0.4000f,    1.0000f, 1.0f),
        ImVec4(0.7000f,    0.3000f,    1.0000f, 1.0f),
        ImVec4(0.8000f,    0.2000f,    1.0000f, 1.0f),
        ImVec4(0.9000f,    0.1000f,    1.0000f, 1.0f),
        ImVec4(1.0000f,        0.f,    1.0000f, 1.0f),
        // ImPlotColormap_Pink
        ImVec4(0.2887f,        0.f,        0.f, 1.0f),
        ImVec4(0.4830f,    0.2582f,    0.2582f, 1.0f),
        ImVec4(0.6191f,    0.3651f,    0.3651f, 1.0f),
        ImVec4(0.7303f,    0.4472f,    0.4472f, 1.0f),
        ImVec4(0.7746f,    0.5916f,    0.5164f, 1.0f),
        ImVec4(0.8165f,    0.7071f,    0.5774f, 1.0f),
        ImVec4(0.8563f,    0.8062f,    0.6325f, 1.0f),
        ImVec4(0.8944f,    0.8944f,    0.6831f, 1.0f),
        ImVec4(0.9309f,    0.9309f,    0.8028f, 1.0f),
        ImVec4(0.9661f,    0.9661f,    0.9068f, 1.0f),
        ImVec4(1.0000f,    1.0000f,    1.0000f, 1.0f),
        // ImPlotColormap_Jet
        ImVec4(    0.f,        0.f,    0.6667f, 1.0f),
        ImVec4(    0.f,        0.f,    1.0000f, 1.0f),
        ImVec4(    0.f,    0.3333f,    1.0000f, 1.0f),
        ImVec4(    0.f,    0.6667f,    1.0000f, 1.0f),
        ImVec4(    0.f,    1.0000f,    1.0000f, 1.0f),
        ImVec4(0.3333f,    1.0000f,    0.6667f, 1.0f),
        ImVec4(0.6667f,    1.0000f,    0.3333f, 1.0f),
        ImVec4(1.0000f,    1.0000f,        0.f, 1.0f),
        ImVec4(1.0000f,    0.6667f,        0.f, 1.0f),
        ImVec4(1.0000f,    0.3333f,        0.f, 1.0f),
        ImVec4(1.0000f,        0.f,        0.f, 1.0f)
    };
    *size_out =  csizes[colormap];
    return &cdata[coffs.Offsets[colormap]];
}

const char* GetColormapName(ImPlotColormap colormap) {
    static const char* cmap_names[]   = {"Default","Deep","Dark","Pastel","Paired","Viridis","Plasma","Hot","Cool","Pink","Jet"};
    return cmap_names[colormap];
}

void ResampleColormap(const ImVec4* colormap_in, int size_in, ImVec4* colormap_out, int size_out) {
    for (int i = 0; i < size_out; ++i) {
        float t = i * 1.0f / (size_out - 1);
        colormap_out[i] = LerpColormap(colormap_in, size_in, t);
    }
}

int GetColormapSize() {
    ImPlotContext& gp = *GImPlot;
    return gp.ColormapSize;
}

ImVec4 GetColormapColor(int index) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(index >= 0, "The Colormap index must be greater than zero!");
    return gp.Colormap[index % gp.ColormapSize];
}

ImVec4 LerpColormap(const ImVec4* colormap, int size, float t) {
    float tc = ImClamp(t,0.0f,1.0f);
    int i1 = (int)((size -1 ) * tc);
    int i2 = i1 + 1;
    if (i2 == size || size == 1)
        return colormap[i1];
    float t1 = (float)i1 / (float)(size - 1);
    float t2 = (float)i2 / (float)(size - 1);
    float tr = ImRemap(t, t1, t2, 0.0f, 1.0f);
    return ImLerp(colormap[i1], colormap[i2], tr);
}

ImVec4 LerpColormap(float t) {
    ImPlotContext& gp = *GImPlot;
    return LerpColormap(gp.Colormap, gp.ColormapSize, t);
}

ImVec4 NextColormapColor() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != NULL, "NextColormapColor() needs to be called between BeginPlot() and EndPlot()!");
    ImVec4 col  = gp.Colormap[gp.CurrentPlot->ColormapIdx % gp.ColormapSize];
    gp.CurrentPlot->ColormapIdx++;
    return col;
}

void ShowColormapScale(double scale_min, double scale_max, float height) {
    ImPlotContext& gp = *GImPlot;
    static ImPlotTickCollection ticks;
    ticks.Reset();
    ImPlotRange range;
    range.Min = scale_min;
    range.Max = scale_max;

    AddTicksDefault(range, 10, 0, ticks);


    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return;
    const float txt_off = 5;
    const float bar_w   = 20;

    ImDrawList &DrawList = *Window->DrawList;
    ImVec2 size(bar_w + txt_off + ticks.MaxWidth + 2 * gp.Style.PlotPadding.x, height);
    ImRect bb_frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + size);
    ImGui::ItemSize(bb_frame);
    if (!ImGui::ItemAdd(bb_frame, 0, &bb_frame))
        return;
    ImGui::RenderFrame(bb_frame.Min, bb_frame.Max, GetStyleColorU32(ImPlotCol_FrameBg));
    ImRect bb_grad(bb_frame.Min + gp.Style.PlotPadding, bb_frame.Min + ImVec2(bar_w + gp.Style.PlotPadding.x, height - gp.Style.PlotPadding.y));

    int num_cols = GetColormapSize();
    float h_step = (height - 2 * gp.Style.PlotPadding.y) / (num_cols - 1);
    for (int i = 0; i < num_cols-1; ++i) {
        ImRect rect(bb_grad.Min.x, bb_grad.Min.y + h_step * i, bb_grad.Max.x, bb_grad.Min.y + h_step * (i + 1));
        ImU32 col1 = ImGui::GetColorU32(GetColormapColor(num_cols - 1 - i));
        ImU32 col2 = ImGui::GetColorU32(GetColormapColor(num_cols - 1 - (i+1)));
        DrawList.AddRectFilledMultiColor(rect.Min, rect.Max, col1, col1, col2, col2);
    }
    ImVec4 col_tik4 = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    col_tik4.w *= 0.25f;
    const ImU32 col_tick = ImGui::GetColorU32(col_tik4);

    ImGui::PushClipRect(bb_frame.Min, bb_frame.Max, true);
    for (int i = 0; i < ticks.Size; ++i) {
        float ypos = ImRemap((float)ticks.Ticks[i].PlotPos, (float)range.Max, (float)range.Min, bb_grad.Min.y, bb_grad.Max.y);
        if (ypos < bb_grad.Max.y - 2 && ypos > bb_grad.Min.y + 2)
            DrawList.AddLine(ImVec2(bb_grad.Max.x-1, ypos), ImVec2(bb_grad.Max.x - (ticks.Ticks[i].Major ? 10.0f : 5.0f), ypos), col_tick, 1.0f);
        DrawList.AddText(ImVec2(bb_grad.Max.x-1, ypos) + ImVec2(txt_off, -ticks.Ticks[i].LabelSize.y * 0.5f), GetStyleColorU32(ImPlotCol_TitleText), ticks.GetLabel(i));
    }
    ImGui::PopClipRect();

    DrawList.AddRect(bb_grad.Min, bb_grad.Max, GetStyleColorU32(ImPlotCol_PlotBorder));
}


//-----------------------------------------------------------------------------
// Style Editor etc.
//-----------------------------------------------------------------------------

static void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

bool ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (ImGui::Combo(label, &style_idx, "Auto\0Classic\0Dark\0Light\0"))
    {
        switch (style_idx)
        {
        case 0: StyleColorsAuto(); break;
        case 1: StyleColorsClassic(); break;
        case 2: StyleColorsDark(); break;
        case 3: StyleColorsLight(); break;
        }
        return true;
    }
    return false;
}

void ShowStyleEditor(ImPlotStyle* ref) {
    ImPlotContext& gp = *GImPlot;
    ImPlotStyle& style = GetStyle();
    static ImPlotStyle ref_saved_style;
    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    if (ImPlot::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    HelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
               "Use \"Export\" below to save them somewhere.");
    if (ImGui::BeginTabBar("##StyleEditor")) {
        if (ImGui::BeginTabItem("Variables")) {
            ImGui::Text("Item Styling");
            ImGui::SliderFloat("LineWeight", &style.LineWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("MarkerSize", &style.MarkerSize, 2.0f, 10.0f, "%.1f");
            ImGui::SliderFloat("MarkerWeight", &style.MarkerWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("FillAlpha", &style.FillAlpha, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("ErrorBarSize", &style.ErrorBarSize, 0.0f, 10.0f, "%.1f");
            ImGui::SliderFloat("ErrorBarWeight", &style.ErrorBarWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("DigitalBitHeight", &style.DigitalBitHeight, 0.0f, 20.0f, "%.1f");
            ImGui::SliderFloat("DigitalBitGap", &style.DigitalBitGap, 0.0f, 20.0f, "%.1f");
            float indent = ImGui::CalcItemWidth() - ImGui::GetFrameHeight();
            ImGui::Indent(ImGui::CalcItemWidth() - ImGui::GetFrameHeight());
            ImGui::Checkbox("AntiAliasedLines", &style.AntiAliasedLines);
            ImGui::Unindent(indent);
            ImGui::Text("Plot Styling");
            ImGui::SliderFloat("PlotBorderSize", &style.PlotBorderSize, 0.0f, 2.0f, "%.0f");
            ImGui::SliderFloat("MinorAlpha", &style.MinorAlpha, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat2("MajorTickLen", (float*)&style.MajorTickLen, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("MinorTickLen", (float*)&style.MinorTickLen, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("MajorTickSize",  (float*)&style.MajorTickSize, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat2("MinorTickSize", (float*)&style.MinorTickSize, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat2("MajorGridSize", (float*)&style.MajorGridSize, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat2("MinorGridSize", (float*)&style.MinorGridSize, 0.0f, 2.0f, "%.1f");
            ImGui::Text("Plot Padding");
            ImGui::SliderFloat2("PlotPadding", (float*)&style.PlotPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("LabelPadding", (float*)&style.LabelPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("LegendPadding", (float*)&style.LegendPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("InfoPadding", (float*)&style.InfoPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("PlotMinSize", (float*)&style.PlotMinSize, 0.0f, 300, "%.0f");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Colors")) {
            static int output_dest = 0;
            static bool output_only_modified = false;

            if (ImGui::Button("Export", ImVec2(75,0))) {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImPlot::GetStyle().Colors;\n");
                for (int i = 0; i < ImPlotCol_COUNT; i++) {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImPlot::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0) {
                        if (IsColorAuto(i))
                            ImGui::LogText("colors[ImPlotCol_%s]%*s= IMPLOT_AUTO_COL;\n",name,14 - (int)strlen(name), "");
                        else
                            ImGui::LogText("colors[ImPlotCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n",
                                        name, 14 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                    }
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on colored square to open color picker,\n"
                "Right-click to open edit options menu.");
            ImGui::Separator();
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImPlotCol_COUNT; i++) {
                const char* name = ImPlot::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImVec4 temp = GetStyleColorVec4(i);
                const bool is_auto = IsColorAuto(i);
                if (!is_auto)
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
                if (ImGui::Button("Auto")) {
                    if (is_auto)
                        style.Colors[i] = temp;
                    else
                        style.Colors[i] = IMPLOT_AUTO_COL;
                    BustItemCache();
                }
                if (!is_auto)
                    ImGui::PopStyleVar();
                ImGui::SameLine();
                if (ImGui::ColorEdit4(name, &temp.x, ImGuiColorEditFlags_NoInputs | alpha_flags)) {
                    style.Colors[i] = temp;
                    BustItemCache();
                }
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0) {
                    ImGui::SameLine(175); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(); if (ImGui::Button("Revert")) {
                        style.Colors[i] = ref->Colors[i];
                        BustItemCache();
                    }
                }
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::Separator();
            ImGui::Text("Colors that are set to Auto (i.e. IMPLOT_AUTO_COL) will\n"
                        "be automatically deduced from your ImGui style or the\n"
                        "current ImPlot Colormap. If you want to style individual\n"
                        "plot items, use Push/PopStyleColor around its function.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Colormaps")) {
            static int output_dest = 0;
            if (ImGui::Button("Export", ImVec2(75,0))) {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("static const ImVec4 colormap[%d] = {\n", gp.ColormapSize);
                for (int i = 0; i < gp.ColormapSize; ++i) {
                    const ImVec4& col = gp.Colormap[i];
                    ImGui::LogText("    ImVec4(%.2ff, %.2ff, %.2ff, %.2ff)%s\n", col.x, col.y, col.z, col.w, i == gp.ColormapSize - 1 ? "" : ",");
                }
                ImGui::LogText("};");
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); HelpMarker("Export code for selected Colormap\n(built in or custom).");
            ImGui::Separator();
            static ImVector<ImVec4> custom;
            static bool custom_set = false;
            for (int i = 0; i < ImPlotColormap_COUNT; ++i) {
                ImGui::PushID(i);
                int size;
                const ImVec4* cmap = GetColormap(i, &size);
                bool selected = cmap == gp.Colormap;
                if (selected) {
                    custom_set = false;
                }

                if (!selected)
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
                if (ImGui::Button(GetColormapName(i), ImVec2(75,0))) {
                    SetColormap(i);
                    custom_set = false;
                }
                if (!selected)
                    ImGui::PopStyleVar();
                ImGui::SameLine();
                for (int c = 0; c < size; ++c) {
                    ImGui::PushID(c);
                    ImGui::ColorButton("",cmap[c]);
                    if (c != size -1)
                        ImGui::SameLine();
                    ImGui::PopID();
                }
                ImGui::PopID();
            }
            if (custom.Size == 0) {
                custom.push_back(ImVec4(1,1,1,1));
                custom.push_back(ImVec4(0.5f,0.5f,0.5f,1));
            }
            ImGui::Separator();
            ImGui::BeginGroup();
            bool custom_set_now = custom_set;
            if (!custom_set_now)
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
            if (ImGui::Button("Custom", ImVec2(75, 0))) {
                SetColormap(&custom[0], custom.Size);
                custom_set = true;
            }
            if (!custom_set_now)
                ImGui::PopStyleVar();
            if (ImGui::Button("+", ImVec2((75 - ImGui::GetStyle().ItemSpacing.x)/2,0))) {
                custom.push_back(ImVec4(0,0,0,1));
                if (custom_set) {
                    SetColormap(&custom[0], custom.Size);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2((75 - ImGui::GetStyle().ItemSpacing.x)/2,0)) && custom.Size > 1) {
                custom.pop_back();
                if (custom_set) {
                    SetColormap(&custom[0], custom.Size);
                }
            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            for (int c = 0; c < custom.Size; ++c) {
                ImGui::PushID(c);
                if (ImGui::ColorEdit4("##Col1", &custom[c].x, ImGuiColorEditFlags_NoInputs) && custom_set) {
                    SetColormap(&custom[0], custom.Size);
                }
                if ((c + 1) % 12 != 0)
                    ImGui::SameLine();
                ImGui::PopID();
            }
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ShowUserGuide() {
        ImGui::BulletText("Left click and drag within the plot area to pan X and Y axes.");
    ImGui::Indent();
        ImGui::BulletText("Left click and drag on an axis to pan an individual axis.");
    ImGui::Unindent();
    ImGui::BulletText("Scroll in the plot area to zoom both X any Y axes.");
    ImGui::Indent();
        ImGui::BulletText("Scroll on an axis to zoom an individual axis.");
    ImGui::Unindent();
    ImGui::BulletText("Right click and drag to box select data.");
    ImGui::Indent();
        ImGui::BulletText("Hold Alt to expand box selection horizontally.");
        ImGui::BulletText("Hold Shift to expand box selection vertically.");
        ImGui::BulletText("Left click while box selecting to cancel the selection.");
    ImGui::Unindent();
    ImGui::BulletText("Double left click to fit all visible data.");
    ImGui::Indent();
        ImGui::BulletText("Double left click on an axis to fit the individual axis.");
    ImGui::Unindent();
    ImGui::BulletText("Double right click to open the full plot context menu.");
    ImGui::Indent();
        ImGui::BulletText("Double right click on an axis to open the axis context menu.");
    ImGui::Unindent();
    ImGui::BulletText("Click legend label icons to show/hide plot items.");
}

void StyleColorsAuto(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 0.25f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_FrameBg]       = IMPLOT_AUTO_COL;
    colors[ImPlotCol_PlotBg]        = IMPLOT_AUTO_COL;
    colors[ImPlotCol_PlotBorder]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_LegendBg]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_LegendBorder]  = IMPLOT_AUTO_COL;
    colors[ImPlotCol_LegendText]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_TitleText]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_InlayText]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_PlotBorder]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_XAxis]         = IMPLOT_AUTO_COL;
    colors[ImPlotCol_XAxisGrid]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_YAxis]         = IMPLOT_AUTO_COL;
    colors[ImPlotCol_YAxisGrid]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_YAxis2]        = IMPLOT_AUTO_COL;
    colors[ImPlotCol_YAxisGrid2]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_YAxis3]        = IMPLOT_AUTO_COL;
    colors[ImPlotCol_YAxisGrid3]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Selection]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Query]         = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Crosshairs]    = IMPLOT_AUTO_COL;
}

void StyleColorsClassic(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 0.5f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_FrameBg]       = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendBg]      = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendText]    = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_XAxis]         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_XAxisGrid]     = ImVec4(0.90f, 0.90f, 0.90f, 0.25f);
    colors[ImPlotCol_YAxis]         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_YAxisGrid]     = ImVec4(0.90f, 0.90f, 0.90f, 0.25f);
    colors[ImPlotCol_YAxis2]        = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_YAxisGrid2]    = ImVec4(0.90f, 0.90f, 0.90f, 0.25f);
    colors[ImPlotCol_YAxis3]        = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_YAxisGrid3]    = ImVec4(0.90f, 0.90f, 0.90f, 0.25f);
    colors[ImPlotCol_Selection]     = ImVec4(0.97f, 0.97f, 0.39f, 1.00f);
    colors[ImPlotCol_Query]         = ImVec4(0.00f, 1.00f, 0.59f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(0.50f, 0.50f, 0.50f, 0.75f);
}

void StyleColorsDark(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 0.25f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendBg]      = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendText]    = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_XAxis]         = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_XAxisGrid]     = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    colors[ImPlotCol_YAxis]         = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_YAxisGrid]     = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    colors[ImPlotCol_YAxis2]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_YAxisGrid2]    = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    colors[ImPlotCol_YAxis3]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_YAxisGrid3]    = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    colors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImPlotCol_Query]         = ImVec4(0.00f, 1.00f, 0.44f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
}

void StyleColorsLight(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 1.0f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.42f, 0.57f, 1.00f, 0.13f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImPlotCol_LegendBg]      = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.82f, 0.82f, 0.82f, 0.80f);
    colors[ImPlotCol_LegendText]    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_XAxis]         = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_XAxisGrid]     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_YAxis]         = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_YAxisGrid]     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_YAxis2]        = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_YAxisGrid2]    = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
    colors[ImPlotCol_YAxis3]        = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_YAxisGrid3]    = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
    colors[ImPlotCol_Selection]     = ImVec4(0.82f, 0.64f, 0.03f, 1.00f);
    colors[ImPlotCol_Query]         = ImVec4(0.00f, 0.84f, 0.37f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
}

}  // namespace ImPlot
