#ifndef __SENSOR_ADJUSTMENT_PAGE_H__
#define __SENSOR_ADJUSTMENT_PAGE_H__

// uncomment this for 5-panel (PIU) mode
#define __5_PANEL__ 

#include <EmbAJAX.h>

#define BUFLEN 30

// display for the current polling rates
EmbAJAXMutableSpan p1pollrate("p1pollrate");
EmbAJAXMutableSpan p2pollrate("p2pollrate");

// displays for the current input values
EmbAJAXMutableSpan p1a1display("p1a1display");
EmbAJAXMutableSpan p1a2display("p1a2display");
EmbAJAXMutableSpan p1a3display("p1a3display");
EmbAJAXMutableSpan p1a4display("p1a4display");
EmbAJAXMutableSpan p1a5display("p1a5display");
EmbAJAXMutableSpan p2a1display("p2a1display");
EmbAJAXMutableSpan p2a2display("p2a2display");
EmbAJAXMutableSpan p2a3display("p2a3display");
EmbAJAXMutableSpan p2a4display("p2a4display");
EmbAJAXMutableSpan p2a5display("p2a5display");

// text inputs to adjust the threshold values
EmbAJAXTextInput<BUFLEN> p1a1threshold("p1a1threshold");
EmbAJAXTextInput<BUFLEN> p1a2threshold("p1a2threshold");
EmbAJAXTextInput<BUFLEN> p1a3threshold("p1a3threshold");
EmbAJAXTextInput<BUFLEN> p1a4threshold("p1a4threshold");
EmbAJAXTextInput<BUFLEN> p1a5threshold("p1a5threshold");
EmbAJAXTextInput<BUFLEN> p2a1threshold("p2a1threshold");
EmbAJAXTextInput<BUFLEN> p2a2threshold("p2a2threshold");
EmbAJAXTextInput<BUFLEN> p2a3threshold("p2a3threshold");
EmbAJAXTextInput<BUFLEN> p2a4threshold("p2a4threshold");
EmbAJAXTextInput<BUFLEN> p2a5threshold("p2a5threshold");

#ifdef __5_PANEL__
  #define PAGE_TITLE "Pump It Up - Pad Configuration"
  #define ARROW_1 "Down-Left"
  #define ARROW_2 "Up-Left"
  #define ARROW_3 "Center"
  #define ARROW_4 "Up-Right"
  #define ARROW_5 "Down-Right"
#else
  #define PAGE_TITLE "Dance Dance Revolution - Pad Configuration"
  #define ARROW_1 "Up"
  #define ARROW_2 "Right"
  #define ARROW_3 "Down"
  #define ARROW_4 "Left"
#endif

// construct the page itself
MAKE_EmbAJAXPage(page, PAGE_TITLE, "",
    new EmbAJAXStatic(""
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
            "<meta name=\"viewport\" content\"width=device-width\">"
            "<style>"
                "html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center; color: white;}"
                "table, th, td {"
                  "background: #212529 !important;"
                  "color: white !important;"
                  "padding: 5px 5px 5px 5px;"
                "}"
                "th, td { padding: 5px }"
                "table { margin-left: auto; margin-right: auto; }"
                "h1 {"
                  "font-size: 3em;"
                  "font-weight: 100 ;"
                "}"
                ".content-section {"
                  "background: #3a3a3a;"
                  "position: relative;"
                  "padding: 30px;"
                  "border-radius: 10px;"
                  "-webkit-transition: all 0.2s ease-in-out;"
                  "-moz-transition: all 0.2s ease-in-out;"
                  "-o-transition: all 0.2s ease-in-out;"
                  "-ms-transition: all 0.2s ease-in-out;"
                "}"
                "body {"
                  "background: linear-gradient(128deg, #172d42, #421717, #423d17, #421717, #172d42);"
                  "background-size: 1000% 1000%;"
                  "-webkit-animation: AnimationName 30s ease infinite;"
                  "-moz-animation: AnimationName 30s ease infinite;"
                  "animation: AnimationName 30s ease infinite;"
                  "max-width: 80%;"
                  "margin: auto;"
                  "padding-top: 50pt;"
                "}"
                "@-webkit-keyframes AnimationName {"
                   "0%{background-position:0% 9%}"
                   "50%{background-position:100% 92%}"
                   "100%{background-position:0% 9%}"
                "}"
                "@-moz-keyframes AnimationName {"
                   "0%{background-position:0% 9%}"
                   "50%{background-position:100% 92%}"
                   "100%{background-position:0% 9%}"
                "}"
                "@keyframes AnimationName {"
                   "0%{background-position:0% 9%}"
                   "50%{background-position:100% 92%}"
                   "100%{background-position:0% 9%}"
                "}"
                ".table thead th {"
                  "vertical-align: bottom;"
                  "border-bottom: 2px solid #3b434a;"
                  "padding: 5pt;"
                "}"
                "hr {"
                  "border-top: 3px solid;"
                  "border-radius: 3px;"
                "}"
                "input[type=text] {"
                  "color: white;"
                  "background: #3a3a3a;"
                  "padding: 4px 0px 4px 4px;"
                  "border: none;"
                  "border-radius: 0;"
                  "border-bottom: 2px solid #37c2f0;"
                  "text-decoration: bold;"
                  "margin-left: 10pt;"
                  "float: right;"
                  "max-width: 60pt;"
                  "font-family: \"Lucida Console\", Monaco, monospace;"
                "}"
                ".contentblock {"
                  "background: #4a4f54;"
                  "padding: 3px 3px 3px 3px;"
                  "color: white;"
                  "text-decoration: bold;"
                  "display: inline-block;"
                  "font-family: \"Lucida Console\", Monaco, monospace;"
                "}"
                "textarea {"
                  "font-family: \"Lucida Console\", Monaco, monospace;"
                "}"
            "</style>"
        "</head>"
        "<body>"
           "<div class=\"content-section\">"
           "<h1>"
           PAGE_TITLE
           "</h1>"
           "<hr />"
            "<h2>Player 1 polling rate: <span class=\"contentblock\">"),
    &p1pollrate,
    new EmbAJAXStatic(""
        " Hz</span></h2>"
        "<h2>Player 2 polling rate: <span class=\"contentblock\">"),
    &p2pollrate,
    new EmbAJAXStatic(""
        " Hz</span></h2>"
        "<div style=\"display: inline-block; margin-right: 15pt;\"><h2>Panel Input Values:</h2>"
        "<hr style=\"margin-left: 10%; margin-right: 10%; margin-bottom: 15pt; border-top: 1.5px solid #4a4f54; border-radius: 1.5px;\"/>"
        "<table class=\"table\">"
            "<thead>"
              "<tr>"
                "<th>Player 1</th>"
                "<th>Player 2</th>"
              "</tr>"
            "</thead>"
            "<tbody>"
    "<tr><td><b>"
    ARROW_1              
    ": </b>"),
    &p1a1display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_1
    ": </b>"),
    &p2a1display,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_2
    ": </b>"),
    &p1a2display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_2
    ": </b>"),
    &p2a2display,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_3
    ": </b>"),
    &p1a3display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_3
    ": </b>"),
    &p2a3display,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_4
    ": </b>"),
    &p1a4display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_4
    ": </b>"),
    &p2a4display,
    new EmbAJAXStatic("</td></tr>"),
#ifdef __5_PANEL__
    new EmbAJAXStatic("<tr><td><b>"
    ARROW_5
    ": </b>"),
    &p1a5display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_5
    ": </b>"),
    &p2a5display,
    new EmbAJAXStatic("</td></tr>"),
#endif
    new EmbAJAXStatic("</tbody>"
        "</table>"
        "</div>"
        "<div style=\"display: inline-block; margin-left: 15pt;\"><h2>Adjust Thresholds:</h2>"
        "<hr style=\"margin-left: 10%; margin-right: 10%; margin-bottom: 15pt; border-top: 1.5px solid #4a4f54; border-radius: 1.5px;\"/>"
        "<table class=\"table\">"
            "<thead>"
                "<tr>"
                    "<th>Player 1</th>"
                    "<th>Player 2</th>"
                "</tr>"
            "</thead>"
            "<tbody>"
    "<tr><td><b>"
    ARROW_1
    ": </b>"),
    &p1a1threshold,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_1
    ": </b>"),
    &p2a1threshold,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_2
    ": </b>"),
    &p1a2threshold,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_2
    ": </b>"),
    &p2a2threshold,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_3
    ": </b>"),
    &p1a3threshold,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_3
    ": </b>"),
    &p2a3threshold,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_4
    ": </b>"),
    &p1a4threshold,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_4
    ": </b>"),
    &p2a4threshold,
    new EmbAJAXStatic("</td></tr>"),
#ifdef __5_PANEL__
    new EmbAJAXStatic("<tr><td><b>"
    ARROW_5
    ": </b>"),
    &p1a5threshold,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_5
    ": </b>"),
    &p2a5threshold,
    new EmbAJAXStatic("</td></tr>"),
#endif
    new EmbAJAXStatic("</tbody>"
        "</table>"
        "</div>"
        "</div>"
        "</body>"
        "</html>")
)

#endif
